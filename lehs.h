/*
 * $ Copyright Cypress Semiconductor $
 */

#ifndef LEHS_H
#define LEHS_H

/* BT Stack includes */
#include "wiced_bt_cfg.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_gatt.h"
#include "wiced_bt_stack.h"
#include "wiced_bt_stack_platform.h"
#include "wiced_bt_trace.h"
#include "wiced_memory.h"
#include "wiced_timer.h"

/* App Library includes */
#include "ga_lib_aics.h"
#include "ga_lib_ascs.h"
#include "ga_lib_bass.h"
#include "ga_lib_csis.h"
#include "ga_lib_gmap.h"
#include "ga_lib_has.h"
#include "ga_lib_mcs.h"
#include "ga_lib_mics.h"
#include "ga_lib_pacs.h"
#include "ga_lib_tbs.h"
#include "ga_lib_tmap.h"
#include "ga_lib_vcs.h"
#include "ga_lib_vocs.h"
#include "gatt_interface.h"
#include "le_audio_bap_broadcast.h"
#include "le_audio_rpc.h"

/* Application includes */
#include "lehs_isoc.h"
#include "lehs_nvram.h"
#include "lehs_rpc.h"

#define DEVICE_NAME "LeAudioHS" /**< Device name used for advertising */
#define HAS_ENABLED 1           /**< Enable Hearing Aid Service */
#define TMAS_ENABLED 1          /**< Enable Telephone Media Audio Service */

#define UNICAST_SINK_EXT_ADV_HANDLE 1 /**< Unicast Sink Extended Advertising Handle */
#define LEHS_MAX_CONNECTIONS 2        /**< Maximum number of simultaneous connections */
#define LEHS_MAX_BIG 1                /**< Maximum number of BIGs */
#define DEFAULT_VOL 100               /**< Default volume level */
#define DEFAULT_MIC_GAIN 100          /**< Default microphone gain level */
#define MAX_BROADCAST_NAME_SIZE 32    /**< Maximum size of the broadcast name that can be sent to client control */
#define VCS_STEP_SIZE 25              /**< Volume Control Step Size */
#define MAX_DESCRIPTION 20 /**< Maximum length for AICS/VOCS description that can be stored in the application */
#define MAX_PACS_SNK_CAP_SUPPORTED 2 /**< Maximum number of PACS Sink capabilities supported */
#define MAX_PACS_SRC_CAP_SUPPORTED 2 /**< Maximum number of PACS Source capabilities supported */
#define MAX_HAS_PRESET_RECORD_NAME_LENGTH                                                                              \
    20 /**< Maximum length for HAS preset record name that can be stored in the application */
#define BROADCAST_MAX_SUB_GROUP 2         /**< Maximum number of sub groups of BIG */
#define BROADCAST_MAX_BIS_PER_SUB_GROUP 2 /**< Max BIS per sub group */
#define MAX_MICS_AICS 1                   /**< Maximum number of AICS included by MICS */

//Enable definition of MAX_SUPPORTED_OCTETS_PER_CODEC_FRAME in makefile for audio config higher than 48_2_2
#ifndef MAX_SUPPORTED_OCTETS_PER_CODEC_FRAME
#define MAX_SUPPORTED_OCTETS_PER_CODEC_FRAME 0x64
#endif                                                               // !MAX_SUPPORTED_OCTETS_PER_CODEC_FRAME
#define LEHS_MAX_SDU_SIZE (MAX_SUPPORTED_OCTETS_PER_CODEC_FRAME * 2) /**< Maximum SDU size handled by LEHS */

#define MAX_MEDIA_PLAYER_NAME_LEN 40    /**< Maximum Media Player Name length */
#define MAX_MEDIA_TRACK_TITLE_LEN 40    /**< Maximum Media Track Title length */
#define MAX_MEDIA_EVENT_DATA_LENGTH 100 /**< Maximum Event Data length */
#define MIN_MEDIA_PLAYBACK_SPEED (-128) /**< Minimum playback speed supported by MCS */
#define MAX_MEDIA_PLAYBACK_SPEED (127)  /**< Maximum playback speed supported by MCS */

/** Static definitions for application audio locations */
#define APP_DEVICE_TYPE_EAR_BUD_LEFT 1  /**< Set if device acts as the left ear bud, of a pair */
#define APP_DEVICE_TYPE_EAR_BUD_RIGHT 2 /**< Set if device acts as the right ear bud, of a pair */
#define APP_DEVICE_TYPE_HEADPHONE 3     /**< Set if device acts as a headphone, with stereo */


/** Static definitions for application appearance */
#define LEHS_APPEARANCE_WEARABLE_DEVICE_HEADPHONE 0x0943 /**< Device appearance as headphone */
#define LEHS_APPEARANCE_WEARABLE_DEVICE_LEFT_EARBUD 0x0945 /**< Device appearance as left earbud */
#define LEHS_APPEARANCE_WEARABLE_DEVICE_RIGHT_EARBUD 0x0946 /**< Device appearance as right earbud */

/**
 * Set to APP_DEVICE_TYPE_EAR_BUD_LEFT or APP_DEVICE_TYPE_EAR_BUD_RIGHT or
 * APP_DEVICE_TYPE_HEADPHONE
 */
#ifndef LEHS_APP_DEVICE_TYPE
#define LEHS_APP_DEVICE_TYPE APP_DEVICE_TYPE_HEADPHONE
#endif /* LEHS_APP_DEVICE_TYPE */

#if (LEHS_APP_DEVICE_TYPE == APP_DEVICE_TYPE_EAR_BUD_LEFT)
#define LEHS_APP_APPEARANCE LEHS_APPEARANCE_WEARABLE_DEVICE_LEFT_EARBUD
#elif (LEHS_APP_DEVICE_TYPE == APP_DEVICE_TYPE_EAR_BUD_RIGHT)
#define LEHS_APP_APPEARANCE LEHS_APPEARANCE_WEARABLE_DEVICE_RIGHT_EARBUD
#else
#define LEHS_APP_APPEARANCE LEHS_APPEARANCE_WEARABLE_DEVICE_HEADPHONE
#endif

/**
* Set default LEHS_APP_SIRK_TYPE. Can be 0 = GA_LIB_CSIS_SIRK_ENCR, 1 = GA_LIB_CSIS_SIRK_PLAIN
*/
#ifndef LEHS_APP_SIRK_TYPE
#define LEHS_APP_SIRK_TYPE GA_LIB_CSIS_SIRK_PLAIN
#endif

/**
* Set default lehs_app_sirk_value.
*/
#ifndef LEHS_APP_SIRK_VALUE
#define LEHS_APP_SIRK_VALUE                                                                                            \
    0x0a, 0x01, 0x02, 0x03, 0x0a, 0x01, 0x02, 0x03, 0x0a, 0x01, 0x02, 0x03, 0x0a, 0x01, 0x02, 0x03
#endif

/** Advertising state */
typedef enum
{
    ADV_STATE_IDLE = 0,                   // 0   /**< Idle state */
    ADV_STATE_SWIFT_PAIR_HIGH_DUTY_CYCLE, // 1   /**< Swift Pair High Duty Cycle state */
    ADV_STATE_SWIFT_PAIR_LOW_DUTY_CYCLE,  // 2   /**< Swift Pair Low Duty Cycle state */
    ADV_STATE_REGULAR_ADV,                // 3   /**< Regular Advertising state */
    ADV_STATE_MAX                         // 4   /**< Maximum Advertising state */
} adv_state_t;

