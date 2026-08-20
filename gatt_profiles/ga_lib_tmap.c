/*
 * $ Copyright Cypress Semiconductor $
 */
 /** @file
  *
  * Volume Control Service Init
  */

#include "ga_lib_tmap.h"


const gatt_intf_characteristic_info_t tmap_characteristic_info[] = {
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TMAP_ROLE},
     .characteristic_type = GA_LIB_TMAP_CHARACTERISTIC_ROLE,
     .name = "tmap_role"}};

const gatt_intf_service_name_uuid_t tmap_id = {
    .name = "tmap_server",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_TMAS,

};

gatt_intf_service_discovery_ctx_t *ga_lib_tmap_discover_service(uint16_t conn_id,
                                                             pfn_on_discovery_complete_t pfn_on_complete)
{
    uint8_t num_characteristics = sizeof(tmap_characteristic_info) / sizeof(tmap_characteristic_info[0]);
    gatt_intf_service_discovery_ctx_t *p_sdc = gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, 0);
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);

    gatt_intf_copy_unique_characteristic_info(p_sdc, tmap_characteristic_info, num_characteristics);

    gatt_intf_start_service_discovery(conn_id, &tmap_id, 1, p_sdc);

    return p_sdc;
}
