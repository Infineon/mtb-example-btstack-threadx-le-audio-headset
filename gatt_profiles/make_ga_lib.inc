GA_LIB_SOURCE :=
LIB_DIR := $(ROOTDR)/libraries/COMPONENT_le_audio_profiles

include  $(LIB_DIR)/makefile.mk

GA_LIB_SOURCE         += $(COMPONENT_le_audio_profiles_SOURCES)

INCLUDES             += -I"$(ROOTDR)/btstack/stack/include/"
INCLUDES             += -I"$(ROOTDR)/btstack/wiced_include/"
INCLUDES             += -I"$(ROOTDR)/btstack/stack/gatt"
INCLUDES             += -I"$(LIB_DIR)/include/"
INCLUDES             += -I"$(ROOTDR)/libraries/COMPONENT_le_audio_profiles/internal"
INCLUDES             += -I"$(ROOTDR)/libraries/COMPONENT_gatt_interface/include"
