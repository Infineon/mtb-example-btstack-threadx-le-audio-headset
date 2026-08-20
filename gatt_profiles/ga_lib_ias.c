/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Immediate Alert Service Init
  */

#include "ga_lib_ias.h"

#ifdef IAS_DEBUG
#define IAS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get IAS library traces */
#else
#define IAS_TRACE(...)
#endif



const gatt_intf_characteristic_info_t ias_characteristic_info[] =
{
    {.uuid = {.len = 2, .uu.uuid16 = UUID_CHARACTERISTIC_ALERT_LEVEL},
     .characteristic_type = GA_LIB_IAS_CHARACTERISTIC_ALERT_LEVEL,
     .name = "alert_level"},
};

const gatt_intf_service_name_uuid_t ias_id = {
    .name = "ias_server",
    .uuid.len = 2,
    .uuid.uu.uuid16 = UUID_SERVICE_IMMEDIATE_ALERT,
};

gatt_intf_service_discovery_ctx_t *ga_lib_ias_discover_service(uint16_t conn_id,
                                                            pfn_on_discovery_complete_t pfn_on_complete)
{
    uint8_t num_characteristics = sizeof(ias_characteristic_info) / sizeof(ias_characteristic_info[0]);
    gatt_intf_service_discovery_ctx_t *p_sdc = gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, 0);
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);

    gatt_intf_copy_unique_characteristic_info(p_sdc, ias_characteristic_info, num_characteristics);

    gatt_intf_start_service_discovery(conn_id, &ias_id, 1, p_sdc);

    return p_sdc;
}
