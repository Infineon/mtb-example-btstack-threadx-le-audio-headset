/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Media Control Client/Service init implementation.
  */


#include "ga_lib_mcs.h"


#ifdef MCS_DEBUG
#define MCS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get MCS library traces */
#else
#define MCS_TRACE(...)
#endif

const gatt_intf_characteristic_info_t mcs_characteristic_info[] =
{
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_PLAYER_NAME},
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYER_NAME,
     .name = "player"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_TRACK_CHANGED},
     .uuid.uu.uuid16 = WICED_BT_UUID_MEDIA_TRACK_CHANGED,
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_CHANGED,
     .name = "track_chg"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_TRACK_TITLE},
     .uuid.uu.uuid16 = WICED_BT_UUID_MEDIA_TRACK_TITLE,
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_TITLE,
     .name = "title"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_TRACK_DURATION},
     .uuid.uu.uuid16 = WICED_BT_UUID_MEDIA_TRACK_DURATION,
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_DURATION,
     .name = "duration"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_TRACK_POSITION},
     .uuid.uu.uuid16 = WICED_BT_UUID_MEDIA_TRACK_POSITION,
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_POSITION,
     .name = "pos"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_PLAYBACK_SPEED},
     .uuid.uu.uuid16 = WICED_BT_UUID_MEDIA_PLAYBACK_SPEED,
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYBACK_SPEED,
     .name = "speed"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_SEEKING_SPEED},
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_SEEKING_SPEED,
     .name = "seek_speed"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_PLAYING_ORDER},
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYING_ORDER,
     .name = "order"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_PLAYING_ORDER_SUPPORTED},
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYING_ORDER_SUPPORTED,
     .name = "ord_support"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_STATE},
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_STATE,
     .name = "state"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_CONTROL_POINT},
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_CONTROL_POINT,
     .name = "cp"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_CONTROL_OPCODE_SUPPORTED},
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_OPCODE_SUPPORTED,
     .name = "opcode_supp"},
#if 0
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_CONTENT_CONTROL_ID},
     .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_CONTENT_CONTROL_ID,
     .name = "ccid"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_ICON_OBJECT},
        .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_ICON_OBJECT,
        .name = "icon_obj"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_ICON_URI},
        .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_ICON_URI,
        .name = "icon_uri"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_TRACK_SEGMENT_OBJECT},
        .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_SEGMENT,
        .name = "segment_obj"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_CURRENT_TRACK_OBJECT},
        .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_CURRENT_TRACK_OBJECT,
        .name = "curr_track_obj"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_NEXT_TRACK_OBJECT},
        .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_NEXT_TRACK_OBJECT,
        .name = "next_track_obj"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_PARENT_GROUP_OBJECT},
        .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_PARENT_GROUP_OBJ,
        .name = "parent_gr_obj"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_CURRENT_GROUP_OBJECT},
        .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_CURRENT_GROUP_OBJ,
        .name = "curr_gr_obj"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_SEARCH_RESULTS_OBJECT},
        .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_SEARCH_RESULTS_OBJ,
        .name = "search_rst_obj"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_SEARCH_CONTROL_POINT},
        .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_SEARCH_CONTROL_POINT,
        .name = "search_cp"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_MEDIA_CONTENT_CONTROL_ID},
        .characteristic_type = GA_LIB_MCS_CHARACTERISTIC_MEDIA_CONTENT_CONTROL_ID,
        .name = "media_ccd"},
#endif
};

const gatt_intf_service_name_uuid_t gmcs_id = {
    .name = "gmcs",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_GENERIC_MEDIA_CONTROL,
};

const gatt_intf_service_name_uuid_t mcs_id = {
    .name = "mcs",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_MEDIA_CONTROL,
};

gatt_intf_service_discovery_ctx_t *ga_lib_mcs_discover_service_int(uint16_t conn_id,
                                                                uint16_t start_handle,
                                                                const gatt_intf_service_name_uuid_t *p_service_id,
                                                                pfn_on_discovery_complete_t pfn_on_complete)
{
    uint8_t num_characteristics = sizeof(mcs_characteristic_info) / sizeof(mcs_characteristic_info[0]);
    gatt_intf_service_discovery_ctx_t *p_sdc = gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, 0);
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);

    gatt_intf_copy_unique_characteristic_info(p_sdc, mcs_characteristic_info, num_characteristics);

    gatt_intf_start_service_discovery(conn_id, p_service_id, start_handle, p_sdc);

    return p_sdc;
}

gatt_intf_service_discovery_ctx_t *ga_lib_gmcs_discover_service(uint16_t conn_id,
                                                             pfn_on_discovery_complete_t pfn_on_complete)
{
    return ga_lib_mcs_discover_service_int(conn_id, 1, &gmcs_id, pfn_on_complete);
}

