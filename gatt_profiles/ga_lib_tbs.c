/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * call Control Profile implementation.
  */
#include "ga_lib_tbs.h"

#ifdef TBS_DEBUG
#define TBS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get TBS library traces */
#else
#define TBS_TRACE(...)
#endif

const gatt_intf_characteristic_info_t tbs_characteristic_info[] =
{

    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_BEARER_PROVIDER_NAME},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_BEARER_PROVIDER_NAME,
     .name = "provider"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_BEARER_UCI},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_BEARER_UCI,
     .name = "uci"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_BEARER_TECHNOLOGY},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_BEARER_TECHNOLOGY,
     .name = "tech"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_BEARER_URI_SCHEMES},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_BEARER_URI_SUPPORTED_SCHEMES,
     .name = "uri"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_BEARER_SIGNAL_STRENGTH},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_BEARER_SIGNAL_STRENGTH,
     .name = "signal_str"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_SIG_STR_REPORTING_INTERVAL},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL,
     .name = "ssri"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_LIST_CURRENT_CALL},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_BEARER_LIST_CURRENT_CALLS,
     .name = "list_call"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_CONTENT_CONTROL_ID},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_CONTENT_CONTROL_ID,
     .name = "ccid"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_INCOMING_TG_URI},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_INCOMING_CALL_TG_BEARER_URI,
     .name = "tg_cid"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_STATUS_FLAGS},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_STATUS_FLAGS,
     .name = "flags"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_CALL_STATE},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_CALL_STATE,
     .name = "state"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_CALL_CONTROL_POINT},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_CALL_CONTROL_POINT,
     .name = "call_cp"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_CALL_CONTROL_POINT_OPTIONAL_OPCODE},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_CALL_CONTROL_POINT_OPTIONAL_OPCODE,
     .name = "cp_opt"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_TERMINATION_REASON},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_CALL_TERMINATION_REASON,
     .name = "term"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_INCOMING_CALL},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_INCOMING_CALL,
     .name = "in_call"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_TBS_CALL_FRIENDLY_NAME},
     .characteristic_type = GA_LIB_TBS_CHARACTERISTIC_CALL_FRIENDLY_NAME,
     .name = "name"},
};

const gatt_intf_service_name_uuid_t tbs_id = {
    .name = "tbs",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_TELEPHONE_BEARER,
};

const gatt_intf_service_name_uuid_t gtbs_id = {
    .name = "gtbs",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_GENERIC_TELEPHONE_BEARER,
};

gatt_intf_service_discovery_ctx_t *ga_lib_tbs_discover_service_int(uint16_t conn_id,
                                                                uint16_t start_handle,
                                                                const gatt_intf_service_name_uuid_t *p_service_id,
                                                                pfn_on_discovery_complete_t pfn_on_complete)
{
    uint8_t num_characteristics = sizeof(tbs_characteristic_info) / sizeof(tbs_characteristic_info[0]);
    gatt_intf_service_discovery_ctx_t *p_sdc = gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, 0);
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);

    gatt_intf_copy_unique_characteristic_info(p_sdc, tbs_characteristic_info, num_characteristics);

    gatt_intf_start_service_discovery(conn_id, p_service_id, start_handle, p_sdc);

    return p_sdc;
}

gatt_intf_service_discovery_ctx_t *ga_lib_gtbs_discover_service(uint16_t conn_id,
                                                             pfn_on_discovery_complete_t pfn_on_complete)
{
    return ga_lib_tbs_discover_service_int(conn_id, 1, &gtbs_id, pfn_on_complete);
}

gatt_intf_service_discovery_ctx_t *ga_lib_tbs_discover_service(uint16_t conn_id,
                                                            uint16_t start_handle,
                                                            pfn_on_discovery_complete_t pfn_on_complete)
{
    return ga_lib_tbs_discover_service_int(conn_id, start_handle, &tbs_id, pfn_on_complete);
}

wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_originate_call(uint16_t conn_id,
                                                              gatt_intf_characteristic_handles_t *p_handle,
                                                              const char *p_uri)
{
    uint8_t buff[256];
    uint8_t *p_data = buff;
    int uri_len = strlen(p_uri);

    if (uri_len > 255)
        return WICED_BT_GATT_INVALID_ATTR_LEN;

    UINT8_TO_STREAM(p_data, GA_LIB_CCP_ACTION_ORIGINATE);
    ARRAY_TO_STREAM(p_data, p_uri, uri_len);

    return gatt_intf_write_no_rsp(conn_id, p_handle, buff, (uint16_t)(p_data - buff));
}

wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_join_call(uint16_t conn_id,
                                                         gatt_intf_characteristic_handles_t *p_handle,
                                                         uint8_t num_call_ids,
                                                         const uint8_t *p_call_ids)
{
    uint8_t buff[256];
    uint8_t *p_data = buff;

    if (num_call_ids > 255)
        return WICED_BT_GATT_INVALID_ATTR_LEN;

    UINT8_TO_STREAM(p_data, GA_LIB_CCP_ACTION_JOIN_CALL);
    ARRAY_TO_STREAM(p_data, p_call_ids, num_call_ids);
    return gatt_intf_write_no_rsp(conn_id, p_handle, buff, (uint16_t)(p_data - buff));
}

static wiced_bt_gatt_status_t ga_lib_tbs_write_cp_opcode_action(uint16_t conn_id,
                                                             gatt_intf_characteristic_handles_t *p_handle,
                                                             uint8_t opcode,
                                                             uint8_t call_id)
{
    uint8_t buff[2];
    uint8_t *p_data = buff;

    UINT8_TO_STREAM(p_data, opcode);
    UINT8_TO_STREAM(p_data, call_id);

    return gatt_intf_write_no_rsp(conn_id, p_handle, buff, (uint16_t)(p_data - buff));
}

wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_accept_call(uint16_t conn_id,
                                                           gatt_intf_characteristic_handles_t *p_handle,
                                                           uint8_t call_id)
{
    return ga_lib_tbs_write_cp_opcode_action(conn_id, p_handle, GA_LIB_CCP_ACTION_ACCEPT_CALL, call_id);
}

wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_terminate_call(uint16_t conn_id,
                                                              gatt_intf_characteristic_handles_t *p_handle,
                                                              uint8_t call_id)
{
    return ga_lib_tbs_write_cp_opcode_action(conn_id, p_handle, GA_LIB_CCP_ACTION_TERMINATE_CALL, call_id);
}

wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_hold_call(uint16_t conn_id,
                                                         gatt_intf_characteristic_handles_t *p_handle,
                                                         uint8_t call_id)
{
    return ga_lib_tbs_write_cp_opcode_action(conn_id, p_handle, GA_LIB_CCP_ACTION_HOLD_CALL, call_id);
}

wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_retrieve_call(uint16_t conn_id,
                                                             gatt_intf_characteristic_handles_t *p_handle,
                                                             uint8_t call_id)
{
    return ga_lib_tbs_write_cp_opcode_action(conn_id, p_handle, GA_LIB_CCP_ACTION_RETRIEVE_CALL, call_id);
}

