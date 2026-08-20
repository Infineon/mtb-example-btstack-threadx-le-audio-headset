/*
 * $ Copyright Cypress Semiconductor $
 */

/**************************************************************************/ /**
  * \file <gatt_interface.h>
  *
  * Definitions for interface between BT stack GATT and service/application. The GATT interface is responsible for
  * routing the incoming GATT commands to the appropriate service/services or application.
  * GATT Interface library helps applications to
  *  1. Discover remote services, included services, characteristics, descriptors and map them to the
  *     registered service implementations
  *  2. Setup local services, included services, characteristics, descriptors and map them to the
  *     registered service implementations
  *  3. Perform basic operations viz, notifying remote clients, setting up notifications, reading characteristics
  *
  */

#ifndef GATT_INTERFACE_H
#define GATT_INTERFACE_H

#include <stdlib.h>
#include "wiced_bt_ble.h"
#include "wiced_bt_gatt.h"
#include "wiced_timer.h"
#include "wiced_bt_trace.h"
#include "wiced_memory.h"

/**
 * @addtogroup Generic_Structures
 * @{
 */

/**
 * @brief Identifies a GATT service by name and UUID.
 *
 * Used as an input parameter to service discovery to specify which service to search for.
 */
typedef struct
{
    char *name;           /**< Human-readable service name, used in trace output */
    wiced_bt_uuid_t uuid; /**< UUID of the service to discover */
} gatt_intf_service_name_uuid_t;

/**
 * @brief Describes the static properties of a single GATT characteristic.
 *
 * Used as an input to the discovery context to specify which characteristics to locate
 * during a service discovery. Each entry in the discovery input array corresponds to one
 * characteristic slot in the result handle array.
 */
typedef struct
{
    wiced_bt_uuid_t uuid;        /**< UUID of the characteristic */
    uint8_t characteristic_type; /**< Application-defined type identifier for the characteristic */
    char *name;                  /**< Human-readable characteristic name, used in trace output */
} gatt_intf_characteristic_info_t;

/**
 * @brief Holds the discovered ATT handles for a single GATT characteristic.
 *
 * Populated by the discovery procedure. The @c handle, @c value_handle, and
 * @c descriptor_handle fields are filled in as each phase of discovery completes.
 */
typedef struct
{
    uint16_t handle;            /**< ATT handle of the characteristic declaration */
    uint16_t value_handle;      /**< ATT handle of the characteristic value */
    uint16_t descriptor_handle; /**< ATT handle of the Client Characteristic Configuration Descriptor (CCCD),
                                     or 0 if no CCCD was found */
} gatt_intf_characteristic_handles_t;

/**
 * @brief Identifies an included GATT service by name and UUID.
 *
 * Used as an input to the discovery context to specify which included services to locate.
 */
typedef struct
{
    char *name;           /**< Human-readable included service name, used in trace output */
    wiced_bt_uuid_t uuid; /**< UUID of the included service */
} gatt_intf_included_service_info_t;

/**
 * @brief Enumeration of GATT service discovery states.
 *
 * Represents the sequential phases of the discovery procedure driven by
 * @c gatt_intf_on_service_discovery_result() and @c gatt_intf_on_service_discovery_complete().
 * The state machine advances from @c SERVICE_DISCOVERY_STATE_IDLE through each phase
 * and ends at @c SERVICE_DISCOVERY_STATE_DISCOVER_COMPLETE, at which point the
 * application callback is invoked.
 */
typedef enum
{
    SERVICE_DISCOVERY_STATE_IDLE,                                /**< No discovery in progress */
    SERVICE_DISCOVERY_STATE_DISCOVER_SERVICE,                    /**< Discovering primary service by UUID */
    SERVICE_DISCOVERY_STATE_DISCOVER_INCLUDE_SERVICES,           /**< Discovering included services (reserved for future use) */
    SERVICE_DISCOVERY_STATE_DISCOVER_CHARACTERISTICS,            /**< Discovering characteristics within the service */
    SERVICE_DISCOVERY_STATE_DISCOVER_CHARACTERISTIC_DESCRIPTORS, /**< Discovering CCCDs and other descriptors */
    SERVICE_DISCOVERY_STATE_DISCOVER_COMPLETE                    /**< All discovery phases complete; application callback invoked */
} service_discovery_state_t;

