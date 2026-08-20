/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Hearing access service/profile implementation header
  */
#ifndef GA_LIB_HAS_H
#define GA_LIB_HAS_H

#include "gatt_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif
    /**
 * @addtogroup Hearing_Access_Profile_APIs
 * @{
 */

    /**
 * @addtogroup ga_lib_has
 * @{
 * @brief Hearing Access Service (HAS) provides control of hearing aid presets for
 *        personalized audio settings. HAS enables remote devices to:
 *        - Discover hearing aid capabilities (binaural/monaural, preset support)
 *        - Read and write preset configurations
 *        - Select active presets
 *        - Receive notifications of preset changes
 *        - Synchronize preset changes across coordinated hearing aid sets
 */

#define GA_LIB_HAS_CHARACTERISTIC_HEARING_AID_FEATURES 0 /**< Hearing Aid Features characteristic */
#define GA_LIB_HAS_CHARACTERISTIC_HEARING_AID_PRESET_CONTROL_POINT                                                     \
    1 /**< Hearing Aid Preset Control Point characteristic */
#define GA_LIB_HAS_CHARACTERISTIC_HEARING_AID_ACTIVE_PRESET_INDEX                                                      \
    2                                             /**< Hearing Aid Active Preset Index characteristic */
#define GA_LIB_HAS_CHARACTERISTIC_MAX 3           /**< Maximum number of HAS characteristics */
    typedef uint8_t ga_lib_has_characteristics_t; /**< HAS characteristics type */

/**
 * @brief HAS error code definitions
 *
 * Application-specific error codes that can be returned by HAS operations
 */
#define GA_LIB_HAS_ERROR_INVALID_OPCODE 0x80                       /**< Control point opcode is not supported */
#define GA_LIB_HAS_ERROR_WRITE_NAME_NOT_ALLOWED 0x81               /**< Writing preset name is not allowed */
#define GA_LIB_HAS_ERROR_PRESET_SYNCHRONIZATION_NOT_SUPPORTED 0x82 /**< Preset synchronization is not supported */
#define GA_LIB_HAS_ERROR_PRESET_OPERATION_NOT_POSSIBLE 0x83 /**< Requested preset operation cannot be performed */
#define GA_LIB_HAS_ERROR_INVALID_PARAMETERS_LENGTH 0x84     /**< Control point parameters have invalid length */


#define GA_LIB_HAS_PRESET_PROPERTIES_WRITABLE              0x01 /**< Preset name is writable */
#define GA_LIB_HAS_PRESET_PROPERTIES_AVAILABLE             0x02 /**< Preset is available */

/**
 * @brief HAS feature bit field definitions
 *
 * These bit flags indicate the capabilities and configuration of the hearing aid device
 */
#define GA_LIB_HAS_FEATURES(hearing_aid_type, preset_sync, independent_presets, dynamic_presets, writable_presets)     \
    ((hearing_aid_type & 0x3) << 0 | (preset_sync & 1) << 2 | (independent_presets & 1) << 3 |                         \
     (dynamic_presets & 1) << 4 | (writable_presets & 1) << 5)

#define GA_LIB_HAS_HEARING_AID_TYPE(features) ((features) & 0x3) /**< Mask for hearing aid type bits */
#define GA_LIB_HAS_PRESET_SYNC_SUPPORTED(features)                                                                     \
    (((features) >> 2) & 0x1) /**< Mask for preset synchronization support bit */
#define GA_LIB_HAS_INDEPENDENT_PRESETS(features)                                                                       \
    (((features) >> 3) & 0x1) /**< Mask for independent presets in coordinated set bit */
#define GA_LIB_HAS_DYNAMIC_PRESETS(features) (((features) >> 4) & 0x1) /**< Mask for dynamic preset support bit */
#define GA_LIB_HAS_WRITABLE_PRESETS_SUPPORTED(features)                                                                \
    (((features) >> 5) & 0x1) /**< Mask for writable preset support bit */

/** Bits 0-1 of the features field */
#define GA_LIB_HAS_FEATURES_HEARING_AID_TYPE_BINAURAL 0 /**< binaural hearing aid type */
#define GA_LIB_HAS_FEATURES_HEARING_AID_TYPE_MONAURAL 1 /**< monoaural hearing aid type */
#define GA_LIB_HAS_FEATURES_HEARING_AID_TYPE_BANDED 2   /**< banded hearing aid type */