/** ASEs Index */
enum
{
    LEHS_ASE_INDEX_SINK_1 = 0, // 0
    LEHS_ASE_INDEX_SINK_2,     // 1
    LEHS_ASE_INDEX_SOURCE_1,   // 2
    LEHS_ASE_INDEX_MAX         // 3
};

/** Broadcast Receive State */
enum
{
    LEHS_BROADCAST_RCV_STATE_0 = 0, // 0
    //LEHS_BROADCAST_RCV_STATE_1,
    LEHS_BROADCAST_RCV_STATE_MAX // 1
};

/**GATT handles for the GATT services and characteristics used in the application */
enum
{
    HDLS_GATT_GENERIC_ATTRIBUTE_SERVICE = 0x1,                        // 0x0001 , 1
    HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_CHANGED,                      // 0x0002 , 2
    HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_CHANGED_VALUE,                // 0x0003 , 3
    HDLD_GATT_GENERIC_ATTRIBUTE_SERVICE_CHANGED_CLIENT_CONFIGURATION, // 0x0004 , 4
    HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_DB_HASH,                      // 0x0005 , 5
    HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_DB_HASH_VALUE,                // 0x0006 , 6
    HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_CLIENT_FEATURES,              // 0x0007 , 7
    HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_CLIENT_FEATURES_VALUE,        // 0x0008 , 8
    HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_SERVER_FEATURES,              // 0x0009 , 9
    HDLC_GATT_GENERIC_ATTRIBUTE_SERVICE_SERVER_FEATURES_VALUE,        // 0x000A , 10

    HDLS_GENERIC_ACCESS = 0x20,                           // 0x0020 , 32
    HDLC_GENERIC_ACCESS_DEVICE_NAME,                      // 0x0021 , 33
    HDLC_GENERIC_ACCESS_DEVICE_NAME_VALUE,                // 0x0022 , 34
    HDLC_GENERIC_ACCESS_APPEARANCE,                       // 0x0023 , 35
    HDLC_GENERIC_ACCESS_APPEARANCE_VALUE,                 // 0x0024 , 36
    HDLC_GENERIC_ACCESS_PREFERRED_CONNECTION_PARAM,       // 0x0025 , 37
    HDLC_GENERIC_ACCESS_PREFERRED_CONNECTION_PARAM_VALUE, // 0x0026 , 38

    HDLS_VCS = 0x30,                            // 0x0030 , 48
    HDLI_VCS_INCLUDED_AICS,                     // 0x0031 , 49
    HDLI_VCS_INCLUDED_VOCS,                     // 0x0032 , 50
    HDLC_VCS_VOLUME_STATE,                      // 0x0033 , 51
    HDLC_VCS_VOLUME_STATE_VALUE,                // 0x0034 , 52
    HDLD_VCS_VOLUME_STATE_CLIENT_CONFIGURATION, // 0x0035 , 53
    HDLC_VCS_VOLUME_CONTROL_POINT,              // 0x0036 , 54
    HDLC_VCS_VOLUME_CONTROL_POINT_VALUE,        // 0x0037 , 55
    HDLC_VCS_VOLUME_FLAGS,                      // 0x0038 , 56
    HDLC_VCS_VOLUME_FLAGS_VALUE,                // 0x0039 , 57
    HDLD_VCS_VOLUME_FLAGS_CLIENT_CONFIGURATION, // 0x003A , 58

    HDLS_VOCS = 0x40,                                        // 0x0040 , 64
    HDLC_VOCS_OFFSET_STATE,                                  // 0x0041 , 65
    HDLC_VOCS_OFFSET_STATE_VALUE,                            // 0x0042 , 66
    HDLD_VOCS_OFFSET_STATE_CLIENT_CONFIGURATION,             // 0x0043 , 67
    HDLC_VOCS_AUDIO_LOCATION,                                // 0x0044 , 68
    HDLC_VOCS_AUDIO_LOCATION_VALUE,                          // 0x0045 , 69
    HDLD_VOCS_AUDIO_LOCATION_CLIENT_CONFIGURATION,           // 0x0046 , 70
    HDLC_VOCS_VOLUME_OFFSET_CONTROL_POINT,                   // 0x0047 , 71
    HDLC_VOCS_VOLUME_OFFSET_CONTROL_POINT_VALUE,             // 0x0048 , 72
    HDLC_VOCS_AUDIO_OUTPUT_DESCRIPTION,                      // 0x0049 , 73
    HDLC_VOCS_AUDIO_OUTPUT_DESCRIPTION_VALUE,                // 0x004A , 74
    HDLD_VOCS_AUDIO_OUTPUT_DESCRIPTION_CLIENT_CONFIGURATION, // 0x004B , 75

#if 0
    HDLS_VCS_AICS = 0x50,                                             // 0x0050 , 80
    HDLC_VCS_AICS_INPUT_STATE,                                        // 0x0051 , 81
    HDLC_VCS_AICS_INPUT_STATE_VALUE,                                  // 0x0052 , 82
    HDLD_VCS_AICS_INPUT_STATE_CLIENT_CONFIGURATION,                   // 0x0053 , 83
    HDLC_VCS_AICS_GAIN_SETTING_ATTR,                                  // 0x0054 , 84
    HDLC_VCS_AICS_GAIN_SETTING_ATTR_VALUE,                            // 0x0055 , 85
    HDLC_VCS_AICS_INPUT_TYPE,                                         // 0x0056 , 86
    HDLC_VCS_AICS_INPUT_TYPE_VALUE,                                   // 0x0057 , 87
    HDLC_VCS_AICS_INPUT_STATUS,                                       // 0x0058 , 88
    HDLC_VCS_AICS_INPUT_STATUS_VALUE,                                 // 0x0059 , 89
    HDLD_VCS_AICS_INPUT_STATUS_CLIENT_CONFIGURATION,                  // 0x005A , 90
    HDLC_VCS_AICS_AUDIO_INPUT_CONTROL_POINT,                          // 0x005B , 91
    HDLC_VCS_AICS_AUDIO_INPUT_CONTROL_POINT_VALUE,                    // 0x005C , 92
    HDLC_VCS_AICS_AUDIO_INPUT_DESCRIPTION,                            // 0x005D , 93
    HDLC_VCS_AICS_AUDIO_INPUT_DESCRIPTION_VALUE,                      // 0x005E , 94
    HDLD_VCS_AICS_AUDIO_INPUT_DESCRIPTION_CLIENT_CONFIGURATION,       // 0x005F , 95
#endif