/** @brief Forward declaration of the service discovery context structure. */
typedef struct _gatt_intf_service_discovery_ctx_t gatt_intf_service_discovery_ctx_t;

/** @brief Forward declaration of the included service discovery context structure (reserved). */
typedef struct _gatt_intf_included_service_discovery_ctx_t gatt_intf_included_service_discovery_ctx_t;

/**
 * @brief Start and end ATT handles delimiting a GATT service attribute range.
 */
typedef struct
{
    uint16_t start_handle; /**< First ATT handle of the service */
    uint16_t end_handle;   /**< Last ATT handle of the service */
} gatt_intf_service_range_t;

/**
 * @brief Holds the discovered ATT handles for a single included GATT service.
 */
typedef struct
{
    uint16_t service_handle;             /**< ATT handle of the include declaration attribute */
    gatt_intf_service_range_t service; /**< Start and end handles of the included service */
} gatt_intf_included_service_handles_t;

/**
 * @brief Holds the output of a completed GATT service discovery procedure.
 *
 * Populated incrementally during the discovery phases and passed to the application
 * callback @c pfn_on_discovery_complete_t when discovery completes.
 *
 * @note @c p_handles and @c p_inc_handles are parallel to the input arrays in
 *       @c gatt_intf_discovery_input_params_t:
 *       - @c p_handles[i] contains the discovered ATT handles for the characteristic
 *         described by @c input_params.pp_characteristic_info[i].
 *       - @c p_inc_handles[i] contains the discovered handles for the included service
 *         described by @c input_params.pp_included_service_info[i].
 */
typedef struct
{
    /** Array of @c max_inc_handles discovered included service handle sets */
    gatt_intf_included_service_handles_t inc_handles;
    /** Array of @c max_inc_handles discovered included service definitions */
    const gatt_intf_included_service_info_t *p_inc_info;
} gatt_intf_included_service_types_t;

typedef struct
{
    /** Discovered handles for the characteristic declaration */
    gatt_intf_characteristic_handles_t handles;
    /** Pointer to the input characteristic definition corresponding to these handles,
      * for reference in the application callback
      */
    const gatt_intf_characteristic_info_t *p_char_info;
} gatt_intf_characteristic_handles_result_t;

typedef struct
{
    /** Pointer to the service identifier (name + UUID) */
    const gatt_intf_service_name_uuid_t *p_id;
    /**< ATT handle range of the discovered service */
    gatt_intf_service_range_t service_range;
    /** Array of @c num_characteristic_handles discovered characteristic handle sets */
    gatt_intf_characteristic_handles_result_t *p_char_handles;
    /**< Discovered included services and their handles */
    gatt_intf_included_service_types_t *p_inc_service_types;
    uint8_t
        max_characteristic_handles; /**< Capacity of the @c p_result_handles array, copied from input params for reference */
    uint8_t
        max_included_services; /**< Capacity of the @c p_included_service_result array, copied from input params for reference */
} gatt_intf_discovery_result_t;

/**
 * @brief Callback invoked when the full service discovery procedure completes.
 *
 * Called by @c gatt_intf_handle_discovery_complete() after all discovery phases have
 * finished. After this callback returns the discovery context (@c p_sdc) is freed and
 * must not be accessed again.
 *
 * @param p_sdc     Pointer to the discovery context associated with the completed procedure.
 * @param conn_id   Connection ID of the GATT connection.
 * @param status    Final GATT status: @c WICED_BT_GATT_SUCCESS on success,
 *                  or an error code if any discovery phase failed.
 * @param p_result  Pointer to the populated discovery result. Valid only for the
 *                  duration of this callback.
 */
