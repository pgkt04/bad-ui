CXX := clang++
BUILD_DIR := build
MACOS_APP := $(BUILD_DIR)/bad-ui-macos

COMMON_SRC := \
	src/ui_object.cpp \
	src/ui_parent.cpp \
	src/ui_form.cpp \
	src/ui_tab.cpp \
	src/ui_group.cpp \
	src/ui_columns.cpp \
	src/ui_checkbox.cpp \
	src/ui_slider.cpp \
	src/ui_dropdown.cpp

.PHONY: macos clean

macos: $(MACOS_APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(MACOS_APP): $(BUILD_DIR) src/main_macos.mm $(COMMON_SRC)
	$(CXX) -std=c++11 -Wall -Wextra -Wno-unused-parameter \
		src/main_macos.mm $(COMMON_SRC) \
		-framework Cocoa -o $(MACOS_APP)

clean:
	rm -rf $(BUILD_DIR)