wiced_bt_gatt_status_t ga_lib_tbs_write_bearer_ssri(uint16_t conn_id,
                                                 gatt_intf_characteristic_handles_t *p_handle,
                                                 uint8_t ssri)
{
    uint8_t *p_write_data;
    uint8_t *p_data;
    int len = 1;

    p_data = p_write_data = gatt_intf_method_get_buffer(len);

    UINT8_TO_STREAM(p_data, ssri);

    wiced_bt_gatt_write_hdr_t write_hdr = {.handle = p_handle->value_handle,
                                           .offset = 0,
                                           .len = (uint16_t)(p_data - p_write_data)};

    return wiced_bt_gatt_client_send_write(conn_id,
                                           GATT_CMD_WRITE,
                                           &write_hdr,
                                           p_write_data,
                                           (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_provider_name(uint16_t conn_id, uint16_t handle, const char *provider_name)
{
    return gatt_intf_send_notification(conn_id, handle, (uint8_t *)provider_name, strlen(provider_name));
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_uri_supported_schemes(uint16_t conn_id, uint16_t handle, const char *schemes)
{
    return gatt_intf_send_notification(conn_id, handle, (uint8_t *)schemes, strlen(schemes));
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_current_calls(uint16_t conn_id,
                                                    uint16_t handle,
                                                    const ga_lib_tbs_current_call_t *p_call,
                                                    int num_calls)
{
    uint8_t *p_data_start, *p_data;
    int required_len = num_calls * 4;

    for (int i = 0; i < num_calls; i++)
    {
        required_len += p_call[i].remote_caller_id_len;
    }

    p_data_start = p_data = gatt_intf_method_get_buffer(required_len);
    if (!p_data_start && num_calls)
    {
        return WICED_BT_GATT_NO_RESOURCES;
    }
    for (int i = 0; i < num_calls; i++)
    {
        UINT8_TO_STREAM(p_data, 3 + p_call[i].remote_caller_id_len);
        UINT8_TO_STREAM(p_data, p_call[i].call_id);
        UINT8_TO_STREAM(p_data, p_call[i].call_state);
        UINT8_TO_STREAM(p_data, p_call[i].call_flags);
        ARRAY_TO_STREAM(p_data, p_call[i].p_remote_caller_id, p_call[i].remote_caller_id_len);
    }

    return wiced_bt_gatt_server_send_notification(conn_id,
                                                  handle,
                                                  p_data - p_data_start,
                                                  p_data_start,
                                                  (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_call_state(uint16_t conn_id,
                                                 uint16_t handle,
                                                 ga_lib_tbs_current_call_t *p_call,
                                                 int num_calls)
{
    uint8_t *p_data_start, *p_data;
    int required_len = num_calls * 3;

    for (int i = 0; i < num_calls; i++)
    {
        required_len += p_call[i].remote_caller_id_len;
    }

    p_data_start = p_data = gatt_intf_method_get_buffer(required_len);

    if (!p_data_start && num_calls)
    {
        return WICED_BT_GATT_NO_RESOURCES;
    }

    for (int i = 0; i < num_calls; i++)
    {
        UINT8_TO_STREAM(p_data, p_call[i].call_id);
        UINT8_TO_STREAM(p_data, p_call[i].call_state);
        UINT8_TO_STREAM(p_data, p_call[i].call_flags);
    }

    return wiced_bt_gatt_server_send_notification(conn_id,
                                                  handle,
                                                  p_data - p_data_start,
                                                  p_data_start,
                                                  (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_technology(uint16_t conn_id, uint16_t handle, uint8_t technology)
{
    uint8_t data = technology;
    return gatt_intf_send_notification(conn_id, handle, &data, sizeof(data));
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_signal_strength(uint16_t conn_id, uint16_t handle, uint8_t signal_strength)
{
    uint8_t data = signal_strength;
    return gatt_intf_send_notification(conn_id, handle, &data, sizeof(data));
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_ssri(uint16_t conn_id, uint16_t handle, uint8_t interval)
{
    uint8_t data = interval;
    return gatt_intf_send_notification(conn_id, handle, &data, sizeof(data));
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_content_control_id(uint16_t conn_id, uint16_t handle, uint8_t content_control_id)
{
    uint8_t data = content_control_id;
    return gatt_intf_send_notification(conn_id, handle, &data, sizeof(data));
}

static wiced_bt_gatt_status_t ga_lib_tbs_notify_call_id_str(uint16_t conn_id,
                                                         uint16_t handle,
                                                         uint8_t call_id,
                                                         const char *p_uri)
{
    uint8_t data[256];
    uint8_t *p_data = data;
    int uri_len = strlen(p_uri);

    UINT8_TO_STREAM(p_data, call_id);
    if (uri_len > sizeof(data) - 1)
    {
        uri_len = sizeof(data) - 1;
    }

    ARRAY_TO_STREAM(p_data, p_uri, uri_len);
    return gatt_intf_send_notification(conn_id, handle, data, p_data - data);
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_incoming_bearer(uint16_t conn_id, uint16_t handle, uint8_t call_id, const char *p_uri)
{
    return ga_lib_tbs_notify_call_id_str(conn_id, handle, call_id, p_uri);
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_status_flag(uint16_t conn_id, uint16_t handle, uint16_t status_flag)
{
    uint8_t data[2];
    uint8_t *p_data = data;

    UINT16_TO_STREAM(p_data, status_flag);

    return gatt_intf_send_notification(conn_id, handle, data, p_data - (uint8_t *)data);
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_call_termination_reason(uint16_t conn_id,
                                                              uint16_t handle,
                                                              uint8_t call_id,
                                                              uint8_t reason)
{
    uint8_t data[2];
    uint8_t *p_data = data;

    UINT8_TO_STREAM(p_data, call_id);
    UINT8_TO_STREAM(p_data, reason);

    return gatt_intf_send_notification(conn_id, handle, data, p_data - (uint8_t *)data);
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_control_point_response(
    uint16_t conn_id, uint16_t handle, uint8_t opcode, uint8_t call_id, uint8_t response)
{
    uint8_t data[3];
    uint8_t *p_data = data;

    UINT8_TO_STREAM(p_data, opcode);
    UINT8_TO_STREAM(p_data, call_id);
    UINT8_TO_STREAM(p_data, response);

    return gatt_intf_send_notification(conn_id, handle, data, p_data - (uint8_t *)data);
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_incoming_call(uint16_t conn_id, uint16_t handle, uint8_t call_id, const char *p_uri)
{
    return ga_lib_tbs_notify_call_id_str(conn_id, handle, call_id, p_uri);
}

wiced_bt_gatt_status_t ga_lib_tbs_notify_call_friendly_name(uint16_t conn_id,
                                                         uint16_t handle,
                                                         uint8_t call_id,
                                                         const char *p_friendly_name)
{
    return ga_lib_tbs_notify_call_id_str(conn_id, handle, call_id, p_friendly_name);
}
