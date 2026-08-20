/*
 * $ Copyright Cypress Semiconductor $
 */

/* Application includes */
#include "audio_driver.h"
#include "lehs.h"

extern wiced_bt_cfg_ble_t lehs_ble_cfg;
extern wiced_bt_cfg_settings_t lehs_cfg_settings;
extern wiced_bt_heap_t *p_lea_default_heap;

lehs_gatt_cb_t g_lehs_gatt_cb;

const uint8_t lehs_gatt_database[] =
{

    /* Primary Service 'Generic Attribute' */
    PRIMARY_SERVICE_UUID16(HDLS_GATT_GENERIC_ATTRIBUTE_SERVICE, UUID_SERVICE_GATT),

    // Service Changed Characteristic

    CHARACTERISTIC_UUID16(HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_CHANGED,
                          HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_CHANGED_VALUE,
                          GATT_UUID_GATT_SRV_CHGD,
                          GATTDB_CHAR_PROP_INDICATE,
                          GATTDB_PERM_NONE),

    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_GATT_GENERIC_ATTRIBUTE_SERVICE_CHANGED_CLIENT_CONFIGURATION,
                                    GATT_UUID_CHAR_CLIENT_CONFIG,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_WRITE_CMD | GATTDB_PERM_WRITE_REQ),

    CHARACTERISTIC_UUID16(HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_DB_HASH,
                          HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_DB_HASH_VALUE,
                          GATT_UUID_GATT_DATABASE_HASH,
                          GATTDB_CHAR_PROP_READ,
                          GATTDB_PERM_READABLE),
    CHARACTERISTIC_UUID16_WRITABLE(HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_CLIENT_FEATURES,
                                   HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_CLIENT_FEATURES_VALUE,
                                   GATT_UUID_CLIENT_SUPPORTED_FEATURES,
                                   (GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_WRITE),
                                   (GATTDB_PERM_READABLE | GATTDB_PERM_WRITE_CMD | GATTDB_PERM_WRITE_REQ)),
    CHARACTERISTIC_UUID16(HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_SERVER_FEATURES,
                          HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_SERVER_FEATURES_VALUE,
                          GATT_UUID_SERVER_SUPPORTED_FEATURES,
                          GATTDB_CHAR_PROP_READ,
                          GATTDB_PERM_READABLE),

    /* Primary Service 'vcs' */
    PRIMARY_SERVICE_UUID16(HDLS_VCS, WICED_BT_UUID_VOLUME_CONTROL),

    /* Characteristic 'Volume State' */
    CHARACTERISTIC_UUID16(HDLC_VCS_VOLUME_STATE,
                          HDLC_VCS_VOLUME_STATE_VALUE,
                          WICED_BT_UUID_VOLUME_STATE,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_VCS_VOLUME_STATE_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    /* Characteristic 'Volume Control Point ' */
    CHARACTERISTIC_UUID16_WRITABLE(HDLC_VCS_VOLUME_CONTROL_POINT,
                                   HDLC_VCS_VOLUME_CONTROL_POINT_VALUE,
                                   WICED_BT_UUID_CONTROL_POINT,
                                   GATTDB_CHAR_PROP_WRITE,
                                   GATTDB_PERM_WRITABLE),

    /* Characteristic 'Volume Flags' */
    CHARACTERISTIC_UUID16(HDLC_VCS_VOLUME_FLAGS,
                          HDLC_VCS_VOLUME_FLAGS_VALUE,
                          WICED_BT_UUID_VOLUME_FLAG,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_VCS_VOLUME_FLAGS_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    PRIMARY_SERVICE_UUID16(HDLS_PACS, WICED_BT_UUID_PUBLISHED_AUDIO_CAPABILITY),

    CHARACTERISTIC_UUID16(HDLC_PACS_SINK_PAC,
                          HDLC_PACS_SINK_PAC_VALUE,
                          WICED_BT_UUID_PACS_SINK_PAC,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_PACS_SINK_PAC_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    CHARACTERISTIC_UUID16_WRITABLE(HDLC_PACS_SINK_AUDIO_LOCATIONS,
                                   HDLC_PACS_SINK_AUDIO_LOCATIONS_VALUE,
                                   WICED_BT_UUID_PACS_SINK_AUDIO_LOCATIONS,
                                   GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_WRITE | GATTDB_CHAR_PROP_NOTIFY,
                                   GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_PACS_SINK_AUDIO_LOCATIONS_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    CHARACTERISTIC_UUID16(HDLC_PACS_SOURCE_PAC,
                          HDLC_PACS_SOURCE_PAC_VALUE,
                          WICED_BT_UUID_PACS_SOURCE_PAC,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_PACS_SOURCE_PAC_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    CHARACTERISTIC_UUID16_WRITABLE(HDLC_PACS_SOURCE_AUDIO_LOCATIONS,
                                   HDLC_PACS_SOURCE_AUDIO_LOCATIONS_VALUE,
                                   WICED_BT_UUID_PACS_SOURCE_AUDIO_LOCATIONS,
                                   GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_WRITE | GATTDB_CHAR_PROP_NOTIFY,
                                   GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_PACS_SOURCE_AUDIO_LOCATIONS_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    CHARACTERISTIC_UUID16(HDLC_PACS_AVAILABLE_AUDIO_CONTEXTS,
                          HDLC_PACS_AVAILABLE_AUDIO_CONTEXTS_VALUE,
                          WICED_BT_UUID_PACS_AUDIO_CONTEXT_AVAILABILITY,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_PACS_AVAILABLE_AUDIO_CONTEXTS_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    CHARACTERISTIC_UUID16(HDLC_PACS_SUPPORTED_AUDIO_CONTEXTS,
                          HDLC_PACS_SUPPORTED_AUDIO_CONTEXTS_VALUE,
                          WICED_BT_UUID_PACS_SUPPORTED_AUDIO_CONTEXT,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_PACS_SUPPORTED_AUDIO_CONTEXTS_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    PRIMARY_SERVICE_UUID16(HDLS_ASCS, WICED_BT_UUID_AUDIO_STREAM_CONTROL),

    CHARACTERISTIC_UUID16(HDLC_ASCS_ASE_SINK,
                          HDLC_ASCS_ASE_SINK_VALUE,
                          WICED_BT_UUID_ASCS_SINK_ASE,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_ASCS_ASE_SINK_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    CHARACTERISTIC_UUID16(HDLC_ASCS_ASE_SOURCE,
                          HDLC_ASCS_ASE_SOURCE_VALUE,
                          WICED_BT_UUID_ASCS_SOURCE_ASE,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_ASCS_ASE_SOURCE_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    CHARACTERISTIC_UUID16(HDLC_ASCS_ASE_SINK_2,
                          HDLC_ASCS_ASE_SINK_2_VALUE,
                          WICED_BT_UUID_ASCS_SINK_ASE,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_ASCS_ASE_SINK_2_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),
    CHARACTERISTIC_UUID16_WRITABLE(HDLC_ASCS_ASE_CONTROL_POINT,
                                   HDLC_ASCS_ASE_CONTROL_POINT_VALUE,
                                   WICED_BT_UUID_ASCS_ASE_CONTROL_POINT,
                                   GATTDB_CHAR_PROP_WRITE | GATTDB_CHAR_PROP_WRITE_NO_RESPONSE |
                                       GATTDB_CHAR_PROP_NOTIFY,
                                   GATTDB_PERM_WRITABLE),

    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_ASCS_ASE_CONTROL_POINT_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    PRIMARY_SERVICE_UUID16(HDLS_CAS, WICED_BT_UUID_COMMON_AUDIO),
    INCLUDE_SERVICE_UUID16(
        HDLI_CAS_INCLUDE_CSIS, HDLS_CSIS, HDLC_CSIS_RANK_VALUE, WICED_BT_UUID_COORDINATE_SET_IDENTIFICATION),

    /* Primary Service 'BASS' */
    PRIMARY_SERVICE_UUID16(HDLS_BASS, WICED_BT_UUID_BROADCAST_AUDIO_SCAN),

    /* Characteristic 'Broadcast Audio Scan Control Point' */
    CHARACTERISTIC_UUID16_WRITABLE(HDLC_BASS_BROADCAST_AUDIO_SCAN_CONTROL_POINT,
                                   HDLC_BASS_BROADCAST_AUDIO_SCAN_CONTROL_POINT_VALUE,
                                   WICED_BT_UUID_BASS_CONTROL_POINT,
                                   GATTDB_CHAR_PROP_WRITE | GATTDB_CHAR_PROP_WRITE_NO_RESPONSE,
                                   GATTDB_PERM_WRITE_CMD | GATTDB_PERM_WRITE_REQ | GATTDB_PERM_AUTH_WRITABLE),

    /* Characteristic 'Broadcast Receive State' */
    CHARACTERISTIC_UUID16(HDLC_BASS_BROADCAST_RECEIVE_STATE_1,
                          HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE,
                          WICED_BT_UUID_BASS_BROADCAST_RECEIVE_STATE,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_BASS_BROADCAST_RECEIVE_STATE_1_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_WRITE_REQ | GATTDB_PERM_WRITE_CMD |
                                        GATTDB_PERM_AUTH_WRITABLE | GATTDB_PERM_AUTH_READABLE),

    /* Primary Service 'CSIS' */
    PRIMARY_SERVICE_UUID16(HDLS_CSIS, WICED_BT_UUID_COORDINATE_SET_IDENTIFICATION),

    /* Characteristic 'CSIS ga_lib_csis_sirk_t state' */
    CHARACTERISTIC_UUID16(HDLC_CSIS_SIRK,
                          HDLC_CSIS_SIRK_VALUE,
                          WICED_BT_UUID_CSIS_SIRK,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),
    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_CSIS_SIRK_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_WRITABLE | GATTDB_PERM_AUTH_READABLE),

    /* Characteristic 'CSIS SIZE state' */
    CHARACTERISTIC_UUID16(HDLC_CSIS_SIZE,
                          HDLC_CSIS_SIZE_VALUE,
                          WICED_BT_UUID_CSIS_SIZE,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),
    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_CSIS_SIZE_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    /* Characteristic 'CSIS Lock state' */
    CHARACTERISTIC_UUID16_WRITABLE(HDLC_CSIS_LOCK,
                                   HDLC_CSIS_LOCK_VALUE,
                                   WICED_BT_UUID_CSIS_LOCK,
                                   GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_WRITE | GATTDB_CHAR_PROP_NOTIFY,
                                   GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITE_REQ |
                                       GATTDB_PERM_AUTH_WRITABLE),
    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_CSIS_LOCK_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_WRITABLE | GATTDB_PERM_AUTH_READABLE),

    /* Characteristic 'CSIS Rank state' */
    CHARACTERISTIC_UUID16(HDLC_CSIS_RANK,
                          HDLC_CSIS_RANK_VALUE,
                          WICED_BT_UUID_CSIS_RANK,
                          GATTDB_CHAR_PROP_READ,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Primary Service 'MICS' */
    PRIMARY_SERVICE_UUID16(HDLS_MICS, WICED_BT_UUID_MICROPHONE_CONTROL),
    /* Included Service 'AICS' */
    INCLUDE_SERVICE_UUID16(HDLI_MICS_INCLUDED_AICS,
                           HDLS_MICS_AICS,
                           HDLD_MICS_AICS_AUDIO_INPUT_DESCRIPTION_CLIENT_CONFIGURATION,
                           WICED_BT_UUID_AUDIO_INPUT_CONTROL),
    /* Characteristic 'Microphone Mute state' */
    CHARACTERISTIC_UUID16_WRITABLE(HDLC_MICS_MUTE_STATE,
                                   HDLC_MICS_MUTE_STATE_VALUE,
                                   WICED_BT_UUID_MICS_MUTE_STATE,
                                   GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_WRITE | GATTDB_CHAR_PROP_NOTIFY,
                                   GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITE_REQ |
                                       GATTDB_PERM_AUTH_WRITABLE),
    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_MICS_MUTE_STATE_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    /* Primary Service 'AICS' */
    SECONDARY_SERVICE_UUID16(HDLS_MICS_AICS, WICED_BT_UUID_AUDIO_INPUT_CONTROL),

    /* Characteristic 'Input State' */
    CHARACTERISTIC_UUID16(HDLC_MICS_AICS_INPUT_STATE,
                          HDLC_MICS_AICS_INPUT_STATE_VALUE,
                          WICED_BT_UUID_INPUT_STATE,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_MICS_AICS_INPUT_STATE_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    /* Characteristic 'Gain Setting Attribute' */
    CHARACTERISTIC_UUID16(HDLC_MICS_AICS_GAIN_SETTING_ATTR,
                          HDLC_MICS_AICS_GAIN_SETTING_ATTR_VALUE,
                          WICED_BT_UUID_GAIN_SETTING_ATTRIBUTE,
                          GATTDB_CHAR_PROP_READ,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Characteristic 'Input Type' */
    CHARACTERISTIC_UUID16(HDLC_MICS_AICS_INPUT_TYPE,
                          HDLC_MICS_AICS_INPUT_TYPE_VALUE,
                          WICED_BT_UUID_INPUT_TYPE,
                          GATTDB_CHAR_PROP_READ,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Characteristic 'Input Status' */
    CHARACTERISTIC_UUID16(HDLC_MICS_AICS_INPUT_STATUS,
                          HDLC_MICS_AICS_INPUT_STATUS_VALUE,
                          WICED_BT_UUID_INPUT_STATUS,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_MICS_AICS_INPUT_STATUS_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    /* Characteristic 'Audio Input Control Point ' */
    CHARACTERISTIC_UUID16_WRITABLE(HDLC_MICS_AICS_AUDIO_INPUT_CONTROL_POINT,
                                   HDLC_MICS_AICS_AUDIO_INPUT_CONTROL_POINT_VALUE,
                                   WICED_BT_UUID_AUDIO_INPUT_CONTROL_POINT,
                                   GATTDB_CHAR_PROP_WRITE,
                                   GATTDB_PERM_WRITE_REQ | GATTDB_PERM_AUTH_WRITABLE),

    /* Characteristic 'Audio Output Description' */
    CHARACTERISTIC_UUID16_WRITABLE(HDLC_MICS_AICS_AUDIO_INPUT_DESCRIPTION,
                                   HDLC_MICS_AICS_AUDIO_INPUT_DESCRIPTION_VALUE,
                                   WICED_BT_UUID_AUDIO_INPUT_DESCRIPTION,
                                   GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_WRITE_NO_RESPONSE | GATTDB_CHAR_PROP_NOTIFY,
                                   GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITE_CMD |
                                       GATTDB_PERM_AUTH_WRITABLE),

    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_MICS_AICS_AUDIO_INPUT_DESCRIPTION_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),
#if HAS_ENABLED

    /* Primary Service 'HAS' */
    PRIMARY_SERVICE_UUID16(HDLS_HAS, WICED_BT_UUID_HEARING_ACCESS),

    /* Characteristic 'HAS hearing aid features' */
    CHARACTERISTIC_UUID16(HDLC_HAS_HEARIND_AID_FEATUES,
                          HDLC_HAS_HEARIND_AID_FEATUES_VALUE,
                          WICED_BT_UUID_HAS_HEARING_AID_FEATURES,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_HAS_HEARIND_AID_FEATUES_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),

    /* Characteristic 'HAS preset control point' */
    CHARACTERISTIC_UUID16_WRITABLE(HDLC_HAS_HEARING_AID_PRESET_CONTROL_POINT,
                                   HDLC_HAS_HEARING_AID_PRESET_CONTROL_POINT_VALUE,
                                   WICED_BT_UUID_HAS_HEARING_AID_PRESET_CONTROL_POINT,
                                   GATTDB_CHAR_PROP_WRITE | GATTDB_CHAR_PROP_NOTIFY | GATTDB_CHAR_PROP_INDICATE,
                                   GATTDB_PERM_WRITABLE),

    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_HAS_HEARING_AID_PRESET_CONTROL_POINT_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_WRITABLE),

    /* Characteristic 'HAS active preset index' */
    CHARACTERISTIC_UUID16(HDLC_HAS_ACTIVE_PRESET_INDEX,
                          HDLC_HAS_ACTIVE_PRESET_INDEX_VALUE,
                          WICED_BT_UUID_HAS_HEARING_AID_ACTIVE_PRESET_INDEX,
                          GATTDB_CHAR_PROP_READ | GATTDB_CHAR_PROP_NOTIFY,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Descriptor 'Client Characteristic Configuration' */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HDLD_HAS_ACTIVE_PRESET_INDEX_CLIENT_CONFIGURATION,
                                    UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                                    GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE | GATTDB_PERM_WRITABLE),
#endif

#if TMAS_ENABLED
    /* Primary Service 'telephone_bearer_service' */
    PRIMARY_SERVICE_UUID16(HDLS_TMAS, WICED_BT_UUID_TMAS),

    /* Characteristic 'bearer_provider_name' */
    CHARACTERISTIC_UUID16(HDLC_TMAS_TELEPHONE_MEDIA_AUDIO_PROFILE_ROLE,
                          HDLC_TMAS_TELEPHONE_MEDIA_AUDIO_PROFILE_ROLE_ROLE_VALUE,
                          WICED_BT_UUID_TMAP_ROLE,
                          GATTDB_CHAR_PROP_READ,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),
#endif

    /* Primary Service Gaming Audio Service */
    PRIMARY_SERVICE_UUID16(HDLS_GMAP, WICED_BT_UUID_GAMING_AUDIO_SERVICE),

    /* Characteristic GMAP Role */
    CHARACTERISTIC_UUID16(HDLS_GMAP_ROLE,
                          HDLS_GMAP_ROLE_VALUE,
                          WICED_BT_UUID_GMAP_ROLE,
                          GATTDB_CHAR_PROP_READ,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Characteristic GMAP UGT Features */
    CHARACTERISTIC_UUID16(HDLS_GMAP_UGT_FEATURES,
                          HDLS_GMAP_UGT_FEATURES_VALUE,
                          WICED_BT_UUID_GMAP_UGT_FEATURES,
                          GATTDB_CHAR_PROP_READ,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),

    /* Characteristic GMAP BGR Features */
    CHARACTERISTIC_UUID16(HDLS_GMAP_BGR_FEATURES,
                          HDLS_GMAP_BGR_FEATURES_VALUE,
                          WICED_BT_UUID_GMAP_BGR_FEATURES,
                          GATTDB_CHAR_PROP_READ,
                          GATTDB_PERM_READABLE | GATTDB_PERM_AUTH_READABLE),
};

const char *app_get_gatt_state_str(lehs_clcb_t *p_clcb)
{
    if (p_clcb->disconnecting)
    {
        return "disconnecting";
    }
    else if (p_clcb->ready)
    {
        return "ready";
    }
    else if (p_clcb->read_characteristics)
    {
        return "read_characteristics";
    }
    else if (p_clcb->enabled_notifications)
    {
        return "enabled_notifications";
    }
    else if (p_clcb->discovery_complete)
    {
        return "discovery_complete";
    }
    else if (p_clcb->mtu_exchanged)
    {
        return "mtu_exchanged";
    }
    else if (p_clcb->connected)
    {
        return "connected";
    }
    else
    {
        return "disconnected";
    }
}

static void lehs_print_gatt_state(lehs_clcb_t *p_clcb)
{
    WICED_BT_TRACE("[%s] %B new %s", __FUNCTION__, p_clcb->identity_bd_address, app_get_gatt_state_str(p_clcb));
}

static char *gatt_event_name[] = {
    "GATT_CONNECTION_STATUS_EVT",      /* 0 */
    "GATT_OPERATION_CPLT_EVT",         /* 1 */
    "GATT_DISCOVERY_RESULT_EVT",       /* 2 */
    "GATT_DISCOVERY_CPLT_EVT",         /* 3 */
    "GATT_ATTRIBUTE_REQUEST_EVT",      /* 4 */
    "GATT_CONGESTION_EVT",             /* 5 */
    "GATT_GET_RESPONSE_BUFFER_EVT",    /* 6 */
    "GATT_APP_BUFFER_TRANSMITTED_EVT", /* 7 */
};

void lehs_set_gatt_state_connection_sts(lehs_clcb_t *p_clcb, uint8_t connected)
{
    p_clcb->connected = connected;
    lehs_print_gatt_state(p_clcb);
}

void lehs_set_gatt_state_mtu_exchanged(lehs_clcb_t *p_clcb)
{
    p_clcb->mtu_exchanged = 1;
    lehs_print_gatt_state(p_clcb);
}

void lehs_set_gatt_state_discovery_complete(lehs_clcb_t *p_clcb)
{
    p_clcb->discovery_complete = 1;
    lehs_print_gatt_state(p_clcb);
}

void lehs_set_gatt_state_enabled_notifications(lehs_clcb_t *p_clcb)
{
    p_clcb->enabled_notifications = 1;
    p_clcb->ready = p_clcb->enabled_notifications & p_clcb->read_characteristics;
    lehs_print_gatt_state(p_clcb);
}

void lehs_set_gatt_state_read_characteristics(lehs_clcb_t *p_clcb)
{
    p_clcb->read_characteristics = 1;
    p_clcb->ready = p_clcb->enabled_notifications & p_clcb->read_characteristics;
    lehs_print_gatt_state(p_clcb);
}

wiced_bt_gatt_status_t lehs_send_disconnect(uint16_t conn_id, char *from)
{
    WICED_BT_TRACE("[%s] conn_id %d from %s\n", __FUNCTION__, conn_id, from ? from : "");

    app_rpc_send_app_status(conn_id, NULL, HCI_CONTROL_MISC_APP_STATE_DISCONNECTING, 0);

    return wiced_bt_gatt_disconnect(conn_id);
}

lehs_clcb_t *lehs_gatt_alloc_cb(uint8_t *p_bd_addr,
                                wiced_bt_ble_address_type_t addr_type,
                                uint16_t conn_id,
                                uint16_t link_role)
{
    lehs_clcb_t *p_clcb = g_lehs_gatt_cb.clcb;

    for (int index = 0; index < LEHS_MAX_CONNECTIONS; index++, p_clcb++)
    {
        if (p_clcb->in_use == TRUE)
        {
            continue;
        }

        memset(p_clcb, 0, sizeof(lehs_clcb_t));
        p_clcb->in_use = TRUE;
        p_clcb->conn_id = conn_id;
        p_clcb->conn_addr_type = addr_type;
        p_clcb->identity_address_type = addr_type;
        memcpy(p_clcb->identity_bd_address, p_bd_addr, BD_ADDR_LEN);
        memcpy(p_clcb->conn_bda, p_bd_addr, BD_ADDR_LEN);
        p_clcb->b_is_central = (HCI_ROLE_CENTRAL == link_role) ? TRUE : FALSE;
        if (conn_id == 0)
        {
            lehs_set_gatt_state_connection_sts(p_clcb, 0);
        }
        else
        {
            lehs_set_gatt_state_connection_sts(p_clcb, 1);
        }
        return p_clcb;
    }

    return p_clcb;
}

lehs_clcb_t *lehs_gatt_get_clcb(uint8_t *p_bd_addr)
{
    lehs_clcb_t *p_clcb = g_lehs_gatt_cb.clcb;

    for (int index = 0; index < LEHS_MAX_CONNECTIONS; index++, p_clcb++)
    {
        if (p_clcb->in_use && !WICED_MEMCMP(p_clcb->conn_bda, p_bd_addr, BD_ADDR_LEN))
        {
            return p_clcb;
        }
    }
    return NULL;
}

void lehs_gatt_free_discovery_ctx(lehs_clcb_t *p_clcb)
{
    if (p_clcb->p_discovery_ctx)
    {
        gatt_intf_free_service_discovery_ctx(p_clcb->p_discovery_ctx);
        p_clcb->p_discovery_ctx = NULL;
    }
}

wiced_bt_gatt_status_t lehs_gatt_free_cb(uint8_t *p_bd_addr)
{
    lehs_clcb_t *p_clcb = lehs_gatt_get_clcb(p_bd_addr);
    if (!p_clcb)
        return WICED_ERROR;

    lehs_gatt_free_discovery_ctx(p_clcb);

    WICED_MEMSET(p_clcb, 0, sizeof(lehs_clcb_t));
    return WICED_SUCCESS;
}

lehs_clcb_t *lehs_gatt_get_clcb_by_conn_id(uint16_t conn_id)
{
    lehs_clcb_t *p_clcb = g_lehs_gatt_cb.clcb;

    for (int index = 0; index < LEHS_MAX_CONNECTIONS; index++, p_clcb++)
    {
        if (p_clcb->in_use && (p_clcb->conn_id == conn_id))
        {
            return p_clcb;
        }
    }
    return NULL;
}

lehs_clcb_t *lehs_gatt_get_clcb_by_conn_handle(uint16_t acl_conn_handle)
{
    lehs_clcb_t *p_clcb = g_lehs_gatt_cb.clcb;
    for (int index = 0; index < LEHS_MAX_CONNECTIONS; index++, p_clcb++)
    {
        if (p_clcb->in_use && (wiced_bt_gatt_get_acl_conn_handle(p_clcb->conn_id) == acl_conn_handle))
        {
            return p_clcb;
        }
    }
    return NULL;
}

typedef struct
{
    uint16_t min_interval; /* N x 0.625 ms */
    uint16_t max_interval; /* N x 0.625 ms */
    uint16_t duration; /* N x 10 ms */
} adv_state_params_t;

const adv_state_params_t st_params[] = {
    /* ADV_STATE_IDLE */
    {.min_interval = 0, .max_interval = 0, .duration = 0 },
    /* ADV_STATE_SWIFT_PAIR_HIGH_DUTY_CYCLE */
    {.min_interval = 32, .max_interval = 32, .duration = 6000},
    /* ADV_STATE_SWIFT_PAIR_LOW_DUTY_CYCLE */
    {.min_interval = 60, .max_interval = 60, .duration = 6000},
    /* ADV_STATE_LOW_DUTY_CYCLE */
    {.min_interval = 40, .max_interval = 40, .duration = 12000}
};

static void lehs_get_adv_duty_cycle_params(adv_state_t adv_state,
                                           wiced_ble_ext_adv_params_t *p_params,
                                           wiced_ble_ext_adv_duration_config_t *p_dur)
{
    if (ADV_STATE_MAX > ((uint8_t)adv_state))
    {
        const adv_state_params_t *p_st = &st_params[adv_state];

        p_params->primary_adv_int_min = p_st->min_interval;
        p_params->primary_adv_int_max = p_st->max_interval;
        p_dur->adv_duration = p_st->duration;
    }

    return;
}

//lehs
const uint8_t ascs_data[] = {0x4E,
                             0x18, //UUID
                             0x01, // Announcement Type (0x00: Genaral, 0x01: Targeted)
                             (BAP_CONTEXT_TYPE_CONVERSATIONAL | BAP_CONTEXT_TYPE_MEDIA | BAP_CONTEXT_TYPE_UNSPECIFIED),
                             0x02,
                             BAP_CONTEXT_TYPE_CONVERSATIONAL | BAP_CONTEXT_TYPE_UNSPECIFIED,
                             0,
                             0}; // metadata len

int lehs_gatt_get_adv_data(adv_state_t adv_state, wiced_bt_adv_ctx_t *p_ctx)
{
    uint16_t adv_opt = g_lehs_gatt_cb.adv_data_options;
    uint8_t flag = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;
    uint16_t volume_uuid = WICED_BT_UUID_VOLUME_CONTROL;
    //uint16_t aics_uuid = WICED_BT_UUID_AUDIO_INPUT_CONTROL;
    uint16_t bscan_uuid = WICED_BT_UUID_BROADCAST_AUDIO_SCAN;
#if TMAS_ENABLED
    uint8_t tmap_data[] = {0x55,
                           0x18,
                           TMAP_ROLE_CALL_TERMINAL | TMAP_ROLE_UNICAST_MEDIA_RECEIVER |
                               TMAP_ROLE_BROADCAST_MEDIA_RECEIVER,
                           0x00};
#endif
    uint8_t gmap_data[] = {0x58, 0x18, g_lehs_gatt_cb.local_service_data.gmap.gmap_role};
    ga_lib_csis_sirk_data_t *p_sirk = lehs_csis_get_sirk();
    ga_lib_csis_psri_t psri;

    ga_lib_csis_generate_psri(&p_sirk->sirk, &psri);

    wiced_bt_ble_advert_elem_t adv_data_elems[] = {
        {.advert_type = BTM_BLE_ADVERT_TYPE_APPEARANCE,
         .p_data = (uint8_t *)&lehs_cfg_settings.p_ble_cfg->appearance,
         .len = sizeof(wiced_bt_gatt_appearance_t)},
        {.advert_type = BTM_BLE_ADVERT_TYPE_FLAG, .p_data = &flag, .len = sizeof(flag)},
        {.advert_type = BTM_BLE_ADVERT_TYPE_SERVICE_DATA,
         .p_data = (uint8_t *)&volume_uuid,
         .len = sizeof(volume_uuid)},
        {.advert_type = BTM_BLE_ADVERT_TYPE_SERVICE_DATA, .p_data = (uint8_t *)&ascs_data, .len = sizeof(ascs_data)},
        {.advert_type = BTM_BLE_ADVERT_TYPE_SERVICE_DATA, .p_data = (uint8_t *)&bscan_uuid, .len = sizeof(bscan_uuid)},
#if TMAS_ENABLED
        {.advert_type = BTM_BLE_ADVERT_TYPE_SERVICE_DATA, .p_data = tmap_data, .len = sizeof(tmap_data)},
#endif
        {.advert_type = BTM_BLE_ADVERT_TYPE_PSRI, .p_data = (uint8_t *)psri, .len = sizeof(ga_lib_csis_psri_t)},
        //.elem = {.advert_type = BTM_BLE_ADVERT_TYPE_SERVICE_DATA, .p_data = (uint8_t *)&aics_uuid, .len = sizeof(aics_uuid)},
        {.advert_type = BTM_BLE_ADVERT_TYPE_SERVICE_DATA, .p_data = gmap_data, .len = sizeof(gmap_data)},
    };
    wiced_bt_ble_advert_elem_t *p_element = adv_data_elems;
    uint16_t adv_len = 0;

    WICED_BT_TRACE("[%s] 0x%x\n", __FUNCTION__, adv_opt);

    //Swift pair LTV
    if ((adv_state == ADV_STATE_SWIFT_PAIR_HIGH_DUTY_CYCLE) || (adv_state == ADV_STATE_SWIFT_PAIR_LOW_DUTY_CYCLE))
    {
        const uint8_t swift_pair_LTV[5 + sizeof(DEVICE_NAME)] = {0x06, 0x00, 0x03, 0x00, 0x80};
        WICED_MEMCPY(&swift_pair_LTV[5], lehs_cfg_settings.device_name, sizeof(DEVICE_NAME));
        wiced_bt_ble_advert_elem_t elem = {.advert_type = BTM_BLE_ADVERT_TYPE_MANUFACTURER,
                                           .p_data = (uint8_t *)swift_pair_LTV,
                                           .len = sizeof(swift_pair_LTV)};
        adv_len += wiced_ble_adv_data_build(p_ctx, &elem);
    }
    else if (adv_opt & 32)
    {
        wiced_bt_ble_advert_elem_t elem = {.advert_type = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE,
                                           .p_data = lehs_cfg_settings.device_name,
                                           .len = sizeof(DEVICE_NAME)};
        adv_len += wiced_ble_adv_data_build(p_ctx, &elem);
    }

    for (int i = 0; i < sizeof(adv_data_elems) / sizeof(adv_data_elems[0]); i++)
    {
        if (adv_opt & (1 << i))
        {
            adv_len += wiced_ble_adv_data_build(p_ctx, p_element);
        }
        p_element++;
    }

    WICED_BT_TRACE("[%s] [len %d %d]\n", __FUNCTION__, adv_len, p_ctx->offset);

    return p_ctx->offset;
}

void lehs_gatt_start_stop_adv(uint32_t b_start, adv_state_t adv_state)
{
    wiced_ble_ext_adv_duration_config_t duration_cfg = {.adv_handle = UNICAST_SINK_EXT_ADV_HANDLE};
    uint8_t addr_type = (lehs_cfg_settings.p_ble_cfg->rpa_refresh_timeout) ? BLE_ADDR_PUBLIC_ID : BLE_ADDR_PUBLIC;
    wiced_ble_ext_adv_params_t params = {.event_properties = WICED_BLE_EXT_ADV_EVENT_PROPERTY_CONNECTABLE_ADV,
                                         .primary_adv_channel_map =
                                             (BTM_BLE_ADVERT_CHNL_37 | BTM_BLE_ADVERT_CHNL_38 | BTM_BLE_ADVERT_CHNL_39),
                                         .own_addr_type = addr_type,
                                         .peer_addr_type = 0,
                                         .adv_filter_policy = BTM_BLE_ADV_POLICY_ACCEPT_CONN_AND_SCAN,
                                         .primary_adv_phy = WICED_BLE_EXT_ADV_PHY_1M,
                                         .secondary_adv_phy = WICED_BLE_EXT_ADV_PHY_1M,
                                         .adv_sid = 1};

    lehs_get_adv_duty_cycle_params(adv_state, &params, &duration_cfg);
    WICED_BT_TRACE("[%s] adv_state : %d n", __FUNCTION__, adv_state);

    if (b_start)
    {
        uint8_t adv_data[70];
        wiced_bt_adv_ctx_t ctx = {.p_adv = adv_data, .adv_len = sizeof(adv_data)};
        int adv_data_len = lehs_gatt_get_adv_data(adv_state, &ctx);

        if (adv_data_len < 31)
        {
            params.event_properties = WICED_BLE_EXT_ADV_EVENT_PROPERTY_CONNECTABLE_ADV |
                                      WICED_BLE_EXT_ADV_EVENT_PROPERTY_SCANNABLE_ADV |
                                      WICED_BLE_EXT_ADV_EVENT_PROPERTY_LEGACY_ADV;
        }

        // Set adv data in LTV format
        wiced_bt_dev_status_t sts = wiced_ble_ext_adv_set_params(UNICAST_SINK_EXT_ADV_HANDLE, &params);
        if (WICED_SUCCESS == sts)
        {
            sts = wiced_ble_ext_adv_set_adv_data(UNICAST_SINK_EXT_ADV_HANDLE, ctx.offset, ctx.p_adv);
        }
    }

    // Start/Stop adv
    wiced_ble_ext_adv_enable(b_start, 1, &duration_cfg);

    app_rpc_send_advertisement_state(adv_state);
}

const adv_state_t swift_pair_next_state[] = {ADV_STATE_SWIFT_PAIR_HIGH_DUTY_CYCLE,
                                             ADV_STATE_SWIFT_PAIR_LOW_DUTY_CYCLE,
                                             ADV_STATE_REGULAR_ADV,
                                             ADV_STATE_IDLE,
                                             ADV_STATE_IDLE};
const adv_state_t regular_adv_next_state[] = {
    ADV_STATE_REGULAR_ADV, ADV_STATE_IDLE, ADV_STATE_IDLE, ADV_STATE_IDLE, ADV_STATE_IDLE};

adv_state_t lehs_move_to_next_adv_state(adv_state_t current, char *from)
{
    const adv_state_t *p_transition = regular_adv_next_state;

    if (g_lehs_gatt_cb.do_swift_pair)
    {
        p_transition = swift_pair_next_state;
    }

    WICED_BT_TRACE("[%s] event %d : state :%d next %d", __FUNCTION__, current, p_transition[current]);

    g_lehs_gatt_cb.adv_state = p_transition[current];

    if (g_lehs_gatt_cb.adv_state != ADV_STATE_IDLE)
    {
        lehs_gatt_start_stop_adv(1, g_lehs_gatt_cb.adv_state);
    }
    else
    {
        lehs_gatt_start_stop_adv(0, g_lehs_gatt_cb.adv_state);
    }

    return g_lehs_gatt_cb.adv_state;
}

/* Read the device and return the nvram id */
uint16_t lehs_read_device_from_nvram(lehs_clcb_t *p_clcb)
{
    lehs_nvram_paired_device_key_t *p_pdkeys = lehs_nvram_get_paired_device_key_info(p_clcb->identity_bd_address);
    if (p_pdkeys == NULL)
    {
        WICED_BT_TRACE("[%s] No paired device found for %B", __FUNCTION__, p_clcb->identity_bd_address);
        return 0;
    }
    lehs_print_nvram_data("read_from_nvram", p_pdkeys);

    WICED_MEMCPY(&p_clcb->peer_profiles, &p_pdkeys->peer_profiles, sizeof(lehs_peer_profiles_t));
    WICED_MEMCPY(&p_clcb->db_hash, &p_pdkeys->db_hash, sizeof(wiced_bt_db_hash_t));
    WICED_MEMCPY(p_clcb->cccd_bits, p_pdkeys->stored_cccd_bits, sizeof(p_clcb->cccd_bits));
    return lehs_nvram_get_nvram_id(p_pdkeys);
}


void lehs_gatt_handle_connection(wiced_bt_gatt_connection_status_t *p_status)
{
    lehs_clcb_t *p_clcb =
        lehs_gatt_alloc_cb(p_status->bd_addr, p_status->addr_type, p_status->conn_id, p_status->link_role);
    /* Allocate GATT control block */
    if (!p_clcb)
    {
        // Assert !!
        return;
    }

    lehs_init_local_ase_data(p_clcb);
    lehs_read_device_from_nvram(p_clcb);

    /* Inform CC */
    app_rpc_send_connect_event(p_status);
    app_rpc_send_app_status(p_status->conn_id, p_status->bd_addr, HCI_CONTROL_MISC_APP_STATE_CONNECTED, 0);
}

void lehs_gatt_handle_disconnection(wiced_bt_gatt_connection_status_t *p_sts)
{

    WICED_BT_TRACE("[%s] disconnected from [%B] reason %d\n", __FUNCTION__, p_sts->bd_addr, p_sts->reason);

    // if device is paired, save the cccd and remote handle data to NVRAM
    lehs_clcb_t *p_clcb = lehs_gatt_get_clcb(p_sts->bd_addr);
    if (p_clcb)
    {
        lehs_save_device_data_to_nvram(p_clcb);
        app_rpc_send_app_status(p_sts->conn_id, p_clcb->identity_bd_address, HCI_CONTROL_MISC_APP_STATE_DISCONNECTED, 0);
        lehs_bass_handle_acl_disconnection(p_clcb->conn_id);
    }
    else
    {
        app_rpc_send_app_status(p_sts->conn_id, p_sts->bd_addr, HCI_CONTROL_MISC_APP_STATE_DISCONNECTED, 0);
    }

    lehs_gatt_free_cb(p_sts->bd_addr);
    app_rpc_send_disconnect_evt(p_sts);

    if (p_sts->reason == HCI_ERR_CONNECTION_TOUT)
    {
        WICED_BT_TRACE("[%s] Restarting adv due to connection timeout\n", __FUNCTION__, p_sts->bd_addr);
        lehs_move_to_next_adv_state(g_lehs_gatt_cb.adv_state, "on_disconnect");
    }
}

const gatt_intf_cccd_map_t lehs_cccd_map[] = {{CCCD_MAP(GATT_GENERIC_ATTRIBUTE_SERVICE_CHANGED)},
    {CCCD_MAP(VCS_VOLUME_STATE)},
                                         {CCCD_MAP(VCS_VOLUME_FLAGS)},
                                         {CCCD_MAP(VOCS_OFFSET_STATE)},
                                         {CCCD_MAP(VOCS_AUDIO_LOCATION)},
                                         {CCCD_MAP(VOCS_AUDIO_OUTPUT_DESCRIPTION)},
#if 0
                                         {CCCD_MAP(VCS_AICS_INPUT_STATE)},
                                         {CCCD_MAP(VCS_AICS_INPUT_STATUS)},
                                         {CCCD_MAP(VCS_AICS_AUDIO_INPUT_DESCRIPTION)},
#endif
                                         {CCCD_MAP(PACS_SINK_PAC)},
                                         {CCCD_MAP(PACS_SINK_AUDIO_LOCATIONS)},
                                         {CCCD_MAP(PACS_SOURCE_PAC)},
                                         {CCCD_MAP(PACS_SOURCE_AUDIO_LOCATIONS)},
                                         {CCCD_MAP(PACS_AVAILABLE_AUDIO_CONTEXTS)},
                                         {CCCD_MAP(PACS_SUPPORTED_AUDIO_CONTEXTS)},
                                         {CCCD_MAP(ASCS_ASE_SINK)},
                                         {CCCD_MAP(ASCS_ASE_SOURCE)},
                                         {CCCD_MAP(ASCS_ASE_SINK_2)},
                                         {CCCD_MAP(ASCS_ASE_CONTROL_POINT)},
                                         {CCCD_MAP(BASS_BROADCAST_RECEIVE_STATE_1)},
                                         //{CCCD_MAP(BASS_BROADCAST_2_RECEIVE_STATE)},
                                         {CCCD_MAP(CSIS_SIRK)},
                                         {CCCD_MAP(CSIS_SIZE)},
                                         {CCCD_MAP(CSIS_LOCK)},
                                         {CCCD_MAP(MICS_MUTE_STATE)},
                                         {CCCD_MAP(MICS_AICS_INPUT_STATE)},
                                         {CCCD_MAP(MICS_AICS_INPUT_STATUS)},
                                         {CCCD_MAP(MICS_AICS_AUDIO_INPUT_DESCRIPTION)},
                                         {CCCD_MAP(HAS_HEARIND_AID_FEATUES)},
                                         {CCCD_MAP(HAS_HEARING_AID_PRESET_CONTROL_POINT)},
                                         {CCCD_MAP(HAS_ACTIVE_PRESET_INDEX)}};



/* C99-compatible compile-time assertion */
#define compile_time_assert(expr, line) typedef char compile_time_assertion_##line##_fail[(expr) ? 1 : -1]

compile_time_assert(sizeof(lehs_cccd_map) / sizeof(lehs_cccd_map[0]) == LEHS_MAX_CCCD_TO_STORE, __LINE__);

int lehs_get_cccd_index(uint16_t handle, uint8_t type)
{
    int index = sizeof(lehs_cccd_map) / sizeof(lehs_cccd_map[0]);
    const gatt_intf_cccd_map_t *p_map = lehs_cccd_map;
    while (index--)
    {
        if (p_map->handle[type] == handle)
        {
            return index = (p_map - lehs_cccd_map);
        }
        p_map++;
    }
    return -1;
}

uint16_t lehs_get_clcb_cccd(lehs_clcb_t *p_clcb, uint16_t handle, gatt_intf_cccd_map_handle_type_t type)
{
    int index = lehs_get_cccd_index(handle, type);
    if (index == -1)
    {
        return 0;
    }

    int byte_index = (index * 2) / 32;
    int byte_offset = (index * 2) % 32;

    WICED_BT_TRACE("[%s] handle %d index %d byte_index %d byte_offset %d",
                   __FUNCTION__,
                   handle,
                   index,
                   byte_index,
                   byte_offset);

    WICED_BT_TRACE("[%s] handle %d cccd[%d] = (0x%x >> %d) & 3 = 0x%x",
                   __FUNCTION__,
                   handle,
                   byte_index,
                   p_clcb->cccd_bits[byte_index],
                   byte_offset,
                   (p_clcb->cccd_bits[byte_index] >> byte_offset) & 0x3);

    return (p_clcb->cccd_bits[byte_index] >> byte_offset) & 0x3;
}

void lehs_set_clcb_cccd(lehs_clcb_t *p_clcb, uint16_t handle, uint16_t value)
{
    int index = lehs_get_cccd_index(handle, GATT_INTF_DESCRIPTOR_HANDLE_INDEX);
    if (index == -1)
    {
        return;
    }
    int byte_index, byte_offset;

    byte_index = (index * 2) / 32;
    byte_offset = (index * 2) % 32;

    WICED_BT_TRACE("[%s] handle %d index %d byte_index %d byte_offset %d",
                   __FUNCTION__,
                   handle,
                   index,
                   byte_index,
                   byte_offset);
    {
        uint32_t current_value = p_clcb->cccd_bits[byte_index];
        uint32_t final_value = (current_value & ~(0x3 << byte_offset)) | ((value & 0x3) << byte_offset);
        WICED_BT_TRACE("[%s] handle %d cccd[%d] = (0x%08x | %d << %d) = 0x%08x",
                       __FUNCTION__,
                       handle,
                       byte_index,
                       current_value,
                       value & 0x3,
                       byte_offset,
                       final_value);
    }

    p_clcb->cccd_bits[byte_index] &= ~(0x3 << byte_offset);
    p_clcb->cccd_bits[byte_index] |= (value & 0x3) << byte_offset;
    WICED_BT_TRACE("[%s] handle %d cccd[%d] = 0x%08x",
                   __FUNCTION__,
                   handle,
                   byte_index,
                   p_clcb->cccd_bits[byte_index]);
    return;
}

uint16_t lehs_handle_get_cccd_value_cb(uint16_t conn_id, uint16_t handle)
{
    lehs_clcb_t *p_clcb = lehs_gatt_get_clcb_by_conn_id(conn_id);
    if (!p_clcb)
        return 0;
    return lehs_get_clcb_cccd(p_clcb, handle, GATT_INTF_VALUE_HANDLE_INDEX);
}


wiced_bt_gatt_status_t lehs_generic_attribute_handle_read_request(lehs_clcb_t *p_clcb,
                                                                  uint16_t handle,
                                                                  uint8_t *p_data,
                                                                  uint16_t *p_len)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;
    uint8_t *p_data_start = p_data;
    switch (handle)
    {
    case HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_DB_HASH_VALUE:
    {
        ARRAY_TO_STREAM(p_data, g_lehs_gatt_cb.db_hash, sizeof(g_lehs_gatt_cb.db_hash));
    }
    break;
    case HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_CLIENT_FEATURES_VALUE:
    {
        ARRAY_TO_STREAM(p_data, p_clcb->csf, sizeof(p_clcb->csf));
    }
    break;
    case HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_SERVER_FEATURES_VALUE:
    {
        UINT8_TO_STREAM(p_data, 0); // No server features supported
    }
    break;
    default:
        status = WICED_BT_GATT_READ_NOT_PERMIT;
        break;
    }
    *p_len = p_data - p_data_start;
    return status;
}


wiced_bt_gatt_status_t app_handle_gatt_read_request(lehs_clcb_t *p_clcb,
                                                    uint16_t handle,
                                                    uint8_t *p_data,
                                                    uint16_t *p_len)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;
    int cccd_index = lehs_get_cccd_index(handle, GATT_INTF_DESCRIPTOR_HANDLE_INDEX);
    const uint8_t *p_data_start = p_data;
    lehs_local_service_data_t *p_local = &g_lehs_gatt_cb.local_service_data;

    if (cccd_index != -1)
    {
        uint16_t cccd = lehs_get_clcb_cccd(p_clcb, handle, GATT_INTF_DESCRIPTOR_HANDLE_INDEX);
        UINT16_TO_STREAM(p_data, cccd);
        *p_len = p_data - p_data_start;
        return WICED_BT_GATT_SUCCESS;
    }

    if (handle >= HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_CHANGED_VALUE &&
        handle <= HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_SERVER_FEATURES_VALUE)
    {
        return lehs_generic_attribute_handle_read_request(p_clcb, handle, p_data, p_len);
    }

    switch (handle)
    {
    case HDLC_VCS_VOLUME_STATE_VALUE:
    {
        ga_lib_vcs_volume_state_t *p_vcs_state = &p_local->vcs.state;
        UINT8_TO_STREAM(p_data, p_vcs_state->volume_setting);
        UINT8_TO_STREAM(p_data, p_vcs_state->mute_state);
        UINT8_TO_STREAM(p_data, p_vcs_state->change_counter);
    }
    break;
    case HDLC_VCS_VOLUME_FLAGS_VALUE:
    {
        UINT8_TO_STREAM(p_data, p_local->vcs.flag);
    }
    break;
    case HDLC_VOCS_OFFSET_STATE_VALUE:
    {
        UINT16_TO_STREAM(p_data, p_local->vocs.volume_offset);
        UINT8_TO_STREAM(p_data, p_local->vocs.change_counter);
    }
    break;
    case HDLC_VOCS_AUDIO_LOCATION_VALUE:
    {
        UINT32_TO_STREAM(p_data, p_local->vocs.audio_location);
    }
    break;
    case HDLC_VOCS_AUDIO_OUTPUT_DESCRIPTION_VALUE:
    {
        int len = strlen(p_local->vocs.vocs_description);
        if (len > *p_len)
        {
            len = (*p_len) - 1;
        }
        strncpy((char *)p_data, p_local->vocs.vocs_description, len);
        p_data += len;
    }
    break;
#if 0
    case HDLC_VCS_AICS_INPUT_STATE_VALUE:
    {

    }
    break;
    case HDLC_VCS_AICS_GAIN_SETTING_ATTR_VALUE:
        break;
    case HDLC_VCS_AICS_INPUT_TYPE_VALUE:
        break;
    case HDLC_VCS_AICS_INPUT_STATUS_VALUE:
        break;
    case HDLC_VCS_AICS_AUDIO_INPUT_DESCRIPTION_VALUE:
        break;
#endif
    case HDLC_PACS_SINK_PAC_VALUE:
    {
        ga_lib_pacs_char_data_t *p_pcd = &g_lehs_pacs_app_data.snk_pac_list;
        int num_records_written = 0;
        p_data += ga_lib_pacs_build_read_rsp_sink_src_cap_pkt(p_pcd->num_records,
                                                              p_pcd->p_records,
                                                              p_data,
                                                              *p_len,
                                                              &num_records_written);
    }
    break;
    case HDLC_PACS_SINK_AUDIO_LOCATIONS_VALUE:
    {
        UINT32_TO_STREAM(p_data, g_lehs_pacs_app_data.snk_audio_location);
    }
    break;
    case HDLC_PACS_SOURCE_PAC_VALUE:
    {
        ga_lib_pacs_char_data_t *p_pcd = &g_lehs_pacs_app_data.src_pac_list;
        int num_records_written = 0;
        p_data += ga_lib_pacs_build_read_rsp_sink_src_cap_pkt(p_pcd->num_records,
                                                              p_pcd->p_records,
                                                              p_data,
                                                              *p_len,
                                                              &num_records_written);
    }
    break;
    case HDLC_PACS_SOURCE_AUDIO_LOCATIONS_VALUE:
    {
        ga_lib_pacs_data_t *p_pacs = &g_lehs_pacs_app_data;
        UINT32_TO_STREAM(p_data, p_pacs->src_audio_location);
    }
    break;

    case HDLC_PACS_AVAILABLE_AUDIO_CONTEXTS_VALUE:
    {
        ga_lib_pacs_data_t *p_pacs = &g_lehs_pacs_app_data;
        UINT16_TO_STREAM(p_data, p_pacs->available.snk_contexts);
        UINT16_TO_STREAM(p_data, p_pacs->available.src_contexts);
    }
    break;
    case HDLC_PACS_SUPPORTED_AUDIO_CONTEXTS_VALUE:
    {
        ga_lib_pacs_data_t *p_pacs = &g_lehs_pacs_app_data;
        UINT16_TO_STREAM(p_data, p_pacs->supported.snk_contexts);
        UINT16_TO_STREAM(p_data, p_pacs->supported.src_contexts);
    }
    break;
    case HDLC_ASCS_ASE_SINK_VALUE:
    {
        p_data += ga_lib_ascs_build_ase_response(&p_clcb->local_ase_data[LEHS_ASE_INDEX_SINK_1].ase, p_data, *p_len);
    }
    break;
    case HDLC_ASCS_ASE_SOURCE_VALUE:
    {
        p_data += ga_lib_ascs_build_ase_response(&p_clcb->local_ase_data[LEHS_ASE_INDEX_SOURCE_1].ase, p_data, *p_len);
    }
    break;
    case HDLC_ASCS_ASE_SINK_2_VALUE:
    {
        p_data += ga_lib_ascs_build_ase_response(&p_clcb->local_ase_data[LEHS_ASE_INDEX_SINK_2].ase, p_data, *p_len);
    }
    break;
    case HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE:
    {
        lehs_bass_data_t *p_bass_data = &p_local->bass.bass_data[LEHS_BROADCAST_RCV_STATE_0];
        p_data += ga_lib_bass_build_receive_state_char_data(&p_bass_data->recv_state, p_data, *p_len);
    }
    break;
    case HDLC_CSIS_SIRK_VALUE:
    {
        lehs_csis_data_t *p_csis = &p_local->csis;
        ga_lib_csis_sirk_t sirk;

        if (p_csis->sirk_data.is_oob)
        {
            return GA_LIB_CSIS_ERROR_OOB_SIRK_ONLY;
        }

        UINT8_TO_STREAM(p_data, p_csis->sirk_data.sirk_type);

        if (p_csis->sirk_data.sirk_type == GA_LIB_CSIS_SIRK_ENCR)
        {
            lehs_encrypt_sirk(p_clcb->conn_bda, &sirk);
        }
        else
        {
            memcpy(&sirk, &p_csis->sirk_data.sirk, sizeof(ga_lib_csis_sirk_t));
        }

        REVERSE_ARRAY_TO_STREAM(p_data, sirk, GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN);
    }
    break;
    case HDLC_CSIS_SIZE_VALUE:
    {
        lehs_csis_data_t *p_csis = &p_local->csis;
        if (p_csis->size == 0)
        {
            p_csis->size = 1; // Size of the set shall be at least 1
        }
        UINT8_TO_STREAM(p_data, p_csis->size);
    }
    break;
    case HDLC_CSIS_LOCK_VALUE:
    {
        lehs_csis_data_t *p_csis = &p_local->csis;
        if (p_csis->lock == 0)
        {
            p_csis->lock = 1; // Set shall be unlocked when size is set
        }
        UINT8_TO_STREAM(p_data, p_csis->lock);
    }
    break;
    case HDLC_CSIS_RANK_VALUE:
    {
        lehs_csis_data_t *p_csis = &p_local->csis;
        if (p_csis->rank == 0)
        {
            p_csis->rank = 1; // Rank of the set shall be at least 1
        }
        UINT8_TO_STREAM(p_data, p_csis->rank);
    }
    break;
    case HDLC_MICS_MUTE_STATE_VALUE:
    {
        UINT8_TO_STREAM(p_data, p_local->mics.mute_state);
    }
    break;
    case HDLC_MICS_AICS_INPUT_STATE_VALUE:
    {
        lehs_aics_t *p_aics = &p_local->mics_aics[0];
        UINT8_TO_STREAM(p_data, p_aics->input_state.gain_setting);
        UINT8_TO_STREAM(p_data, p_aics->input_state.mute_mode);
        UINT8_TO_STREAM(p_data, p_aics->input_state.gain_mode);
        UINT8_TO_STREAM(p_data, p_aics->input_state.change_counter);
    }
    break;
    case HDLC_MICS_AICS_GAIN_SETTING_ATTR_VALUE:
    {
        lehs_aics_t *p_aics = &p_local->mics_aics[0];
        UINT8_TO_STREAM(p_data, p_aics->gain_setting.gain_setting_units);
        UINT8_TO_STREAM(p_data, p_aics->gain_setting.min_gain_setting);
        UINT8_TO_STREAM(p_data, p_aics->gain_setting.max_gain_setting);
    }
    break;
    case HDLC_MICS_AICS_INPUT_TYPE_VALUE:
    {
        lehs_aics_t *p_aics = &p_local->mics_aics[0];
        UINT8_TO_STREAM(p_data, p_aics->input_type);
    }
    break;
    case HDLC_MICS_AICS_INPUT_STATUS_VALUE:
    {
        lehs_aics_t *p_aics = &p_local->mics_aics[0];
        UINT8_TO_STREAM(p_data, p_aics->input_status);
    }
    break;
    case HDLC_MICS_AICS_AUDIO_INPUT_DESCRIPTION_VALUE:
    {
        lehs_aics_t *p_aics = &p_local->mics_aics[0];
        int len_to_copy = p_aics->description_len;
        if (len_to_copy > *p_len)
        {
            len_to_copy = (*p_len) - 1;
        }
        ARRAY_TO_STREAM(p_data, p_aics->description, len_to_copy);
        *p_len = len_to_copy;
    }
    break;
    case HDLC_HAS_HEARIND_AID_FEATUES_VALUE:
    {
        lehs_has_data_t *p_has = &p_local->has;
        UINT8_TO_STREAM(p_data, p_has->hearing_aid_features);
    }
    break;
    case HDLC_HAS_ACTIVE_PRESET_INDEX_VALUE:
    {
        lehs_has_data_t *p_has = &p_local->has;
        UINT8_TO_STREAM(p_data, p_has->active_preset_index);
    }
    break;
    case HDLC_TMAS_TELEPHONE_MEDIA_AUDIO_PROFILE_ROLE_ROLE_VALUE:
    {
        uint16_t tmap_role =
            TMAP_ROLE_CALL_TERMINAL | TMAP_ROLE_UNICAST_MEDIA_RECEIVER | TMAP_ROLE_BROADCAST_MEDIA_RECEIVER;
        UINT16_TO_STREAM(p_data, tmap_role);
    }
    break;
    case HDLS_GMAP_ROLE_VALUE:
    {
        UINT8_TO_STREAM(p_data, p_local->gmap.gmap_role);
    }
    break;
    case HDLS_GMAP_UGT_FEATURES_VALUE:
    {
        UINT8_TO_STREAM(p_data, p_local->gmap.ugt_features);
    }
    break;
    case HDLS_GMAP_BGR_FEATURES_VALUE:
    {
        UINT8_TO_STREAM(p_data, p_local->gmap.bgr_features);
    }
    break;
    default:
        status = WICED_BT_GATT_INVALID_HANDLE;
        break;
    }

    *p_len = p_data - p_data_start;

    return status;
}

uint16_t lehs_get_cccd_value(uint16_t conn_id, uint16_t char_handle)
{
    lehs_clcb_t *p_clcb = lehs_gatt_get_clcb_by_conn_id(conn_id);
    if (!p_clcb)
        return 0;
    return lehs_get_clcb_cccd(p_clcb, char_handle, GATT_INTF_VALUE_HANDLE_INDEX);
}

wiced_bt_gatt_status_t app_handle_gatt_write_request(uint16_t conn_id,
                                                     lehs_clcb_t *p_clcb,
                                                     wiced_bt_gatt_write_req_t *p_write_req)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;
    int cccd_index = lehs_get_cccd_index(p_write_req->handle, GATT_INTF_DESCRIPTOR_HANDLE_INDEX);
    uint8_t *p_data = p_write_req->p_val;
    const uint8_t *p_data_start = p_write_req->p_val;
    lehs_local_service_data_t *p_local = &g_lehs_gatt_cb.local_service_data;
    int len_to_write = p_write_req->val_len;

    if (cccd_index != -1)
    {
        if (len_to_write != 2)
        {
            return WICED_BT_GATT_INVALID_ATTR_LEN;
        }

        uint16_t cccd;
        STREAM_TO_UINT16(cccd, p_data);
        lehs_set_clcb_cccd(p_clcb, p_write_req->handle, cccd);
        return WICED_BT_GATT_SUCCESS;
    }

    switch (p_write_req->handle)
    {
    case HDLC_VCS_VOLUME_CONTROL_POINT_VALUE:
    {
        status = lehs_handle_vcs_cp_write(conn_id, p_data, len_to_write);
    }
    break;
    case HDLC_VOCS_AUDIO_LOCATION_VALUE:
    {
        if (len_to_write != 4)
        {
            return WICED_BT_GATT_INVALID_ATTR_LEN;
        }
        STREAM_TO_UINT32(p_local->vocs.audio_location, p_data);
        ga_lib_vocs_notify_audio_location(conn_id, HDLC_VOCS_AUDIO_LOCATION_VALUE, p_local->vocs.audio_location);
    }
    break;
    case HDLC_VOCS_VOLUME_OFFSET_CONTROL_POINT_VALUE:
    {
        lehs_vocs_t *p_vocs = &p_local->vocs;
        uint8_t opcode, change_counter;
        // Format
        // Byte 0 ==> Opcode
        // Byte 1 ==> change Counter
        // Byte 2 ==> offset value
        // Byte 3 ==> offset value

        // Check opcode
        STREAM_TO_UINT8(opcode, p_data);
        if (opcode != VOLUME_CONTROL_OFFSET_OPCODE)
            return GA_LIB_VOCS_ERROR_OPCODE_NOT_SUPPORTED;

        if (len_to_write != VOCS_OFFSET_CONTROL_POINT_LEN)
            return WICED_BT_GATT_INVALID_ATTR_LEN;

        // check change counter
        STREAM_TO_UINT8(change_counter, p_data);
        if (change_counter != p_vocs->change_counter)
            return GA_LIB_VOCS_ERROR_INVALID_CHANGE_COUNTER;

        uint16_t volume_offset;
        STREAM_TO_UINT16(volume_offset, p_data);

        if (volume_offset < VOLUME_OFFSET_MIN_VALUE || volume_offset > VOLUME_OFFSET_MAX_VALUE)
            return GA_LIB_VOCS_ERROR_VALUE_OUT_OF_RANGE;

        if (volume_offset != p_vocs->volume_offset)
        {
            //Increment change counter
            if (p_vocs->change_counter == 0xFF)
                p_vocs->change_counter = 0;
            else
                p_vocs->change_counter++;

            p_vocs->volume_offset = volume_offset;

            ga_lib_vocs_notify_volume_offset(conn_id,
                                             HDLC_VOCS_OFFSET_STATE_VALUE,
                                             p_vocs->volume_offset,
                                             p_vocs->change_counter);
        }
    }
    break;
    case HDLC_VOCS_AUDIO_OUTPUT_DESCRIPTION_VALUE:
    {
        lehs_vocs_t *p_vocs = &p_local->vocs;
        int len_to_copy = len_to_write;
        if ((len_to_copy > (sizeof(p_vocs->vocs_description) - 1)) || (len_to_copy <= 0))
        {
            return WICED_BT_GATT_INVALID_ATTR_LEN;
        }

        if (strncmp(p_vocs->vocs_description, (char *)p_data_start, len_to_copy) != 0)
        {
            memset(p_vocs->vocs_description, 0, sizeof(p_vocs->vocs_description));
            memcpy(p_vocs->vocs_description, p_data_start, len_to_copy);
            ga_lib_vocs_notify_audio_description(conn_id,
                                                 HDLC_VOCS_AUDIO_OUTPUT_DESCRIPTION_VALUE,
                                                 (const char *)p_vocs->vocs_description,
                                                 strlen(p_vocs->vocs_description) + 1);
        }
    }
    break;
    case HDLC_PACS_SINK_AUDIO_LOCATIONS_VALUE:
    {
        if (len_to_write != 4)
        {
            return WICED_BT_GATT_INVALID_ATTR_LEN;
        }
        STREAM_TO_UINT32(g_lehs_pacs_app_data.snk_audio_location, p_data);
    }
    break;
    case HDLC_PACS_SOURCE_AUDIO_LOCATIONS_VALUE:
    {
        if (len_to_write != 4)
        {
            return WICED_BT_GATT_INVALID_ATTR_LEN;
        }
        STREAM_TO_UINT32(g_lehs_pacs_app_data.src_audio_location, p_data);
    }
    break;
    case HDLC_ASCS_ASE_CONTROL_POINT_VALUE:
    {
        status = lehs_ascs_handle_write_req_evt(conn_id, p_clcb, p_data, len_to_write);
    }
    break;
    case HDLC_BASS_BROADCAST_AUDIO_SCAN_CONTROL_POINT_VALUE:
    {
        status = lehs_bass_handle_write_req_evt(conn_id, p_clcb, p_data, len_to_write);
    }
    break;
    case HDLC_CSIS_LOCK_VALUE:
    {
        status = lehs_csis_handle_write_req_evt(conn_id, p_clcb, p_data, len_to_write);
    }
    break;
    case HDLC_MICS_MUTE_STATE_VALUE:
    {
        lehs_mics_t *p_mics = &p_local->mics;
        STREAM_TO_UINT8(p_mics->mute_state, p_data);
        audio_driver_set_mic_mute_state(p_mics->mute_state);
        le_audio_rpc_send_mics_mute_state(conn_id, p_mics->mute_state);
        ga_lib_mics_notify_mute_state(conn_id, HDLC_MICS_MUTE_STATE_VALUE, p_mics->mute_state);
    }
        break;
    case HDLC_MICS_AICS_AUDIO_INPUT_CONTROL_POINT_VALUE:
    {
        lehs_aics_t *p_aics = &p_local->mics_aics[0];
        uint8_t opcode;
        uint8_t change_counter;
        int is_auto_mute = 0;

        if (p_aics->input_state.gain_mode == GA_LIB_AICS_GAIN_MODE_AUTO ||
            p_aics->input_state.gain_mode == GA_LIB_AICS_GAIN_MODE_AUTO_ONLY)
        {
            is_auto_mute = 1;
        }

        if (len_to_write < 2)
            return WICED_BT_GATT_INVALID_ATTR_LEN;

        // Format
        // Byte 0 ==> Opcode
        // Byte 1 ==> change Counter
        // Byte 2 ==> Gain Setting (if OPCODE = GA_LIB_AICS_OPCODE_SET_GAIN_SETTINGS)
        STREAM_TO_UINT8(opcode, p_data);
        STREAM_TO_UINT8(change_counter, p_data);
        len_to_write -= 2;

        // Check opcode
        if (opcode > GA_LIB_AICS_OPCODE_SET_AUTO_GAIN_MODE)
            return (wiced_bt_gatt_status_t)GA_LIB_AICS_ERROR_OPCODE_NOT_SUPPORTED;

        // check change counter
        if (change_counter != p_aics->input_state.change_counter)
            return GA_LIB_AICS_ERROR_INVALID_CHANGE_COUNTER;

        switch (opcode)
        {
        case GA_LIB_AICS_OPCODE_SET_GAIN_SETTINGS:
        {
            uint8_t gain_setting;
            if (len_to_write != 1)
                return WICED_BT_GATT_INVALID_ATTR_LEN;

            STREAM_TO_INT8(gain_setting, p_data);

            if (is_auto_mute)
            {
                return GA_LIB_AICS_ERROR_VALUE_OUT_OF_RANGE;
            }
            if (p_aics->input_state.gain_mode == GA_LIB_AICS_GAIN_MODE_AUTO ||
                p_aics->input_state.gain_mode == GA_LIB_AICS_GAIN_MODE_AUTO_ONLY)
            {
                break;
            }

            if (p_aics->input_state.gain_setting == gain_setting)
            {
                return WICED_BT_GATT_SUCCESS;
            }

            p_aics->input_state.gain_setting = gain_setting;
            audio_driver_set_mic_gain(p_aics->input_state.gain_setting);
        }
        break;
        case GA_LIB_AICS_OPCODE_SET_UNMUTE:
        {
            if (p_aics->input_state.mute_mode == GA_LIB_AICS_MUTE_DISABLED)
            {
                status = GA_LIB_AICS_ERROR_MUTE_DISABLED;
                break;
            }

            if (p_aics->input_state.mute_mode == GA_LIB_AICS_UNMUTE)
            {
                return WICED_BT_GATT_SUCCESS;
            }
            p_aics->input_state.mute_mode = GA_LIB_AICS_UNMUTE;
            audio_driver_set_mic_mute_state(FALSE);
        }
        break;
        case GA_LIB_AICS_OPCODE_SET_MUTE:
        {
            if (p_aics->input_state.mute_mode == GA_LIB_AICS_MUTE_DISABLED)
            {
                status = GA_LIB_AICS_ERROR_MUTE_DISABLED;
                break;
            }

            if (p_aics->input_state.mute_mode == GA_LIB_AICS_MUTE)
            {
                return WICED_BT_GATT_SUCCESS;
            }
            p_aics->input_state.mute_mode = GA_LIB_AICS_MUTE;
            audio_driver_set_mic_mute_state(TRUE);
        }
        break;
        case GA_LIB_AICS_OPCODE_SET_MANUAL_GAIN_MODE:
        {
            if (is_auto_mute)
            {
                status = GA_LIB_AICS_GAIN_MODE_CHANGE_NOT_ALLOWED;
                break;
            }
            p_aics->input_state.gain_mode = GA_LIB_AICS_GAIN_MODE_MANUAL;
        }
        break;
        case GA_LIB_AICS_OPCODE_SET_AUTO_GAIN_MODE:
            if (is_auto_mute)
            {
                status = GA_LIB_AICS_GAIN_MODE_CHANGE_NOT_ALLOWED;
                break;
            }
            p_aics->input_state.gain_mode = GA_LIB_AICS_GAIN_MODE_AUTO;
            break;
        }

        if (status == WICED_BT_GATT_SUCCESS)
        {
            ga_lib_aics_notify_input_state(conn_id, HDLC_MICS_AICS_INPUT_STATE_VALUE, &p_aics->input_state);
            le_audio_rpc_send_mics_aics_input_state(conn_id, 0, &p_aics->input_state);
        }
    }
    break;
    case HDLC_MICS_AICS_AUDIO_INPUT_DESCRIPTION_VALUE:
    {
        lehs_aics_t *p_aics = &p_local->mics_aics[0];
        memset(&p_aics->description, 0, sizeof(p_aics->description));
        p_aics->description_len = MIN(sizeof(p_aics->description), len_to_write - 1);
        WICED_MEMCPY(p_aics->description, p_data, p_aics->description_len);
        le_audio_rpc_send_mics_aics_description(conn_id, 0, p_aics->description);
    }
    break;
    case HDLC_HAS_HEARING_AID_PRESET_CONTROL_POINT_VALUE:
    {
        uint8_t opcode;

        if (len_to_write < 1)
        {
            status = WICED_BT_GATT_INVALID_ATTR_LEN;
            break;
        }
        STREAM_TO_UINT8(opcode, p_data);
        switch (opcode)
        {
        case GA_LIB_HAS_OPCODE_READ_PRESETS_REQUEST:
        {
            uint8_t start_index, num_presets;
            STREAM_TO_UINT8(start_index, p_data);
            STREAM_TO_UINT8(num_presets, p_data);
            if ((num_presets == 0) || (start_index == 0))
            {
                status = WICED_BT_GATT_OUT_OF_RANGE;
            }

            status = lehs_has_handle_read_preset_record(conn_id, start_index, num_presets);
        }
        break;
        case GA_LIB_HAS_OPCODE_WRITE_PRESET_NAME:
        {
            uint8_t preset_index;

            STREAM_TO_UINT8(preset_index, p_data);

            status = lehs_has_handle_write_preset_name(conn_id, preset_index, p_data, len_to_write - 2);
        }
        break;
        case GA_LIB_HAS_OPCODE_SET_ACTIVE_PRESET:
        case GA_LIB_HAS_OPCODE_SET_ACTIVE_PRESET_SYNCHRONIZED_LOCALLY:
        {
            uint8_t preset_index;

            STREAM_TO_UINT8(preset_index, p_data);

            status = lehs_has_set_active_preset(conn_id, preset_index);
        }
        break;
        case GA_LIB_HAS_OPCODE_SET_NEXT_PRESET:
        {
            status = lehs_has_set_next_preset(conn_id, 1);
        }
        break;
        case GA_LIB_HAS_OPCODE_SET_PREVIOUS_PRESET:
        {
            status = lehs_has_set_next_preset(conn_id, -1);
        }
        break;
        case GA_LIB_HAS_OPCODE_SET_NEXT_PRESET_SYNCHRONIZED_LOCALLY:
        case GA_LIB_HAS_OPCODE_SET_PREVIOUS_PRESET_SYNCHRONIZED_LOCALLY:
        {
            status = lehs_has_set_preset_synchronization(opcode);
        }
        break;
        default:
            status = GA_LIB_HAS_ERROR_INVALID_OPCODE;
            break;
        }
    }
    break;
    default:
        status = WICED_BT_GATT_INVALID_HANDLE;
        break;
    }

    return status;
}

wiced_bt_gatt_status_t app_handle_gatt_read_by_type_request(lehs_clcb_t *p_clcb,
                                                            wiced_bt_gatt_attribute_request_t *p_att_req, uint16_t *p_err_handle)
{
    wiced_bt_gatt_read_by_type_t *p_read_req = &p_att_req->data.read_by_type;
    uint16_t attr_handle = p_read_req->s_handle;
    uint8_t value_len = 0;
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;
    uint8_t *p_data = gatt_intf_method_get_buffer(p_att_req->len_requested);
    int used = 0;

    *p_err_handle = p_read_req->s_handle;
    WICED_BT_TRACE("[%s] code 0x%x 0x%x - 0x%x uuid %A len %d",
                   __FUNCTION__,
                   p_att_req->opcode,
                   p_read_req->s_handle,
                   p_read_req->e_handle,
                   &p_read_req->uuid.uu,
                   p_read_req->uuid.len,
                   p_att_req->len_requested);
    if (!p_data)
    {
        return WICED_BT_GATT_NO_RESOURCES;
    }
    /* Read by type returns all attributes of the specified type, between the start and end handles */
    while (1)
    {
        uint16_t attr_len = 0;
        uint8_t attr[512]; // stack buffer set to the largest size

        attr_handle = wiced_bt_gatt_find_handle_by_type(attr_handle, p_read_req->e_handle, &p_read_req->uuid);

        if (attr_handle == 0)
        {
            WICED_BT_TRACE("[%s] attribute not found for 0x%04x, %d\n",
                           __FUNCTION__,
                           p_read_req->uuid.uu.uuid16,
                           p_read_req->uuid.len);
            break;
        }

        status = app_handle_gatt_read_request(p_clcb, attr_handle, attr, &attr_len);
        if (status != WICED_BT_GATT_SUCCESS)
        {
            gatt_intf_method_free_buffer(p_data);
            *p_err_handle = attr_handle;
            return status;
        }

        {
            int filled = wiced_bt_gatt_put_read_by_type_rsp_in_stream(p_data + used,
                                                                      p_att_req->len_requested - used,
                                                                      &value_len,
                                                                      attr_handle,
                                                                      attr_len,
                                                                      attr);
            if (filled == 0)
            {
                break;
            }
            used += filled;
        }

        /* Increment starting handle for next search to one past current */
        attr_handle++;
    }

    if (used)
    {
        status = wiced_bt_gatt_server_send_read_by_type_rsp(p_att_req->conn_id,
                                                            p_att_req->opcode,
                                                            value_len,
                                                            used,
                                                            p_data,
                                                            (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);
        if (WICED_BT_GATT_SUCCESS != status)
        {
            gatt_intf_method_free_buffer(p_data);
        }
    }
    else
    {
        status = WICED_BT_GATT_ATTRIBUTE_NOT_FOUND;
        *p_err_handle = p_read_req->s_handle;
        gatt_intf_method_free_buffer(p_data);
    }

    return status;
}

wiced_bt_gatt_status_t app_handle_gatt_attribute_request(wiced_bt_gatt_attribute_request_t *p_att_req, uint16_t *p_err_handle)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;
    lehs_clcb_t *p_clcb = lehs_gatt_get_clcb_by_conn_id(p_att_req->conn_id);
    if (!p_clcb)
    {
        return WICED_BT_GATT_ERROR;
    }

    *p_err_handle = 0;
    switch (p_att_req->opcode)
    {
    case GATT_REQ_MTU:
        /* Exchange MTU Request */
        {
            int my_mtu = MIN(p_att_req->data.remote_mtu, lehs_ble_cfg.ble_max_rx_pdu_size);
            wiced_bt_gatt_server_send_mtu_rsp(p_att_req->conn_id, p_att_req->data.remote_mtu, my_mtu);
            lehs_set_gatt_state_mtu_exchanged(p_clcb);
        }
        break;
    case GATT_REQ_READ:
    case GATT_REQ_READ_BLOB:
    {
        uint8_t buf[512];
        /* Read Request and Read Blob Request are handled in the common gatt handler after invoking
         * the profile specific handler
         */
        uint16_t read_len = sizeof(buf);
        status = app_handle_gatt_read_request(p_clcb, p_att_req->data.read_req.handle, buf, &read_len);

        *p_err_handle = p_att_req->data.read_req.handle;
        if (status != WICED_BT_GATT_SUCCESS)
        {
            break;
        }
        gatt_intf_send_read_response(p_att_req, status, buf, read_len);
    }
    break;
    case GATT_REQ_WRITE:
    case GATT_CMD_WRITE:
    {
        status = app_handle_gatt_write_request(p_att_req->conn_id, p_clcb, &p_att_req->data.write_req);
        *p_err_handle = p_att_req->data.write_req.handle;
        if (status == WICED_BT_GATT_SUCCESS && (p_att_req->opcode == GATT_REQ_WRITE))
        {
            wiced_bt_gatt_server_send_write_rsp(p_att_req->conn_id,
                                                p_att_req->opcode,
                                                p_att_req->data.write_req.handle);
        }
    }
    break;
    case GATT_HANDLE_VALUE_IND:
    {
        // Handle Value Indication is handled in the common gatt handler after invoking the profile specific handler
        // Send queued packets if any
    }
    break;
    case GATT_REQ_READ_BY_TYPE:
    {

        // Read By Type Request is handled in the common gatt handler after invoking the profile specific handler
        status = app_handle_gatt_read_by_type_request(p_clcb, p_att_req, p_err_handle);
    }
    break;
    default:
        break;
    }
    return status;
}

void lehs_app_sec_bond(TIMER_PARAM_TYPE arg)
{
    lehs_clcb_t *p_clcb = (lehs_clcb_t *)arg;
    WICED_BT_TRACE("[%s] for %B\n", __FUNCTION__, p_clcb->conn_bda);
    wiced_bt_device_link_keys_t dev_link_keys;
    memcpy(dev_link_keys.bd_addr, p_clcb->conn_bda, BD_ADDR_LEN);
    if (lehs_nvram_read_keys(&dev_link_keys))
    {
        wiced_bt_ble_sec_action_type_t encryption_type = BTM_BLE_SEC_ENCRYPT;
        wiced_bt_dev_set_encryption(p_clcb->conn_bda, BT_TRANSPORT_LE, &encryption_type);
    }
    else
    {
        wiced_bt_dev_sec_bond(p_clcb->conn_bda, p_clcb->conn_addr_type, BT_TRANSPORT_LE, 0, NULL);
    }
}

void app_handle_read_complete(uint16_t conn_id,
                              lehs_clcb_t *p_clcb,
                              wiced_bt_gatt_status_t status,
                              wiced_bt_gatt_data_t *p_gatt_data)
{
    lehs_peer_profiles_t *p_peer = &p_clcb->peer_profiles;
    uint8_t *p_read_buf = p_gatt_data->p_data;

    WICED_BT_TRACE("[%s] conn_id %d hdl %d len %d sts 0x%x",
                   __FUNCTION__,
                   conn_id,
                   p_gatt_data->handle,
                   p_gatt_data->len,
                   status);

    if (p_peer->gmcs_service.start_handle <= p_gatt_data->handle &&
        p_peer->gmcs_service.end_handle >= p_gatt_data->handle)
    {
        lehs_gmcs_app_handle_read_complete(conn_id, p_clcb, p_gatt_data);
    }
    else if (p_peer->gtbs_service.start_handle <= p_gatt_data->handle &&
             p_peer->gtbs_service.end_handle >= p_gatt_data->handle)
    {
        lehs_gtbs_app_handle_read_complete(conn_id, p_clcb, p_gatt_data);
    }
    else if (p_peer->gmap_service.start_handle <= p_gatt_data->handle &&
             p_peer->gmap_service.end_handle >= p_gatt_data->handle)
    {
        lehs_gmap_app_handle_read_complete(conn_id, p_clcb, p_gatt_data);
    }

    if (p_read_buf)
    {
        gatt_intf_method_free_buffer(p_read_buf);
    }

    return;
}

wiced_result_t app_handle_notification_evt(uint16_t conn_id, lehs_clcb_t *p_clcb, wiced_bt_gatt_data_t *p_gatt_data)
{
    lehs_peer_profiles_t *p_peer = &p_clcb->peer_profiles;

    WICED_BT_TRACE("[%s] hdl %d", __FUNCTION__, p_gatt_data->handle);

    if (p_peer->gmcs_service.start_handle <= p_gatt_data->handle &&
        p_peer->gmcs_service.end_handle >= p_gatt_data->handle)
    {
        lehs_gmcs_app_handle_read_complete(conn_id, p_clcb, p_gatt_data);
    }
    else if (p_peer->gtbs_service.start_handle <= p_gatt_data->handle &&
             p_peer->gtbs_service.end_handle >= p_gatt_data->handle)
    {
        lehs_gtbs_app_handle_read_complete(conn_id, p_clcb, p_gatt_data);
    }
    else if (p_peer->gmap_service.start_handle <= p_gatt_data->handle &&
             p_peer->gmap_service.end_handle >= p_gatt_data->handle)
    {
        lehs_gmap_app_handle_read_complete(conn_id, p_clcb, p_gatt_data);
    }
    return WICED_SUCCESS;
}

void app_handle_gatt_operation_complete(wiced_bt_gatt_operation_complete_t *p_op_cplt)
{
    lehs_clcb_t *p_clcb = lehs_gatt_get_clcb_by_conn_id(p_op_cplt->conn_id);
    WICED_BT_TRACE("[%s] op %d clcb 0x%x p_op 0x%x",
                   __FUNCTION__,
                   p_op_cplt->op,
                   p_clcb,
                   p_clcb ? p_clcb->p_op : NULL);
    if (!p_clcb)
    {
        return;
    }

    if (GATTC_OPTYPE_CONFIG_MTU == p_op_cplt->op)
    {
        lehs_set_gatt_state_mtu_exchanged(p_clcb);

        /* TODO: if bonded, get GATT Db info from NVRAM and start encrption, else start bonding*/
        if (p_clcb->b_is_central)
        {
            lehs_app_sec_bond(p_clcb);
        }
    }
    else if (GATTC_OPTYPE_READ_HANDLE == p_op_cplt->op)
    {
        /* Read response is handled in the common gatt handler after invoking the profile specific handler */
        app_handle_read_complete(p_op_cplt->conn_id, p_clcb, p_op_cplt->status, &p_op_cplt->response_data.att_value);
    }
    else if (GATTC_OPTYPE_WRITE_NO_RSP == p_op_cplt->op || GATTC_OPTYPE_WRITE_WITH_RSP == p_op_cplt->op)
    {
        /* Write response is handled in the common gatt handler after invoking the profile specific handler */
    }
    else if ((GATTC_OPTYPE_NOTIFICATION == p_op_cplt->op) || (GATTC_OPTYPE_INDICATION == p_op_cplt->op))
    {
        /* Notification confirmation is handled in the common gatt handler after invoking the profile specific handler */
        app_handle_notification_evt(p_op_cplt->conn_id, p_clcb, &p_op_cplt->response_data.att_value);

        if (GATTC_OPTYPE_INDICATION == p_op_cplt->op)
        {
            /* Indication confirmation is handled in the common gatt handler after invoking the profile specific handler */
            wiced_bt_gatt_client_send_indication_confirm(p_op_cplt->conn_id, p_op_cplt->response_data.handle);
        }
    }
    else
    {
        WICED_BT_TRACE("[%s] unhandled op %d", __FUNCTION__, p_op_cplt->op);
    }

    /* checks if there is a queue operation and calls the next */
    if ((p_op_cplt->op == GATTC_OPTYPE_READ_HANDLE) || (p_op_cplt->op == GATTC_OPTYPE_WRITE_NO_RSP) ||
        (p_op_cplt->op == GATTC_OPTYPE_WRITE_WITH_RSP))
    {
        if (p_clcb->p_op)
        {
            gatt_intf_handle_gatt_operation_complete(p_op_cplt, p_clcb->p_op);
        }
    }
}

wiced_bt_gatt_status_t lehs_gatt_cback(wiced_bt_gatt_evt_t event, wiced_bt_gatt_event_data_t *p_ed)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;

    WICED_BT_TRACE("[%s] event [0x%x] max_heap %d\n",
                   __FUNCTION__,
                   event,
                   wiced_bt_get_largest_heap_buffer(p_lea_default_heap));

    switch (event)
    {
    case GATT_CONNECTION_STATUS_EVT:
    {
        (p_ed->connection_status.connected) ? lehs_gatt_handle_connection(&p_ed->connection_status)
                                            : lehs_gatt_handle_disconnection(&p_ed->connection_status);
    }
    break;
    case GATT_OPERATION_CPLT_EVT:
    {
        app_handle_gatt_operation_complete(&p_ed->operation_complete);
    }
    break;
    case GATT_DISCOVERY_CPLT_EVT:
    {
        lehs_clcb_t *p_clcb = NULL;
        p_clcb = lehs_gatt_get_clcb_by_conn_id(p_ed->operation_complete.conn_id);

        if (p_clcb && p_clcb->p_discovery_ctx)
        {
            gatt_intf_on_service_discovery_complete(p_clcb->p_discovery_ctx, &p_ed->discovery_complete);

            if (p_clcb->p_discovery_ctx)
            {
                if (gatt_intf_is_service_discovery_complete(p_clcb->p_discovery_ctx))
                {
                    gatt_intf_free_service_discovery_ctx(p_clcb->p_discovery_ctx);
                    p_clcb->p_discovery_ctx = NULL;
                }
            }
        }
    }
    break;
    case GATT_DISCOVERY_RESULT_EVT:
    {
        lehs_clcb_t *p_clcb = NULL;
        p_clcb = lehs_gatt_get_clcb_by_conn_id(p_ed->operation_complete.conn_id);

        WICED_BT_TRACE("[%s]", gatt_event_name[event]);
        p_clcb = lehs_gatt_get_clcb_by_conn_id(p_ed->operation_complete.conn_id);
        if (p_clcb && p_clcb->p_discovery_ctx)
        {
            gatt_intf_on_service_discovery_result(p_clcb->p_discovery_ctx, &p_ed->discovery_result);
        }
    }
    break;
    case GATT_ATTRIBUTE_REQUEST_EVT:
    {
        uint16_t err_handle;
        wiced_bt_gatt_attribute_request_t *p_req = &p_ed->attribute_request;

        status = app_handle_gatt_attribute_request(p_req, &err_handle);
        if (status != WICED_BT_GATT_SUCCESS)
        {
            int send_err_rsp = 1;

            switch (p_req->opcode)
            {
            case GATT_HANDLE_VALUE_NOTIF:
            case GATT_HANDLE_VALUE_IND:
            case GATT_HANDLE_VALUE_CONF:
            case GATT_CMD_WRITE:
            case GATT_CMD_SIGNED_WRITE:
                send_err_rsp = 0;
                break;
            default:
                break;
            }
            if (send_err_rsp)
            {
                wiced_bt_gatt_server_send_error_rsp(p_req->conn_id, p_req->opcode, err_handle, status);
            }
        }
    }
    break;
    case GATT_CONGESTION_EVT:
    {
        lehs_clcb_t *p_clcb = p_clcb = g_lehs_gatt_cb.clcb;

        for (int i = 0; i < LEHS_MAX_CONNECTIONS; i++, p_clcb++)
        {
            if((p_clcb->in_use == FALSE) || (p_clcb->p_op == NULL))
            {
                continue;
            }

            gatt_intf_execute_handle_operations(p_clcb->p_op); /* continue after congestion */
            WICED_BT_TRACE("[%s] conn_id %d congested %d",
                           __FUNCTION__,
                           p_ed->congestion.conn_id,
                           p_ed->congestion.congested);
            break;
        }
    }
    break;
    case GATT_GET_RESPONSE_BUFFER_EVT:
    {
        wiced_bt_gatt_buffer_request_t *p_req = &p_ed->buffer_request;
        if (p_req->len_requested)
        {
            p_req->buffer.p_app_rsp_buffer = gatt_intf_method_get_buffer(p_req->len_requested);
            p_req->buffer.p_app_ctxt = (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer;
        }

        if (!p_req->buffer.p_app_rsp_buffer)
            GATT_INTERFACE_TRACE_CRIT("[%s] get ptr %x ctx %x len %d",
                                      __FUNCTION__,
                                      p_req->buffer.p_app_rsp_buffer,
                                      p_req->buffer.p_app_ctxt,
                                      p_req->len_requested);
    }
    break;
    case GATT_APP_BUFFER_TRANSMITTED_EVT:
    {
        void (*pfn_free)(uint8_t *ptr) = (void (*)(uint8_t *))p_ed->buffer_xmitted.p_app_ctxt;

        if (pfn_free && p_ed->buffer_xmitted.p_app_data)
        {
            pfn_free(p_ed->buffer_xmitted.p_app_data);
        }
        GATT_INTERFACE_TRACE("[%s] free ptr %x ctx %x",
                             __FUNCTION__,
                             p_ed->buffer_xmitted.p_app_data,
                             p_ed->buffer_xmitted.p_app_ctxt);
    }
    break;

    default:
        WICED_BT_TRACE("Unknown event [0x%x]", event);
        break;
    }

    return status;
}

wiced_bt_gatt_status_t lehs_gatt_init(int max_connections)
{
    wiced_bt_gatt_status_t gatt_status = WICED_BT_SUCCESS;

    wiced_bt_gatt_server_enable_caching();

    gatt_status = wiced_bt_gatt_db_init(lehs_gatt_database, sizeof(lehs_gatt_database), g_lehs_gatt_cb.db_hash);
    if (WICED_BT_SUCCESS != gatt_status)
        return gatt_status;

    gatt_status = wiced_bt_gatt_register(lehs_gatt_cback);
    if (WICED_BT_SUCCESS != gatt_status)
        return gatt_status;

#ifdef SIMULATED_NVRAM
    lehs_nvram_init();
#endif

    lehs_set_audio_location(LEHS_APP_DEVICE_TYPE);

    {
        uint8_t set_size;
        uint8_t set_rank;
        ga_lib_csis_sirk_t sirk = {LEHS_APP_SIRK_VALUE};

#if (LEHS_APP_DEVICE_TYPE == APP_DEVICE_TYPE_EAR_BUD_LEFT)
        set_size = 2;
        set_rank = 1;
#elif (LEHS_APP_DEVICE_TYPE == APP_DEVICE_TYPE_EAR_BUD_RIGHT)
        set_size = 2;
        set_rank = 2;
#else
        set_size = 1;
        set_rank = 1;
#endif

        lehs_csis_initialize_data(set_size, set_rank, LEHS_APP_SIRK_TYPE, sirk);
    }

    lehs_vcs_initialize_data();
    lehs_has_initialize_data();
    lehs_mics_initialize_data();

    gatt_intf_set_cccd_value_callback(lehs_handle_get_cccd_value_cb);

    wiced_ble_ext_adv_register_cback(lehs_ext_adv_cback);

    return gatt_status;
}

uint8_t mcs_read_play_state_chars[] = {GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_TITLE,
                                       GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_POSITION,
                                       GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_DURATION};
const uint8_t tbs_read_call_state_chars[] = {GA_LIB_TBS_CHARACTERISTIC_BEARER_LIST_CURRENT_CALLS,
                                             GA_LIB_TBS_CHARACTERISTIC_CALL_FRIENDLY_NAME};

void on_read_remote_cmpl(uint16_t conn_id, wiced_bt_gatt_status_t status, void *pv_ctx)
{
    lehs_clcb_t *p_clcb = (lehs_clcb_t *)pv_ctx;

    WICED_BT_TRACE("[%s] conn_id %d status 0x%x", __FUNCTION__, conn_id, status);
    if (p_clcb->p_op)
    {
        gatt_intf_free_operation_handle_list(p_clcb->p_op);
        p_clcb->p_op = NULL;
    }

    lehs_set_gatt_state_read_characteristics(p_clcb);
    app_rpc_send_app_status(conn_id, p_clcb->identity_bd_address, HCI_CONTROL_MISC_APP_STATE_READY, status);

    if (status == WICED_BT_GATT_SUCCESS )
    {
        wiced_bt_db_hash_t null_db_hash = {0};

        if (memcmp(p_clcb->db_hash, null_db_hash, sizeof(wiced_bt_db_hash_t)) == 0)
        {
            p_clcb->db_hash[0] = 1; // set a non-zero value to indicate that the db hash is valid
        }
    }
}

wiced_result_t lehs_read_remote_characteristics(lehs_clcb_t *p_clcb)
{
    uint8_t mcs_read_chars[] = {GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYER_NAME, GA_LIB_MCS_CHARACTERISTIC_MEDIA_STATE};
    uint8_t tbs_read_chars[] = {GA_LIB_TBS_CHARACTERISTIC_BEARER_PROVIDER_NAME, GA_LIB_TBS_CHARACTERISTIC_CALL_STATE};

    uint8_t num_mcs_chars = sizeof(mcs_read_chars) / sizeof(mcs_read_chars[0]);
    uint8_t num_tbs_chars = sizeof(tbs_read_chars) / sizeof(tbs_read_chars[0]);
    uint8_t total_chars = num_mcs_chars + num_tbs_chars;

    p_clcb->p_op = gatt_intf_alloc_read_handle_list(p_clcb->conn_id, total_chars, on_read_remote_cmpl, p_clcb);

    if (p_clcb->p_op == NULL)
    {
        return WICED_BT_NO_RESOURCES;
    }

    {
        const uint8_t *p_chars = mcs_read_chars;
        for (int i = 0; i < num_mcs_chars; i++)
        {
            gatt_intf_add_characteristic_to_list(p_clcb->p_op, &p_clcb->peer_profiles.gmcs[*p_chars++], i);
        }
    }
    {
        const uint8_t *p_chars = tbs_read_chars;
        for (int i = 0; i < num_tbs_chars; i++)
        {
            gatt_intf_add_characteristic_to_list(p_clcb->p_op,
                                                 &p_clcb->peer_profiles.gtbs[*p_chars++],
                                                 i + num_mcs_chars);
        }
    }

    return gatt_intf_execute_handle_operations(p_clcb->p_op); /* continue read_remote */
}

void on_enable_notification_cmpl(uint16_t conn_id, wiced_bt_gatt_status_t status, void *pv_ctx)
{
    lehs_clcb_t *p_clcb = (lehs_clcb_t *)pv_ctx;

    WICED_BT_TRACE("[%s] conn_id %d status 0x%x", __FUNCTION__, conn_id, status);

    if (p_clcb->p_op)
    {
        gatt_intf_free_operation_handle_list(p_clcb->p_op);
        p_clcb->p_op = NULL;
    }

    lehs_set_gatt_state_enabled_notifications(p_clcb);
    lehs_read_remote_characteristics(p_clcb);
}

wiced_result_t lehs_enable_app_notifications(lehs_clcb_t *p_clcb)
{
    uint8_t mcs_enable_notification_chars[] = {GA_LIB_MCS_CHARACTERISTIC_MEDIA_STATE,
                                               GA_LIB_MCS_CHARACTERISTIC_MEDIA_CONTROL_POINT,
                                               GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_TITLE,
                                               GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_CHANGED,
                                               GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_POSITION,
                                               GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_DURATION};
    uint8_t tbs_enable_notification_chars[] = {GA_LIB_TBS_CHARACTERISTIC_CALL_CONTROL_POINT,
                                               GA_LIB_TBS_CHARACTERISTIC_CALL_STATE,
                                               GA_LIB_TBS_CHARACTERISTIC_INCOMING_CALL,
                                               GA_LIB_TBS_CHARACTERISTIC_STATUS_FLAGS,
                                               GA_LIB_TBS_CHARACTERISTIC_CALL_TERMINATION_REASON,
                                               GA_LIB_TBS_CHARACTERISTIC_CALL_FRIENDLY_NAME};
    uint8_t num_mcs_chars = sizeof(mcs_enable_notification_chars) / sizeof(mcs_enable_notification_chars[0]);
    uint8_t num_tbs_chars = sizeof(tbs_enable_notification_chars) / sizeof(tbs_enable_notification_chars[0]);

    p_clcb->p_op = gatt_intf_alloc_notification_handle_list(p_clcb->conn_id,
                                                            num_mcs_chars + num_tbs_chars,
                                                            on_enable_notification_cmpl,
                                                            p_clcb);
    if (p_clcb->p_op == NULL)
    {
        return WICED_BT_NO_RESOURCES;
    }

    for (int i = 0; i < num_mcs_chars; i++)
    {
        gatt_intf_add_characteristic_to_list(p_clcb->p_op,
                                             &p_clcb->peer_profiles.gmcs[mcs_enable_notification_chars[i]],
                                             i);
    }
    for (int i = 0; i < num_tbs_chars; i++)
    {
        gatt_intf_add_characteristic_to_list(p_clcb->p_op,
                                             &p_clcb->peer_profiles.gtbs[tbs_enable_notification_chars[i]],
                                             i + num_mcs_chars);
    }

    WICED_BT_TRACE("[%s] Executing enable notifications", __FUNCTION__);
    return gatt_intf_execute_handle_operations(p_clcb->p_op); /* continue enable notifications */
}

void lehs_gatt_handle_discovery_complete(lehs_clcb_t *p_clcb, wiced_bt_gatt_status_t status)
{
    WICED_BT_TRACE("[%s] status %d", __FUNCTION__, status);
    if (status)
    {
        // wiced_bt_gatt_disconnect here
        return;
    }
    if (status == WICED_BT_GATT_SUCCESS)
    {
        lehs_set_gatt_state_discovery_complete(p_clcb);
        app_rpc_send_app_status(p_clcb->conn_id,
                                p_clcb->identity_bd_address,
                                HCI_CONTROL_MISC_APP_STATE_INITING,
                                HCI_CONTROL_MISC_APP_STATE_INIT_READING);
    }

    // Read initial values of specific characteristics
    lehs_enable_app_notifications(p_clcb);
}

void lehs_cache_discovery_results_and_cleanup(lehs_clcb_t *p_clcb,
                                              gatt_intf_discovery_result_t *p_result,
                                              gatt_intf_service_range_t *p_service_range,
                                              gatt_intf_characteristic_handles_t *p_handles)
{
    gatt_intf_characteristic_handles_result_t *p_char_handles = p_result->p_char_handles;
    gatt_intf_print_handles(p_result);

    memcpy(p_service_range, &p_result->service_range, sizeof(gatt_intf_service_range_t));
    for (int i = 0; i < p_result->max_characteristic_handles; i++, p_handles++, p_char_handles++)
    {
        memcpy(p_handles, &p_char_handles->handles, sizeof(gatt_intf_characteristic_handles_t));
    }

    lehs_gatt_free_discovery_ctx(p_clcb);
}

void on_tmap_discovery_complete(gatt_intf_service_discovery_ctx_t *p_sdc,
                                uint16_t conn_id,
                                wiced_bt_gatt_status_t status,
                                gatt_intf_discovery_result_t *p_result)
{
    lehs_clcb_t *p_clcb = lehs_gatt_get_clcb_by_conn_id(conn_id);
    if (!p_clcb)
    {
        WICED_BT_TRACE("[%s] No clcb found for conn_id %d", __FUNCTION__, conn_id);
        return;
    }

    if (status == WICED_BT_GATT_SUCCESS)
    {
        lehs_cache_discovery_results_and_cleanup(p_clcb,
                                            p_result,
                                            &p_clcb->peer_profiles.tmap_service,
                                            p_clcb->peer_profiles.tmap);

        app_rpc_send_app_status(conn_id, p_clcb->identity_bd_address, HCI_CONTROL_MISC_APP_STATE_DISCOVERY_COMPLETE, status);
        lehs_gatt_handle_discovery_complete(p_clcb, status);
    }
}

void on_tbs_discovery_complete(gatt_intf_service_discovery_ctx_t *p_sdc,
                               uint16_t conn_id,
                               wiced_bt_gatt_status_t status,
                               gatt_intf_discovery_result_t *p_result)
{
    lehs_clcb_t *p_clcb = lehs_gatt_get_clcb_by_conn_id(conn_id);
    if (!p_clcb)
    {
        WICED_BT_TRACE("[%s] No clcb found for conn_id %d", __FUNCTION__, conn_id);
        return;
    }

    if (status == WICED_BT_GATT_SUCCESS)
    {
        lehs_cache_discovery_results_and_cleanup(p_clcb,
                                            p_result,
                                            &p_clcb->peer_profiles.gtbs_service,
                                            p_clcb->peer_profiles.gtbs);
        p_clcb->p_discovery_ctx = ga_lib_tmap_discover_service(p_clcb->conn_id, on_tmap_discovery_complete);
    }
}

void on_mcs_discovery_complete(gatt_intf_service_discovery_ctx_t *p_sdc,
                               uint16_t conn_id,
                               wiced_bt_gatt_status_t status,
                               gatt_intf_discovery_result_t *p_result)
{
    lehs_clcb_t *p_clcb = lehs_gatt_get_clcb_by_conn_id(conn_id);
    if (!p_clcb)
    {
        WICED_BT_TRACE("[%s] No clcb found for conn_id %d", __FUNCTION__, conn_id);
        return;
    }

    if (status == WICED_BT_GATT_SUCCESS)
    {
        lehs_cache_discovery_results_and_cleanup(p_clcb,
                                            p_result,
                                            &p_clcb->peer_profiles.gmcs_service,
                                            p_clcb->peer_profiles.gmcs);

        p_clcb->p_discovery_ctx = ga_lib_gtbs_discover_service(p_clcb->conn_id, on_tbs_discovery_complete);
    }
}

void lehs_gatt_start_discovery(lehs_clcb_t *p_clcb)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;

    if (!p_clcb)
    {
        return;
    }

    p_clcb->p_discovery_ctx = ga_lib_gmcs_discover_service(p_clcb->conn_id, on_mcs_discovery_complete);
    if (!p_clcb)
    {
        return;
    }

    if (status)
    {
        WICED_BT_TRACE_CRIT("[%s] status [%d] \n", __FUNCTION__, status);
    }
}

lehs_gatt_cb_t *broadcast_sink_gatt_get_cb(uint8_t *p_bd_addr)
{
    lehs_gatt_cb_t *p_gatt_cb = &g_lehs_gatt_cb;

    return p_gatt_cb;
}

lehs_gatt_cb_t *broadcast_sink_gatt_get_cb_by_conn_id(uint16_t conn_id)
{
    lehs_gatt_cb_t *p_gatt_cb = &g_lehs_gatt_cb;

    return p_gatt_cb;
}
