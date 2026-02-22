PYTHON_SYS = python
VENV = .venv
APP_NAME = esp32-provisioning-tool
APP_VER ?= 0.0.1-local-dev
MAIN_FILE = main.py
DEV_FILE = dev_setup.py

ifeq ($(OS),Windows_NT)
	BIN_DIR = Scripts
	SEP = ;
	RM_DIR = rmdir /s /q
	RM_FILE = del /f /q
	CLEAN_CMD = \
		if exist build $(RM_DIR) build & \
		if exist dist $(RM_DIR) dist & \
		if exist $(VENV) $(RM_DIR) $(VENV) & \
		if exist *.spec $(RM_FILE) *.spec
else
	BIN_DIR = bin
	SEP = :
	RM_DIR = rm -rf
	RM_FILE = rm -f
	CLEAN_CMD = $(RM_DIR) build dist $(VENV) *.spec
endif

BIN = $(VENV)/$(BIN_DIR)
PYTHON = $(BIN)/python
PIP = $(BIN)/pip
PYINSTALLER = $(BIN)/pyinstaller

.PHONY: all venv install gui setup build clean

# default target
all: gui

$(BIN)/activate:
	$(PYTHON_SYS) -m venv $(VENV)
	$(PYTHON) -m pip install --upgrade pip

venv: $(BIN)/activate

install: venv
	$(PIP) install .

gui: venv
	$(PYTHON) $(MAIN_FILE)

setup: venv
	$(PYTHON) $(DEV_FILE)

build: venv
	$(PIP) install pyinstaller Pillow
	@echo APP_VER = \'$(APP_VER)\' > version.py
	$(PYINSTALLER) --noconfirm \
		--onefile \
		--windowed \
		--icon="res/icon.png" \
		--add-data "res/icon.png$(SEP)res" \
		--collect-all esptool \
		--name $(APP_NAME)-v$(APP_VER) \
		$(MAIN_FILE)

clean:
	@$(CLEAN_CMD)
	@$(RM_FILE) version.py