typedef void (*pfn_on_discovery_complete_t)(gatt_intf_service_discovery_ctx_t *p_sdc,
                                            uint16_t conn_id,
                                            wiced_bt_gatt_status_t status,
                                            gatt_intf_discovery_result_t *p_result);

/**
 * @brief Internal context structure for a GATT service discovery procedure.
 *
 * Allocated by @c gatt_intf_alloc_service_discovery_ctx() as a single contiguous block
 * that also holds the @c p_handles, @c pp_characteristic_info, @c p_inc_handles, and
 * @c pp_included_service_info arrays immediately following the struct.
 *
 * @note This context is freed automatically inside the application callback. Do not
 *       retain a pointer to it after the callback returns.
 *
 * Typical usage:
 * 1. Call @c gatt_intf_alloc_service_discovery_ctx() to allocate.
 * 2. Populate characteristic and included service info via the helper functions.
 * 3. Register the completion callback with @c gatt_intf_set_discovery_complete_callback().
 * 4. Start discovery with @c gatt_intf_start_service_discovery().
 * 5. Forward stack events to @c gatt_intf_on_service_discovery_result() and
 *    @c gatt_intf_on_service_discovery_complete() from the GATT event handler.
 */
struct _gatt_intf_service_discovery_ctx_t
{
    gatt_intf_discovery_result_t result; /**< Output: handle arrays populated during discovery */
    pfn_on_discovery_complete_t
        pfn_on_discovery_complete_app_cb; /**< Application callback invoked on discovery completion; context is freed before return */
    service_discovery_state_t service_discovery_state; /**< Current phase of the discovery state machine */
    wiced_timer_t discovery_cmpl_timer;                /**< Disconnect timer started on discovery failure */
    uint16_t last_found_char_handle; /**< ATT handle of the most recently discovered characteristic declaration,
                                          used to associate CCCD descriptors with the correct characteristic */
    void *
        p_app_ctx; /**< Optional pointer to application context data, allocated contiguously with the discovery context by @c gatt_intf_alloc_service_discovery_ctx() */
};


typedef enum
{
    GATT_INTF_VALUE_HANDLE_INDEX,
    GATT_INTF_DESCRIPTOR_HANDLE_INDEX,
    GATT_INTF_MAX_HANDLE_INDEX
} gatt_intf_cccd_map_handle_type_t;
typedef struct
{
    uint16_t handle[GATT_INTF_MAX_HANDLE_INDEX];
} gatt_intf_cccd_map_t;

#define CCCD_MAP(x)                                                                                                    \
    .handle[GATT_INTF_VALUE_HANDLE_INDEX] = HDLC_##x##_VALUE, .handle[GATT_INTF_DESCRIPTOR_HANDLE_INDEX] = HDLD_##x##_CLIENT_CONFIGURATION

typedef void (*gatt_intf_pfn_on_operation_complete_t)(uint16_t conn_id, wiced_bt_gatt_status_t status, void *pv_ctx);

typedef struct gatt_intf_operation_s gatt_intf_operation_t;

gatt_intf_operation_t *gatt_intf_alloc_notification_handle_list(uint16_t conn_id,
                                                         uint16_t max_characteristics,
                                                         gatt_intf_pfn_on_operation_complete_t pfn_on_complete,
                                                         void *pv_ctx);

gatt_intf_operation_t *gatt_intf_alloc_read_handle_list(uint16_t conn_id,
                                                        uint16_t max_characteristics,
                                                        gatt_intf_pfn_on_operation_complete_t pfn_on_complete,
                                                        void *pv_ctx);

void gatt_intf_free_operation_handle_list(gatt_intf_operation_t *p_op);

wiced_result_t gatt_intf_add_characteristic_to_list(gatt_intf_operation_t *p_op,
                                                    gatt_intf_characteristic_handles_t *p_handle,
                                                    uint16_t index);
