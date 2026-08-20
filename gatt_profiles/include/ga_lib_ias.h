/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Immediate Alert Service implementation header
  */
#ifndef GA_LIB_IAS_H
#define GA_LIB_IAS_H

#include "gatt_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @addtogroup Proximity_Profile_APIs
 * @{
 */

    /**
 * @addtogroup ga_lib_ias
 * @{
 * @brief Immediate Alert Service (IAS) provides an alert mechanism for proximity
 *        monitoring applications. The service allows a client device to command
 *        the server to alert the user by writing to the Alert Level characteristic.
 *
 * Alert levels:
 * - No Alert (0): Stop alerting
 * - Mild Alert (1): Alert with low intensity (e.g., vibration)
 * - High Alert (2): Alert with high intensity (e.g., loud sound + vibration)
 *
 * Typical use cases:
 * - Find My Device: Trigger alert when user wants to locate a misplaced device
 * - Proximity Monitoring: Alert when devices move out of range (link loss)
 * - Anti-theft: Alert when unauthorized device separation is detected
 */

#define GA_LIB_IAS_CHARACTERISTIC_ALERT_LEVEL 0 /**< Alert Level characteristic (write-only) */
#define GA_LIB_IAS_CHARACTERISTIC_MAX 1         /**< Number of IAS characteristics */
    typedef uint8_t ga_lib_ias_characteristics_t;      /**< IAS characteristics type */

/**
 * @brief Alert level value definitions
 *
 * Standard alert levels defined by the Immediate Alert Service specification
 */
#define IAS_ALERT_LEVEL_NO_ALERT 0 /**< No alert - stop any ongoing alert */
#define IAS_ALERT_LEVEL_MILD 1     /**< Mild alert - low intensity notification */
#define IAS_ALERT_LEVEL_HIGH 2     /**< High alert - high intensity notification */

    /**
 * @brief Discover Immediate Alert Service on remote GATT server
 *
 * This function initiates GATT service discovery to find the Immediate Alert
 * Service and its Alert Level characteristic on a connected remote device.
 * The IAS service contains a single write-only characteristic that allows
 * the client to trigger alerts on the server.
 *
 * Discovery process:
 * 1. Discover IAS service by UUID (range 0x0001 to 0xFFFF)
 * 2. Discover Alert Level characteristic within service range
 * 3. Discover characteristic descriptors (if any)
 *
 * After discovery completes, the client can write alert level values to
 * trigger alerts:
 * - Write 0 (No Alert): Stop alerting
 * - Write 1 (Mild Alert): Trigger low-intensity alert
 * - Write 2 (High Alert): Trigger high-intensity alert
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] pfn_on_complete   Callback function invoked when discovery completes
 *
 * @return Pointer to the service discovery context, or NULL on failure (e.g., allocation failure)
 *
 * @note Application must save the discovered Alert Level characteristic handle for
 *       subsequent write operations. The Alert Level characteristic is write-only
 *       (no read or notification support).
 *
 * @note IAS is typically used in conjunction with Link Loss Service (LLS) for
 *       comprehensive proximity monitoring solutions.
 */
    gatt_intf_service_discovery_ctx_t *ga_lib_ias_discover_service(uint16_t conn_id,
                                                                   pfn_on_discovery_complete_t pfn_on_complete);

    /**@} ga_lib_ias */
    /**@} Proximity_Profile_APIs */

#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_IAS_H */
