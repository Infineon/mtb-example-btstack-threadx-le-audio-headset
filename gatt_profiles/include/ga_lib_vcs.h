/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Volume Control Profile implementation header
  */

#ifndef GA_LIB_VCS_H
#define GA_LIB_VCS_H

#include "gatt_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief VCS characteristic enumeration
     */
#define GA_LIB_VCS_CHARACTERISTIC_VOLUME_STATE                                                                         \
    0 /**< Volume State characteristic (read/notify) - volume setting, mute state, change counter */
#define GA_LIB_VCS_CHARACTERISTIC_CONTROL_POINT 1 /**< Volume Control Point characteristic (write) - control commands */
#define GA_LIB_VCS_CHARACTERISTIC_VOLUME_FLAG 2 /**< Volume Flag characteristic (read/notify) - persisted state flag */
#define GA_LIB_VCS_CHARACTERISTIC_MAX 3         /**< Number of VCS characteristics */
    typedef uint8_t ga_lib_vcs_characteristics_t; /**< volume characteristics type */

/**
 * @brief VCS error code values
 */
#define GA_LIB_VCS_ERROR_INVALID_CHANGE_COUNTER 0x80 /**< Change counter value is invalid*/
#define GA_LIB_VCS_ERROR_OPCODE_NOT_SUPPORTED 0x81   /**< Opcode is invalid*/

/**
 * @addtogroup Volume_And_Gain_Control_APIs
 * @{
 * @brief This profile enables a device to expose the controls and state of a device that can control the volume of an audio output such as one or more speakers and Control the peer device audio output as a client.
 * VCS may include zero or more instances of VOCS, AICS.
 * VCP which is the client side of the volume Control Service consists of APIs to control volume and gain of a peer device. It acts as client for VCS, VOCS and AICS
 */

/**
 * @addtogroup wiced_bt_ga_vcs
 * @{
 */

/** VCS minimum volume */
#define GA_LIB_VCS_MINIMUM_VOLUME 0x0

/** VCS maximum volume */
#define GA_LIB_VCS_MAXIMUM_VOLUME 0xFF

/** VCS muted state */
#define GA_LIB_VCS_MUTED 0x1

/** VCS unmuted state */
#define GA_LIB_VCS_NOT_MUTED 0x0

    /** Volume profile opcode codes */
#define VOLUME_CONTROL_OPCODE_RELATIVE_VOLUME_DOWN 0x00        /**< Relative volume down */
#define VOLUME_CONTROL_OPCODE_RELATIVE_VOLUME_UP 0x01          /**< Relative volume up */
#define VOLUME_CONTROL_OPCODE_UNMUTE_RELATIVE_VOLUME_DOWN 0x02 /**< Unmute and relative volume down */
#define VOLUME_CONTROL_OPCODE_UNMUTE_RELATIVE_VOLUME_UP 0x03   /**< Unmute and relative volume up */
#define VOLUME_CONTROL_OPCODE_SET_ABSOLUTE_VOLUME 0x04         /**< Set Absolute volume */
#define VOLUME_CONTROL_OPCODE_UNMUTE 0x05                      /**< Unmute */
#define VOLUME_CONTROL_OPCODE_MUTE 0x06                        /**< Mute */
    typedef uint8_t ga_lib_vcs_volume_control_opcodes_t;       /**< VCS opcodes*/

#define VOLUME_OFFSET_MIN_VALUE -255 /**< MIN Volume offset value */
#define VOLUME_OFFSET_MAX_VALUE 255  /**< MAX Volume offset value */

    /**
 * @brief Mute state values
 */
#define GA_LIB_MUTE_STATE_NOT_MUTED 0 /**< Set mute state to unmute */
#define GA_LIB_MUTE_STATE_MUTED 1     /**< Set mute state to mute */
#define GA_LIB_MUTE_STATE_DISABLED 2   /**< Set mute state to unmute */
    typedef uint8_t ga_lib_mute_val_t; /**< Mute values  */

    /**
 * @brief Volume Flag values
 */