/** Bits 2 of the features field */
#define GA_LIB_HAS_FEATURES_PRESET_SYNC_NOT_SUPPORTED 0x0 /**< Preset synchronization not supported flag */
#define GA_LIB_HAS_FEATURES_PRESET_SYNC_SUPPORTED 0x1     /**< Preset synchronization supported flag */

/** Bit 3 of the features field */
/** The list of preset records on this server is identical to the list of preset records in the other server
 of the Coordinated Set */
#define GA_LIB_HAS_FEATURES_INDEPENDENT_PRESETS_NOT_SUPPORTED 0
/** The list of preset records on this server may be different from the list of preset records in the
other server of the Coordinated Set */
#define GA_LIB_HAS_FEATURES_INDEPENDENT_PRESETS_SUPPORTED 1

/** Bit 4 of the features field */
#define GA_LIB_HAS_FEATURES_DYNAMIC_PRESETS_NOT_SUPPORTED                                                              \
    0x0 /**< Dynamic presets not supported flag
         (device cannot add/remove presets dynamically) */
#define GA_LIB_HAS_FEATURES_DYNAMIC_PRESETS_SUPPORTED                                                                  \
    0x1 /**< Dynamic presets supported flag
         (device can add/remove presets dynamically) */
/** Bit 5 of the features field */
#define GA_LIB_HAS_FEATURES_WRITABLE_PRESETS_NOT_SUPPORTED                                                             \
    0x0 /**< Writable presets not supported flag (device does not allow writing preset names) */
#define GA_LIB_HAS_FEATURES_WRITABLE_PRESETS_SUPPORTED                                                                 \
    0x1 /**< Writable presets supported flag (device allows writing preset names) */

    /** Invalid opcode */
#define GA_LIB_HAS_OPCODE_INVALID 0
    /** Request to read preset records */
#define GA_LIB_HAS_OPCODE_READ_PRESETS_REQUEST 1
    /** Response containing preset record data */
#define GA_LIB_HAS_OPCODE_READ_PRESET_RESPONSE 2
    /** Notification that a preset has changed */
#define GA_LIB_HAS_OPCODE_PRESET_CHANGED 3
    /** Request to write/update a preset name */
#define GA_LIB_HAS_OPCODE_WRITE_PRESET_NAME 4
    /** Request to set active preset (no sync) */
#define GA_LIB_HAS_OPCODE_SET_ACTIVE_PRESET 5
    /** Request to activate next preset (no sync) */
#define GA_LIB_HAS_OPCODE_SET_NEXT_PRESET 6
    /** Request to activate previous preset (no sync) */
#define GA_LIB_HAS_OPCODE_SET_PREVIOUS_PRESET 7
    /** Request to set active preset (with local sync) */
#define GA_LIB_HAS_OPCODE_SET_ACTIVE_PRESET_SYNCHRONIZED_LOCALLY 8
    /** Request to activate next preset (with local sync) */
#define GA_LIB_HAS_OPCODE_SET_NEXT_PRESET_SYNCHRONIZED_LOCALLY 9
    /** Request to activate previous preset (with local sync) */
#define GA_LIB_HAS_OPCODE_SET_PREVIOUS_PRESET_SYNCHRONIZED_LOCALLY 10
    typedef uint8_t ga_lib_has_opcode_t; /**< HAS control point opcode definitions */