    HDLS_PACS = 0xD0,                                        // 0x00D0 , 208
    HDLC_PACS_SINK_PAC,                                      // 0x00D1 , 209
    HDLC_PACS_SINK_PAC_VALUE,                                // 0x00D2 , 210
    HDLD_PACS_SINK_PAC_CLIENT_CONFIGURATION,                 // 0x00D3 , 211
    HDLC_PACS_SINK_AUDIO_LOCATIONS,                          // 0x00D4 , 212
    HDLC_PACS_SINK_AUDIO_LOCATIONS_VALUE,                    // 0x00D5 , 213
    HDLD_PACS_SINK_AUDIO_LOCATIONS_CLIENT_CONFIGURATION,     // 0x00D6 , 214
    HDLC_PACS_SOURCE_PAC,                                    // 0x00D7 , 215
    HDLC_PACS_SOURCE_PAC_VALUE,                              // 0x00D8 , 216
    HDLD_PACS_SOURCE_PAC_CLIENT_CONFIGURATION,               // 0x00D9 , 217
    HDLC_PACS_SOURCE_AUDIO_LOCATIONS,                        // 0x00DA , 218
    HDLC_PACS_SOURCE_AUDIO_LOCATIONS_VALUE,                  // 0x00DB , 219
    HDLD_PACS_SOURCE_AUDIO_LOCATIONS_CLIENT_CONFIGURATION,   // 0x00DC , 220
    HDLC_PACS_AVAILABLE_AUDIO_CONTEXTS,                      // 0x00DD , 221
    HDLC_PACS_AVAILABLE_AUDIO_CONTEXTS_VALUE,                // 0x00DE , 222
    HDLD_PACS_AVAILABLE_AUDIO_CONTEXTS_CLIENT_CONFIGURATION, // 0x00DF , 223
    HDLC_PACS_SUPPORTED_AUDIO_CONTEXTS,                      // 0x00E0 , 224
    HDLC_PACS_SUPPORTED_AUDIO_CONTEXTS_VALUE,                // 0x00E1 , 225
    HDLD_PACS_SUPPORTED_AUDIO_CONTEXTS_CLIENT_CONFIGURATION, // 0x00E2 , 226

    HDLS_ASCS = 0x100,                                // 0x0100 , 256
    HDLC_ASCS_ASE_SINK,                               // 0x0101 , 257
    HDLC_ASCS_ASE_SINK_VALUE,                         // 0x0102 , 258
    HDLD_ASCS_ASE_SINK_CLIENT_CONFIGURATION,          // 0x0103 , 259
    HDLC_ASCS_ASE_SOURCE,                             // 0x0104 , 260
    HDLC_ASCS_ASE_SOURCE_VALUE,                       // 0x0105 , 261
    HDLD_ASCS_ASE_SOURCE_CLIENT_CONFIGURATION,        // 0x0106 , 262
    HDLC_ASCS_ASE_SINK_2,                             // 0x0107 , 263
    HDLC_ASCS_ASE_SINK_2_VALUE,                       // 0x0108 , 264
    HDLD_ASCS_ASE_SINK_2_CLIENT_CONFIGURATION,        // 0x0109 , 265
    HDLC_ASCS_ASE_CONTROL_POINT,                      // 0x010A , 266
    HDLC_ASCS_ASE_CONTROL_POINT_VALUE,                // 0x010B , 267
    HDLD_ASCS_ASE_CONTROL_POINT_CLIENT_CONFIGURATION, // 0x010C , 268

    HDLS_CAS = 320,        // 0x0140 , 320
    HDLI_CAS_INCLUDE_CSIS, // 0x0141 , 321

    HDLS_BASS = 384,                                          // 0x0180 , 384
    HDLC_BASS_BROADCAST_AUDIO_SCAN_CONTROL_POINT,             // 0x0181 , 385
    HDLC_BASS_BROADCAST_AUDIO_SCAN_CONTROL_POINT_VALUE,       // 0x0182 , 386
    HDLC_BASS_BROADCAST_RECEIVE_STATE_1,                      // 0x0183 , 387
    HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE,                // 0x0184 , 388
    HDLD_BASS_BROADCAST_RECEIVE_STATE_1_CLIENT_CONFIGURATION, // 0x0185 , 389

    HDLS_CSIS = 400,                     // 0x0190, 400
    HDLC_CSIS_SIRK,                      // 0x0191, 401
    HDLC_CSIS_SIRK_VALUE,                // 0x0192, 402
    HDLD_CSIS_SIRK_CLIENT_CONFIGURATION, // 0x0193, 403
    HDLC_CSIS_SIZE,                      // 0x0194, 404
    HDLC_CSIS_SIZE_VALUE,                // 0x0195, 405
    HDLD_CSIS_SIZE_CLIENT_CONFIGURATION, // 0x0196, 406
    HDLC_CSIS_LOCK,                      // 0x0197, 407
    HDLC_CSIS_LOCK_VALUE,                // 0x0198, 408
    HDLD_CSIS_LOCK_CLIENT_CONFIGURATION, // 0x0199, 409
    HDLC_CSIS_RANK,                      // 0x019A, 410
    HDLC_CSIS_RANK_VALUE,                // 0x019B, 411

    HDLS_MICS = 420,                           // 0x01A4, 420
    HDLI_MICS_INCLUDED_AICS,                   // 0x01A5, 421
    HDLC_MICS_MUTE_STATE,                      // 0x01A6, 422
    HDLC_MICS_MUTE_STATE_VALUE,                // 0x01A7, 423
    HDLD_MICS_MUTE_STATE_CLIENT_CONFIGURATION, // 0x01A8, 424

    HDLS_MICS_AICS = 432,                                        // 0x01B0, 432
    HDLC_MICS_AICS_INPUT_STATE,                                  // 0x01B1, 433
    HDLC_MICS_AICS_INPUT_STATE_VALUE,                            // 0x01B2, 434
    HDLD_MICS_AICS_INPUT_STATE_CLIENT_CONFIGURATION,             // 0x01B3, 435
    HDLC_MICS_AICS_GAIN_SETTING_ATTR,                            // 0x01B4, 436
    HDLC_MICS_AICS_GAIN_SETTING_ATTR_VALUE,                      // 0x01B5, 437
    HDLC_MICS_AICS_INPUT_TYPE,                                   // 0x01B6, 438
    HDLC_MICS_AICS_INPUT_TYPE_VALUE,                             // 0x01B7, 439
    HDLC_MICS_AICS_INPUT_STATUS,                                 // 0x01B8, 440
    HDLC_MICS_AICS_INPUT_STATUS_VALUE,                           // 0x01B9, 441
    HDLD_MICS_AICS_INPUT_STATUS_CLIENT_CONFIGURATION,            // 0x01BA, 442
    HDLC_MICS_AICS_AUDIO_INPUT_CONTROL_POINT,                    // 0x01BB, 443
    HDLC_MICS_AICS_AUDIO_INPUT_CONTROL_POINT_VALUE,              // 0x01BC, 444
    HDLC_MICS_AICS_AUDIO_INPUT_DESCRIPTION,                      // 0x01BD, 445
    HDLC_MICS_AICS_AUDIO_INPUT_DESCRIPTION_VALUE,                // 0x01BE, 446
    HDLD_MICS_AICS_AUDIO_INPUT_DESCRIPTION_CLIENT_CONFIGURATION, // 0x01BF, 447

