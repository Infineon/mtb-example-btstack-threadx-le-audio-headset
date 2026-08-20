/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Audio Input Control Service implementation header
  */

#ifndef GA_LIB_AICS_H
#define GA_LIB_AICS_H

#include "gatt_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup Volume_And_Gain_Control_APIs
 * @{
 */

/**
 * @addtogroup ga_lib_aics
 * @{
 * @brief This profile exposes the settings of an audio input such as a Bluetooth audio stream, microphone, etc.
 * Multiple audio inputs may be combined as part of the server's audio mixing functionality.
 */

// AICS Characteristic index
#define GA_LIB_AICS_CHARACTERISTIC_INPUT_STATE 0             /**< AICS characteristic input state */
#define GA_LIB_AICS_CHARACTERISTIC_GAIN_SETTING_PROPERTIES 1 /**< AICS characteristic gain settings properties */
#define GA_LIB_AICS_CHARACTERISTIC_INPUT_TYPE 2              /**< AICS characteristic input type */
#define GA_LIB_AICS_CHARACTERISTIC_INPUT_STATUS 3            /**< AICS characteristic input status */
#define GA_LIB_AICS_CHARACTERISTIC_INPUT_CONTROL_POINT 4     /**< AICS characteristic input control point */
#define GA_LIB_AICS_CHARACTERISTIC_INPUT_DESCRIPTION 5       /**< AICS characteristic input description */
#define GA_LIB_AICS_CHARACTERISTIC_MAX 6                     /**< AICS characteristic max */
    typedef uint8_t ga_lib_aics_characteristics_t;           /**< AICS characteristic type */

    /** Audio Input Control Service opcodes */
#define GA_LIB_AICS_OPCODE_SET_GAIN_SETTINGS 1    /**< Set gain setting */
#define GA_LIB_AICS_OPCODE_SET_UNMUTE 2           /**< Set mute state to unmute */
#define GA_LIB_AICS_OPCODE_SET_MUTE 3             /**< Set mute state to mute */
#define GA_LIB_AICS_OPCODE_SET_MANUAL_GAIN_MODE 4 /**< Set gain mode to manual */
#define GA_LIB_AICS_OPCODE_SET_AUTO_GAIN_MODE 5   /**< Set gain mode to auto */
    typedef uint8_t ga_lib_aics_opcode_t;         /**< AICS control point opcode */

    /** AICS Mute mode */
#define GA_LIB_AICS_UNMUTE 0            /**< UnMuted */
#define GA_LIB_AICS_MUTE 1              /**< Muted */
#define GA_LIB_AICS_MUTE_DISABLED 2     /**< Mute Disabled */
    typedef uint8_t ga_lib_aics_mute_t; /**< AICS Mute mode */

    /** AICS Gain Mode */
#define GA_LIB_AICS_GAIN_MODE_MANUAL_ONLY 0 /**< Manual only */
#define GA_LIB_AICS_GAIN_MODE_AUTO_ONLY 1   /**< Automatic only*/
#define GA_LIB_AICS_GAIN_MODE_MANUAL 2      /**< Manual Gain Mode */
#define GA_LIB_AICS_GAIN_MODE_AUTO 3        /**< Auto Gain Mode */

    typedef uint8_t ga_lib_aics_gain_mode_t; /**< AICS Gain Mode */

    /** AICS Input Type */
#define GA_LIB_AICS_INPUT_TYPE_UNSPECIFIED 0 /**< Unspecified input */
#define GA_LIB_AICS_INPUT_TYPE_BLUETOOTH 1   /**< Bluetooth audio stream */
#define GA_LIB_AICS_INPUT_TYPE_MICROPHONE 2  /**< Microphone */
#define GA_LIB_AICS_INPUT_TYPE_ANALOG 3      /**< Analog interface */
#define GA_LIB_AICS_INPUT_TYPE_DIGITAL 4     /**< Digital interface */
#define GA_LIB_AICS_INPUT_TYPE_RADIO 5       /**< AM / FM / XM / etc */
#define GA_LIB_AICS_INPUT_TYPE_STREAMING 6   /**< Streaming audio source */

    typedef uint8_t ga_lib_aics_input_type_t;

    /** AICS Input Status */
#define GA_LIB_AICS_INPUT_STATUS_INACTIVE 0 /**< Input Status Inactive */
#define GA_LIB_AICS_INPUT_STATUS_ACTIVE 1   /**< Input Status Active */

    typedef uint8_t ga_lib_aics_input_status_t;

    /**
 * @brief AICS error code values
 */
#define GA_LIB_AICS_ERROR_INVALID_CHANGE_COUNTER 0x80 /**< Change counter value is invalid */
#define GA_LIB_AICS_ERROR_OPCODE_NOT_SUPPORTED 0x81   /**< Opcode is invalid*/
#define GA_LIB_AICS_ERROR_MUTE_DISABLED 0x82          /**< Mute Disabled */
#define GA_LIB_AICS_ERROR_VALUE_OUT_OF_RANGE 0x83     /**< Gain setting values is out of range */
#define GA_LIB_AICS_GAIN_MODE_CHANGE_NOT_ALLOWED 0x84 /**< Gain Mode Change Not allowed */

    /** AICS Gain Setting Attribute */
    typedef struct
    {
        uint8_t gain_setting_units; /**< Gain Settings Units : 1 Unit = 0.1db */
        int8_t max_gain_setting;    /**< Maximum Gain Setting */
        int8_t min_gain_setting;    /**< Minimum Gain Setting */
    } ga_lib_aics_gain_settings_params_t;

    /** AICS Input State */
    typedef struct
    {
        int8_t gain_setting;               /**< Gain Settings */
        ga_lib_aics_mute_t mute_mode;      /**< Mute Mode */
        ga_lib_aics_gain_mode_t gain_mode; /**< Gain Mode */
        uint8_t change_counter;            /**< change counter */
    } ga_lib_aics_input_state_t;

    /** AICS Control Point data */
    typedef struct
    {
        ga_lib_aics_opcode_t opcode;           /**< AICS control point opcode #ga_lib_aics_opcode_t */
        ga_lib_aics_input_state_t input_state; /**< AICS Input State */
    } ga_lib_aics_control_point_t;

    /**
 * @brief Discover Audio Input Control Service on the remote GATT server
 *
 * This function initiates GATT service discovery to find the Audio Input Control Service
 * and its characteristics on a connected remote device.
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] pfn_on_complete   Callback function to be invoked when service discovery completes
 * @param[in] start_handle      Starting handle for the discovery range
 * @param[in] end_handle        Ending handle for the discovery range
 *
 * @return Pointer to the service discovery context, or NULL on failure
 */
    gatt_intf_service_discovery_ctx_t *ga_lib_aics_discover_service(uint16_t conn_id,
                                                                    pfn_on_discovery_complete_t pfn_on_complete,
                                                                    uint16_t start_handle,
                                                                    uint16_t end_handle);

    /**
 * @brief Send notification for Audio Input Description characteristic
 *
 * This function sends a GATT notification to inform the client of changes to the
 * Audio Input Description characteristic value.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Audio Input Description characteristic
 * @param[in] p_desc    Pointer to the description data buffer
 * @param[in] desc_len  Length of the description data in bytes
 *
 * @return WICED_BT_GATT_SUCCESS if notification was sent successfully,
 *         otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_aics_notify_description(uint16_t conn_id,
                                                          uint16_t handle,
                                                          uint8_t *p_desc,
                                                          uint16_t desc_len);

    /**
 * @brief Send notification for Audio Input State characteristic
 *
 * This function sends a GATT notification to inform the client of changes to the
 * Audio Input State characteristic. The change counter in the input state is
 * automatically incremented (wraps to 0 after 0xFF).
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Audio Input State characteristic
 * @param[in,out] p_state Pointer to the input state structure. The change_counter
 *                        field will be incremented by this function.
 *
 * @return WICED_BT_GATT_SUCCESS if notification was sent successfully,
 *         otherwise a GATT error code
 *
 * @note The change_counter field in p_state is modified by this function
 */
    wiced_bt_gatt_status_t ga_lib_aics_notify_input_state(uint16_t conn_id,
                                                          uint16_t handle,
                                                          ga_lib_aics_input_state_t *p_state);

    /**
 * @brief Send notification for Audio Input Status characteristic
 *
 * This function sends a GATT notification to inform the client of changes to the
 * Audio Input Status characteristic (active/inactive state).
 *
 * @param[in] conn_id       Connection identifier for the GATT connection
 * @param[in] handle        Attribute handle of the Audio Input Status characteristic
 * @param[in] input_status  Input status value (GA_LIB_AICS_INPUT_STATUS_INACTIVE or
 *                          GA_LIB_AICS_INPUT_STATUS_ACTIVE)
 *
 * @return WICED_BT_GATT_SUCCESS if notification was sent successfully,
 *         otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_aics_notify_input_status(uint16_t conn_id,
                                                           uint16_t handle,
                                                           ga_lib_aics_input_status_t input_status);

    /**
 * @brief Write to Audio Input Control Point characteristic
 *
 * This function writes a control point command to the remote GATT server to control
 * the audio input (set gain, mute/unmute, change gain mode). The write includes the
 * opcode, change counter, and optionally the gain setting for SET_GAIN_SETTINGS opcode.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] p_handle  Pointer to characteristic handles structure containing the
 *                      control point value handle
 * @param[in] opcode    Control point opcode (see #ga_lib_aics_opcode_t)
 * @param[in] p_state   Pointer to input state containing change_counter and optionally
 *                      gain_setting (used only for GA_LIB_AICS_OPCODE_SET_GAIN_SETTINGS)
 *
 * @return WICED_BT_GATT_SUCCESS if write request was sent successfully,
 *         WICED_BT_GATT_NO_RESOURCES if buffer allocation failed,
 *         otherwise a GATT error code
 *
 * @note This function uses GATT_REQ_WRITE which requires a response from the server
 */
    wiced_bt_gatt_status_t ga_lib_aics_write_cp(uint16_t conn_id,
                                                gatt_intf_characteristic_handles_t *p_handle,
                                                uint8_t opcode,
                                                ga_lib_aics_input_state_t *p_state);

    /**
 * @brief Write to Audio Input Description characteristic
 *
 * This function writes a new description string to the Audio Input Description
 * characteristic on the remote GATT server using a write command (no response).
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] p_handle  Pointer to characteristic handles structure containing the
 *                      description value handle
 * @param[in] name      Pointer to the description string buffer
 * @param[in] name_len  Length of the description string in bytes
 *
 * @return WICED_BT_GATT_SUCCESS if write command was sent successfully,
 *         WICED_BT_GATT_NO_RESOURCES if buffer allocation failed,
 *         otherwise a GATT error code
 *
 * @note This function uses GATT_CMD_WRITE which does not require a response
 */
    wiced_bt_gatt_status_t ga_lib_aics_write_input_description(uint16_t conn_id,
                                                               gatt_intf_characteristic_handles_t *p_handle,
                                                               uint8_t *name,
                                                               uint16_t name_len);

    /**@} ga_lib_aics */
    /**@} Volume_And_Gain_Control_APIs */

#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_AICS_H */
