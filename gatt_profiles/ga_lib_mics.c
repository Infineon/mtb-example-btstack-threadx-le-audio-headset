/*
 * $ Copyright Cypress Semiconductor $
 */
 /** @file
  *
  * Volume Control Control implementation.
  */

#include "ga_lib_mics.h"

#ifdef MICS_DEBUG
#define MICS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get MICS library traces */
#else
#define MICS_TRACE(...)
#endif

const gatt_intf_characteristic_info_t mics_characteristic_info[] = {
    {
        .uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MICS_MUTE_STATE},
        .characteristic_type = GA_LIB_MICS_CHARACTERISTIC_MUTE_STATE,
        .name = "mute"
    },
};

const gatt_intf_service_name_uuid_t mics_id = {
    .name = "mics",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_MICROPHONE_CONTROL,
};

const gatt_intf_included_service_info_t mics_included[] = {
    {.name = "aics", .uuid.len = 2, .uuid.uu.uuid16 = WICED_BT_UUID_AUDIO_INPUT_CONTROL},
};

gatt_intf_service_discovery_ctx_t *ga_lib_mics_discover_service(uint16_t conn_id,
                                                             pfn_on_discovery_complete_t pfn_on_complete,
                                                             uint8_t max_num_aics_instances)
{
    uint8_t num_characteristics = sizeof(mics_characteristic_info) / sizeof(mics_characteristic_info[0]);
    gatt_intf_service_discovery_ctx_t *p_sdc =
        gatt_intf_alloc_service_discovery_ctx(num_characteristics, max_num_aics_instances, sizeof(gatt_intf_service_discovery_ctx_t));
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);

    /* ignoring the return value */
    gatt_intf_copy_unique_characteristic_info(p_sdc, mics_characteristic_info, num_characteristics);

    /* ignoring the return value */
    gatt_intf_repeat_included_service_info(p_sdc,             /* discovery ctx*/
                                           0,                 /* included service start index */
                                           &mics_included[0], /* uuid of included service  */
                                           max_num_aics_instances); /* number of times to repeat the included service */

    gatt_intf_start_service_discovery(conn_id, &mics_id, 1, p_sdc);

    return p_sdc;
}

wiced_bt_gatt_status_t ga_lib_mics_notify_mute_state(uint16_t conn_id, uint16_t handle, ga_lib_mute_val_t mute_state)
{
    return gatt_intf_send_notification(conn_id, handle, &mute_state, 1);
}

wiced_bt_gatt_status_t ga_lib_mics_write_mute_state(uint16_t conn_id,
                                                 gatt_intf_characteristic_handles_t *p_handle,
                                                 ga_lib_mute_val_t mute_val)
{
    uint8_t *p_write_data = gatt_intf_method_get_buffer(1);
    uint8_t *p_data = p_write_data;

    UINT8_TO_STREAM(p_data, mute_val);

    {
        wiced_bt_gatt_write_hdr_t write_hdr = {.handle = p_handle->value_handle,
                                               .offset = 0,
                                               .len = (uint16_t)(p_data - p_write_data)};

        return wiced_bt_gatt_client_send_write(conn_id,
                                               GATT_REQ_WRITE,
                                               &write_hdr,
                                               p_write_data,
                                               (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);
    }
}
