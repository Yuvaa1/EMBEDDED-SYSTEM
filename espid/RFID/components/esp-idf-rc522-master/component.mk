# Name of the component
COMPONENT_NAME := rc522

# Source files for the component
COMPONENT_SRCS := rc522.c

# Include directories for the component
COMPONENT_ADD_INCLUDEDIRS := .

# Component requires other components (if any)
COMPONENT_REQUIRES :=

include $(IDF_PATH)/make/component.mk