    HDLS_HAS = 480,                                                 // 0x1E0, 480
    HDLC_HAS_HEARIND_AID_FEATUES,                                   // 0x1E1, 481
    HDLC_HAS_HEARIND_AID_FEATUES_VALUE,                             // 0x1E2, 482
    HDLD_HAS_HEARIND_AID_FEATUES_CLIENT_CONFIGURATION,              // 0x1E3, 483
    HDLC_HAS_HEARING_AID_PRESET_CONTROL_POINT,                      // 0x1E4, 484
    HDLC_HAS_HEARING_AID_PRESET_CONTROL_POINT_VALUE,                // 0x1E5, 485
    HDLD_HAS_HEARING_AID_PRESET_CONTROL_POINT_CLIENT_CONFIGURATION, // 0x1E6, 486
    HDLC_HAS_ACTIVE_PRESET_INDEX,                                   // 0x1E7, 487
    HDLC_HAS_ACTIVE_PRESET_INDEX_VALUE,                             // 0x1E8, 488
    HDLD_HAS_ACTIVE_PRESET_INDEX_CLIENT_CONFIGURATION,              // 0x1E9, 489

    HDLS_TMAS = 512,                                         // 0x0200, 512
    HDLC_TMAS_TELEPHONE_MEDIA_AUDIO_PROFILE_ROLE,            // 0x0201, 513
    HDLC_TMAS_TELEPHONE_MEDIA_AUDIO_PROFILE_ROLE_ROLE_VALUE, // 0x0202, 514

    HDLS_GMAP = 528,              // 0x0210, 528
    HDLS_GMAP_ROLE,               // 0x0211, 529
    HDLS_GMAP_ROLE_VALUE,         // 0x0212, 530
    HDLS_GMAP_UGT_FEATURES,       // 0x0213, 531
    HDLS_GMAP_UGT_FEATURES_VALUE, // 0x0214, 532
    HDLS_GMAP_BGR_FEATURES,       // 0x0215, 533
    HDLS_GMAP_BGR_FEATURES_VALUE, // 0x0216, 534
};

/** ASE data structure */
struct lehs_ase_data_t_
{
    ga_lib_ascs_ase_t ase;              /**< ASE data structure */
    uint16_t acl_conn_handle;           /**< ACL connection handle */
    uint16_t cis_conn_handle;           /**< CIS connection handle */
    uint16_t gatt_ase_value_handle;     /**< GATT value handle for this ASE */
    uint32_t data_path_established : 1; /**< Data path established ? */
};

/** Media Control Service(MCS) data structure */
typedef struct
{
    char media_player_name[MAX_MEDIA_PLAYER_NAME_LEN]; /**< Media Player Name */
    char track_title[MAX_MEDIA_TRACK_TITLE_LEN];       /**< Track Title */
    int32_t track_duration;                            /**< Track Duration */
    int32_t track_position;                            /**< Track Position */
    int8_t playback_speed;                             /**< Playback Speed */
    int8_t seeking_speed;                              /**< Seeking Speed */
    ga_lib_mcs_playing_order_t playing_order;          /**< Playing Order */
    uint16_t playing_order_supported;                  /**< Playing Order Supported bit field */
    ga_lib_mcs_state_t media_state;                    /**< Media State */
    uint32_t media_control_supported_opcodes;          /**< Media Control Supported Opcodes */
    uint8_t content_control_id;                        /**< Content Control ID */
} lehs_mcs_data_t;

/** Gaming and Media Profile(GAMAP) data structure */
typedef struct
{
    ga_lib_gmap_role_t gmap_role; /**< GMAP role bit field */
    uint8_t ugg_features;         /**< GMAP UGG features bit field */
    uint8_t ugt_features;         /**< GMAP UGT features bit field */
    uint8_t bgs_features;         /**< GMAP BGS features bit field */
    uint8_t bgr_features;         /**< GMAP BGR features bit field */
} lehs_gmap_data_t;

/** Connection Link Control Block(CLCB) data structure */
typedef struct
{
    uint32_t in_use : 1;                /**< In use flag */
    uint32_t connected : 1;             /**< Connected flag */
    uint32_t mtu_exchanged : 1;         /**< MTU exchanged flag */
    uint32_t discovery_complete : 1;    /**< Service discovery complete flag */
    uint32_t enabled_notifications : 1; /**< Notifications enabled flag */
    uint32_t read_characteristics : 1;  /**< Characteristics read flag */
    uint32_t ready : 1;                 /**< Ready flag */
    uint32_t disconnecting : 1;         /**< Disconnecting flag */
    uint32_t b_is_central : 1;          /**< True if this device is a central */

    uint16_t conn_id;                                   /**< Connection ID */
    uint8_t conn_addr_type;                             /**< Connection address type */
    uint8_t identity_address_type;                      /**< Identity address type */
    wiced_bt_device_address_t conn_bda;                 /**< Connection Bluetooth device address */
    wiced_bt_device_address_t identity_bd_address;      /**< Identity Bluetooth device address */
    lehs_peer_profiles_t peer_profiles;                 /**< Peer profiles */
    lehs_ase_data_t local_ase_data[LEHS_ASE_INDEX_MAX]; /**< Local ASE data */
    lehs_mcs_data_t mcs;                                /**< MCS data */
    lehs_gmap_data_t gmap;                              /**< GMAP data */
    gatt_intf_service_discovery_ctx_t *p_discovery_ctx; /**< service discovery context */
    gatt_intf_operation_t *p_op;                        /**< handle list operation context */
    uint32_t cccd_bits[LEHS_MAX_DWORD_TO_STORE_CCCD(
        LEHS_MAX_CCCD_TO_STORE)]; /**< To store the CCCD bits of the peer device for future reference */
    wiced_bt_db_hash_t db_hash;   /**< To store the DB hash of the peer device for future reference */
    wiced_bt_gatt_client_supported_features_t
        csf; /**< To store the Client Supported Features of the peer device for future reference */
} lehs_clcb_t;

/** Volume Control Service (VCS) data structure */
typedef struct
{
    ga_lib_vcs_volume_state_t state; /**< current volume state */
    uint8_t flag;                    /**< current volume flag */
} lehs_vcs_t;

/** Audio Input Control Service(AICS) data structure */
typedef struct
{
    uint16_t gatt_value_handle;                      /**< Attribute handle */
    ga_lib_aics_input_state_t input_state;           /**< Audio Input State */
    ga_lib_aics_gain_settings_params_t gain_setting; /**< Audio Input Gain setting */
    ga_lib_aics_input_type_t input_type;             /**< Audio Input Type */
    ga_lib_aics_input_status_t input_status;         /**< Audio Input Status */
    char description[MAX_DESCRIPTION];               /**< Audio Input description */
    uint8_t description_len;                         /**< Audio Input description length */
} lehs_aics_t;

/** Microphone Control Service(MICS) data structure */
typedef struct
{
    uint8_t mute_state; /**< Mute state */
} lehs_mics_t;

/** Broadcast Audio Scan Service (BASS) data structure */
typedef struct
{
    wiced_bool_t is_used;                                                   /**< True if this instance is in use */
    uint16_t conn_id;                                                       /**< Connection ID */
    wiced_bool_t waiting_broadcast_code;                                    /**< True if waiting for broadcast code */
    uint16_t sync_handle;                                                   /**< Sync handle of periodic adv data */
    uint32_t bis_index_bits;                                                /**< BIS index bits */
    ga_lib_bass_receive_state_t recv_state;                                 /**< Receive state */
    ga_lib_bass_sub_group_data_t sub_group[GA_LIB_BASS_MAX_SUBGROUP_COUNT]; /**< Subgroup Data */
} lehs_bass_data_t;

