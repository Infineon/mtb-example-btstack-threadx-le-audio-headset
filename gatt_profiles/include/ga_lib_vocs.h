/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Volume Offset Control Service implementation header
  */

#ifndef GA_LIB_VOCS_H
#define GA_LIB_VOCS_H

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
 * @addtogroup ga_lib_vocs
 * @{
 * @brief Volume Offset Control Service (VOCS) enables per-channel volume offset
 *        adjustment for multi-channel audio devices. VOCS is typically included
 *        as part of the Volume Control Service (VCS) and provides one instance
 *        per audio output channel (e.g., left/right speakers, individual hearing
 *        aids in a coordinated set), allowing fine-tuned balance adjustment.
 */

/**
 * @brief VOCS characteristic enumeration
 */
#define GA_LIB_VOCS_CHARACTERISTIC_OFFSET_STATE  0 /**< Offset State characteristic (read/notify) - current offset and change counter */
#define GA_LIB_VOCS_CHARACTERISTIC_AUDIO_LOCATION 1 /**< Audio Location characteristic (read/write/notify) - 32-bit location bitmap */
#define GA_LIB_VOCS_CHARACTERISTIC_VOLUME_OFFSET_CONTROL_POINT 2 /**< Volume Offset Control Point (write) - control commands */
#define GA_LIB_VOCS_CHARACTERISTIC_AUDIO_DESCRIPTION 3 /**< Audio Description characteristic (read/write/notify) - UTF-8 string */
#define GA_LIB_VOCS_CHARACTERISTIC_MAX 4               /**< Number of VOCS characteristics */
typedef uint8_t ga_lib_vocs_characteristics_t;

/**
 * @brief VOCS error code values
 */
#define GA_LIB_VOCS_ERROR_INVALID_CHANGE_COUNTER 0x80 /**< Change counter value is invalid*/
#define GA_LIB_VOCS_ERROR_OPCODE_NOT_SUPPORTED 0x81   /**< Opcode is invalid*/
#define GA_LIB_VOCS_ERROR_VALUE_OUT_OF_RANGE 0x82     /**< Change counter value is invalid*/

/**
 * @brief Definition for application error codes which can be sent by Audio Location profile
 */
#define GA_LIB_VOCS_LEFT_AUDIO 0x00  /**< Left Audio Location */
#define GA_LIB_VOCS_RIGHT_AUDIO 0x01 /**< Right Audio Location */

// Volume Control offset opcodes [Refer section 3.3.2.1 Set Volume Offset procedure]
#define VOLUME_CONTROL_OFFSET_OPCODE 0x01 /**< Set Volume Offset opcode for the Volume Offset Control Point */

#define VOCS_OFFSET_STATE_LEN 3         /**< Length of Offset State value in bytes (offset:2 + change_counter:1) */
#define VOCS_AUDIO_LOCATION_LEN 4       /**< Length of Audio Location value in bytes (32-bit bitmap) */
#define VOCS_OFFSET_CONTROL_POINT_LEN 4 /**< Length of Volume Offset Control Point write payload in bytes */

/** Volume offset data which is passed between application and profile */
typedef struct
{
    int16_t volume_offset;   /**< VOCS offset value (signed; range -255 to +255) */
    uint8_t change_counter;  /**< VOCS change counter (incremented on each state change, wraps at 0xFF) */
    uint32_t audio_location; /**< Audio location bitmap value (see BAP audio location definitions) */
} ga_lib_vocs_data_t;

/**
 * @brief Discover Volume Offset Control Service on remote GATT server
 *
 * This function initiates GATT service discovery to find the VOCS service and its
 * characteristics (Offset State, Audio Location, Volume Offset Control Point, Audio
 * Description) on a connected remote device. VOCS is typically discovered as an
 * included service of VCS rather than as a standalone primary service.
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] pfn_on_complete   Callback function invoked when discovery completes
 * @param[in] start_handle      Starting handle for the discovery range
 * @param[in] end_handle        Ending handle for the discovery range
 *
 * @return Pointer to the service discovery context, or NULL on failure (e.g., allocation failure)
 *
 * @note Application must save the discovered characteristic handles for subsequent
 *       offset operations
 */
gatt_intf_service_discovery_ctx_t *ga_lib_vocs_discover_vocs_service(uint16_t conn_id,
                                                                     pfn_on_discovery_complete_t pfn_on_complete,
                                                                     uint16_t start_handle,
                                                                     uint16_t end_handle);

/**
 * @brief Send notification for Audio Description characteristic
 *
 * This function sends a GATT notification with the audio description string
 * (UTF-8 encoded) for this VOCS instance (e.g., "Front Left", "Hearing Aid Left").
 *
 * @param[in] conn_id       Connection identifier for the GATT connection
 * @param[in] handle        Attribute handle of the Audio Description characteristic
 * @param[in] p_desc        Pointer to UTF-8 description buffer
 * @param[in] len_to_write  Length of description data in bytes
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_vocs_notify_audio_description(uint16_t conn_id,
                                                         uint16_t handle,
                                                         const char *p_desc,
                                                         uint16_t len_to_write);

/**
 * @brief Send notification for Offset State characteristic
 *
 * This function sends a GATT notification with the current volume offset and
 * change counter values.
 *
 * Notification format: [volume_offset (2 bytes, little-endian)][change_counter (1 byte)]
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] handle            Attribute handle of the Offset State characteristic
 * @param[in] volume_offset     Signed 16-bit volume offset value (range -255 to +255)
 * @param[in] change_counter    Current change counter value
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 *
 * @note The caller is responsible for incrementing the change_counter before calling
 *       this function whenever the offset state changes
 */
wiced_bt_gatt_status_t ga_lib_vocs_notify_volume_offset(uint16_t conn_id,
                                                     uint16_t handle,
                                                     int16_t volume_offset,
                                                     uint8_t change_counter);

/**
 * @brief Send notification for Audio Location characteristic
 *
 * This function sends a GATT notification with the 32-bit audio location bitmap
 * indicating the spatial position(s) associated with this VOCS instance (e.g.,
 * Front Left, Front Right, Back Center).
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] handle            Attribute handle of the Audio Location characteristic
 * @param[in] audio_location    32-bit audio location bitmap (see BAP audio location defines,
 *                              e.g., BAP_AUDIO_LOCATION_FRONT_LEFT)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_vocs_notify_audio_location(uint16_t conn_id, uint16_t handle, uint32_t audio_location);

/**@} ga_lib_vocs */
/**@} Volume_And_Gain_Control_APIs */
#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_VOCS_H */
