/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Gaming Profile implementation header
  */
#ifndef GA_LIB_GMAP_H
#define GA_LIB_GMAP_H

#include "gatt_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @addtogroup Gaming_Audio_Profile_APIs
 * @{
 */

    /**
 * @addtogroup ga_lib_gmap
 * @{
 * @brief Gaming Audio Profile (GMAP) provides support for gaming audio use cases
 *        with low-latency audio streaming for both unicast and broadcast scenarios.
 *        GMAP defines roles for gaming devices:
 *        - Unicast Game Gateway (UGG): Initiates unicast gaming audio to one or more UGT devices
 *        - Unicast Game Terminal (UGT): Receives unicast gaming audio from UGG
 *        - Broadcast Game Sender (BGS): Broadcasts gaming audio to one or more BGR devices
 *        - Broadcast Game Receiver (BGR): Receives broadcast gaming audio from BGS
 */

#define GA_LIB_GMAP_CHARACTERISTIC_ROLE 0         /**< GMAP Role characteristic */
#define GA_LIB_GMAP_CHARACTERISTIC_UGG_FEATURES 1 /**< GMAP UGG Features characteristic */
#define GA_LIB_GMAP_CHARACTERISTIC_UGT_FEATURES 2 /**< GMAP UGT Features characteristic */
#define GA_LIB_GMAP_CHARACTERISTIC_BGS_FEATURES 3 /**< GMAP BGS Features characteristic */
#define GA_LIB_GMAP_CHARACTERISTIC_BGR_FEATURES 4 /**< GMAP BGR Features characteristic */
#define GA_LIB_GMAP_CHARACTERISTIC_MAX 5          /**< Maximum number of GMAP characteristics */
typedef uint8_t ga_lib_gmap_characteristics_t;           /**< GMAP characteristics type */

/**
 * @brief GMAP role bit field definitions
 *
 * These bit flags indicate which GMAP roles a device supports. Multiple roles
 * can be supported by setting multiple bits.
 */
#define GMAP_ROLE_UNICAST_GAME_GATEWAY (1 << 0)    /**< Unicast Game Gateway - initiates unicast gaming audio */
#define GMAP_ROLE_UNICAST_GAME_TERMINAL (1 << 1)   /**< Unicast Game Terminal - receives unicast gaming audio */
#define GMAP_ROLE_BROADCAST_GAME_SENDER (1 << 4)   /**< Broadcast Game Sender - broadcasts gaming audio */
#define GMAP_ROLE_BROADCAST_GAME_RECEIVER (1 << 5) /**< Broadcast Game Receiver - receives broadcast gaming audio */

    typedef uint8_t ga_lib_gmap_role_t; /**< GMAP role bit field */

/**
 * @brief GMAP Unicast Game Gateway (UGG) feature bit field definitions
 *
 * These bit flags indicate optional features supported by a UGG device.
 */
#define UGG_MULTIPLEX_FEATURE_SUPPORTED                                                                                \
    (1 << 0) /**< UGG Multiplex feature: supports multiplexing multiple audio streams */
#define UGG_96_KBPS_SOURCE_FEATURE_SUPPORTED                                                                           \
    (1 << 1)                                     /**< UGG 96 kbps Source feature: supports 96 kbps audio encoding */
#define UGG_MULTISINK_FEATURE_SUPPORTED (1 << 2) /**< UGG Multisink feature: supports streaming to multiple sinks */

/**
 * @brief GMAP Unicast Game Terminal (UGT) feature bit field definitions
 *
 * These bit flags indicate optional features supported by a UGT device.
 */
#define UGT_SOURCE_FEATURE_SUPPORTED (1 << 0) /**< UGT Source feature: can act as audio source */
#define UGT_80_KBPS_SOURCE_FEATURE_SUPPORTED                                                                           \
    (1 << 1)                                /**< UGT 80 kbps Source feature: supports 80 kbps audio encoding */
#define UGT_SINK_FEATURE_SUPPORTED (1 << 2) /**< UGT Sink feature: can act as audio sink */
#define UGT_64_KBPS_SOURCE_FEATURE_SUPPORTED                                                                           \
    (1 << 3)                                     /**< UGT 64 kbps Source feature: supports 64 kbps audio encoding */
#define UGT_MULTIPLEX_FEATURE_SUPPORTED (1 << 4) /**< UGT Multiplex feature: supports multiplexing */
#define UGT_MULTIPLEX_SINK_FEATURE_SUPPORTED (1 << 5) /**< UGT Multiplex Sink feature: supports multiplexing as sink */
#define UGT_MULTIPLEX_SOURCE_FEATURE_SUPPORTED                                                                         \
    (1 << 6) /**< UGT Multiplex Source feature: supports multiplexing as source */

/**
 * @brief GMAP Broadcast Game Sender (BGS) feature bit field definitions
 *
 * These bit flags indicate optional features supported by a BGS device.
 */
#define BGS_96_KBPS_SOURCE_FEATURE_SUPPORTED                                                                           \
    (1 << 0) /**< BGS 96 kbps Source feature: supports 96 kbps broadcast encoding */

/**
 * @brief GMAP Broadcast Game Receiver (BGR) feature bit field definitions
 *
 * These bit flags indicate optional features supported by a BGR device.
 */
#define BGR_MULTISINK_FEATURE_SUPPORTED                                                                                \
    (1 << 0) /**< BGR Multisink feature: supports receiving multiple broadcast streams */
#define BGR_MULTIPLEX_FEATURE_SUPPORTED (1 << 1) /**< BGR Multiplex feature: supports multiplexed broadcast reception */

    /**
 * @brief Discover Gaming Audio Profile Service on remote GATT server
 *
 * This function initiates GATT service discovery to find the Gaming Audio Profile
 * Service and its characteristics on a connected remote device. The GMAP service
 * includes characteristics for:
 * - Role: Indicates which GMAP roles the device supports (UGG, UGT, BGS, BGR)
 * - UGG Features: Feature flags for Unicast Game Gateway role
 * - UGT Features: Feature flags for Unicast Game Terminal role
 * - BGS Features: Feature flags for Broadcast Game Sender role
 * - BGR Features: Feature flags for Broadcast Game Receiver role
 *
 * Discovery process:
 * 1. Discover GMAP service by UUID (range 0x0001 to 0xFFFF)
 * 2. Discover characteristics within service range
 * 3. Discover characteristic descriptors
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] pfn_on_complete   Callback function invoked when discovery completes
 *
 * @return Pointer to the service discovery context, or NULL on failure (e.g., allocation failure)
 *
 * @note Application must save the discovered service handles for subsequent read operations
 *       to determine the gaming capabilities of the remote device
 */
    gatt_intf_service_discovery_ctx_t *ga_lib_gmap_discover_service(uint16_t conn_id,
                                                                    pfn_on_discovery_complete_t pfn_on_complete);

    /**@} ga_lib_gmap */
    /**@} Gaming_Audio_Profile_APIs */

#ifdef __cplusplus
}
#endif
#endif /* GA_LIB_GMAP_H */
