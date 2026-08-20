/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Telephony profile implementation header
  */
#ifndef GA_LIB_TMAP_H
#define GA_LIB_TMAP_H

#include "gatt_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup Telephony_Media_Audio_Profile_APIs
 * @{
 */

/**
 * @addtogroup ga_lib_tmap
 * @{
 * @brief Telephony and Media Audio Profile (TMAP) defines roles and requirements for
 *        devices supporting telephony and media audio use cases over LE Audio. TMAP
 *        roles cover both unicast (point-to-point) and broadcast scenarios:
 *        - Call Gateway (CG): Originates telephony audio (e.g., smartphone)
 *        - Call Terminal (CT): Receives/sends telephony audio (e.g., headset)
 *        - Unicast Media Sender (UMS): Sends media audio to a single receiver
 *        - Unicast Media Receiver (UMR): Receives media audio from a single sender
 *        - Broadcast Media Sender (BMS): Broadcasts media audio to multiple receivers
 *        - Broadcast Media Receiver (BMR): Receives broadcast media audio
 *
 *        A single device can support multiple roles by setting the corresponding bits
 *        in the Role characteristic.
 */

/**
 * @brief TMAP characteristic enumeration
 */
#define GA_LIB_TMAP_CHARACTERISTIC_ROLE 0 /**< Role characteristic indicating supported TMAP roles (bitmask) */
#define GA_LIB_TMAP_CHARACTERISTIC_MAX 1  /**< Number of TMAP characteristics */
typedef uint8_t ga_lib_tmap_characteristics_t;

/**
 * @brief TMAP role bit field definitions
 *
 * These bit flags indicate which TMAP roles a device supports. Multiple roles
 * can be supported simultaneously by setting multiple bits in the Role characteristic.
 */
#define TMAP_ROLE_CALL_GATEWAY (1 << 0)             /**< Call gateway - originates telephony audio */
#define TMAP_ROLE_CALL_TERMINAL (1 << 1)            /**< Call terminal - endpoint for telephony audio */
#define TMAP_ROLE_UNICAST_MEDIA_SENDER (1 << 2)     /**< Unicast media sender - point-to-point media source */
#define TMAP_ROLE_UNICAST_MEDIA_RECEIVER (1 << 3)   /**< Unicast media receiver - point-to-point media sink */
#define TMAP_ROLE_BROADCAST_MEDIA_SENDER (1 << 4)   /**< Broadcast media sender - one-to-many media source */
#define TMAP_ROLE_BROADCAST_MEDIA_RECEIVER (1 << 5) /**< Broadcast media receiver - one-to-many media sink */

/**
 * @brief Discover Telephony and Media Audio Profile Service on remote GATT server
 *
 * This function initiates GATT service discovery to find the TMAP service and its
 * Role characteristic on a connected remote device. The Role characteristic exposes
 * a bitmask indicating which TMAP roles (Call Gateway/Terminal, Unicast/Broadcast
 * Media Sender/Receiver) the remote device supports.
 *
 * Discovery process:
 * 1. Discover TMAP service by UUID (range 0x0001 to 0xFFFF)
 * 2. Discover Role characteristic within service range
 * 3. Discover characteristic descriptors
 *
 * After discovery, the client should read the Role characteristic to determine
 * the device's capabilities and select an appropriate audio role.
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] pfn_on_complete   Callback function invoked when discovery completes
 *
 * @return Pointer to the service discovery context, or NULL on failure (e.g., allocation failure)
 *
 * @note Application must save the discovered Role characteristic handle for subsequent
 *       read operations to query the device's TMAP role capabilities
 */
gatt_intf_service_discovery_ctx_t *ga_lib_tmap_discover_service(uint16_t conn_id,
                                                                pfn_on_discovery_complete_t pfn_on_complete);

/**@} ga_lib_tmap */
/**@} Telephony_Media_Audio_Profile_APIs */

#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_TMAP_H */
