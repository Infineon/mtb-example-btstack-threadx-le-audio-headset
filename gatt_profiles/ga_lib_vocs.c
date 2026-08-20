/*
 * $ Copyright Cypress Semiconductor $
 */

#include "ga_lib_vocs.h"

#ifdef VOCS_DEBUG
#define VOCS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get VOCS library traces */
#else
#define VOCS_TRACE(...)
#endif

const gatt_intf_characteristic_info_t vocs_characteristic_info[] =
{
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_VOLUME_OFFSET_STATE},
     .characteristic_type = GA_LIB_VOCS_CHARACTERISTIC_OFFSET_STATE,
     .name = "state"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_AUDIO_LOCATION},
     .characteristic_type = GA_LIB_VOCS_CHARACTERISTIC_AUDIO_LOCATION,
     .name = "aud_loc"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_VOLUME_OFFSET_CONTROL_POINT},
     .characteristic_type = GA_LIB_VOCS_CHARACTERISTIC_VOLUME_OFFSET_CONTROL_POINT,
     .name = "offset_cp"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_AUDIO_OUTPUT_DESCRIPTION},
     .characteristic_type = GA_LIB_VOCS_CHARACTERISTIC_AUDIO_DESCRIPTION,
     .name = "desc"}
};

const gatt_intf_service_name_uuid_t vocs_id = {
    .name = "vocs",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_VOLUME_OFFSET_CONTROL,
};

typedef struct
{
    gatt_intf_service_discovery_ctx_t discovery_ctx;
    gatt_intf_characteristic_handles_t handle_list[GA_LIB_VOCS_CHARACTERISTIC_MAX];
} ga_lib_vocs_service_discovery_ctx_t;

gatt_intf_service_discovery_ctx_t *ga_lib_vocs_discover_vocs_service(uint16_t conn_id,
                                                                     pfn_on_discovery_complete_t pfn_on_complete,
                                                                     uint16_t start_handle, uint16_t end_handle)
{
    uint8_t num_characteristics = sizeof(vocs_characteristic_info) / sizeof(vocs_characteristic_info[0]);
    gatt_intf_service_discovery_ctx_t *p_sdc = gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, 0);
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);

    gatt_intf_copy_unique_characteristic_info(p_sdc, vocs_characteristic_info, num_characteristics);

    gatt_intf_start_secondary_service_discovery(conn_id, &vocs_id, start_handle, end_handle, p_sdc);

    return p_sdc;
}

wiced_bt_gatt_status_t ga_lib_vocs_notify_audio_description(uint16_t conn_id,
                                                         uint16_t handle,
                                                         const char *p_desc,
                                                         uint16_t len_to_write)
{
    return gatt_intf_send_notification(conn_id, handle, (uint8_t *)p_desc, len_to_write);
}

wiced_bt_gatt_status_t ga_lib_vocs_notify_volume_offset(uint16_t conn_id,
                                                     uint16_t handle,
                                                     int16_t volume_offset,
                                                     uint8_t change_counter)
{
    uint8_t buff[3];
    uint8_t *ptr = buff;

    UINT16_TO_STREAM(ptr, volume_offset);
    UINT8_TO_STREAM(ptr, change_counter);
    return gatt_intf_send_notification(conn_id, handle, buff, ptr - buff);
}

wiced_bt_gatt_status_t ga_lib_vocs_notify_audio_location(uint16_t conn_id, uint16_t handle, uint32_t audio_location)
{
    uint8_t buff[4];
    uint8_t *ptr = buff;
    UINT32_TO_STREAM(ptr, audio_location);
    return gatt_intf_send_notification(conn_id, handle, buff, ptr - buff);
}
