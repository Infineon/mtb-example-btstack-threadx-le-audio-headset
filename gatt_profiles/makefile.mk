#
# $ Copyright Cypress Semiconductor $
#
NAME := COMPONENT_le_audio_profiles
COMMON_LIB_SRC := libraries/$(NAME)

current_dir = $(notdir $(shell pwd))
$(info $(current_dir))

$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_aics.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_aics_client.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_aics_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_ascs.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_ascs_client.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_ascs_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_bap_unicast.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_bass.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_bass_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_bass_client.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_bap.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_ccp.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_csip.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_csis.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_csis_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_csis_psri.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_hap.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_has_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_has.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_iap.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_ias_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_ias.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_mcp.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_mcs_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_mcs.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_micp.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_mics.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_mics_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_pacs.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_pacs_client.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_pacs_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_tbs.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_tbs_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_tmas.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_tmap_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_tmap.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_gmap_client.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_gmap_server.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_gmap_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_vcp.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_vcs_init.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_vcs.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_vocs.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_vocs_client.c
$(NAME)_SOURCES += $(COMMON_LIB_SRC)/ga_lib_vocs_init.c
$(NAME)_SOURCES += libraries/iso_data_handler_module_lib/iso_data_handler.c

########################################################################
################ DO NOT MODIFY FILE BELOW THIS LINE ####################
########################################################################
include $(LIBRARY_COMMON_MAKE)
