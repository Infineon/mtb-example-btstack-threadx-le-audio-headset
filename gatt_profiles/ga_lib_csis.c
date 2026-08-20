/*
 * $ Copyright Cypress Semiconductor $
 */
 /** @file
  *
  * Media Control Client/Service init implementation.
  */

#include "ga_lib_csis.h"

#ifdef CSIS_DEBUG
#define CSIS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get CSIS library traces */
#else
#define CSIS_TRACE(...)
#endif

typedef struct
{
    gatt_intf_service_discovery_ctx_t discovery_ctx;
    gatt_intf_characteristic_handles_t handle_list[GA_LIB_CSIS_CHARACTERISTIC_MAX];
} ga_lib_csis_service_discovery_ctx_t;

const gatt_intf_characteristic_info_t csis_characteristic_info[] = {
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_CSIS_SIRK},
     .characteristic_type = GA_LIB_CSIS_CHARACTERISTIC_SIRK,
     .name = "sirk"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_CSIS_SIZE},
     .characteristic_type = GA_LIB_CSIS_CHARACTERISTIC_SIZE,
     .name = "size"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_CSIS_LOCK},
     .characteristic_type = GA_LIB_CSIS_CHARACTERISTIC_LOCK,
     .name = "lock"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_CSIS_RANK},
     .characteristic_type = GA_LIB_CSIS_CHARACTERISTIC_RANK,
     .name = "rank"},
};


const gatt_intf_service_name_uuid_t csis_id = {
    .name = "csis",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_COORDINATE_SET_IDENTIFICATION,
};

/**< Starts CSIS service discovery
* Invokes \p pfn_store callback with the service handles on discovering the CSIS service and its characteristics.
* The application must save the service handles for subsequent operations on the service on discovery complete
*
* The function starts discovering services by issueing a wiced_bt_gatt_client_send_discover with the discovery_type
* set to GATT_DISCOVER_SERVICES_BY_UUID in range 1 to 0xffff. This returns the service information
*
* On discovery complete, then issue wiced_bt_gatt_client_send_discover with the discovery_type set to
* GATT_DISCOVER_CHARACTERISTICS, and range set to the start and end returned by the earlier event
*
* The last step is to issue wiced_bt_gatt_client_send_discover with discovery_type set to
* GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS which is used to get the descriptor handles
*
*/
gatt_intf_service_discovery_ctx_t *ga_lib_csis_discover_service(uint16_t conn_id,
                                                             pfn_on_discovery_complete_t pfn_on_complete)
{
    uint8_t num_characteristics = sizeof(csis_characteristic_info) / sizeof(csis_characteristic_info[0]);
    gatt_intf_service_discovery_ctx_t *p_sdc = gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, 0);
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);

    gatt_intf_copy_unique_characteristic_info(p_sdc, csis_characteristic_info, num_characteristics);

    gatt_intf_start_service_discovery(conn_id, &csis_id, 1, p_sdc);

    return p_sdc;
}

wiced_bt_gatt_status_t ga_lib_csis_notify_sirk(uint16_t conn_id, uint16_t handle, ga_lib_csis_sirk_data_t *p_sirk)
{
    uint8_t buff[1 + GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN];
    uint8_t *ptr = buff;

    UINT8_TO_STREAM(ptr, p_sirk->sirk_type);
    REVERSE_ARRAY_TO_STREAM(ptr, p_sirk->sirk, GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN);

    return gatt_intf_send_notification(conn_id, handle, buff, ptr - buff);
}

wiced_bt_gatt_status_t ga_lib_csis_notify_sirk_size(uint16_t conn_id, uint16_t handle, uint8_t value)
{
    return gatt_intf_send_notification(conn_id, handle, &value, sizeof(value));
}

wiced_bt_gatt_status_t ga_lib_csis_notify_lock(uint16_t conn_id, uint16_t handle, uint8_t value)
{
    return gatt_intf_send_notification(conn_id, handle, &value, sizeof(value));
}