/** BASS data structure */
typedef struct
{
    lehs_bass_data_t bass_data[LEHS_BROADCAST_RCV_STATE_MAX]; /**< BASS data for each broadcast receive state */
    ga_lib_bass_operation_t op_data;                          /**< BASS operation data */
} lehs_bass_t;

/** Volume Offset Control Service (VOCS) data structure */
typedef struct
{
    int16_t volume_offset;     /**< VOCS offset value */
    uint8_t change_counter;    /**< VOCS change counter */
    uint32_t audio_location;   /**< Audio location value */
    char vocs_description[20]; /**< VOCS description */
} lehs_vocs_t;

/** Coordinated Set Identification Service (CSIS) data structure */
typedef struct
{
    ga_lib_csis_sirk_data_t sirk_data; /**< CSIS SIRK data */
    uint8_t size;                      /**< CSIS set size */
    uint8_t rank;                      /**< CSIS member rank */
    uint16_t conn_id_of_lock_owner;    /**< CSIS connection ID of lock owner */
    ga_lib_csis_lock_val_t lock;       /**< CSIS lock value */
    wiced_timer_t lock_timer;          /**< CSIS lock timer */
} lehs_csis_data_t;

/**HAS Preset Record data structure */
typedef struct
{
    ga_lib_has_preset_records_t rec;                     /**< HAS preset record data */
    uint8_t name[MAX_HAS_PRESET_RECORD_NAME_LENGTH + 1]; /**< HAS preset record name */
} lehs_has_preset_rec_t;

/** Hearing Aid Service (HAS) data structure */
typedef struct
{
    uint8_t procedure_in_progress; /**< Procedure in progress */
    uint8_t hearing_aid_features;  /**< Hearing aid features */
    uint8_t active_preset_index;   /**< Active preset index */
} lehs_has_data_t;

/** Audio Stream Control Service (ASCS) data structure */
typedef struct
{
    uint16_t min_data_per_frame; /**< Min data per codec frame */
    uint16_t max_data_per_frame; /**< Max data per codec frame */
    uint16_t sf;                 /**< Sampling frequency */
    uint8_t frame_duration;      /**< Frame duration */
    uint8_t audio_ch_count;      /**< Audio channel count */
    uint8_t frame_per_sdu;       /**< Frame per sdu */
    uint16_t octet_per_frame;    /**< Octects per frame */
    uint16_t blocks_per_sdu;     /**< Blocks per sdu */
} lehs_utils_pacs_t;

/** Local service data structure */
typedef struct
{
    lehs_vcs_t vcs;                       /**< local volume data */
    lehs_vocs_t vocs;                     /**< local vocs data */
    lehs_mics_t mics;                     /**< local mics data */
    lehs_aics_t mics_aics[MAX_MICS_AICS]; /**< local mics aics data */
    lehs_bass_t bass;                     /**< local bass data */
    lehs_csis_data_t csis;                /**< local csis data */
    lehs_gmap_data_t gmap;                /**< local gmap data */
    lehs_has_data_t has;                  /**< local has data */
} lehs_local_service_data_t;

/**< Broadcast Sink Control Block */
struct lehs_broadcast_sink_cb_
{
    // app info
    wiced_bool_t in_use;               /**< True if it has been allocated for any BIG */
    uint8_t big_handle;                /**< big handle of BIG */
    uint8_t adv_handle;                /**< adv handle of BIG */
    wiced_bt_device_address_t bd_addr; /**< device address of broadcast source */
    wiced_bool_t b_base_updated;       /**< True if received periodic adv data and updated base information correctly */
    wiced_bool_t b_biginfo_updated;    /**< True if received big_info(information about BIG) adv */
    uint8_t sync_state;                /**< Synchronization state */

    // controller info
    uint8_t pa_sync_in_progress;              /**< True if periodic adv sync is in progress */
    wiced_ble_padv_sync_handle_t sync_handle; /**< Sync handle of periodic adv data */
    wiced_bool_t big_sync_in_progress;        /**< True if synchronization is in progress */
    uint8_t number_of_subevents;              /**< for sink only (received in BIGInfo) */
    uint16_t iso_interval;                    /**< ISO interval */
    uint8_t bis_conn_id_count;                /**< Total BISes present in BIG (Sum of number of bis in all subgroups) */
    uint16_t bis_conn_id_list[BROADCAST_MAX_BIS_PER_SUB_GROUP *
                              BROADCAST_MAX_SUB_GROUP]; /**< Stores conn_hdl of all BISes */
    uint8_t
        bis_index_list[BROADCAST_MAX_BIS_PER_SUB_GROUP * BROADCAST_MAX_SUB_GROUP]; /**< Stores bis index of all BISes */
    wiced_bool_t b_encryption;                  /**< True for encrypted broadcast audio stream */
    ga_lib_bap_broadcast_code_t broadcast_code; /**< Stores Broadcast code if encrypted streaming */

    // profile info
    le_audio_bap_broadcast_base_t base; /**< Stores base information */
};

/**< Broadcast Source Control Block */
typedef struct
{
    uint32_t broadcast_id;                      /**< Broadcast ID of the broadcast source */
    ga_lib_bap_broadcast_code_t broadcast_code; /**< Broadcast code of the broadcast source */
    uint32_t bis_index_bits;                    /**< BIS index bits of the broadcast source that to be synchronized */
} broadcast_source_t;

/**< GATT Control Block */
typedef struct
{
    wiced_bt_db_hash_t db_hash;                   /**< To store the DB hash of the local device for future reference */
    lehs_local_service_data_t local_service_data; /**< Local service data */
    lehs_clcb_t clcb[LEHS_MAX_CONNECTIONS];       /**< Number of simultaneous connections */
    lehs_broadcast_sink_cb_t broadcast_sink_cb[LEHS_MAX_BIG]; /**< Broadcast Sink Control Block */
    wiced_bool_t broadcast_sink_periodic_sync_in_progress;    /**< True if synchronization is in progress */
    broadcast_source_t broadcast_source;                      /**< Broadcast Source Control Block */
    adv_state_t adv_state;                                    /**< Advertising state */
    uint16_t adv_data_options;                                /**< Advertising data options */
    uint8_t adv_tx_power;                                     /**< Advertising transmit power */
    uint8_t do_swift_pair;                                    /**< Swift pair flag */
} lehs_gatt_cb_t;

extern lehs_gatt_cb_t g_lehs_gatt_cb;
extern ga_lib_pacs_data_t g_lehs_pacs_app_data;

/*=================================================== GATT =============================================================*/
/*
* @brief Returns the reference to the configuration settings structure.
*
* @return wiced_bt_cfg_settings_t*: Pointer to the configuration settings structure
*/
extern wiced_bt_cfg_settings_t *app_get_cfg_settings(void);

/*
* @brief GATT initialization function
*
* @param[in] max_connections: Maximum number of simultaneous connections supported
* @return wiced_bt_gatt_status_t: Status of the GATT initialization operation
*/
wiced_bt_gatt_status_t lehs_gatt_init(int max_connections);

/*
* @brief Initiates GATT service discovery for the connected peer device
*
* @param[in] p_clcb: Reference to the connection link control block
*/
void lehs_gatt_start_discovery(lehs_clcb_t *p_clcb);

/*
* @brief Starts or stops advertising based on the input parameters
*
* @param[in] b_start: Flag indicating whether to start (1) or stop (0) advertising
* @param[in] adv_state: The advertising state to be set when starting advertising
*/
void lehs_gatt_start_stop_adv(uint32_t b_start, adv_state_t adv_state);