wiced_bt_gatt_status_t gatt_intf_execute_handle_operations(gatt_intf_operation_t *p_op);

void gatt_intf_handle_gatt_operation_complete(wiced_bt_gatt_operation_complete_t *p_op_cplt,
                                              gatt_intf_operation_t *p_op);

/** @} Generic_Structures */

#ifdef __cplusplus
extern "C"
{
#endif

#define GATT_INTERFACE_TRACE(...) WICED_BT_TRACE(__VA_ARGS__)
#define GATT_INTERFACE_TRACE_CRIT(...) WICED_BT_TRACE(__VA_ARGS__)

    /**
     * @brief Allocates a buffer from the BT heap.
     *
     * Thin wrapper around @c wiced_bt_get_buffer().
     *
     * @param len  Number of bytes to allocate.
     * @return     Pointer to the allocated buffer, or @c NULL if the allocation fails.
     */
    uint8_t *gatt_intf_method_get_buffer(int len);

    /**
     * @brief Frees a buffer previously allocated by @c gatt_intf_method_get_buffer().
     *
     * Thin wrapper around @c wiced_bt_free_buffer(). Passing @c NULL is safe.
     *
     * @param ptr  Pointer to the buffer to free.
     */
    void gatt_intf_method_free_buffer(uint8_t *ptr);

    /**
     * @brief Sends a GATT Read Handle request for the value of a characteristic.
     *
     * Allocates a temporary read buffer of @p len bytes and issues
     * @c wiced_bt_gatt_client_send_read_handle() on @c p_handle->value_handle.
     *
     * @param conn_id   Connection ID of the target GATT connection.
     * @param p_handle  Pointer to the characteristic handles; @c value_handle is used.
     * @param len       Maximum number of bytes to read (size of the internal read buffer).
     * @return          @c WICED_BT_NO_RESOURCES if buffer allocation fails, otherwise the
     *                  result of @c wiced_bt_gatt_client_send_read_handle().
     */
    wiced_result_t gatt_intf_read_characteristic(uint16_t conn_id, gatt_intf_characteristic_handles_t *p_handle);

    /**
     * @brief Writes a Client Characteristic Configuration Descriptor (CCCD) to subscribe
     *        to notifications or indications on a remote characteristic.
     *
     * @param conn_id   Connection ID of the target GATT connection.
     * @param p_handle  Pointer to the characteristic handles; @c descriptor_handle is used
     *                  as the CCCD attribute handle.
     * @param cccd      CCCD value to write (e.g. @c GATT_CLIENT_CONFIG_NOTIFICATION or
     *                  @c GATT_CLIENT_CONFIG_INDICATION).
     * @return          Result of the underlying GATT write operation.
     */
    wiced_result_t gatt_intf_enable_notifications(uint16_t conn_id,
                                                  gatt_intf_characteristic_handles_t *p_handle,
                                                  uint16_t cccd);

    /**
     * @brief Writes data to a remote characteristic without requesting an ATT response
     *        (Write Command / Write Without Response).
     *
     * @param conn_id    Connection ID of the target GATT connection.
     * @param p_handle   Pointer to the characteristic handles; @c value_handle is used.
     * @param p_data_in  Pointer to the data to write.
     * @param len        Number of bytes to write from @p p_data_in.
     * @return           Result of the underlying GATT write operation.
     */
    wiced_result_t gatt_intf_write_no_rsp(uint16_t conn_id,
                                          gatt_intf_characteristic_handles_t *p_handle,
                                          uint8_t *p_data_in,
                                          uint16_t len);

    /**
     * @brief Writes data to a remote characteristic and waits for an ATT Write Response.
     *
     * @param conn_id    Connection ID of the target GATT connection.
     * @param p_handle   Pointer to the characteristic handles; @c value_handle is used.
     * @param p_data_in  Pointer to the data to write.
     * @param len        Number of bytes to write from @p p_data_in.
     * @return           Result of the underlying GATT write operation.
     */
    wiced_result_t gatt_intf_write_with_rsp(uint16_t conn_id,
                                            gatt_intf_characteristic_handles_t *p_handle,
                                            uint8_t *p_data_in,
                                            uint16_t len);

    typedef uint16_t (*pfn_get_cccd_value_t)(uint16_t conn_id, uint16_t char_handle);

    void gatt_intf_set_cccd_value_callback(pfn_get_cccd_value_t pfn_get_cccd_value);

    /**
     * @brief Sends a GATT notification or indication to a connected client.
     *
     * Allocates an internal copy of @p p_data and dispatches either
     * @c wiced_bt_gatt_server_send_indication() or @c wiced_bt_gatt_server_send_notification()
     * depending on the bits set in @p cccd. The internal copy is freed automatically by
     * the stack via @c gatt_intf_method_free_buffer() after the send completes.
     *
     * @param conn_id  Connection ID of the target GATT connection.
     * @param handle   ATT value handle of the characteristic to notify/indicate.
     * @param cccd     Current CCCD value for this client. Must have
     *                 @c GATT_CLIENT_CONFIG_NOTIFICATION or @c GATT_CLIENT_CONFIG_INDICATION set;
     *                 returns @c WICED_BT_GATT_CCCD_IMPROPER_CONFIGURED otherwise.
     * @param p_data   Pointer to the payload to send.
     * @param len      Length of the payload in bytes.
     * @return         @c WICED_BT_GATT_NO_RESOURCES if buffer allocation fails,
     *                 @c WICED_BT_GATT_CCCD_IMPROPER_CONFIGURED if @p cccd has neither
     *                 notification nor indication enabled, otherwise the result of the
     *                 underlying send operation.
     */
    wiced_bt_gatt_status_t gatt_intf_send_notification(uint16_t conn_id, uint16_t handle, uint8_t *p_data, uint16_t len);

    wiced_bt_gatt_status_t gatt_intf_send_read_response(wiced_bt_gatt_attribute_request_t *p_att_req,
                                                        wiced_bt_gatt_status_t status,
                                                        uint8_t *p_data,
                                                        uint16_t len);

    /**
     * @brief Processes an intermediate GATT discovery result event from the BT stack.
     *
     * Must be called from the application GATT event handler for every
     * @c GATT_DISCOVERY_RESULT_EVT while a discovery initiated by
     * @c gatt_intf_start_service_discovery() is in progress. Internally routes the result
     * to the appropriate handler for services, included services, characteristics, or
     * descriptors based on @c p_data->discovery_type.
     *
     * @param p_sdc   Pointer to the active service discovery context.
     * @param p_data  Pointer to the discovery result data provided by the BT stack.
     */
    void gatt_intf_on_service_discovery_result(gatt_intf_service_discovery_ctx_t *p_sdc,
                                               wiced_bt_gatt_discovery_result_t *p_data);

    /**
     * @brief Processes a GATT discovery complete event from the BT stack.
     *
     * Must be called from the application GATT event handler for every
     * @c GATT_DISCOVERY_CMPL_EVT while a discovery initiated by
     * @c gatt_intf_start_service_discovery() is in progress. Advances the discovery
     * state machine to the next phase or, when all phases are complete, invokes the
     * application callback and frees the discovery context.
     *
     * On failure the context is also freed and a 2 ms disconnect timer is started to
     * recover the connection.
     *
     * @param p_sdc   Pointer to the active service discovery context. After this call
     *                returns with state @c SERVICE_DISCOVERY_STATE_DISCOVER_COMPLETE or
     *                on failure, the context has been freed and must not be accessed.
     * @param p_data  Pointer to the discovery complete data provided by the BT stack.
     */
    void gatt_intf_on_service_discovery_complete(gatt_intf_service_discovery_ctx_t *p_sdc,
                                                 wiced_bt_gatt_discovery_complete_t *p_data);

    /**
     * @brief Allocates a service discovery context sized for the specified number of handles.
     *
     * Allocates a single contiguous block containing the @c gatt_intf_service_discovery_ctx_t
     * header followed by the @c p_handles array, @c pp_characteristic_info pointer array,
     * @c p_inc_handles array, and @c pp_included_service_info pointer array.
     *
     * After allocation, populate the input arrays using @c gatt_intf_copy_unique_characteristic_info(),
     * @c gatt_intf_repeat_characteristic_info(), and @c gatt_intf_repeat_included_service_info(),
     * then register a callback with @c gatt_intf_set_discovery_complete_callback() before
     * calling @c gatt_intf_start_service_discovery().
     *
     * @param max_char_handles  Number of characteristic handle slots to allocate.
     *                          Must match the total number of characteristic info entries
     *                          that will be registered before discovery starts.
     * @param max_inc_handles   Number of included service handle slots to allocate.
     *                          Must match the total number of included service info entries
     *                          that will be registered before discovery starts.
     * @param app_ctx_size     Optional size in bytes of an application context to allocate
     * @return                  Pointer to the allocated and zero-initialised context,
     *                          or @c NULL if the allocation fails.
     */
    gatt_intf_service_discovery_ctx_t *gatt_intf_alloc_service_discovery_ctx(uint8_t max_char_handles,
                                                                             uint8_t max_inc_handles,
                                                                             uint16_t app_ctx_size);

    /**
     * @brief Registers a contiguous array of unique characteristic definitions into the
     *        discovery context, one slot per entry.
     *
     * Fills @c input_params.pp_characteristic_info[0..num_char_info-1] with pointers to
     * consecutive entries in @p p_char_info_list, and sets @c result.num_characteristic_handles
     * accordingly. Use this when each characteristic in the array has a distinct UUID.
     *
     * @param p_sdc             Pointer to an allocated discovery context.
     * @param p_char_info_list  Pointer to the first element of a static characteristic
     *                          info array. Must remain valid for the duration of discovery.
     * @param num_char_info     Number of entries in @p p_char_info_list.
     *                          Must not exceed @c input_params.max_characteristic_handles.
     * @return                  Total number of characteristic slots registered, or @c 0
     *                          if @p num_char_info exceeds the context capacity.
     */
    uint8_t gatt_intf_copy_unique_characteristic_info(gatt_intf_service_discovery_ctx_t *p_sdc,
                                                     const gatt_intf_characteristic_info_t *p_char_info_list,
                                                     uint8_t num_char_info);

    /**
     * @brief Registers the same characteristic definition into multiple consecutive slots
     *        of the discovery context.
     *
     * Fills @c input_params.pp_characteristic_info[start_index..start_index+num_repeat-1]
     * with the same pointer @p p_char_info_list. Use this for repeating characteristics
     * (e.g., multiple ASE instances of the same UUID).
     *
     * @param p_sdc             Pointer to an allocated discovery context.
     * @param start_index       Zero-based index of the first slot to fill.
     * @param p_char_info_list  Pointer to the characteristic definition to repeat.
     *                          Must remain valid for the duration of discovery.
     * @param num_repeat        Number of consecutive slots to fill with @p p_char_info_list.
     *                          @c start_index + @c num_repeat must not exceed
     *                          @c input_params.max_characteristic_handles.
     * @return                  Total number of characteristic slots now registered
     *                          (i.e. @c start_index + @c num_repeat), or @c 0 on overflow.
     */
    uint8_t gatt_intf_repeat_characteristic_info(gatt_intf_service_discovery_ctx_t *p_sdc,
                                                 uint8_t start_index,
                                                 const gatt_intf_characteristic_info_t *p_char_info,
                                                 uint8_t num_repeat);

    /**
     * @brief Registers the same included service definition into multiple consecutive slots
     *        of the discovery context.
     *
     * Fills @c input_params.pp_included_service_info[start_index..start_index+num_repeat-1]
     * with the same pointer @p p_inc_info_list.
     *
     * @param p_sdc            Pointer to an allocated discovery context.
     * @param start_index      Zero-based index of the first slot to fill.
     * @param p_inc_info_list  Pointer to the included service definition to repeat.
     *                         Must remain valid for the duration of discovery.
     * @param num_repeat       Number of consecutive slots to fill with @p p_inc_info_list.
     *                         @c start_index + @c num_repeat must not exceed
     *                         @c input_params.max_inc_handles.
     * @return                 Total number of included service slots now registered
     *                         (i.e. @c start_index + @c num_repeat), or @c 0 on overflow.
     */
    uint8_t gatt_intf_repeat_included_service_info(gatt_intf_service_discovery_ctx_t *p_sdc,
                                                   uint8_t start_index,
                                                   const gatt_intf_included_service_info_t *p_inc_info,
                                                   uint8_t num_repeat);

    /**
     * @brief Registers the application callback to invoke when discovery completes.
     *
     * The callback is invoked by @c gatt_intf_on_service_discovery_complete() once all
     * discovery phases finish. After the callback returns, the discovery context is freed.
     *
     * @param p_sdc          Pointer to the discovery context.
     * @param pfn_on_complete Callback function to invoke on completion. Must not be @c NULL.
     */
    void gatt_intf_set_discovery_complete_callback(gatt_intf_service_discovery_ctx_t *p_sdc,
                                                   pfn_on_discovery_complete_t pfn_on_complete);

    /**
     * @brief Initiates GATT service discovery for the service identified by @p p_id.
     *
     * Validates preconditions, sets the discovery state to
     * @c SERVICE_DISCOVERY_STATE_DISCOVER_SERVICE, and issues a
     * @c GATT_DISCOVER_SERVICES_BY_UUID request starting from @p start_handle.
     * Subsequent BT stack events must be forwarded to @c gatt_intf_on_service_discovery_result()
     * and @c gatt_intf_on_service_discovery_complete() to drive the state machine.
     *
     * @param conn_id       Connection ID of the target GATT connection.
     * @param p_id          Pointer to the service identifier (name + UUID). Must not be @c NULL.
     * @param start_handle  ATT handle from which the UUID search begins (typically @c 0x0001).
     * @param p_sdc         Pointer to an allocated and fully configured discovery context.
     *                      Must be in @c SERVICE_DISCOVERY_STATE_IDLE; returns
     *                      @c WICED_ADDRESS_IN_USE if a discovery is already in progress.
     * @return              @c WICED_BT_ERROR if @p p_id is @c NULL,
     *                      @c WICED_ADDRESS_IN_USE if the context is not idle,
     *                      otherwise the result of @c wiced_bt_gatt_client_send_discover().
     */
    wiced_result_t gatt_intf_start_service_discovery(uint16_t conn_id,
                                                     const gatt_intf_service_name_uuid_t *p_id,
                                                     uint16_t start_handle,
                                                     gatt_intf_service_discovery_ctx_t *p_sdc);

    /**
    * @brief Initiates GATT secondary service discovery for the service identified by @p p_id.
    *
    * Validates preconditions, sets the discovery state to
    * @c SERVICE_DISCOVERY_STATE_DISCOVER_SERVICE, and issues a
    * @c GATT_DISCOVER_SERVICES_BY_UUID request starting from @p start_handle to @p end_handle.
    * Subsequent BT stack events must be forwarded to @c gatt_intf_on_service_discovery_result()
    * and @c gatt_intf_on_service_discovery_complete() to drive the state machine.
    *
    * @param conn_id       Connection ID of the target GATT connection.
    * @param p_id          Pointer to the service identifier (name + UUID). Must not be @c NULL.
    * @param start_handle  ATT handle from which the UUID search begins (typically @c 0x0001).
    * @param end_handle    ATT handle at which the UUID search ends (typically @c 0xFFFF).
    * @param p_sdc         Pointer to an allocated and fully configured discovery context.
    *                      Must be in @c SERVICE_DISCOVERY_STATE_IDLE; returns
    *                      @c WICED_ADDRESS_IN_USE if a discovery is already in progress.
    * @return              @c WICED_BT_ERROR if @p p_id is @c NULL,
    *                      @c WICED_ADDRESS_IN_USE if the context is not idle,
    *                      otherwise the result of @c wiced_bt_gatt_client_send_discover().
    */
    wiced_result_t gatt_intf_start_secondary_service_discovery(uint16_t conn_id,
                                                               const gatt_intf_service_name_uuid_t *p_id,
                                                               uint16_t start_handle,
                                                               uint16_t end_handle,
                                                               gatt_intf_service_discovery_ctx_t *p_sdc);

    /**
    * Helper functions to count valid entries in the result handle arrays based on the discovery input parameters.
    * @param p_handles Pointer to the array of characteristic handles
    * @param max      Maximum number of handles to consider.
    * @return        Number of valid handles found.
    */
    int gatt_intf_get_valid_handle_count(gatt_intf_characteristic_handles_t *p_handles, int max);

    /**
    * Helper functions to count valid entries in the result handle arrays based on the discovery input parameters.
    * @param p_inc Pointer to the array of included service handles
    * @param max      Maximum number of handles to consider.
    * @return        Number of valid handles found.
    */
    int gatt_intf_get_valid_included_handle_count(gatt_intf_included_service_handles_t *p_inc, int max);

    /**
    * Checks if the service discovery process is complete. Application can free the service discovery context after this returns true.
    *
    * @param p_sdc Pointer to the discovery context.
    * @return      1 if the discovery is complete, 0 otherwise.
    */
    int gatt_intf_is_service_discovery_complete(gatt_intf_service_discovery_ctx_t *p_sdc);

    /**
     * Frees the service discovery context. Should be called after the discovery is complete.
     *
     * @param p_sdc Pointer to the discovery context to free.
     */
    void gatt_intf_free_service_discovery_ctx(gatt_intf_service_discovery_ctx_t *p_sdc);

    /**
 * Function     gatt_intf_method_alloc_server_notification_packet
 *
 *              Free buffer allocated with \ref gatt_intf_method_get_buffer
 *
 *  @param[in]  conn_id    : GATT connection id
 *  @param[in]  p_handle   : characteristic handle
 *  @param[in]  type       : Notification or Indication
 *  @param[in]  req_len    : Requested buffer len
 *  @return @link buffer allocated @endlink
 */
    uint8_t *gatt_intf_method_alloc_server_notification_packet(uint16_t conn_id,
                                                               uint16_t handle,
                                                               wiced_bt_gatt_client_char_config_t type,
                                                               int req_len);

    /**
 * Function     gatt_intf_method_free_buffer
 *
 *              Free buffer allocated with \ref gatt_intf_method_get_buffer or \ref gatt_intf_method_alloc_server_notification_packet
 *
 *
 *  @param[in]  len : length of the buffer
 *
 *  @return @link buffer allocated @endlink
 */
    void gatt_intf_method_free_buffer(uint8_t *ptr);

    void gatt_intf_print_handles(gatt_intf_discovery_result_t *p_result);

    int gatt_intf_find_characteristic_type_by_value_handle(gatt_intf_characteristic_handles_t *p_handles,
                                                           int num_handles,
                                                           uint16_t handle);

    wiced_bt_gatt_status_t gatt_intf_method_send_indication(uint16_t conn_id,
                                                            uint16_t handle,
                                                            uint16_t data_len,
                                                            uint8_t *p_data,
                                                            void *pv_ctx);
#ifdef __cplusplus
}
#endif

#endif /* GATT_INTERFACE_H */
