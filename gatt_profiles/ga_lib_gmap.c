/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Gaming Audio Service Init
  */

#include "ga_lib_gmap.h"


const gatt_intf_characteristic_info_t gmap_characteristic_info[] = {
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_GMAP_ROLE},
     .characteristic_type = GA_LIB_GMAP_CHARACTERISTIC_ROLE,
     .name = "gmap_role"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_GMAP_UGG_FEATURES},
     .characteristic_type = GA_LIB_GMAP_CHARACTERISTIC_UGG_FEATURES,
     .name = "ugg_features"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_GMAP_UGT_FEATURES},
     .characteristic_type = GA_LIB_GMAP_CHARACTERISTIC_UGT_FEATURES,
     .name = "ugt_features"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_GMAP_BGS_FEATURES},
     .characteristic_type = GA_LIB_GMAP_CHARACTERISTIC_BGS_FEATURES,
     .name = "bgs_features"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_GMAP_BGR_FEATURES},
     .characteristic_type = GA_LIB_GMAP_CHARACTERISTIC_BGR_FEATURES,
     .name = "bgr_features"},
};

const gatt_intf_service_name_uuid_t gmap_id = {
    .name = "gmap",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_GAMING_AUDIO_SERVICE,

};

gatt_intf_service_discovery_ctx_t *ga_lib_gmap_discover_service(uint16_t conn_id,
                                                             pfn_on_discovery_complete_t pfn_on_complete)
{
    uint8_t num_characteristics = sizeof(gmap_characteristic_info) / sizeof(gmap_characteristic_info[0]);
    gatt_intf_service_discovery_ctx_t *p_sdc = gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, 0);
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);

    gatt_intf_copy_unique_characteristic_info(p_sdc, gmap_characteristic_info, num_characteristics);

    gatt_intf_start_service_discovery(conn_id, &gmap_id, 1, p_sdc);

    return p_sdc;
}
