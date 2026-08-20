#
# $ Copyright 2016-YEAR Cypress Semiconductor $
#

ifeq ($(WHICHFILE),true)
$(info Processing $(lastword $(MAKEFILE_LIST)))
endif

#
# Basic Configuration
#
LIBNAME=le_audio_profiles_lib
TOOLCHAIN=GCC_ARM
CONFIG?=Debug

#
# Advanced Configuration
#
SOURCES=
INCLUDES=$(CURDIR)/include $(CY_BASELIB_PATH)/include $(CY_APP_PATH)/../../../bluetooth_apps/include
DEFINES=
VFP_SELECT=
CFLAGS=
CXXFLAGS=
ASFLAGS=
LDFLAGS=
LDLIBS=
LINKER_SCRIPT=
PREBUILD=
POSTBUILD=
COMPONENT=

$(info $(INCLUDES))

################################################################################
# Paths
################################################################################

# Path (absolute or relative) to the project
CY_APP_PATH=.

CY_SHARED_PATH=$(CY_APP_PATH)/../../../wiced_btsdk

# FV2
CY_GETLIBS_SHARED_PATH=$(CY_SHARED_PATH)

# absolute path to the root of the BTSDK
CY_SHARED_PATH_ABS=$(CURDIR)/../../../wiced_btsdk

# Path (absolute or relative) to the base library
CY_BASELIB_PATH=$(CY_SHARED_PATH)/dev-kit/baselib/$(CY_TARGET_DEVICE)/COMPONENT_$(CY_TARGET_DEVICE)
CY_BASELIB_CORE_PATH=$(CY_SHARED_PATH)/../core-make

# flow version 1 settings, only for prebuilts
CY_BSP_PATH=$(CY_SHARED_PATH)/dev-kit/bsp/TARGET_$(TARGET)
override CY_COMPILER_DIR_BWC=$(CY_COMPILER_DIR)
override CY_MODUS_SHELL_DIR_BWC=$(CY_MODUS_SHELL_DIR)

# Extra makefiles includes - target makefile
CY_EXTRA_INCLUDES=$(CY_SHARED_PATH)/dev-kit/bsp/TARGET_$(TARGET)/$(TARGET).mk

# Absolute path to the compiler (Default: GCC in the tools)
CY_COMPILER_PATH=

# Locate ModusToolbox IDE helper tools folders in default installation
# locations for Windows, Linux, and macOS.
CY_WIN_HOME=$(subst \,/,$(USERPROFILE))
CY_TOOLS_PATHS ?= $(wildcard \
    $(CY_WIN_HOME)/ModusToolbox/tools_* \
    $(HOME)/ModusToolbox/tools_* \
    /Applications/ModusToolbox/tools_* \
    $(CY_IDE_TOOLS_DIR))

# If you install ModusToolbox IDE in a custom location, add the path to its
# "tools_X.Y" folder (where X and Y are the version number of the tools
# folder).
CY_TOOLS_PATHS+=

# Default to the newest installed tools folder, or the users override (if it's
# found).
CY_TOOLS_DIR=$(lastword $(sort $(wildcard $(CY_TOOLS_PATHS))))

ifeq ($(CY_TOOLS_DIR),)
$(error Unable to find any of the available CY_TOOLS_PATHS -- $(CY_TOOLS_PATHS))
endif

# getlibs path
CY_GETLIBS_PATH=.

# if not coming from app makefile (IDE), skip recipe
ifeq ($(CY_TARGET_DEVICE)$(TARGET),)
CY_SKIP_RECIPE=1
endif

include $(CY_TOOLS_DIR)/make/start.mk

# if not coming from app makefile (IDE), stub out all: target
ifeq ($(CY_TARGET_DEVICE)$(TARGET),)
all:
	@echo This is not a buildable application project - \"make $@\" : Success
endif
