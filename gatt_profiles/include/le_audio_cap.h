/*
 * $ Copyright 2022-YEAR Cypress Semiconductor $
 */

#ifndef GA_LIB_CAP_H
#define GA_LIB_CAP_H

#include "wiced_bt_isoc.h"
#include "wiced_bt_types.h"
#include "gatt_interface.h"
#include "ga_lib_ascs.h"
#include "ga_lib_pacs.h"
#include "ga_lib_mcs.h"
#include "ga_lib_vcs.h"
#include "ga_lib_aics.h"
#include "ga_lib_bass.h"
#include "ga_lib_gmap.h"
#include "ga_lib_vocs.h"
#include "ga_lib_aics.h"

/**
 * @addtogroup Stream_Control_APIs
 * @{
 */

/**
 * @addtogroup ga_lib_cap
 * @{
 */

#define GA_LIB_CAP_DIRECTION_SRC 1 /**< The device is acting in source role */
#define GA_LIB_CAP_DIRECTION_SNK 2 /**< The device is acting in sink role */

#define MAX_NUM_ASE_ID 5 /**< Maximum number of ASEs supported */

#ifdef CAP_DEBUG
#define CAP_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get CAP library traces */
#define CAP_TRACE_CRIT(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get CAP library traces */
#else
#define CAP_TRACE(...)
#define CAP_TRACE_CRIT(...)
#endif

/**
* @brief CAP Events
*/
enum le_audio_cap_event_t
{
    GA_LIB_CAP_STATE_CHANGED_EVENT, /**< CAP ASE State  */
    GA_LIB_CAP_ERROR_EVENT,         /**< CAP Error Event */
};

typedef uint8_t le_audio_cap_event_t; /**< CAP Events (see #le_audio_cap_event_t) */

/**
* @brief CAP Device data
*/
typedef struct
{
    uint16_t conn_id;                   /**< Connection Id */
    ga_lib_pacs_data_t *p_pacs_data; /**< PACS Data */
    uint8_t num_ase;                    /**< Number of ASE ID */
    uint8_t ase_notification_count;     /**< Number of Notifications which has been sent to ASEs */
    ga_lib_ascs_ase_t *ascs_data[MAX_NUM_ASE_ID]; /**< ASCS Data */
    ga_lib_vcs_volume_state_t *vcs_data;          /**< VCS Data */
    ga_lib_mute_val_t mics_mute_val;              /**< MICS Mute State */
} le_audio_cap_device_data_t;

/**
* @brief CAP APP data
*/
typedef struct
{
    wiced_bool_t is_bonded;                          /**< Is device bonded */
    uint8_t num_devices;                             /**< Number of Devices */
    le_audio_cap_device_data_t *device_info_list; /**< Device Info List */
    wiced_bool_t is_disconnecting;                   /**< DO NOT SET IN APP. Used by profile internally */
    wiced_bool_t is_disabling;                       /**< DO NOT SET IN APP. Used by profile internally */
} le_audio_cap_app_data_t;

/**
* @brief CAP Event Data
*/
typedef struct {
    le_audio_cap_app_data_t *p_app_data;                /**< App data */
    ga_lib_ascs_state_t group_state;         /**< CAP Group state */
} le_audio_cap_event_data_t;

/**
* @brief CAP Start Unicast Streaming Data
*/
typedef struct
{
    ga_lib_bap_context_type_t context_type;               /**< Targeted Context Type */
    ga_lib_ascs_metadata_t metadata; /**< Metadata */
    uint8_t dir;                         /**< Direction of the stream */
    uint8_t num_of_cis;                  /**< Number of CIS */
    ga_lib_ascs_config_codec_args_t *p_codec_configuration; /**< Targeted Codec Configuration */
    ga_lib_ascs_config_qos_args_t *p_qos_configuration;     /**< Targeted QOS Configuration */
} le_audio_cap_start_unicast_param_t;

/**
* @brief Codec configuration
*/
typedef struct
{
    uint16_t min_data_per_frame; /**< Min data per codec frame */
    uint16_t max_data_per_frame; /**< Max data per codec frame */
    uint16_t sf;                 /**< Sampling frequency */
    uint8_t frame_duration;      /**< Frame duration */
    uint8_t audio_ch_count;      /**< Audio channel count */
    uint8_t frame_per_sdu;       /**< Frame per sdu */
    uint16_t octet_per_frame;    /**< Octets per frame */
    uint16_t blocks_per_sdu;     /**< Blocks per sdu */
} le_audio_cap_codec_param_t;


/** CAP Start Broadcast Streaming Data */
typedef struct
{
    wiced_ble_isoc_phy_t phy;            /**< ISOC LE PHY */
    wiced_ble_isoc_packing_t packing;    /**< ISOC packing methods  */
    wiced_ble_isoc_framing_t framing;    /**< ISOC Framing types */
    wiced_ble_isoc_encryption_t encrypt; /**< ISOC Encryption */
    uint8_t max_transport_latency;      /**< Maximmum Transport Latency */
    uint8_t rtn;                        /**<Retransmission Number */
    uint32_t sdu_interval;              /**<SDU Interval */
    uint16_t max_sdu;                   /**< MAX SDU to be used */
    uint8_t *broadcast_code;            /**< Broadcast Code */
} le_audio_cap_start_broadcast_param_t;

/**
 * @brief CAP event callback
 *
 * Callback for CAP event notification
 * Registered using #le_audio_cap_register_cb
 *
 * @param conn_id  : GATT connection id
 * @param event             : Event ID
 * @param p_event_data      : Event data
 *
 * @return none
 */
