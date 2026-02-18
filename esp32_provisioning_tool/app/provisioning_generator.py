import os
import json
import datetime
import hashlib
import qrcode
from datetime import datetime, timezone, timedelta
from cryptography import x509
from cryptography.x509.oid import NameOID
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa

class ProvisioningGenerator:
  def __init__(self, output_dir):
    self.output_dir = output_dir

  def prepare_directory(self):
    if not os.path.exists(self.output_dir):
      os.makedirs(self.output_dir)
      return True
    return False
  
  def generate_ssl_keys(self, cn_value):
    private_key = rsa.generate_private_key(
      public_exponent=65537,
      key_size=2048,
    )
    subject = issuer = x509.Name([
      x509.NameAttribute(NameOID.COMMON_NAME, cn_value),
    ])
    now = datetime.now(timezone.utc)
    cert = x509.CertificateBuilder().subject_name(
      subject
    ).issuer_name(
      issuer
    ).public_key(
        private_key.public_key()
    ).serial_number(
      x509.random_serial_number()
    ).not_valid_before(
      # -10 years for issues with time synchronization on ESP32
      now - timedelta(days=3650)
    ).not_valid_after(
      now + timedelta(days=36500)
    ).sign(private_key, hashes.SHA256())

    key_path = os.path.join(self.output_dir, "prvtkey.pem")
    with open(key_path, "wb") as f:
      f.write(private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.TraditionalOpenSSL,
        encryption_algorithm=serialization.NoEncryption()
      ))

    cert_path = os.path.join(self.output_dir, "cacert.pem")
    with open(cert_path, "wb") as f:
      f.write(cert.public_bytes(serialization.Encoding.PEM))

    return key_path, cert_path
  
  def generate_config_json(self, network_data):
    config_path = os.path.join(self.output_dir, "config.json")
    with open(config_path, "w") as f:
      json.dump(network_data, f, indent=4)
    return config_path

  def generate_qr_code(self, network_data, cert_path):
    with open(cert_path, "rb") as f:
      pem_data = f.read()
      cert = x509.load_pem_x509_certificate(pem_data)
      der_data = cert.public_bytes(serialization.Encoding.DER)
      fingerprint = hashlib.sha256(der_data).hexdigest().lower()

    qr_payload = {
      "ip": network_data["ip"],
      "port": network_data["port"],
      "pass": network_data["default_password"],
      "hash": fingerprint
    }
    qr_json = json.dumps(qr_payload)

    qr = qrcode.QRCode(version=1, box_size=10, border=5)
    qr.add_data(qr_json)
    qr.make(fit=True)

    qr_img = qr.make_image(fill_color="black", back_color="white")
    return qr_img, qr_json
