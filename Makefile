TOOL_DIR = esp32_provisioning_tool

.PHONY: setup

# default target
all: setup

setup:
	$(MAKE) -C $(TOOL_DIR) setup