typedef void le_audio_cap_cback_t(uint16_t conn_id, le_audio_cap_event_t event, le_audio_cap_event_data_t *p_event_data);

/**
 *
 *
 * @brief:      CAP Register event callback handler
 *
 * @param[in]   cb_ptr  : CAP event callback
 *
 * @return      None
 *
 */
void le_audio_cap_register_cb(le_audio_cap_cback_t *cb_ptr);

/**
 *
 * @brief           CAP Start Unicast Streaming procedure
 *                   The ASEs go to the \ref GA_LIB_ASCS_STATE_ENABLING state on successful completion the procedure
 *
 * State Transition: Current State -------------------->  Final State
 *                   Idle->CODEC_Configured->QoS_Configured->Enabling
 *
 * @param[in]        app_data : App device list info data pointer
 * @param[in]        params   : start unicast param data pointer
 *
 * @return           wiced_result_t
 *
 */
wiced_result_t le_audio_cap_start_unicast_streaming(le_audio_cap_app_data_t *app_data,
                                                       le_audio_cap_start_unicast_param_t *params);

/**
 *
 * @brief          CAP Update Unicast Streaming procedure
 *
 * @param[in]       app_data : App device list info data pointer
 * @param[in]       unicast_param : unicast params to be used
 * @param[in]       context_type : Context type
 * @param[in]       metadata : Metadata
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t le_audio_cap_update_streaming(le_audio_cap_app_data_t *app_data,
                                                le_audio_cap_start_unicast_param_t *unicast_param,
                                                ga_lib_bap_context_type_t context_type,
                                                ga_lib_ascs_metadata_t *metadata);

/**
 * @brief            CAP Stop Unicast Streaming procedure
 *                    The stream reverts to the \ref GA_LIB_ASCS_STATE_QOS_CONFIGURED state on successful completion of the procedure
 *
 * State Transitions: Current State --------> Final State
 *                    Enabling->Disabling->QoS_Configured
 *                    Streaming->Disabling->QoS_Configured
 *
 * @param[in]         app_data : App device list info data pointer
 *
 * @return            wiced_result_t
 *
 */
wiced_result_t le_audio_cap_disable_stream(le_audio_cap_app_data_t *app_data);

/**
 * @brief            CAP Stop Unicast Streaming procedure
 *                    The stream reverts to the \ref GA_LIB_ASCS_STATE_IDLE state on successful completion of the procedure
 *
 * State Transitions: Current State --------> Final State
 *                    Streaming->Releasing->Idle
 *                    Enabling->Releasing->Idle
 *                    QoS_Configured->Releasing->Idle
 *                    Codec_Configured->Releasing->Codec_Configured
 *
 * @param[in]         app_data : App device list info data pointer
 *
 * @return            wiced_result_t
 *
 */

wiced_result_t le_audio_cap_release_stream(le_audio_cap_app_data_t *app_data);

/**
 *
 * @brief          CAP Set Absolute Volume
 *
 * @param[in]       app_data         : App device list info data pointer
 * @param[in]       opcode           : volume control point opcode
 * @param[in]       abs_vol          : Volume (Range 0-255)
 *
 * @return      wiced_result_t
 *
 */
wiced_result_t le_audio_cap_set_volume(le_audio_cap_app_data_t *app_data,
                                       ga_lib_vcs_volume_control_opcodes_t opcode,
                                       uint8_t abs_vol);

/**
 *
 * @brief          Set absolute volume on all the devices mentioned in device_info_list
 *
 * @param[in]       app_data   : App device list info data pointer
 * @param[in]       mute_state : Mute State
 *
 * @return      wiced_result_t
 *
 */
wiced_result_t le_audio_cap_set_volume_mute_state(le_audio_cap_app_data_t *app_data,
                                                     ga_lib_mute_val_t mute_state);

/**
 * @brief          Set volume offset on all the devices mentioned in device_info_list
 *
 * @param[in]       app_data   : App device list info data pointer
 * @param[in]       volume_offset : Volume offset
 *
 * @return      wiced_result_t
 *
 */
wiced_result_t le_audio_cap_set_volume_offset(le_audio_cap_app_data_t *app_data, int16_t volume_offset);

/**
 *
 * @brief       Set Mute State on all the devices mentioned in device_info_list
 *
 * @param[in]       app_data   : App device list info data pointer
 * @param[in]       mute_state : Mute State
 *
 * @return      wiced_result_t
 *
 */
wiced_result_t le_audio_cap_set_mics_mute_state(le_audio_cap_app_data_t *app_data,
                                                   ga_lib_mute_val_t mute_state);

/**
 *
 *  @brief         Set MICS Input Gain on all the devices mentioned in device_info_list
 *
 * @param[in]       app_data   : App device list info data pointer
 * @param[in]       instance   : Included servive instance
 * @param[in]       mute_state : Mute State
 *
 * @return      wiced_result_t
 *
 */
wiced_result_t le_audio_cap_set_mics_aics_mute_state(le_audio_cap_app_data_t *app_data,
                                                     uint32_t instance,
                                                     ga_lib_mute_val_t mute_state);

/**
 *
 *  @brief         Set MICS Input Gain on all the devices mentioned in device_info_list
 *
 * @param[in]       app_data   : App device list info data pointer
 * @param[in]       instance   : Included servive instance
 * @param[in]       gain       : Gain in dB
 *
 * @return      wiced_result_t
 *
 */
wiced_result_t le_audio_cap_set_mics_aics_gain(le_audio_cap_app_data_t *app_data, uint32_t instance, int8_t gain);

/**@} ga_lib_cap */
/**@} Stream_Control_APIs */

#endif /* GA_LIB_CAP_H */
