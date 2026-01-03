import tkinter as tk
from app.provisioning_ui import ProvisioningUI

def main():
  root = tk.Tk()
  app = ProvisioningUI(root)
  app.setup_ui()
  root.mainloop()

if __name__ == "__main__":
  main()