/*
* @brief Moves to the next advertising state based on the current state and the source of the request
*
* @param[in] adv_state: The current advertising state
* @param[in] from: The source of the request (e.g., "app", "btm", etc.)
* @return adv_state_t: The next advertising state
*/
adv_state_t lehs_move_to_next_adv_state(adv_state_t adv_state, char *from);

/*
* @brief Returns the connection link control block for the given Bluetooth device address
*
* @param[in] p_bd_addr: The Bluetooth device address of the peer device
* @return lehs_clcb_t*: Pointer to the connection link control block, or NULL if not found
*/
lehs_clcb_t *lehs_gatt_get_clcb(uint8_t *p_bd_addr);

/*
* @brief Returns the connection link control block for the given connection ID
*
* @param[in] conn_id: The connection ID of the device
* @return lehs_clcb_t*: Pointer to the connection link control block, or NULL if not found
*/
lehs_clcb_t *lehs_gatt_get_clcb_by_conn_id(uint16_t conn_id);

/*
* @brief Returns the connection link control block for the given ACL connection handle
*
* @param[in] acl_conn_handle: The ACL connection handle of the device
* @return lehs_clcb_t*: Pointer to the connection link control block, or NULL if not found
*/
lehs_clcb_t *lehs_gatt_get_clcb_by_conn_handle(uint16_t acl_conn_handle);

/*
* @brief Creates a connection to a remote device.
*
* @param[in] addr_type: The address type of the remote device
* @param[in] bd_addr: The Bluetooth device address of the remote device
* @return wiced_result_t: Result of the connection creation operation
*/
wiced_result_t app_create_connection(uint8_t addr_type, wiced_bt_device_address_t bd_addr);

/*
* @brief Disconnects the device with the given connection ID.
*
* @param[in] conn_id: The connection ID of the device to be disconnected
* @return wiced_bt_gatt_status_t: Result of the disconnect operation
*/
wiced_bt_gatt_status_t lehs_disconnect_device(uint16_t conn_id);

/*
* @brief Reads remote characteristics from the peer device and stores them in the lehs_clcb_t structure.
*
* @param[in] p_clcb: Reference to the connection link control block
* @return wiced_result_t: Result of the read operation
*/
wiced_result_t lehs_read_remote_characteristics(lehs_clcb_t *p_clcb);

/*
* @brief Sends device info to client control for storage in NVRAM.
*
* @param[in] p_clcb: Pointer to the connection link control block
*/
void lehs_save_device_data_to_nvram(lehs_clcb_t *p_clcb);

/*
* @brief Handles Bluetooth management callbacks.
*
* @param[in] event: The Bluetooth management event received from the controller
* @param[in] p_event_data: Reference to the Bluetooth management event data
* @return wiced_result_t: Result of the callback handling
*/
wiced_result_t lehs_btm_cback(wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data);
/*======================================================================================================================*/

/***************************************PROFILE SPECIFIC FUNCTIONS*******************************************************/

/*==================================================== ASCS ============================================================*/
/*
* @brief Initializes the local ASE data for the given connection link control block
*
* @param[in] p_clcb: Pointer to the connection link control block
*/
void lehs_init_local_ase_data(lehs_clcb_t *p_clcb);

/*
* @brief Handles write requests for the ASCS control point
*
* @param[in] conn_id: The connection ID of the peer device
* @param[in] p_clcb: Pointer to the connection link control block
* @param[in] p_stream: Pointer to the data stream containing the write request
* @param[in] length: Length of the data stream
* @return wiced_result_t: Result of the write request handling
*/
wiced_result_t lehs_ascs_handle_write_req_evt(uint16_t conn_id, lehs_clcb_t *p_clcb, uint8_t *p_stream, int length);
/*======================================================================================================================*/

/*==================================================== BAP-BIS =========================================================*/

/*
* @brief Handles Isoc events received from controller
*
* @param[in] event: The Isoc event received from controller
* @param[in] p_event_data: Pointer to the Isoc event data
*/
void lehs_isoc_event_handler(wiced_ble_isoc_event_t event, wiced_ble_isoc_event_data_t *p_event_data);

/*
* @brief Handles extended advertising events received from controller
*
* @param[in] event: The extended advertising event received from controller
* @param[in] p_ed: Pointer to the extended advertising event data
*/
void lehs_ext_adv_cback(wiced_ble_ext_adv_event_t event, wiced_ble_ext_adv_event_data_t *p_ed);

/*
* @brief Handles BIG events received from controller
*
* @param[in] event: The BIG event received from controller
* @param[in] p_ed: Pointer to the BIG event data
*/
void lehs_bis_isoc_cb(wiced_ble_isoc_event_t event, wiced_ble_isoc_event_data_t *p_ed);

/*
* @brief Discovers available broadcast sources
*
* @param[in] start: True to start discovery, False to stop discovery
*/
void lehs_bis_discover_sources(uint8_t start);

/*
* @brief Clears the broadcast sink control block data
*/
void broadcast_sink_clear_data();

/*
* @brief Synchronizes to the periodic advertising having base data with the given broadcast ID
*
* @param[in] broadcast_id: The broadcast ID of the periodic advertising
*/
void lehs_sync_to_pa(uint32_t broadcast_id);

/*
* @brief Returns the synchronization progress state of the broadcast sink
*
* @return wiced_bool_t: True if synchronization is in progress, False otherwise
*/
wiced_bool_t lehs_broadcast_get_sync_progress(void);

/*
* @brief Synchronizes to the Broadcast streams of the specified broadcast source
*
* @param[in] source: The broadcast source to be synchronized
*/
void lehs_bis_sync_to_source(broadcast_source_t source);

/*
* @brief Terminates the synchronization to the periodic advertising/Broadcast Streams having base data with the given broadcast ID
*
* @param[in] broadcast_id: The broadcast ID of the periodic advertising
*/
void lehs_bis_terminate_sync(uint32_t broadcast_id);

/*
* @brief Returns the broadcast sink control block for the given broadcast ID
*
* @param[in] br_id: The broadcast ID of the periodic advertising
* @return lehs_broadcast_sink_cb_t*: Reference to the broadcast sink control block
*/
lehs_broadcast_sink_cb_t *lehs_bis_get_big_by_broadcast_id(uint32_t br_id);

/*
* @brief Allocates a new broadcast sink control block for the given broadcast ID
*
* @param[in] broadcast_id: The broadcast ID of the periodic advertising
* @param[in] bd_addr: The Bluetooth device address of the broadcast source
* @param[in] adv_sid: The advertising SID of the periodic advertising
* @return lehs_broadcast_sink_cb_t*: Reference to the allocated broadcast sink control block
*/
lehs_broadcast_sink_cb_t *lehs_bis_alloc_big(uint32_t broadcast_id, wiced_bt_device_address_t bd_addr, uint8_t adv_sid);

/*
* @brief Frees the broadcast sink control block
*
* @param[in] p_big: Reference to the broadcast sink control block to be freed
*/
void lehs_bis_free_big(lehs_broadcast_sink_cb_t *p_big);

