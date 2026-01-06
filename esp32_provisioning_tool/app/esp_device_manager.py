import os
import sys
import serial.tools.list_ports
from littlefs import LittleFS
import esptool
import re

class LogRedirector:
  def __init__(self, callback):
    self.callback = callback
    self.ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')

  def write(self, text):
    clean_text = self.ansi_escape.sub("", text)
    clean_text = clean_text.strip()
    if clean_text:
      self.callback(clean_text)

  def flush(self):
    pass

class EspDeviceManager:
  def __init__(self):
    self.PARTITION_SIZE = 0x20000  # 128KB
    self.BLOCK_SIZE = 4096
    self.PROG_SIZE = 256

    self.ADDR_APP = "0x10000" # app
    self.ADDR_STORAGE = "0x3D0000" # littlefs

  def get_serial_ports(self):
    return [comport.device for comport in serial.tools.list_ports.comports()]

  def create_littlefs_bin(self, source_dir, output_bin_path):
    if not os.path.exists(source_dir) or not os.listdir(source_dir):
      raise Exception("directory is empty")

    ALLOWED_EXTENSIONS = {".json", ".pem", ".crt", ".key"}
    block_count = self.PARTITION_SIZE // self.BLOCK_SIZE
    fs = LittleFS(block_size=self.BLOCK_SIZE, block_count=block_count, prog_size=self.PROG_SIZE)

    files_packed = 0
    for filename in os.listdir(source_dir):
      file_path = os.path.join(source_dir, filename)
      _, ext = os.path.splitext(filename)

      if os.path.isfile(file_path) and ext.lower() in ALLOWED_EXTENSIONS:
        with open(file_path, "rb") as src:
          data = src.read()
        with fs.open(filename, "wb") as target:
          target.write(data)
        files_packed += 1

    with open(output_bin_path, "wb") as f:
      f.write(fs.context.buffer)

    return output_bin_path

  def flash_device(self, port, app_bin_path, storage_bin_path, log_callback=None):
    if not port:
      raise ValueError("COM Port is not selected!")

    args = [
      "--chip", "esp32",
      "--port", port,
      "--baud", "460800",
      "--before", "default-reset",
      "--after", "hard-reset",
      "write-flash",
      "-z",
      self.ADDR_APP, app_bin_path,
      self.ADDR_STORAGE, storage_bin_path
    ]
    if log_callback:
      log_callback(f"Executing internal esptool with args: {args}")

    original_stdout = sys.stdout
    original_stderr = sys.stderr

    try:
      if log_callback:
        sys.stdout = LogRedirector(log_callback)
        sys.stderr = LogRedirector(log_callback)

      esptool.main(args)
    except SystemExit as e:
      if e.code != 0:
        raise Exception(f"Esptool failed with exit code {e.code}")
    except Exception as e:
      raise Exception(f"Esptool internal error: {str(e)}")
    finally:
      sys.stdout = original_stdout
      sys.stderr = original_stderr
