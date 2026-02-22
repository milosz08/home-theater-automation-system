import os
import sys
import hashlib
import shutil
from dotenv import load_dotenv
from app.provisioning_generator import ProvisioningGenerator

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
env_path = os.path.join(parent_dir, ".env")

if not os.path.exists(env_path):
  print("not found end file!")
  sys.exit(1)

load_dotenv(dotenv_path=env_path)
print(f"load config from: {env_path}")

LOCAL_STAGING_DIR = "../setup_data"
ESP_DATA_DIR = "../esp32_firmware/data"

ESP_IP = os.getenv("ESP_IP_ADDRESS", "192.168.0.2")
ESP_GATEWAY = os.getenv("ESP_GATEWAY", "192.168.0.1")
ESP_NETMASK = os.getenv("ESP_NETMASK", "255.255.255.0")
ESP_DNS = os.getenv("ESP_DNS", "1.1.1.1")
ESP_PORT = int(os.getenv("ESP_PORT", "443"))
ESP_NTP_TIMEZONE = os.getenv("ESP_NTP_TIMEZONE", "CET-1CEST,M3.5.0,M10.5.0/3")
ESP_NTP_SERVER = os.getenv("ESP_NTP_SERVER", "pool.ntp.org")
ESP_PASS = os.getenv("ESP_DEFAULT_PASSWORD", "admin")

def deploy_to_esp32_folder():
  files_to_deploy = ["prvtkey.pem", "cacert.pem", "config.json"]
  if not os.path.exists(ESP_DATA_DIR):
    os.makedirs(ESP_DATA_DIR)
    
  for filename in files_to_deploy:
    src = os.path.join(LOCAL_STAGING_DIR, filename)
    dst = os.path.join(ESP_DATA_DIR, filename)
    if os.path.exists(src):
      shutil.copy2(src, dst)

def main():
  generator = ProvisioningGenerator(LOCAL_STAGING_DIR)
  generator.prepare_directory()
  print(f"generated all files in: {LOCAL_STAGING_DIR}")

  _, cert_path = generator.generate_ssl_keys(ESP_IP)

  config_data = {
    "ip": ESP_IP,
    "gateway": ESP_GATEWAY,
    "netmask": ESP_NETMASK,
    "dns": ESP_DNS,
    "port": ESP_PORT,
    "ntp_server": ESP_NTP_SERVER,
    "ntp_timezone": ESP_NTP_TIMEZONE,
    "default_password": ESP_PASS
  }
  generator.generate_config_json(config_data)

  qr_img_obj, qr_raw_str = generator.generate_qr_code(config_data, cert_path)
  qr_save_path = os.path.join(LOCAL_STAGING_DIR, "pairing_qr.png")
  qr_img_obj.save(qr_save_path)
  print(f"generated qr code, payload: {qr_raw_str}")

  with open(cert_path, "rb") as f:
    cert_data = f.read()
    fingerprint = hashlib.sha256(cert_data).hexdigest()
  print(f"SHA-256 fingerprint: {fingerprint}")

  deploy_to_esp32_folder()
  print(f"copied files to: {ESP_DATA_DIR}")

if __name__ == "__main__":
  main()
