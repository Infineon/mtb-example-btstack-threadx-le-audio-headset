/*
 * $ Copyright Cypress Semiconductor $
 */

#include "ga_lib_has.h"

#ifdef HAS_DEBUG
#define HAS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get HAS library traces */
#else
#define HAS_TRACE(...)
#endif

typedef struct
{
    gatt_intf_service_discovery_ctx_t discovery_ctx;
    gatt_intf_characteristic_handles_t handle_list[GA_LIB_HAS_CHARACTERISTIC_MAX];
} ga_lib_has_service_discovery_ctx_t;

const gatt_intf_characteristic_info_t has_characteristic_info[] =
{
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_HAS_HEARING_AID_FEATURES},
     .characteristic_type = GA_LIB_HAS_CHARACTERISTIC_HEARING_AID_FEATURES,
     .name = "features"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_HAS_HEARING_AID_PRESET_CONTROL_POINT},
     .characteristic_type = GA_LIB_HAS_CHARACTERISTIC_HEARING_AID_PRESET_CONTROL_POINT,
     .name = "preset_cp"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_HAS_HEARING_AID_ACTIVE_PRESET_INDEX},
     .characteristic_type = GA_LIB_HAS_CHARACTERISTIC_HEARING_AID_ACTIVE_PRESET_INDEX,
     .name = "active_preset_index"},
};

const gatt_intf_service_name_uuid_t has_id = {
    .name = "has",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_HEARING_ACCESS,
};