/*
* @brief Returns the broadcast sink control block for the given synchronization handle
*
* @param[in] sync_handle: The synchronization handle of the periodic advertising
* @return lehs_broadcast_sink_cb_t*: Reference to the broadcast sink control block
*/
lehs_broadcast_sink_cb_t *lehs_bis_get_big_by_sync_handle(wiced_ble_padv_sync_handle_t sync_handle);

/*
* @brief Synchronizes to the Broadcast streams of the specified broadcast sink control block with the given BIS index bits
*
* @param[in] p_big: Reference to the broadcast sink control block
* @param[in] bis_index_bits: The BIS index bits to be synchronized
*/
void lehs_sync_to_source(lehs_broadcast_sink_cb_t *p_big, uint32_t bis_index_bits);
/*======================================================================================================================*/

/*==================================================== BASS ============================================================*/

/*
* @brief Notifies the specified device of the current PA sync state.
*
* @param[in] p_big: Pointer to the broadcast sink control block.
* @param[in] pa_sync_state: The current PA sync state to be notified.
*/
void lehs_bass_notify_pa_sync_state(lehs_broadcast_sink_cb_t *p_big, uint8_t pa_sync_state);

/*
* @brief Notifies the specified device that a BIG sync has been lost.
*
* @param[in] p_big: Pointer to the broadcast sink control block.
*/
void lehs_bass_notify_big_sync_state(lehs_broadcast_sink_cb_t *p_big);

/*
* @brief Returns the reference to the BASS data.
*
* @return lehs_bass_t*: Reference to the BASS data structure.
*/
lehs_bass_t *lehs_bass_get_bass_data();

/*
* @brief Handles write requests for the BASS control point
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] p_clcb: Pointer to the connection link control block.
* @param[in] p_evt_data: Pointer to the event data received in the write request.
* @param[in] len_to_write: The length of the data to be written.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_bass_handle_write_req_evt(uint16_t conn_id,
                                              lehs_clcb_t *p_clcb,
                                              uint8_t *p_evt_data,
                                              uint16_t len_to_write);

/*
* @brief Handles PA sync transfer parameter events for the BASS service
*
* @param[in] p_sync_param: Pointer to the PA sync transfer parameter event data.
*/
void lehs_bass_handle_pa_sync_transfer_param_evt(wiced_ble_set_padv_sync_transfer_param_event_data_t *p_sync_param);

/*
* @brief Handles periodic sync transfer events for the BASS service
*
* @param[in] p_sync_evt: Pointer to the periodic sync established event data.
*/

void lehs_bass_handle_periodic_sync_transfer_evt(wiced_ble_padv_sync_established_event_data_t *p_sync_evt);

/*
* @brief Synchronizes to the BIS stream
*
* @param[in] p_bass: Pointer to the BASS data structure.
*/
void lehs_bass_synchronize_to_source(lehs_bass_data_t *p_bass);

/*
* @brief Synchronizes to the PA sync
*
* @param[in] pa_sync_param: The PA sync parameters.
* @param[in] p_bass: Pointer to the BASS data structure.
*/
void lehs_bass_synchronize_to_pa(ga_lib_bass_pa_sync_param_t pa_sync_param, lehs_bass_data_t *p_bass);

/*
* @brief Processes the BIG advertising report and synchronizes to the BIG stream
*
* @param[in] p_big: Pointer to the broadcast sink control block.
* @param[in] p_bigrpt: Pointer to the BIG advertising report data.
*/
void lehs_bass_process_big_adv_report_and_sync(lehs_broadcast_sink_cb_t *p_big,
                                                wiced_ble_biginfo_adv_report_t *p_bigrpt);

/*
* @brief Handles ACL disconnection events for the BASS service
*
* @param[in] conn_id: The connection ID of the GATT connection that was disconnected.
*/
void lehs_bass_handle_acl_disconnection(uint16_t conn_id);
/*======================================================================================================================*/

/*===================================================== CSIS ===========================================================*/
/*
* @brief Initializes the CSIS data.
*
* @param[in] set_size: The size of coordinated set.
* @param[in] member_rank: The rank of the member.
* @param[in] sirk_type: The type of the SIRK.
* @param[in] sirk: The SIRK data.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_csis_initialize_data(uint8_t set_size,
                                         uint8_t member_rank,
                                         ga_lib_csis_sirk_type_t sirk_type,
                                         const ga_lib_csis_sirk_t sirk);

/*
* @brief Returns the reference to the CSIS data.
*
* @return lehs_csis_data_t*: Reference to the CSIS data structure.
*/
lehs_csis_data_t *lehs_get_csis_data(void);

/*
* @brief Sets the size of the Coordinated set.
*
* @param[in] size: The size of the Coordinated set.
*/
void lehs_csis_set_size(uint8_t size);

/*
* @brief Sets the rank of the Coordinated set member.
*
* @param[in] rank: The rank of the Coordinated set member.
*/
void lehs_csis_set_rank(uint8_t rank);

/*
* @brief Sets the SIRK of the Coordinated set.
*
* @param[in] p_sirk: Reference to the SIRK data structure to be set.
*/
void lehs_csis_set_sirk(ga_lib_csis_sirk_data_t *p_sirk);

/*
* @brief Returns SIRK of the Coordinated set.
*
* @return ga_lib_csis_sirk_data_t*: Reference to the SIRK data structure.
*/
ga_lib_csis_sirk_data_t *lehs_csis_get_sirk(void);

/*
* @brief Encrypts the SIRK for the given device address
*
* @param[in] bdaddr: The Bluetooth device address for which the SIRK needs to be encrypted.
* @param[out] p_sirk_encrypted: Pointer to the structure where the encrypted SIRK will be stored.
*/
void lehs_encrypt_sirk(wiced_bt_device_address_t bdaddr, ga_lib_csis_sirk_t *p_sirk_encrypted);

/*
* @brief Handles write requests for the CSIS control point
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] p_clcb: Pointer to the connection link control block.
* @param[in] p_evt_data: Pointer to the event data received in the write request.
* @param[in] len_to_write: The length of the data to be written.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_csis_handle_write_req_evt(uint16_t conn_id,
                                              lehs_clcb_t *p_clcb,
                                              uint8_t *p_evt_data,
                                              uint16_t len_to_write);
/*======================================================================================================================*/

/*===================================================== GMAP ===========================================================*/
/*
* @brief Initializes the GMAP data
*/
void lehs_gmap_init();

/*
* @brief Handles read complete events for the GMAP service
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] p_clcb: Pointer to the connection link control block.
* @param[in] p_gatt_data: Pointer to the GATT data received in the read complete event.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_gmap_app_handle_read_complete(uint16_t conn_id,
                                                  lehs_clcb_t *p_clcb,
                                                  wiced_bt_gatt_data_t *p_gatt_data);
/*======================================================================================================================*/

/*===================================================== GMCS ===========================================================*/

/*
* @brief Handles media control service play/pause requests.
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] play: A boolean value indicating whether to play (WICED_TRUE) or pause (WICED_FALSE) the media.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_mcs_play_pause(uint16_t conn_id, wiced_bool_t play);

/*
* @brief Handles GATT read complete events for the GMCS service
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] p_clcb: Pointer to the connection link control block.
* @param[in] p_gatt_data: Pointer to the GATT data received in the read complete event.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_gmcs_app_handle_read_complete(uint16_t conn_id,
                                                  lehs_clcb_t *p_clcb,
                                                  wiced_bt_gatt_data_t *p_gatt_data);

/*
* @brief Handles read complete events for the GMCS service
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] p_clcb: Pointer to the connection link control block.
* @param[in] p_gatt_data: Pointer to the GATT data received in the read complete event.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_gmcs_app_handle_read_complete(uint16_t conn_id,
                                                  lehs_clcb_t *p_clcb,
                                                  wiced_bt_gatt_data_t *p_gatt_data);
/*======================================================================================================================*/

