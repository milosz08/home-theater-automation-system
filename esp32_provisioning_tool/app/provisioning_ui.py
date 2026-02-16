import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import os
from PIL import ImageTk, Image
import threading
import shutil
import datetime
from pathlib import Path
import sys

from .provisioning_generator import ProvisioningGenerator
from .esp_device_manager import EspDeviceManager
from .ntp_service import NtpService

class ProvisioningUI:
  def __init__(self, root):
    self.root = root
    self.root.title("ESP32 provisioning tool (by Miłosz Gilga)")
    self.root.resizable(False, True)
    self.root.geometry("900x700")
    self.root.iconphoto(True, tk.PhotoImage(file=self.resource_path("res/icon.png")))

    self.device_mgr = EspDeviceManager()
    self.ntp_service = NtpService()

    self.suggested_name = "pairing_qr"
    self.DEFAULTS = {
      "ip": "192.168.0.2",
      "gateway": "192.168.0.1",
      "netmask":  "255.255.255.0",
      "dns": "1.1.1.1",
      "port": "443",
      "password": "admin",
      "ntp_server": self.ntp_service.get_suggested_ntp_server(),
      "ntp_timezone": self.ntp_service.get_local_posix_str(),
    }
    self.setup_path = tk.StringVar(value=os.path.join(Path.home(), "setup"))

    self.firmware_path = tk.StringVar(value="")
    self.should_flash = tk.BooleanVar(value=False)
    self.selected_port = tk.StringVar()

    self.esp_ip = tk.StringVar(value=self.DEFAULTS["ip"])
    self.esp_gateway = tk.StringVar(value=self.DEFAULTS["gateway"])
    self.esp_netmask = tk.StringVar(value=self.DEFAULTS["netmask"])
    self.esp_dns = tk.StringVar(value=self.DEFAULTS["dns"])
    self.esp_port = tk.StringVar(value=self.DEFAULTS["port"])
    self.esp_password = tk.StringVar(value=self.DEFAULTS["password"])
    self.esp_ntp_server = tk.StringVar(value=self.DEFAULTS["ntp_server"])
    self.esp_ntp_timezone = tk.StringVar(value=self.DEFAULTS["ntp_timezone"])

    self.qr_image_tk = None
    self.flash_check_btn = None

    self.current_qr_pil = None
    self.current_qr_payload = None

  def resource_path(self, relative_path):
    try:
      base_path = sys._MEIPASS
    except Exception:
      base_path = os.path.abspath(".")
    return os.path.join(base_path, relative_path)

  def setup_ui(self):
    self.main_container = ttk.Frame(self.root, padding="15")
    self.main_container.pack(fill=tk.BOTH, expand=True)

    top_section = ttk.Frame(self.main_container)
    top_section.pack(fill=tk.X, expand=False)

    self.left_panel = ttk.Frame(top_section)
    self.left_panel.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

    op_frame = ttk.LabelFrame(self.left_panel, text=" Firmware & operations ", padding="10")
    op_frame.pack(fill=tk.X, pady=(0, 5))

    # left panel: select .bin file
    bin_row = ttk.Frame(op_frame)
    bin_row.pack(fill=tk.X, pady=(0, 10))
    ttk.Label(bin_row, text="Firmware (.bin):").pack(side=tk.LEFT)
    ttk.Entry(bin_row, textvariable=self.firmware_path).pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)
    ttk.Button(bin_row, text="Select...", width=8, command=self.browse_firmware).pack(side=tk.RIGHT)

    # left panel: com port
    port_row = ttk.Frame(op_frame)
    port_row.pack(fill=tk.X, pady=(0, 10))
    ttk.Label(port_row, text="COM port:").pack(side=tk.LEFT)
    self.port_combo = ttk.Combobox(port_row, textvariable=self.selected_port, width=15)
    self.port_combo.pack(side=tk.LEFT, padx=5)
    ttk.Button(port_row, text="Refresh", width=8, command=self.refresh_ports).pack(side=tk.LEFT)

    # left panel: flash + run
    action_row = ttk.Frame(op_frame)
    action_row.pack(fill=tk.X)

    self.run_btn = ttk.Button(action_row, text="RUN PROVISIONING", command=self.handle_provisioning)
    self.run_btn.pack(side=tk.LEFT, padx=(0, 10), ipady=8, ipadx=10)

    self.flash_check_btn = ttk.Checkbutton(
      action_row,
      text="Flash to ESP32",
      variable=self.should_flash,
      state=tk.DISABLED
    )
    self.flash_check_btn.pack(side=tk.LEFT)
    self.firmware_path.trace_add("write", self.validate_flash_availability)

    self.refresh_ports()

    # left panel: output path
    path_frame = ttk.LabelFrame(self.left_panel, text=" Config output location ", padding="10")
    path_frame.pack(fill=tk.X, pady=5)
    ttk.Entry(path_frame, textvariable=self.setup_path).pack(side=tk.LEFT, fill=tk.X, expand=True)
    ttk.Button(path_frame, text="...", width=3, command=self.browse_path).pack(side=tk.RIGHT, padx=2)

    # left panel: network parameters
    config_frame = ttk.LabelFrame(self.left_panel, text=" Network parameters ", padding="10")
    config_frame.pack(fill=tk.X, pady=5)

    inputs = [
      ("IP address:", self.esp_ip, "ip"),
      ("Gateway:", self.esp_gateway, "gateway"),
      ("Netmask:", self.esp_netmask, "netmask"),
      ("DNS server:", self.esp_dns, "dns"),
      ("Server port:", self.esp_port, "port"),
      ("NTP server:", self.esp_ntp_server, "ntp_server"),
      ("NTP timezone (posix):", self.esp_ntp_timezone, "ntp_timezone"),
      ("Default password:", self.esp_password, "password"),
    ]
    for _, (label_text, var, d_key) in enumerate(inputs):
      cell = ttk.Frame(config_frame)
      cell.pack(fill=tk.X, pady=2)
      ttk.Label(cell, text=label_text, width=15).pack(side=tk.LEFT)
      reset_btn = ttk.Button(cell, text="R", width=3, command=lambda v=var, k=d_key: v.set(self.DEFAULTS[k]))
      reset_btn.pack(side=tk.RIGHT)
      ttk.Entry(cell, textvariable=var).pack(side=tk.RIGHT, expand=True, fill=tk.X, padx=5)

    # righ panel: qr code
    self.qr_container = ttk.LabelFrame(top_section, text=" Pairing QR code (TOFU) ", padding="10")
    self.qr_container.pack(side=tk.RIGHT, fill=tk.Y, expand=False, padx=(15, 0))
    self.qr_container.configure(width=420, height=420)
    self.qr_container.pack_propagate(False)
    self.qr_display = ttk.Label(self.qr_container, text="QR code will appear here")
    self.qr_display.pack(expand=True)

    # right panel: qr code buttons
    qr_btn_frame = ttk.Frame(self.qr_container)
    qr_btn_frame.pack(fill=tk.X, pady=(5, 0))

    self.btn_save_qr = ttk.Button(qr_btn_frame, text="Save to file", command=self.save_qr_file, state=tk.DISABLED)
    self.btn_save_qr.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0, 2))

    self.btn_copy_qr = ttk.Button(qr_btn_frame, text="Copy to clipboard", command=self.copy_qr_payload,
                                  state=tk.DISABLED)
    self.btn_copy_qr.pack(side=tk.RIGHT, fill=tk.X, expand=True, padx=(2, 0))

    # bottom: logs
    log_frame = ttk.LabelFrame(self.main_container, text=" System logs ", padding="5")
    log_frame.pack(fill=tk.BOTH, expand=True, pady=(10, 0))
    self.log_text = tk.Text(log_frame, state=tk.DISABLED, bg="#1e1e1e", fg="#00ff00", font=("Consolas", 10))
    self.log_text.pack(fill=tk.BOTH, expand=True)

  def browse_firmware(self):
    filename = filedialog.askopenfilename(
      title="Select ESP32 Firmware",
      filetypes=[("Binary files", "*.bin"), ("All files", "*.*")]
    )
    if filename:
      self.firmware_path.set(filename)

  def validate_flash_availability(self, *_):
    path = self.firmware_path.get().strip()
    if path and os.path.isfile(path) and path.lower().endswith(".bin"):
      self.flash_check_btn.config(state=tk.NORMAL)
    else:
      self.should_flash.set(False)
      self.flash_check_btn.config(state=tk.DISABLED)

  def browse_path(self):
    directory = filedialog.askdirectory()
    if directory:
      self.setup_path.set(directory)

  def refresh_ports(self):
    ports = self.device_mgr.get_serial_ports()
    self.port_combo["values"] = ports
    if ports:
      self.port_combo.current(0)

  def log(self, message):
    def _update():
      self.log_text.config(state=tk.NORMAL)
      if message:
        self.log_text.insert(tk.END, f"> {message}\n")
        self.log_text.see(tk.END)
      self.log_text.config(state=tk.DISABLED)
    self.root.after(0, _update)

  def save_qr_file(self):
    if not self.current_qr_pil:
      return

    current_time = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    file_path = filedialog.asksaveasfilename(
      defaultextension=".png",
      initialfile=f"{self.suggested_name}_{current_time}",
      filetypes=[("PNG files", "*.png"), ("All files", "*.*")],
      title="Save QR Code"
    )
    if file_path:
      try:
        self.current_qr_pil.save(file_path)
        messagebox.showinfo("Saved", f"QR code saved to:\n{file_path}")
      except Exception as e:
        messagebox.showerror("Error", f"Failed to save file: {e}")

  def copy_qr_payload(self):
    if self.current_qr_payload:
      self.root.clipboard_clear()
      self.root.clipboard_append(self.current_qr_payload)
      self.root.update()
      messagebox.showinfo("Copied", "QR JSON Payload copied to clipboard!")

  def handle_provisioning(self):
    ui_data = {
      "output_dir": self.setup_path.get().strip(),
      "firmware_path": self.firmware_path.get(),
      "port": self.selected_port.get(),
      "should_flash": self.should_flash.get(),
      "network_config": {
        "ip": self.esp_ip.get().strip(),
        "gateway": self.esp_gateway.get().strip(),
        "netmask": self.esp_netmask.get().strip(),
        "dns": self.esp_dns.get().strip(),
        "port": int(self.esp_port.get() or 443),
        "ntp_server": self.esp_ntp_server.get().strip(),
        "ntp_timezone": self.esp_ntp_timezone.get().strip(),
        "default_password": self.esp_password.get()
      }
    }
    if hasattr(self, "run_btn"):
      self.run_btn.config(state=tk.DISABLED)
    self.log("STARTING FLASHING PROCESS")

    thread = threading.Thread(target=self._run_provisioning_thread, args=(ui_data,))
    thread.daemon = True
    thread.start()

  def _run_provisioning_thread(self, data):
    base_dir = os.path.dirname(data["output_dir"])
    temp_dir = os.path.join(base_dir, ".temp")
    try:
      generator = ProvisioningGenerator(data["output_dir"])

      self.log("preparing directory...")
      generator.prepare_directory()

      cn = data["network_config"]["ip"] if data["network_config"]["ip"] else "esp32.local"
      _, cert_path = generator.generate_ssl_keys(cn)
      self.log("generated keys")

      generator.generate_config_json(data["network_config"])
      self.log("generated config")

      qr_img_obj, qr_raw_str = generator.generate_qr_code(data["network_config"], cert_path)
      self.root.after(0, lambda: self.show_qr(qr_img_obj, qr_raw_str))
      self.log(f"generated qr code, payload: {qr_raw_str}")

      if data["should_flash"]:
        if not data["port"]:
          raise ValueError("No COM port selected.")

        if not os.path.exists(temp_dir):
            os.makedirs(temp_dir)
        try:
          bin_path = os.path.join(os.path.dirname(data["output_dir"]), "storage.bin")
          self.device_mgr.create_littlefs_bin(data["output_dir"], bin_path)
          self.log(f"littleFS binary created at {bin_path}")

          self.log("starting ESPTOOL...")
          self.device_mgr.flash_device(
            data["port"],
            data["firmware_path"],
            bin_path,
            log_callback=self.log,
          )
          self.log("FLASHING COMPLETE SUCCESS!")
        finally:
          if os.path.exists(temp_dir):
            shutil.rmtree(temp_dir)
      else:
        self.log("Done (No flash).")

    except Exception as e:
      self.log(f"ERROR: {str(e)}")
      self.root.after(0, lambda: messagebox.showerror("Error", str(e)))
    finally:
      self.root.after(0, lambda: self.run_btn.config(state=tk.NORMAL))

  def show_qr(self, img_obj, payload_str):
    self.current_qr_pil = img_obj
    self.current_qr_payload = payload_str

    self.btn_save_qr.config(state=tk.NORMAL)
    self.btn_copy_qr.config(state=tk.NORMAL)

    display_img = img_obj.resize((380, 380), Image.Resampling.LANCZOS)
    self.qr_image_tk = ImageTk.PhotoImage(display_img)
    self.qr_display.config(image=self.qr_image_tk, text="")