/**< Starts HAS service discovery
* Invokes \p pfn_store callback with the service handles on discovering the HAS service and its characteristics.
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
gatt_intf_service_discovery_ctx_t *ga_lib_has_discover_service(uint16_t conn_id,
                                                            pfn_on_discovery_complete_t pfn_on_complete)
{
    uint8_t num_characteristics = sizeof(has_characteristic_info) / sizeof(has_characteristic_info[0]);
    gatt_intf_service_discovery_ctx_t *p_sdc = gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, 0);
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);

    gatt_intf_copy_unique_characteristic_info(p_sdc, has_characteristic_info, num_characteristics);

    gatt_intf_start_service_discovery(conn_id, &has_id, 1, p_sdc);

    return p_sdc;
}

wiced_bt_gatt_status_t ga_lib_has_notify_read_preset_rsp(uint16_t conn_id,
                                                      uint16_t handle,
                                                      ga_lib_has_preset_records_t *p_rec)
{
    uint8_t *ptr, *ptr_start;
    int max_len = wiced_bt_gatt_get_bearer_mtu(conn_id) - WICED_GATT_HDR_SIZE;

    max_len = MIN(max_len, 5 + strlen(p_rec->name) + 1);
    ptr_start = ptr =
        gatt_intf_method_alloc_server_notification_packet(conn_id, handle, GATT_CLIENT_CONFIG_INDICATION, max_len);

    if (!ptr)
    {
        return WICED_BT_GATT_NO_RESOURCES;
    }

    UINT8_TO_STREAM(ptr, GA_LIB_HAS_OPCODE_READ_PRESET_RESPONSE);
    UINT8_TO_STREAM(ptr, p_rec->is_last);
    UINT8_TO_STREAM(ptr, p_rec->preset_index);
    UINT8_TO_STREAM(ptr, p_rec->properties);
    ARRAY_TO_STREAM(ptr, p_rec->name, strlen(p_rec->name) + 1);

    wiced_bt_gatt_status_t status =
        gatt_intf_method_send_indication(conn_id,
                                         handle,
                                         ptr - ptr_start,
                                         ptr_start,
                                         (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);

    return status;
}

wiced_bt_gatt_status_t ga_lib_has_notify_preset_changed(uint16_t conn_id,
                                                     uint16_t handle,
                                                     ga_lib_has_cp_rsp_preset_changed_t *p_change)
{
    uint8_t *ptr, *ptr_start;
    int max_len = wiced_bt_gatt_get_bearer_mtu(conn_id) - WICED_GATT_HDR_SIZE;
    ga_lib_has_preset_records_t *p_rec = p_change->p_rec;

    max_len = MIN(max_len, 6 + strlen(p_rec->name) + 1);
    ptr_start = ptr =
        gatt_intf_method_alloc_server_notification_packet(conn_id, handle, GATT_CLIENT_CONFIG_INDICATION, max_len);

    if (!ptr)
    {
        return WICED_BT_GATT_NO_RESOURCES;
    }

    UINT8_TO_STREAM(ptr, GA_LIB_HAS_OPCODE_PRESET_CHANGED);
    UINT8_TO_STREAM(ptr, p_change->change_id);
    UINT8_TO_STREAM(ptr, p_change->is_last);

    if (p_change->change_id == GA_LIB_HAS_GENERIC_UPDATE)
    {
        UINT8_TO_STREAM(ptr, p_change->prev_index);
        UINT8_TO_STREAM(ptr, p_rec->preset_index);
        UINT8_TO_STREAM(ptr, p_rec->properties);
        ARRAY_TO_STREAM(ptr, p_rec->name, strlen(p_rec->name) + 1);
    }
    else
    {
        UINT8_TO_STREAM(ptr, p_rec->preset_index);
    }

    wiced_bt_gatt_status_t status =
        gatt_intf_method_send_indication(conn_id,
                                         handle,
                                         ptr - ptr_start,
                                         ptr_start,
                                         (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);

    return status;
}

wiced_bt_gatt_status_t ga_lib_has_notify_active_preset_index(uint16_t conn_id,
                                                          uint16_t handle,
                                                          uint8_t active_preset_index)
{
    uint8_t *ptr, *ptr_start;
    int max_len = wiced_bt_gatt_get_bearer_mtu(conn_id) - WICED_GATT_HDR_SIZE;

    max_len = MIN(max_len, 5);
    ptr_start = ptr =
        gatt_intf_method_alloc_server_notification_packet(conn_id, handle, GATT_CLIENT_CONFIG_INDICATION, max_len);

    if (!ptr)
    {
        return WICED_BT_GATT_NO_RESOURCES;
    }

    UINT8_TO_STREAM(ptr, active_preset_index);

    wiced_bt_gatt_status_t status =
        gatt_intf_method_send_indication(conn_id,
                                         handle,
                                         ptr - ptr_start,
                                         ptr_start,
                                         (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);

    return status;
}

wiced_bt_gatt_status_t ga_lib_has_write_control_point_read_presets(uint16_t conn_id,
                                                                gatt_intf_characteristic_handles_t *p_handles,
                                                                uint8_t start_index,
                                                                uint8_t num_presets)
{
    uint8_t buff[3];
    uint8_t *p_buff = buff;

    UINT8_TO_STREAM(p_buff, GA_LIB_HAS_OPCODE_READ_PRESETS_REQUEST);
    UINT8_TO_STREAM(p_buff, start_index);
    UINT8_TO_STREAM(p_buff, num_presets);

    return gatt_intf_write_with_rsp(conn_id, p_handles, buff, p_buff - buff);
}

wiced_bt_gatt_status_t ga_lib_has_write_control_point_preset_name(uint16_t conn_id,
                                                               gatt_intf_characteristic_handles_t *p_handles,
                                                               uint8_t preset_index,
                                                               const char *preset_name)
{
    int name_len = strlen(preset_name) + 1;
    uint8_t *p_buff_start = gatt_intf_method_get_buffer(1 + 1 + name_len);
    uint8_t *p_buff = p_buff_start;

    UINT8_TO_STREAM(p_buff, GA_LIB_HAS_OPCODE_WRITE_PRESET_NAME);
    UINT8_TO_STREAM(p_buff, preset_index);
    ARRAY_TO_STREAM(p_buff, preset_name, name_len);

    {
        wiced_bt_gatt_write_hdr_t write_hdr = {.handle = p_handles->value_handle,
                                               .offset = 0,
                                               .len = p_buff - p_buff_start,
                                               .auth_req = GATT_AUTH_REQ_NONE};

        return wiced_bt_gatt_client_send_write(conn_id,
                                               GATT_REQ_WRITE,
                                               &write_hdr,
                                               p_buff_start,
                                               (wiced_bt_gatt_app_context_t)wiced_bt_free_buffer);
    }
}

wiced_bt_gatt_status_t ga_lib_has_write_control_point_set_active_preset(uint16_t conn_id,
                                                                     gatt_intf_characteristic_handles_t *p_handles,
                                                                     uint8_t preset_index,
                                                                     uint8_t is_synchronized_locally)
{
    uint8_t buff[3];
    uint8_t *p_buff = buff;

    if (is_synchronized_locally)
    {
        UINT8_TO_STREAM(p_buff, GA_LIB_HAS_OPCODE_SET_ACTIVE_PRESET_SYNCHRONIZED_LOCALLY);
    }
    else
    {
        UINT8_TO_STREAM(p_buff, GA_LIB_HAS_OPCODE_SET_ACTIVE_PRESET);
    }
    UINT8_TO_STREAM(p_buff, preset_index);

    return gatt_intf_write_with_rsp(conn_id, p_handles, buff, p_buff - buff);
}

static wiced_bt_gatt_status_t ga_lib_has_write_control_point_opcode(uint16_t conn_id,
                                                                 gatt_intf_characteristic_handles_t *p_handles,
                                                                 uint8_t opcode)
{
    uint8_t buff[3];
    uint8_t *p_buff = buff;

    UINT8_TO_STREAM(p_buff, opcode);

    return gatt_intf_write_with_rsp(conn_id, p_handles, buff, p_buff - buff);
}

wiced_bt_gatt_status_t ga_lib_has_write_control_point_set_next_preset(uint16_t conn_id,
                                                                   gatt_intf_characteristic_handles_t *p_handles,
                                                                   uint8_t is_synchronized_locally)
{
    if (is_synchronized_locally)
    {
        return ga_lib_has_write_control_point_opcode(conn_id,
                                                  p_handles,
                                                  GA_LIB_HAS_OPCODE_SET_NEXT_PRESET_SYNCHRONIZED_LOCALLY);
    }
    else
    {
        return ga_lib_has_write_control_point_opcode(conn_id, p_handles, GA_LIB_HAS_OPCODE_SET_NEXT_PRESET);
    }
}

wiced_bt_gatt_status_t ga_lib_has_write_control_point_set_previous_preset(uint16_t conn_id,
                                                                       gatt_intf_characteristic_handles_t *p_handles,
                                                                       uint8_t is_synchronized_locally)
{
    if (is_synchronized_locally)
    {
        return ga_lib_has_write_control_point_opcode(conn_id,
                                                  p_handles,
                                                  GA_LIB_HAS_OPCODE_SET_PREVIOUS_PRESET_SYNCHRONIZED_LOCALLY);
    }
    else
    {
        return ga_lib_has_write_control_point_opcode(conn_id, p_handles, GA_LIB_HAS_OPCODE_SET_PREVIOUS_PRESET);
    }
}