#define GA_LIB_HAS_GENERIC_UPDATE 0                /**< Generic preset update (properties/name changed) */
#define GA_LIB_HAS_PRESET_RECORD_DELETED 1         /**< Preset record has been deleted */
#define GA_LIB_HAS_PRESET_RECORD_AVAILABLE 2       /**< Preset record is now available */
#define GA_LIB_HAS_PRESET_RECORD_UNAVAILABLE 3     /**< Preset record is now unavailable */
    typedef uint8_t ga_lib_has_preset_change_id_t; /**< Preset change type definitions */

    /**
 * @brief Hearing aid preset record structure
 *
 * Represents a single preset configuration with its properties and name
 */
    typedef struct
    {
        uint8_t preset_index; /**< Unique index identifying this preset */
        uint8_t properties;   /**< Preset properties bit field */
        uint8_t is_last;      /**< Flag indicating this is the last preset in a sequence */
        char *name;           /**< Null-terminated preset name string */
        uint8_t name_len;     /**< Length of preset name (including null terminator) */
    } ga_lib_has_preset_records_t;

    /**
 * @brief Preset changed notification structure
 *
 * Describes a change to a preset configuration
 */
    typedef struct
    {
        ga_lib_has_preset_change_id_t change_id; /**< Type of change that occurred */
        uint8_t is_last;                         /**< Flag indicating this is the last change notification */
        ga_lib_has_preset_records_t *p_rec;      /**< Pointer to preset record (new/updated data) */
        uint8_t prev_index;                      /**< Previous preset index (for generic updates) */
        uint8_t preset_index;                    /**< Current preset index */
    } ga_lib_has_cp_rsp_preset_changed_t;

    /**
 * @brief Discover Hearing Access Service on remote GATT server
 *
 * This function initiates GATT service discovery to find the Hearing Access Service
 * and its characteristics on a connected remote device. The HAS service includes:
 * - Hearing Aid Features: Indicates device capabilities (binaural/monaural, preset support)
 * - Hearing Aid Preset Control Point: Interface for preset operations
 * - Active Preset Index: Currently active preset
 *
 * Discovery process:
 * 1. Discover HAS service by UUID (range 0x0001 to 0xFFFF)
 * 2. Discover characteristics within service range
 * 3. Discover characteristic descriptors for notification/indication support
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] pfn_on_complete   Callback function invoked when discovery completes
 *
 * @return Pointer to the service discovery context, or NULL on failure (e.g., allocation failure)
 *
 * @note Application must save the discovered service handles for subsequent preset operations
 */
    gatt_intf_service_discovery_ctx_t *ga_lib_has_discover_service(uint16_t conn_id,
                                                                   pfn_on_discovery_complete_t pfn_on_complete);

    /**
 * @brief Send Read Preset Response indication
 *
 * This function sends a GATT indication containing preset record data in response
 * to a Read Presets Request from the client. The indication includes the preset
 * index, properties, and name.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Preset Control Point characteristic
 * @param[in] p_rec     Pointer to preset record structure containing data to send
 *
 * @return WICED_BT_GATT_SUCCESS if indication was sent successfully,
 *         WICED_BT_GATT_NO_RESOURCES if buffer allocation failed,
 *         otherwise a GATT error code
 *
 * @note The indication size is limited by the connection MTU
 */
    wiced_bt_gatt_status_t ga_lib_has_notify_read_preset_rsp(uint16_t conn_id,
                                                             uint16_t handle,
                                                             ga_lib_has_preset_records_t *p_rec);

    /**
 * @brief Send Preset Changed indication
 *
 * This function sends a GATT indication to notify the client of a preset change.
 * The notification format varies based on the change type:
 * - Generic Update: Includes previous index, new index, properties, and name
 * - Other changes: Includes only the affected preset index
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Preset Control Point characteristic
 * @param[in] p_change  Pointer to structure describing the preset change
 *
 * @return WICED_BT_GATT_SUCCESS if indication was sent successfully,
 *         WICED_BT_GATT_NO_RESOURCES if buffer allocation failed,
 *         otherwise a GATT error code
 *
 * @note The indication size is limited by the connection MTU
 */
    wiced_bt_gatt_status_t ga_lib_has_notify_preset_changed(uint16_t conn_id,
                                                            uint16_t handle,
                                                            ga_lib_has_cp_rsp_preset_changed_t *p_change);

    /**
 * @brief Send Active Preset Index indication
 *
 * This function sends a GATT indication to notify the client of the currently
 * active preset index.
 *
 * @param[in] conn_id               Connection identifier for the GATT connection
 * @param[in] handle                Attribute handle of the Active Preset Index characteristic
 * @param[in] active_preset_index   Index of the currently active preset
 *
 * @return WICED_BT_GATT_SUCCESS if indication was sent successfully,
 *         WICED_BT_GATT_NO_RESOURCES if buffer allocation failed,
 *         otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_has_notify_active_preset_index(uint16_t conn_id,
                                                                 uint16_t handle,
                                                                 uint8_t active_preset_index);

    /**
 * @brief Write control point to read presets
 *
 * This function writes a Read Presets Request to the Preset Control Point
 * characteristic to request preset records from the hearing aid. The server
 * will respond with one or more Read Preset Response indications.
 *
 * @param[in] conn_id       Connection identifier for the GATT connection
 * @param[in] p_handles     Pointer to characteristic handles structure
 * @param[in] start_index   Starting preset index to read from
 * @param[in] num_presets   Number of presets to read
 *
 * @return WICED_BT_GATT_SUCCESS if write was sent successfully,
 *         otherwise a GATT error code
 *
 * @note This uses GATT write with response to ensure reliability
 */
    wiced_bt_gatt_status_t ga_lib_has_write_control_point_read_presets(uint16_t conn_id,
                                                                       gatt_intf_characteristic_handles_t *p_handles,
                                                                       uint8_t start_index,
                                                                       uint8_t num_presets);

    /**
 * @brief Write control point to update preset name
 *
 * This function writes a Write Preset Name command to the Preset Control Point
 * characteristic to update the name of a specific preset. This operation is only
 * supported if the HAS_WRITABLE_PRESETS_SUPPORTED feature flag is set.
 *
 * @param[in] conn_id       Connection identifier for the GATT connection
 * @param[in] p_handles     Pointer to characteristic handles structure
 * @param[in] preset_index  Index of the preset to update
 * @param[in] preset_name   Null-terminated string containing the new preset name
 *
 * @return WICED_BT_GATT_SUCCESS if write was sent successfully,
 *         otherwise a GATT error code
 *
 * @note This uses GATT write command (no response) for efficiency
 */
    wiced_bt_gatt_status_t ga_lib_has_write_control_point_preset_name(uint16_t conn_id,
                                                                      gatt_intf_characteristic_handles_t *p_handles,
                                                                      uint8_t preset_index,
                                                                      const char *preset_name);

    /**
 * @brief Write control point to set active preset
 *
 * This function writes a Set Active Preset command to the Preset Control Point
 * characteristic to activate a specific preset. The command can optionally
 * synchronize the change across a coordinated hearing aid set.
 *
 * @param[in] conn_id                   Connection identifier for the GATT connection
 * @param[in] p_handles                 Pointer to characteristic handles structure
 * @param[in] preset_index              Index of the preset to activate
 * @param[in] is_synchronized_locally   If TRUE, synchronize change across coordinated set;
 *                                      if FALSE, change only this device
 *
 * @return WICED_BT_GATT_SUCCESS if write was sent successfully,
 *         otherwise a GATT error code
 *
 * @note This uses GATT write with response to ensure reliability
 */
    wiced_bt_gatt_status_t ga_lib_has_write_control_point_set_active_preset(
        uint16_t conn_id,
        gatt_intf_characteristic_handles_t *p_handles,
        uint8_t preset_index,
        uint8_t is_synchronized_locally);

    /**
 * @brief Write control point to set next preset
 *
 * This function writes a Set Next Preset command to the Preset Control Point
 * characteristic to activate the next preset in sequence. The command can
 * optionally synchronize the change across a coordinated hearing aid set.
 *
 * @param[in] conn_id                   Connection identifier for the GATT connection
 * @param[in] p_handles                 Pointer to characteristic handles structure
 * @param[in] is_synchronized_locally   If TRUE, synchronize change across coordinated set;
 *                                      if FALSE, change only this device
 *
 * @return WICED_BT_GATT_SUCCESS if write was sent successfully,
 *         otherwise a GATT error code
 *
 * @note This uses GATT write with response to ensure reliability
 */
    wiced_bt_gatt_status_t ga_lib_has_write_control_point_set_next_preset(uint16_t conn_id,
                                                                          gatt_intf_characteristic_handles_t *p_handles,
                                                                          uint8_t is_synchronized_locally);

    /**
 * @brief Write control point to set previous preset
 *
 * This function writes a Set Previous Preset command to the Preset Control Point
 * characteristic to activate the previous preset in sequence. The command can
 * optionally synchronize the change across a coordinated hearing aid set.
 *
 * @param[in] conn_id                   Connection identifier for the GATT connection
 * @param[in] p_handles                 Pointer to characteristic handles structure
 * @param[in] is_synchronized_locally   If TRUE, synchronize change across coordinated set;
 *                                      if FALSE, change only this device
 *
 * @return WICED_BT_GATT_SUCCESS if write was sent successfully,
 *         otherwise a GATT error code
 *
 * @note This uses GATT write with response to ensure reliability
 */
    wiced_bt_gatt_status_t ga_lib_has_write_control_point_set_previous_preset(
        uint16_t conn_id, gatt_intf_characteristic_handles_t *p_handles, uint8_t is_synchronized_locally);

    /**@} ga_lib_has */
    /**@} Hearing_Access_Profile_APIs */

#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_HAS_H */
