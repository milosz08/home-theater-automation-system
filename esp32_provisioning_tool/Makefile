PYTHON_SYS = python
VENV = .venv
BIN = $(VENV)/Scripts
PYTHON = $(BIN)/python
PIP = $(BIN)/pip
PYINSTALLER = $(BIN)/pyinstaller
APP_NAME = esp32-provisioning-tool
MAIN_FILE = main.py
DEV_FILE = dev_setup.py

all: run

$(BIN)/activate:
	$(PYTHON_SYS) -m venv $(VENV)
	$(PIP) install --upgrade pip

venv: $(BIN)/activate

install: venv
	$(PIP) install .

gui: venv
	$(PYTHON) $(MAIN_FILE)

setup: venv
	$(PYTHON) $(DEV_FILE)

build: venv
	$(PIP) install pyinstaller
	$(PYINSTALLER) --noconfirm \
		--onefile \
		--windowed \
		--icon="res/icon.png" \
		--add-data "res/icon.png;res" \
		--collect-all esptool \
		--name $(APP_NAME) \
		$(MAIN_FILE)

clean:
	@if exist build rmdir /s /q build
	@if exist dist rmdir /s /q dist
	@if exist $(VENV) rmdir /s /q $(VENV)
	@if exist *.spec del /f /q *.spec