/*====================================================== GTBS ==========================================================*/

/*
* @brief Handles read complete events for the GTBS service
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] p_clcb: Pointer to the connection link control block.
* @param[in] p_gatt_data: Pointer to the GATT data received in the read complete event.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_gtbs_app_handle_read_complete(uint16_t conn_id,
                                                  lehs_clcb_t *p_clcb,
                                                  wiced_bt_gatt_data_t *p_gatt_data);

/*
* @brief Handles RPC call control point actions
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] p_clcb: Pointer to the connection link control block.
* @param[in] call_id: The call ID for the RPC action.
* @param[in] opcode: The opcode for the RPC action.
* @return void
*/
void lehs_rpc_call_control_point_action(uint16_t conn_id, lehs_clcb_t *p_clcb, uint8_t call_id, uint8_t opcode);
/*======================================================================================================================*/

/*====================================================== HAS ===========================================================*/
/*
* @brief Initializes the HAS data.
*/
void lehs_has_initialize_data(void);

/*
* @brief Handles the read request for the HAS preset control point.
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] start_index: The starting index of the preset records to be read.
* @param[in] num_presets: The number of preset records to be read.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_has_handle_read_preset_record(uint16_t conn_id, uint8_t start_index, uint8_t num_presets);

/*
* @brief Sets the active preset index to preset index and notifies to the client.
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] preset_index: The index of the preset to be set as active.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_has_set_active_preset(uint16_t conn_id, uint8_t preset_index);

/*
* @brief Synchronizes the preset index if synchronization is supported and notifies to the client.
*
* @param[in] opcode: The opcode for the preset synchronization operation.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_has_set_preset_synchronization(uint8_t opcode);

/*
* @brief Handles write requests for the preset name
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] preset_index: The index of the preset to write the name for.
* @param[in] p_data: Pointer to the data to be written.
* @param[in] len_to_write: The length of the data to be written.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_has_handle_write_preset_name(uint16_t conn_id,
                                                 uint8_t preset_index,
                                                 uint8_t *p_data,
                                                 uint8_t len_to_write);

/*
* @brief Handles write requests for the set next preset
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] incr: The increment value for the preset index.
* @return wiced_result_t: The result of the operation.
*/
wiced_result_t lehs_has_set_next_preset(uint16_t conn_id, int incr);
/*======================================================================================================================*/

/*===================================================== MICS ===========================================================*/
/*
* @brief Initializes the MICS data.
*/
void lehs_mics_initialize_data(void);

/*
* @brief Sets the mute state of the microphones and notifies to the client.
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] mute: The mute state to be set.
* @return wiced_bt_gatt_status_t: The GATT status of the operation.
*/
wiced_bt_gatt_status_t lehs_mics_mute(uint16_t conn_id, uint8_t mute);

/*
* @brief Sets the mute state of the individual microphone(AICS) and notifies to the client.
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] instance: The instance of the AICS to be set.
* @param[in] mute: The mute state to be set.
* @return wiced_bt_gatt_status_t: The GATT status of the operation.
*/
wiced_bt_gatt_status_t lehs_mics_aics_mute(uint16_t conn_id, uint32_t instance, uint8_t mute);

/*
* @brief Sets the input gain of the individual microphone(AICS) and notifies to the client.
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] instance: The instance of the AICS to be set.
* @param[in] opcode: The opcode for the gain setting operation.
* @param[in] input_gain: The input gain value to be set.
* @return wiced_bt_gatt_status_t: The GATT status of the operation.
*/
wiced_bt_gatt_status_t lehs_mics_aics_set_gain(uint16_t conn_id, uint32_t instance, uint8_t opcode, int8_t input_gain);
/*======================================================================================================================*/

/*===================================================== PACS ===========================================================*/
/*
* @brief Initializes the local PACS data
*/
void lehs_pacs_init_data(void);

/*
* @brief Sets the audio location for the device.
*
* @param[in] audio_location: The audio location value to be set.
*/
void lehs_set_audio_location(uint32_t audio_location);

/*
* @brief Verifies codec configuration parameters against PACS capabilities for source ASEs
*
* @param[in] p_cc: Reference to the codec configuration arguments.
* @return wiced_bool_t: WICED_TRUE if the codec configuration is valid, WICED_FALSE otherwise.
*/
wiced_bool_t lehs_pacs_verify_src_codec_config(ga_lib_ascs_config_codec_args_t *p_cc);

/*
* @brief Verifies codec configuration parameters against PACS capabilities for sink ASEs
*
* @param[in] p_cc: Reference to the codec configuration arguments.
* @return wiced_bool_t: WICED_TRUE if the codec configuration is valid, WICED_FALSE otherwise.
*/
wiced_bool_t lehs_pacs_verify_snk_codec_config(ga_lib_ascs_config_codec_args_t *p_cc);

/*
* @brief Verifies context type against PACS capabilities for sink ASEs
*
* @param[in] ase_type: The ASE type (source or sink).
* @param[in] req_context: The requested context type.
* @return wiced_bool_t: WICED_TRUE if the requested context type is supported, WICED_FALSE otherwise.
*/
wiced_bool_t lehs_pacs_verify_context_type(uint8_t ase_type, uint16_t req_context);

/*
* @brief Verifies audio location against PACS capabilities for sink ASEs
*
* @param[in] ase_type: The ASE type (source or sink).
* @param[in] req: The requested audio location.
* @return wiced_bool_t: WICED_TRUE if the requested audio location is supported, WICED_FALSE otherwise.
*/
wiced_bool_t lehs_pacs_verify_audio_location(uint8_t ase_type, ga_lib_pacs_audio_location_t req);
/*======================================================================================================================*/

/*===================================================== VCS ============================================================*/
/*
* @brief Initializes the local VCS data
*/
void lehs_vcs_initialize_data(void);

/*
* @brief Sets the volume of the VCS and notifies to the client.
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] vcs_opcode: The volume control opcode to be performed.
* @param[in] abs_vol: The absolute volume value to be set.
* @return wiced_bt_gatt_status_t: The GATT status of the operation.
*/
wiced_bt_gatt_status_t lehs_vcs_set_volume(uint16_t conn_id,
                                           ga_lib_vcs_volume_control_opcodes_t vcs_opcode,
                                           uint8_t abs_vol);

/*
* @brief Handles write requests for the VCS control point
*
* @param[in] conn_id: The connection ID of the GATT connection.
* @param[in] p_data: Pointer to the data to be written.
* @param[in] len_to_write: Length of the data to be written.
* @return wiced_bt_gatt_status_t: The GATT status of the write operation.
*/
wiced_bt_gatt_status_t lehs_handle_vcs_cp_write(uint16_t conn_id, const uint8_t *p_data, uint16_t len_to_write);

/*======================================================================================================================*/

/************************************************************************************************************************/

#endif /* LEHS_H */