#define GA_LIB_VOLUME_FLAG_VOLUME_SETTING_NOT_PERSISTED 0 /**< Set volume flag state to volume setting not persisted */
#define GA_LIB_VOLUME_FLAG_VOLUME_SETTING_PERSISTED 1     /**< Set volume flag state to volume setting persisted */
    typedef uint8_t ga_lib_volume_flag_val_t; /**< Volume flag values  */

    /** Volume state data */
    typedef struct
    {
        uint8_t volume_setting;       /**< current volume setting value (0-255) */
        ga_lib_mute_val_t mute_state; /**< current mute state value */
        uint8_t change_counter;       /**< Change counter incremented on each state change (wraps from 0xFF to 0) */
    } ga_lib_vcs_volume_state_t;

    /**
     * @brief Discover Volume Control Service on remote GATT server
     *
     * This function initiates GATT service discovery to find the VCS service, its
     * characteristics (Volume State, Control Point, Volume Flag), and any included
     * AICS (Audio Input Control Service) and VOCS (Volume Offset Control Service)
     * instances on a connected remote device. Memory is allocated for the specified
     * number of included service instances.
     *
     * @param[in] conn_id               Connection identifier for the GATT connection
     * @param[in] pfn_on_complete       Callback function invoked when discovery completes
     * @param[in] num_aics_instances    Maximum number of included AICS instances to discover
     * @param[in] num_vocs_instances    Maximum number of included VOCS instances to discover
     *
     * @return Pointer to the service discovery context, or NULL on failure (e.g., allocation failure)
     *
     * @note Application must save the discovered handles for subsequent volume control
     *       operations and included service access
     */
    gatt_intf_service_discovery_ctx_t *ga_lib_vcs_discover_service(uint16_t conn_id,
                                                                   pfn_on_discovery_complete_t pfn_on_complete,
                                                                   uint8_t num_aics_instances,
                                                                   uint8_t num_vocs_instances);

    /**
     * @brief Validate and apply volume control opcode to volume state
     *
     * This function simulates the effect of a volume control opcode on the current
     * volume state to determine if the operation would result in a state change. If
     * the resulting state differs from the current state, the function updates
     * p_state with the new values. This is typically used by the server to validate
     * incoming control point writes before applying them.
     *
     * Opcode behaviors:
     * - RELATIVE_VOLUME_DOWN: Decreases volume by step_size (clamped to 0)
     * - RELATIVE_VOLUME_UP: Increases volume by step_size (clamped to 255)
     * - UNMUTE_RELATIVE_VOLUME_DOWN/UP: Same as above plus sets mute_state to unmuted
     * - SET_ABSOLUTE_VOLUME: Sets volume to new_volume_setting
     * - UNMUTE: Sets mute_state to unmuted
     * - MUTE: Sets mute_state to muted
     *
     * @param[in] opcode             Volume control opcode (see #ga_lib_vcs_volume_control_opcodes_t)
     * @param[in,out] p_state        Pointer to current volume state; updated if state changes
     * @param[in] new_volume_setting New volume value (used only for SET_ABSOLUTE_VOLUME)
     * @param[in] step_size          Step size for relative volume changes
     *
     * @return WICED_SUCCESS if state changed and was updated,
     *         WICED_ERROR if opcode is invalid or state remains unchanged
     */
    wiced_result_t ga_lib_vcs_check_volume_state_validity(ga_lib_vcs_volume_control_opcodes_t opcode,
                                                          ga_lib_vcs_volume_state_t *p_state,
                                                          uint8_t new_volume_setting,
                                                          int step_size);

    /**
     * @brief Send notification for Volume State characteristic
     *
     * This function sends a GATT notification with the current volume setting, mute
     * state, and change counter. The change counter is automatically incremented
     * before sending (wraps from 0xFF to 0).
     *
     * Notification format: [volume_setting][mute_state][change_counter]
     *
     * @param[in] conn_id   Connection identifier for the GATT connection
     * @param[in,out] handle Attribute handle of the Volume State characteristic
     * @param[in,out] p_state Pointer to volume state structure; change_counter is incremented
     *
     * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
     *
     * @note The change_counter field in p_state is modified by this function
     */
    wiced_bt_gatt_status_t ga_lib_vcs_notify_volume_state(uint16_t conn_id,
                                                          uint16_t handle,
                                                          ga_lib_vcs_volume_state_t *p_state);

    /**
     * @brief Send notification for Volume Flag characteristic
     *
     * This function sends a GATT notification with the volume flag value indicating
     * whether the volume setting has been persisted (saved across reboots).
     *
     * @param[in] conn_id   Connection identifier for the GATT connection
     * @param[in] handle    Attribute handle of the Volume Flag characteristic
     * @param[in] flag      Volume flag value (see #ga_lib_volume_flag_val_t)
     *
     * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
     */
    wiced_bt_gatt_status_t ga_lib_vcs_notify_volume_flag(uint16_t conn_id, uint16_t handle, uint8_t flag);

    /**
     * @brief Write to Volume Control Point characteristic
     *
     * This function writes a volume control command to the remote server. The write
     * includes the opcode and the current change counter, plus an additional volume
     * setting byte for SET_ABSOLUTE_VOLUME opcode. Other opcodes (mute, unmute, relative
     * volume changes) do not include extra parameters.
     *
     * Packet format:
     * - 1 byte: opcode
     * - 1 byte: change counter
     * - 1 byte: volume setting (only for SET_ABSOLUTE_VOLUME)
     *
     * @param[in] conn_id   Connection identifier for the GATT connection
     * @param[in] p_handle  Pointer to characteristic handles for the Control Point
     * @param[in] opcode    Volume control opcode (see #ga_lib_vcs_volume_control_opcodes_t)
     * @param[in] p_state   Pointer to volume state containing change_counter and
     *                      volume_setting (used for absolute volume)
     *
     * @return WICED_BT_GATT_SUCCESS if write command was sent successfully,
     *         WICED_BT_GATT_NO_RESOURCES if buffer allocation failed,
     *         WICED_BT_GATT_ERROR if opcode is invalid,
     *         otherwise a GATT error code
     *
     * @note Uses GATT_CMD_WRITE (write command, no response)
     */
    wiced_bt_gatt_status_t ga_lib_vcs_write_control_point(uint16_t conn_id,
                                                          gatt_intf_characteristic_handles_t *p_handle,
                                                          ga_lib_vcs_volume_control_opcodes_t opcode,
                                                          ga_lib_vcs_volume_state_t *p_state);

/**@} wiced_bt_ga_vcs */
/**@} Volume_And_Gain_Control_APIs */
#ifdef __cplusplus
}
#endif
#endif /* GA_LIB_VCS_H */