gatt_intf_service_discovery_ctx_t *ga_lib_mcs_discover_service(uint16_t conn_id,
                                                            uint16_t start_handle,
                                                            pfn_on_discovery_complete_t pfn_on_complete)
{
    return ga_lib_mcs_discover_service_int(conn_id, start_handle, &mcs_id, pfn_on_complete);
}

wiced_bt_gatt_status_t ga_lib_mcs_write_control_media(uint16_t conn_id,
                                                   gatt_intf_characteristic_handles_t *p_handle,
                                                   ga_lib_mcs_operation_t *op_data)
{
    uint8_t *p_write_data = gatt_intf_method_get_buffer(5);
    uint8_t *p = p_write_data;

    if (!p_write_data)
    {
        return WICED_BT_GATT_NO_RESOURCES;
    }

    //Validate the opcode
    UINT8_TO_STREAM(p, op_data->opcode);

    switch (op_data->opcode)
    {
    case GA_LIB_MCS_MOVE_RELATIVE:
    {
        UINT32_TO_STREAM(p, op_data->data.move_relative_offset);
    }
    break;
    case GA_LIB_MCS_GOTO_TRACK:
    {
        UINT32_TO_STREAM(p, op_data->data.track_number);
    }
    break;
    case GA_LIB_MCS_GOTO_SEGMENT:
    {
        UINT32_TO_STREAM(p, op_data->data.segment_number);
    }
    break;
    case GA_LIB_MCS_GOTO_GROUP:
    {
        UINT32_TO_STREAM(p, op_data->data.group_number);
    }
    break;
    default:
        break;
    }

    wiced_bt_gatt_write_hdr_t write_hdr = {.handle = p_handle->value_handle, .offset = 0, .len = (uint16_t)(p - p_write_data)};

    return wiced_bt_gatt_client_send_write(conn_id,
                                        GATT_CMD_WRITE,
                                        &write_hdr,
                                        p_write_data,
                                        (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);
}

wiced_bt_gatt_status_t ga_lib_mcs_notify_player_name(uint16_t conn_id, uint16_t handle, const char *name)
{
    return gatt_intf_send_notification(conn_id, handle, (uint8_t *)name, strlen(name));
}

wiced_bt_gatt_status_t ga_lib_mcs_notify_track_title(uint16_t conn_id, uint16_t handle, const char *title)
{
    return gatt_intf_send_notification(conn_id, handle, (uint8_t *)title, strlen(title));
}

wiced_bt_gatt_status_t ga_lib_mcs_notify_track_changed(uint16_t conn_id, uint16_t handle)
{
    return gatt_intf_send_notification(conn_id, handle, NULL, 0);
}

wiced_bt_gatt_status_t ga_lib_mcs_notify_track_duration(uint16_t conn_id, uint16_t handle, int32_t duration)
{
    uint8_t data[4], *p_data = data;
    UINT32_TO_STREAM(p_data, duration);

    return gatt_intf_send_notification(conn_id, handle, data, sizeof(data));
}

wiced_bt_gatt_status_t ga_lib_mcs_notify_track_position(uint16_t conn_id, uint16_t handle, int32_t position)
{
    uint8_t data[4], *p_data = data;
    UINT32_TO_STREAM(p_data, position);

    return gatt_intf_send_notification(conn_id, handle, data, sizeof(data));
}

wiced_bt_gatt_status_t ga_lib_mcs_notify_playback_speed(uint16_t conn_id, uint16_t handle, int8_t speed)
{
    return gatt_intf_send_notification(conn_id, handle, (uint8_t *)&speed, sizeof(speed));
}

wiced_bt_gatt_status_t ga_lib_mcs_notify_seeking_speed(uint16_t conn_id, uint16_t handle, int8_t speed)
{
    return gatt_intf_send_notification(conn_id, handle, (uint8_t *)&speed, sizeof(speed));
}

wiced_bt_gatt_status_t ga_lib_mcs_notify_playing_order(uint16_t conn_id, uint16_t handle, uint8_t order)
{
    return gatt_intf_send_notification(conn_id, handle, &order, sizeof(order));
}

wiced_bt_gatt_status_t ga_lib_mcs_notify_media_state(uint16_t conn_id, uint16_t handle, uint8_t state)
{
    return gatt_intf_send_notification(conn_id, handle, &state, sizeof(state));
}

wiced_bt_gatt_status_t ga_lib_mcs_notify_cp_result(uint16_t conn_id, uint16_t handle, uint8_t opcode, uint8_t result)
{
    uint8_t data[2];
    data[0] = opcode;
    data[1] = result;
    return gatt_intf_send_notification(conn_id, handle, data, sizeof(data));
}

wiced_bt_gatt_status_t ga_lib_mcs_notify_media_control_opcodes_supported(uint16_t conn_id, uint16_t handle, uint32_t opcodes_supported)
{
    uint8_t data[4], *p_data = data;
    UINT32_TO_STREAM(p_data, opcodes_supported);

    return gatt_intf_send_notification(conn_id, handle, data, sizeof(data));
}
