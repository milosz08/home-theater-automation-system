TOOL_DIR = esp32_provisioning_tool

.PHONY: setup

setup:
	$(MAKE) -C $(TOOL_DIR) setup
