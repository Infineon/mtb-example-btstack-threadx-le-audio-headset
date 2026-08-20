/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Microphone Input Control Service header
  */

#ifndef GA_LIB_MICS_H
#define GA_LIB_MICS_H

#include "ga_lib_vcs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup Volume_And_Gain_Control_APIs
 * @{
 */

/**
 * @addtogroup ga_lib_mics
 * @{
 * @brief Microphone Input Control Service (MICS) enables control of microphone mute
 *        state on a server device. MICS may include one or more Audio Input Control
 *        Service (AICS) instances as included services to provide additional control
 *        over individual microphone inputs (gain, input type, etc.).
 */

/**
 * @brief MICS characteristic enumeration
 */
#define GA_LIB_MICS_CHARACTERISTIC_MUTE_STATE 0 /**< Mute State characteristic (read/write/notify) */
#define GA_LIB_MICS_CHARACTERISTIC_MAX 1        /**< Number of MICS characteristics */
    typedef uint8_t ga_lib_mics_characteristics_t;     /**< Type for MICS characteristics */

/**
 * @brief MICS error code values
 */
#define GA_LIB_MICS_ERROR_MUTE_DISABLED  0x80 /**< Mute disabled - mute operation not allowed */

/**
 * @brief Discover Microphone Input Control Service on remote GATT server
 *
 * This function initiates GATT service discovery to find the Microphone Input Control
 * Service, its Mute State characteristic, and any included AICS (Audio Input Control
 * Service) instances on a connected remote device. Memory is allocated for the
 * specified number of AICS instances.
 *
 * @param[in] conn_id               Connection identifier for the GATT connection
 * @param[in] pfn_on_complete       Callback function invoked when discovery completes
 * @param[in] num_aics_instances    Maximum number of included AICS instances to discover
 *
 * @return Pointer to the service discovery context, or NULL on failure (e.g., allocation failure)
 *
 * @note Application must save the discovered handles for subsequent mute operations
 *       and AICS instance access
 */
gatt_intf_service_discovery_ctx_t *ga_lib_mics_discover_service(uint16_t conn_id,
                                                             pfn_on_discovery_complete_t pfn_on_complete,
                                                             uint8_t num_aics_instances);

/**
 * @brief Write to Mute State characteristic on remote GATT server
 *
 * This function writes a new mute state value to the Mute State characteristic to
 * mute or unmute the microphone on the remote device.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] p_handle  Pointer to characteristic handles structure for the Mute State characteristic
 * @param[in] mute_val  New mute state value (see #ga_lib_mute_val_t):
 *                      - Not Muted
 *                      - Muted
 *                      - Mute Disabled
 *
 * @return WICED_BT_GATT_SUCCESS if write request was sent successfully,
 *         GA_LIB_MICS_ERROR_MUTE_DISABLED if the server has disabled mute control,
 *         otherwise a GATT error code
 *
 * @note Uses GATT_REQ_WRITE (write with response) for reliable delivery
 */
wiced_bt_gatt_status_t ga_lib_mics_write_mute_state(uint16_t conn_id,
                                                 gatt_intf_characteristic_handles_t *p_handle,
                                                 ga_lib_mute_val_t mute_val);

/**
 * @brief Send notification for Mute State characteristic
 *
 * This function sends a GATT notification to inform the client of changes to the
 * Mute State characteristic value.
 *
 * @param[in] conn_id       Connection identifier for the GATT connection
 * @param[in] handle        Attribute handle of the Mute State characteristic
 * @param[in] mute_state    Current mute state value (see #ga_lib_mute_val_t)
 *
 * @return WICED_BT_GATT_SUCCESS if notification was sent successfully,
 *         otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_mics_notify_mute_state(uint16_t conn_id, uint16_t handle, ga_lib_mute_val_t mute_state);

/**@} ga_lib_mics */
/**@} Volume_And_Gain_Control_APIs */

#ifdef __cplusplus
}
#endif
#endif /* GA_LIB_MICS_H */
