/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
 *
 * Gatt Interface implementation
 */

#include "gatt_interface.h"

#define GATT_INTF_TRACE BTU_trace_debug

struct gatt_intf_operation_s
{
    uint16_t conn_id;
    wiced_bt_gatt_status_t (*pfn_operation)(gatt_intf_operation_t *p_op);
    gatt_intf_pfn_on_operation_complete_t pfn_on_operation_complete;
    void *pv_ctx;
    uint16_t max_num_characteristics;
    uint16_t current_index;
    uint8_t iterating_loop; /**< Indicates if the operation is currently iterating in a loop */
    uint8_t ops_in_progress;
};


pfn_get_cccd_value_t pfn_gatt_intf_get_cccd_value = NULL;

wiced_bt_gatt_status_t gatt_intf_internal_send_next_service_discovery_cmd(uint16_t conn_id,
                                                                          gatt_intf_service_discovery_ctx_t *p_sdc)
{
    wiced_bt_gatt_discovery_param_t param;
    wiced_bt_gatt_discovery_type_t discovery_type = GATT_DISCOVER_SERVICES_ALL;
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;
    service_discovery_state_t state_in = p_sdc->service_discovery_state;

    param.s_handle = p_sdc->result.service_range.start_handle;
    param.e_handle = p_sdc->result.service_range.end_handle;
    param.uuid.len = 0;

    GATT_INTF_TRACE("[%s] conn_id %d state %d sdc 0x%p %d - %d",
                   __FUNCTION__,
                   conn_id,
                   state_in,
                   p_sdc,
                   param.s_handle,
                   param.e_handle);

    switch (state_in)
    {
    case SERVICE_DISCOVERY_STATE_DISCOVER_SERVICE:
    {
        if (p_sdc->result.p_inc_service_types)
        {
            p_sdc->service_discovery_state = SERVICE_DISCOVERY_STATE_DISCOVER_INCLUDE_SERVICES;
            discovery_type = GATT_DISCOVER_INCLUDED_SERVICES;
        }
        else
        {
            p_sdc->service_discovery_state = SERVICE_DISCOVERY_STATE_DISCOVER_CHARACTERISTICS;
            discovery_type = GATT_DISCOVER_CHARACTERISTICS;
        }
    }
    break;
    case SERVICE_DISCOVERY_STATE_DISCOVER_INCLUDE_SERVICES:
    {
        p_sdc->service_discovery_state = SERVICE_DISCOVERY_STATE_DISCOVER_CHARACTERISTICS;
        discovery_type = GATT_DISCOVER_CHARACTERISTICS;
    }
    break;
    case SERVICE_DISCOVERY_STATE_DISCOVER_CHARACTERISTICS:
    {
        p_sdc->service_discovery_state = SERVICE_DISCOVERY_STATE_DISCOVER_CHARACTERISTIC_DESCRIPTORS;
        discovery_type = GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS;
    }
    break;
    case SERVICE_DISCOVERY_STATE_DISCOVER_CHARACTERISTIC_DESCRIPTORS:
    {
        p_sdc->service_discovery_state = SERVICE_DISCOVERY_STATE_DISCOVER_COMPLETE;
    }
    break;
    default:
        break;
    }

    if (p_sdc->service_discovery_state != SERVICE_DISCOVERY_STATE_DISCOVER_COMPLETE)
    {
        status = wiced_bt_gatt_client_send_discover(conn_id, discovery_type, &param);
    }

    GATT_INTF_TRACE("[%s] status %d state in %d out %d disc_type %d uuid %04x",
                    __FUNCTION__,
                    status,
                    state_in,
                    p_sdc->service_discovery_state,
                    discovery_type,
                    param.uuid.uu.uuid16);

    if (status != WICED_BT_GATT_SUCCESS)
    {
        p_sdc->service_discovery_state = SERVICE_DISCOVERY_STATE_DISCOVER_COMPLETE;
    }

    return status;
}

void gatt_interface_discovery_timout_cback(WICED_TIMER_PARAM_TYPE param)
{
    uint16_t conn_id = *((uint16_t *)param);
    GATT_INTF_TRACE("[%s] conn_id %x", __FUNCTION__, conn_id);
    if (wiced_bt_gatt_validate_conn_id(conn_id) == WICED_BT_GATT_SUCCESS)
    {
        wiced_bt_gatt_disconnect(conn_id);
    }
    else
        GATT_INTF_TRACE("[%s] already disconnected", __FUNCTION__);
}

void gatt_intf_handle_discovery_complete(uint16_t conn_id,
                                         gatt_intf_service_discovery_ctx_t *p_sdc,
                                         wiced_bt_gatt_status_t status)
{
    gatt_intf_discovery_result_t *p_result = &p_sdc->result;
    gatt_intf_characteristic_handles_result_t *p_char_handles = p_result->p_char_handles;

    for (int i = 0; i < p_sdc->result.max_characteristic_handles; i++, p_char_handles++)
    {
        const gatt_intf_characteristic_info_t *p_char_info = p_char_handles->p_char_info;
        gatt_intf_characteristic_handles_t *p_handle = &p_char_handles->handles;

        GATT_INTF_TRACE("[%s, %s] ----char #uuid 0x%x %d %d %d 0x(%x %x %x)",
                       p_result->p_id->name,
                       p_char_info->name,
                       p_char_info->uuid.uu.uuid16,
                       p_handle->handle,
                       p_handle->value_handle,
                       p_handle->descriptor_handle,
                       p_handle->handle,
                       p_handle->value_handle,
                       p_handle->descriptor_handle);
    }

    if (p_sdc->pfn_on_discovery_complete_app_cb)
    {
        p_sdc->pfn_on_discovery_complete_app_cb(p_sdc, conn_id, status, &p_sdc->result);
    }
}

void gatt_intf_on_service_discovery_complete(gatt_intf_service_discovery_ctx_t *p_sdc,
                                             wiced_bt_gatt_discovery_complete_t *p_data)
{
    GATT_INTF_TRACE("[%s] conn_id %d state %d status %d discovery_type %d",
                    __FUNCTION__,
                    p_data->conn_id,
                    p_sdc->service_discovery_state,
                    p_data->status,
                    p_data->discovery_type);

    if (p_data->status == WICED_BT_GATT_SUCCESS || p_data->status == WICED_BT_GATT_ATTRIBUTE_NOT_FOUND)
    {
        gatt_intf_internal_send_next_service_discovery_cmd(p_data->conn_id, p_sdc);
        if (p_sdc->service_discovery_state == SERVICE_DISCOVERY_STATE_DISCOVER_COMPLETE)
        {
            gatt_intf_handle_discovery_complete(p_data->conn_id, p_sdc, WICED_BT_GATT_SUCCESS);
        }
    }
    else
    {
        //if (p_sdc->pfn_on_discovery_complete)
        {
            //p_sdc->pfn_on_discovery_complete(p_data->conn_id, p_sdc, p_data->status);
            gatt_intf_handle_discovery_complete(p_data->conn_id, p_sdc, p_data->status);
        }
        wiced_deinit_timer(&p_sdc->discovery_cmpl_timer);
        wiced_init_timer(&p_sdc->discovery_cmpl_timer,
                         gatt_interface_discovery_timout_cback,
                         (WICED_TIMER_PARAM_TYPE)&p_data->conn_id,
                         WICED_MILLI_SECONDS_TIMER);
        wiced_start_timer(&p_sdc->discovery_cmpl_timer, 2);
    }
}

void gatt_intf_handle_included_service_discovery(gatt_intf_service_discovery_ctx_t *p_sdc,
                                                 const wiced_bt_gatt_included_service_t *p_inc_service)
{
    gatt_intf_included_service_types_t *p_inc_type = p_sdc->result.p_inc_service_types;
    GATT_INTF_TRACE("[%s] uuid %04x hdl 0x%x s_hdl 0x%x e_hdl 0x%x",
                   __FUNCTION__,
                   p_inc_service->service_type.uu.uuid16,
                   p_inc_service->handle,
                   p_inc_service->s_handle,
                   p_inc_service->e_handle);

    int found = 0;

    for (int i = 0; i < p_sdc->result.max_included_services; i++, p_inc_type++)
    {
        if (p_inc_type->p_inc_info == NULL)
        {
            GATT_INTF_TRACE("[%s] No info for included service index %d", __FUNCTION__, i);
            break;
        }
        if (p_inc_type->inc_handles.service_handle != 0)
        {
            GATT_INTF_TRACE("[%s] Duplicate included service type %04x @ %d", __FUNCTION__, p_inc_type->p_inc_info->uuid.uu.uuid16, i);
            continue;
        }

        if (wiced_bt_is_same_uuid(&p_inc_type->p_inc_info->uuid, &p_inc_service->service_type))
        {
            gatt_intf_included_service_handles_t *p_inc_handles = &p_inc_type->inc_handles;

            p_inc_handles->service_handle = p_inc_service->handle;
            p_inc_handles->service.start_handle = p_inc_service->s_handle;
            p_inc_handles->service.end_handle = p_inc_service->e_handle;

            found = 1;
            break;
        }
    }

    GATT_INTF_TRACE("[%s] %s uuid %04x hdl 0x%x 0x%x - 0x%x",
                   __FUNCTION__,
                   found ? p_inc_type->p_inc_info->name : "unknown",
                   p_inc_service->service_type.uu.uuid16,
                   p_inc_service->handle,
                   p_inc_service->s_handle,
                   p_inc_service->e_handle);

    return;
}

void gatt_intf_handle_characteristic_discovery(gatt_intf_service_discovery_ctx_t *p_sdc,
                                               const wiced_bt_gatt_char_declaration_t *p_char_decl)
{
    gatt_intf_characteristic_handles_result_t *p_char = p_sdc->result.p_char_handles;
    int found = 0;

    for (int i = 0; i < p_sdc->result.max_characteristic_handles; i++, p_char++)
    {
        if (p_char->handles.handle != 0)
        {
            continue;
        }

        if (wiced_bt_is_same_uuid(&p_char->p_char_info->uuid, &p_char_decl->char_uuid))
        {
            p_char->handles.handle = p_char_decl->handle;
            p_char->handles.value_handle = p_char_decl->val_handle;
            found = 1;
            break;
        }
    }

    GATT_INTF_TRACE("[%s] %s uuid %04x hdl 0x%x 0x%x",
                   __FUNCTION__,
                   found ? p_char->p_char_info->name : "unknown",
                   p_char_decl->char_uuid.uu.uuid16,
                   p_char_decl->handle,
                   p_char_decl->val_handle);
}

void gatt_intf_handle_descriptor_discovery(gatt_intf_service_discovery_ctx_t *p_sdc,
                                           uint16_t char_handle,
                                           const wiced_bt_gatt_char_descr_info_t *p_desc_decl)
{

    GATT_INTF_TRACE("[%s] uuid %04x hdl 0x%x 0x%x",
                   __FUNCTION__,
                   p_desc_decl->type.uu.uuid16,
                   char_handle,
                   p_desc_decl->handle);
    if ((p_desc_decl->type.len != 2) || (p_desc_decl->type.uu.uuid16 != GATT_UUID_CHAR_CLIENT_CONFIG))
    {
        return;
    }

    gatt_intf_characteristic_handles_result_t *p_char_handles = p_sdc->result.p_char_handles;

    for (int i = 0; i < p_sdc->result.max_characteristic_handles; i++, p_char_handles++)
    {
        if (p_char_handles->handles.handle == char_handle)
        {
            p_char_handles->handles.descriptor_handle = p_desc_decl->handle;

            GATT_INTF_TRACE("[%s] %s uuid %04x hdl 0x%x 0x%x",
                           __FUNCTION__,
                           p_char_handles->p_char_info->name,
                           p_desc_decl->type.uu.uuid16,
                           char_handle,
                           p_desc_decl->handle);

            break;
        }
    }
}

void gatt_intf_on_service_discovery_result(gatt_intf_service_discovery_ctx_t *p_sdc,
                                           wiced_bt_gatt_discovery_result_t *p_result)
{
    wiced_bt_gatt_discovery_data_t *p_data = &p_result->discovery_data;

    switch (p_result->discovery_type)
    {
    case GATT_DISCOVER_SERVICES_BY_UUID: /**< discover service by UUID */
    {
        GATT_INTF_TRACE("Service s:%04x e:%04x uuid:%04x\n",
                       p_data->group_value.s_handle,
                       p_data->group_value.e_handle,
                       p_data->group_value.service_type.uu.uuid16);

        /* Save the service start and end handles in the discovery context */
        p_sdc->result.service_range.start_handle = p_result->discovery_data.group_value.s_handle;
        p_sdc->result.service_range.end_handle = p_result->discovery_data.group_value.e_handle;
    }
    break;
    case GATT_DISCOVER_INCLUDED_SERVICES: /**< discover an included service within a service */
    {
        gatt_intf_handle_included_service_discovery(p_sdc, &p_result->discovery_data.included_service_type);
    }
    break;
    case GATT_DISCOVER_CHARACTERISTICS: /**< discover characteristics of a service with/without type requirement */
    {
        //p_sdc->pfn_on_characteristic(p_sdc, &p_result->discovery_data.characteristic_declaration);
        gatt_intf_handle_characteristic_discovery(p_sdc, &p_result->discovery_data.characteristic_declaration);
    }
    break;
    case GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS: /**< discover characteristic descriptors of a character */
    {
        const wiced_bt_gatt_char_descr_info_t *p_desc = &p_result->discovery_data.char_descr_info;

        if ((p_desc->type.len == 2) && p_desc->type.uu.uuid16 == GATT_UUID_CHAR_DECLARE)
        {
            p_sdc->last_found_char_handle = p_desc->handle;
        }
        else
        {
            if (IS_CHAR_CLIENT_CONFIG_UUID(p_desc->type) && p_desc->handle != 0)
            {
                //p_sdc->pfn_on_descriptor(p_sdc, p_sdc->last_found_char_handle, p_desc);
                gatt_intf_handle_descriptor_discovery(p_sdc, p_sdc->last_found_char_handle, p_desc);
            }
        }
    }
    break;
    }
}

gatt_intf_service_discovery_ctx_t *gatt_intf_alloc_service_discovery_ctx(uint8_t max_char_handles,
                                                                         uint8_t max_inc_handles,
                                                                         uint16_t app_ctx_size)
{
    gatt_intf_service_discovery_ctx_t *p_sdc;
    int ctx_size = sizeof(gatt_intf_service_discovery_ctx_t);

    ctx_size += max_char_handles * sizeof(gatt_intf_characteristic_handles_result_t);
    ctx_size += max_inc_handles * sizeof(gatt_intf_included_service_types_t);
    ctx_size += app_ctx_size;

    GATT_INTF_TRACE("ctx %d char_hdl %d inc_hdl %d app %d",
                   sizeof(gatt_intf_service_discovery_ctx_t),
                   sizeof(gatt_intf_characteristic_handles_result_t),
                   sizeof(gatt_intf_included_service_types_t),
                   app_ctx_size);

    p_sdc = (gatt_intf_service_discovery_ctx_t *)wiced_bt_get_buffer(ctx_size);
    if (!p_sdc)
    {
        return NULL;
    }
    WICED_MEMSET(p_sdc, 0, ctx_size);
    p_sdc->result.max_characteristic_handles = max_char_handles;
    p_sdc->result.max_included_services = max_inc_handles;

    p_sdc->result.p_char_handles = (gatt_intf_characteristic_handles_result_t *)(p_sdc + 1);
    if (max_inc_handles)
    {
        p_sdc->result.p_inc_service_types =
            (gatt_intf_included_service_types_t *)(p_sdc->result.p_char_handles + max_char_handles);
    }

    if (app_ctx_size)
    {
        p_sdc->p_app_ctx = (uint8_t *)(p_sdc->result.p_inc_service_types + max_inc_handles);
    }

    GATT_INTF_TRACE("[%s] ctx %x %d char_info %x inc_info %x app_ctx %x",
                   __FUNCTION__,
                   p_sdc,
                   ctx_size,
                   p_sdc->result.p_char_handles,
                   p_sdc->result.p_inc_service_types,
                   p_sdc->p_app_ctx);
    return p_sdc;
}

void *gatt_intf_discovery_get_app_context(gatt_intf_service_discovery_ctx_t *p_sdc)
{
    return p_sdc->p_app_ctx;
}

wiced_result_t gatt_intf_start_service_discovery(uint16_t conn_id,
                                                 const gatt_intf_service_name_uuid_t *p_id,
                                                 uint16_t start_handle,
                                                 gatt_intf_service_discovery_ctx_t *p_sdc)
{
    if (!p_id)
    {
        return WICED_BT_ERROR;
    }
    if (p_sdc->service_discovery_state != SERVICE_DISCOVERY_STATE_IDLE)
    {
        return WICED_ADDRESS_IN_USE;
    }

    p_sdc->result.p_id = p_id;
    p_sdc->service_discovery_state = SERVICE_DISCOVERY_STATE_DISCOVER_SERVICE;
    p_sdc->last_found_char_handle = 0;

    wiced_bt_gatt_discovery_param_t param;
    WICED_MEMCPY(&param.uuid, &p_sdc->result.p_id->uuid, sizeof(wiced_bt_uuid_t));
    param.s_handle = start_handle;
    param.e_handle = 0xffff;
    return wiced_bt_gatt_client_send_discover(conn_id, GATT_DISCOVER_SERVICES_BY_UUID, &param);
}

wiced_result_t gatt_intf_start_secondary_service_discovery(uint16_t conn_id,
                                                           const gatt_intf_service_name_uuid_t *p_id,
                                                           uint16_t start_handle,
                                                           uint16_t end_handle,
                                                           gatt_intf_service_discovery_ctx_t *p_sdc)
{
    if (!p_id)
    {
        return WICED_BT_ERROR;
    }
    if (p_sdc->service_discovery_state != SERVICE_DISCOVERY_STATE_IDLE)
    {
        return WICED_BT_ERROR;
    }
    p_sdc->result.p_id = p_id;
    p_sdc->last_found_char_handle = 0;
    p_sdc->service_discovery_state = SERVICE_DISCOVERY_STATE_DISCOVER_INCLUDE_SERVICES;
    p_sdc->result.service_range.start_handle = start_handle;
    p_sdc->result.service_range.end_handle = end_handle;

    return gatt_intf_internal_send_next_service_discovery_cmd(conn_id, p_sdc);
}

wiced_result_t gatt_intf_read_characteristic(uint16_t conn_id, gatt_intf_characteristic_handles_t *p_char)
{
    wiced_result_t status = WICED_BT_NO_RESOURCES;
    uint16_t len = wiced_bt_gatt_get_bearer_mtu(conn_id) - WICED_GATT_HDR_SIZE;
    uint8_t *p_buf = wiced_bt_get_buffer(len);

    if (p_buf)
    {
        status =
            wiced_bt_gatt_client_send_read_handle(conn_id, p_char->value_handle, 0, p_buf, len, GATT_AUTH_REQ_NONE);
    }
    GATT_INTF_TRACE("[%s] conn 0x%x hdl %d buf 0x%x status 0x%x",
                   __FUNCTION__,
                   conn_id,
                   p_char->value_handle,
                   p_buf,
                   status);
    return status;
}

void gatt_intf_set_cccd_value_callback(pfn_get_cccd_value_t pfn_get_cccd_value)
{
    pfn_gatt_intf_get_cccd_value = pfn_get_cccd_value;
}

wiced_bt_gatt_status_t gatt_intf_send_notification(uint16_t conn_id, uint16_t handle, uint8_t *p_data, uint16_t len)
{
    wiced_bt_gatt_status_t status;
    uint16_t mtu = wiced_bt_gatt_get_bearer_mtu(conn_id) - WICED_GATT_HDR_SIZE;
    if (len > mtu)
    {
        GATT_INTF_TRACE("[%s] data length %d exceeds mtu %d", __FUNCTION__, len, mtu);
        return WICED_BT_GATT_INVALID_ATTR_LEN;
    }

    if (pfn_gatt_intf_get_cccd_value == NULL)
    {
        GATT_INTF_TRACE("[%s] CCCD callback not set", __FUNCTION__);
        return WICED_BT_GATT_CCCD_IMPROPER_CONFIGURED;
    }
    uint16_t cccd_value = pfn_gatt_intf_get_cccd_value(conn_id, handle);
    if (0 == (cccd_value & (GATT_CLIENT_CONFIG_NOTIFICATION | GATT_CLIENT_CONFIG_INDICATION)))
    {
        GATT_INTF_TRACE("[%s] Notification not enabled for handle 0x%04x", __FUNCTION__, handle);
        return WICED_BT_GATT_SUCCESS;
    }
    {
        wiced_bt_gatt_app_context_t ctx = (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer;
        uint8_t *p_rsp_data = gatt_intf_method_get_buffer(len);

        if ((len != 0) && !p_rsp_data)
        {
            return WICED_BT_GATT_NO_RESOURCES;
        }
        WICED_MEMCPY(p_rsp_data, p_data, len);
        if (cccd_value & GATT_CLIENT_CONFIG_INDICATION)
        {
            status = wiced_bt_gatt_server_send_indication(conn_id, handle, len, p_rsp_data, ctx);
        }
        else
        {
            status = wiced_bt_gatt_server_send_notification(conn_id, handle, len, p_rsp_data, ctx);
        }

        {
            GATT_INTF_TRACE("[%s] conn 0x%x sent %s hdl %d len %d status 0x%x",
                            __FUNCTION__,
                            conn_id,
                            (cccd_value & GATT_CLIENT_CONFIG_INDICATION) ? "indication" : "notification",
                            handle,
                            len,
                            status);
        }
    }
    return status;
}

uint8_t gatt_intf_copy_unique_characteristic_info(gatt_intf_service_discovery_ctx_t *p_sdc,
                                                  const gatt_intf_characteristic_info_t *p_char_info_list,
                                                  uint8_t num_char_info)
{
    gatt_intf_characteristic_handles_result_t *p_char = p_sdc->result.p_char_handles;

    if (num_char_info > p_sdc->result.max_characteristic_handles)
    {
        return 0;
    }
    for (; num_char_info--;p_char++, p_char_info_list++)
    {
        p_char->p_char_info = p_char_info_list;
    }
    return p_char - p_sdc->result.p_char_handles;
}

uint8_t gatt_intf_repeat_characteristic_info(gatt_intf_service_discovery_ctx_t *p_sdc,
                                             uint8_t start_index,
                                             const gatt_intf_characteristic_info_t *p_char_info,
                                             uint8_t num_repeat)
{
    gatt_intf_characteristic_handles_result_t *p_char = p_sdc->result.p_char_handles + start_index;

    if ((start_index + num_repeat) > p_sdc->result.max_characteristic_handles)
    {
        return 0;
    }

    for (; num_repeat--; p_char++)
    {
        p_char->p_char_info = p_char_info;
    }
    return p_char - p_sdc->result.p_char_handles;
}

uint8_t gatt_intf_repeat_included_service_info(gatt_intf_service_discovery_ctx_t *p_sdc,
                                               uint8_t start_index,
                                               const gatt_intf_included_service_info_t *p_inc_info,
                                               uint8_t num_repeat)
{
    gatt_intf_included_service_types_t *p_inc = p_sdc->result.p_inc_service_types + start_index;

    if ((start_index + num_repeat) > p_sdc->result.max_included_services)
    {
        return 0;
    }

    for (; num_repeat--; p_inc++)
    {
        p_inc->p_inc_info = p_inc_info;
    }

    return p_inc - p_sdc->result.p_inc_service_types;
}

void gatt_intf_set_discovery_complete_callback(gatt_intf_service_discovery_ctx_t *p_sdc,
                                               pfn_on_discovery_complete_t pfn_on_complete)
{
    p_sdc->pfn_on_discovery_complete_app_cb = pfn_on_complete;
    return;
}

uint8_t *gatt_intf_method_get_buffer(int len)
{
    return wiced_bt_get_buffer(len);
}

void gatt_intf_method_free_buffer(uint8_t *ptr)
{
    wiced_bt_free_buffer(ptr);
}

int gatt_intf_get_valid_handle_count(gatt_intf_characteristic_handles_t *p_handles, int max)
{
    int count = 0;
    for (int i = 0; i < max; i++)
    {
        if (p_handles[i].handle)
        {
            count++;
        }
        else
        {
            break;
        }
    }
    return count;
}

int gatt_intf_get_valid_included_handle_count(gatt_intf_included_service_handles_t *p_inc, int max)
{
    int count = 0;
    for (int i = 0; i < max; i++)
    {
        if (p_inc[i].service_handle)
        {
            count++;
        }
        else
        {
            break;
        }
    }
    return count;
}

int gatt_intf_is_service_discovery_complete(gatt_intf_service_discovery_ctx_t *p_sdc)
{
    if (p_sdc->service_discovery_state == SERVICE_DISCOVERY_STATE_DISCOVER_COMPLETE)
    {
        return 1;
    }
    return 0;
}

void gatt_intf_free_service_discovery_ctx(gatt_intf_service_discovery_ctx_t *p_sdc)
{
    GATT_INTF_TRACE("[%s] 0x%x", __FUNCTION__, p_sdc);
    wiced_bt_free_buffer(p_sdc);

    return;
}

static wiced_result_t gatt_intf_write_int(uint16_t conn_id,
                                          gatt_intf_characteristic_handles_t *p_handle,
                                          uint8_t *p_data_in,
                                          uint16_t len,
                                          wiced_bt_gatt_opcode_t opcode)
{
    wiced_result_t status = WICED_BT_NO_RESOURCES;
    wiced_bt_gatt_write_hdr_t write_hdr = {.handle = p_handle->value_handle,
                                           .offset = 0,
                                           .len = len,
                                           .auth_req = GATT_AUTH_REQ_NONE};
    wiced_bt_gatt_app_context_t ctx = (wiced_bt_gatt_app_context_t)wiced_bt_free_buffer;

    uint8_t *p_data = wiced_bt_get_buffer(len);
    if (p_data)
    {
        memcpy(p_data, p_data_in, len);

        status = wiced_bt_gatt_client_send_write(conn_id, opcode, &write_hdr, p_data, ctx);
    }

    GATT_INTF_TRACE("[%s] conn 0x%x sent %s hdl %d len %d status 0x%x",
                    __FUNCTION__,
                    conn_id,
                    opcode == GATT_REQ_WRITE ? "write_req" : "write_cmd",
                    p_handle->value_handle,
                    len,
                    status);
    return status;
}


wiced_result_t gatt_intf_write_no_rsp(uint16_t conn_id,
                                      gatt_intf_characteristic_handles_t *p_handle,
                                      uint8_t *p_data_in,
                                      uint16_t len)
{
    return gatt_intf_write_int(conn_id, p_handle, p_data_in, len, GATT_CMD_WRITE);
}

wiced_result_t gatt_intf_write_with_rsp(uint16_t conn_id,
                                        gatt_intf_characteristic_handles_t *p_handle,
                                        uint8_t *p_data_in,
                                        uint16_t len)
{
    return gatt_intf_write_int(conn_id, p_handle, p_data_in, len, GATT_REQ_WRITE);
}


/* Send a write request to enable notifications */
wiced_result_t gatt_intf_enable_notifications(uint16_t conn_id,
                                              gatt_intf_characteristic_handles_t *p_handle,
                                              uint16_t cccd)
{
    uint8_t len = sizeof(cccd);
    wiced_bt_gatt_write_hdr_t write_hdr = {.handle = p_handle->descriptor_handle,
                                           .offset = 0,
                                           .len = len,
                                           .auth_req = GATT_AUTH_REQ_NONE};

    uint8_t *p_data = wiced_bt_get_buffer(sizeof(cccd));
    if (!p_data)
    {
        return WICED_BT_NO_RESOURCES;
    }
    {
        uint8_t *p = p_data;
        UINT16_TO_STREAM(p, cccd);
    }

    GATT_INTF_TRACE("[%s] handle 0x%x cccd 0x%x", __FUNCTION__, write_hdr.handle, cccd);

    return wiced_bt_gatt_client_send_write(conn_id,
                                           GATT_REQ_WRITE,
                                           &write_hdr,
                                           p_data,
                                           (wiced_bt_gatt_app_context_t)wiced_bt_free_buffer);
}


wiced_bt_gatt_status_t app_send_read_response(wiced_bt_gatt_attribute_request_t *p_att_req,
                                              wiced_bt_gatt_status_t status,
                                              uint8_t *p_data,
                                              uint16_t len)
{
    uint8_t *p_rsp_data = wiced_bt_get_buffer(len);
    if (!p_rsp_data)
    {
        return WICED_BT_GATT_NO_RESOURCES;
    }
    memcpy(p_rsp_data, p_data, len);
    wiced_bt_gatt_server_send_read_handle_rsp(p_att_req->conn_id,
                                              p_att_req->opcode,
                                              len,
                                              p_rsp_data,
                                              (wiced_bt_gatt_app_context_t)wiced_bt_free_buffer);
    return WICED_BT_GATT_SUCCESS;
}

uint8_t *gatt_intf_method_alloc_server_notification_packet(uint16_t conn_id,
                                                           uint16_t handle,
                                                           wiced_bt_gatt_client_char_config_t type,
                                                           int len)
{
    uint8_t *p_dest = NULL;

    p_dest = gatt_intf_method_get_buffer(len);
    if (!p_dest)
    {
        goto ERROR;
    }

    return p_dest;

ERROR:
    GATT_INTERFACE_TRACE_CRIT("[%s] handle 0x%x id %d p_dest 0x%x", __FUNCTION__, handle, handle, p_dest);

    return NULL;
}

wiced_bt_gatt_status_t gatt_intf_send_read_response(wiced_bt_gatt_attribute_request_t *p_att_req,
                                                    wiced_bt_gatt_status_t status,
                                                    uint8_t *p_data,
                                                    uint16_t len)
{
    uint8_t *p_rsp_data = NULL;
    if (len)
    {
        p_rsp_data = gatt_intf_method_get_buffer(len);
        if (!p_rsp_data)
        {
            return WICED_BT_GATT_NO_RESOURCES;
        }
        memcpy(p_rsp_data, p_data, len);
    }
    return wiced_bt_gatt_server_send_read_handle_rsp(p_att_req->conn_id,
                                              p_att_req->opcode,
                                              len,
                                              p_rsp_data,
                                              (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);
}

wiced_bt_gatt_status_t gatt_intf_method_send_indication(uint16_t conn_id,
                                                        uint16_t handle,
                                                        uint16_t data_len,
                                                        uint8_t *p_data,
                                                        void *pv_ctx)
{
    wiced_bt_gatt_status_t status =
        wiced_bt_gatt_server_send_indication(conn_id, handle, data_len, p_data, pv_ctx);
    if (status != WICED_BT_GATT_SUCCESS)
    {
        void (*pfn_free)(uint8_t *ptr) = (void (*)(uint8_t *))pv_ctx;
        if (pfn_free)
        {
            pfn_free(p_data);
        }
    }
    return status;
}

wiced_bt_gatt_status_t gatt_intf_method_send_write_response(const gatt_intf_characteristic_handles_t *p_char,
                                                            uint16_t conn_id,
                                                            wiced_bt_gatt_opcode_t opcode,
                                                            wiced_bt_gatt_status_t status)
{
    wiced_bt_gatt_status_t result = WICED_BT_GATT_ERROR;

    if (status == WICED_BT_GATT_SUCCESS)
        result = wiced_bt_gatt_server_send_write_rsp(conn_id, opcode, p_char->value_handle);
    else
        result = wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_char->value_handle, status);

    return result;
}

int gatt_intf_find_characteristic_type_by_value_handle(gatt_intf_characteristic_handles_t *p_handles,
                                                       int num_handles,
                                                       uint16_t handle)
{
    for (int i = 0; i < num_handles; i++)
    {
        if (p_handles->value_handle == handle)
        {
            return i;
        }
        p_handles++;
    }
    return -1;
}


void gatt_intf_print_handles(gatt_intf_discovery_result_t *p_result)
{
    gatt_intf_characteristic_handles_result_t *p_char = p_result->p_char_handles;
    gatt_intf_included_service_types_t *p_inc = p_result->p_inc_service_types;

    GATT_INTF_TRACE("[%s] %s discovery complete service range [%d - %d]",
                   __FUNCTION__,
                   p_result->p_id->name,
                   p_result->service_range.start_handle,
                   p_result->service_range.end_handle);
    for (int i = 0; i < p_result->max_characteristic_handles; i++, p_char++)
    {
        GATT_INTF_TRACE("[%s] %s - %s char %d handle %d value_handle %d descriptor_handle %d",
                       __FUNCTION__,
                       p_result->p_id->name,
                       p_char->p_char_info->name,
                       i,
                       p_char->handles.handle,
                       p_char->handles.value_handle,
                       p_char->handles.descriptor_handle);
    }

    for (int i = 0; i < p_result->max_included_services; i++, p_inc++)
    {
        GATT_INTF_TRACE("[%s] %s inc service %d handle %d included service uuid 0x%04x",
                       __FUNCTION__,
                       p_inc->p_inc_info->name,
                       i,
                       p_inc->inc_handles.service_handle,
                       p_inc->p_inc_info->uuid.uu.uuid16);
    }
}


gatt_intf_operation_t *gatt_intf_alloc_operation_list(uint16_t conn_id,
                                                      uint16_t max_characteristics,
                                                      gatt_intf_pfn_on_operation_complete_t pfn_on_complete,
                                                      void *pv_ctx)
{
    int len = sizeof(gatt_intf_operation_t) + (max_characteristics * sizeof(gatt_intf_characteristic_handles_t *));
    gatt_intf_operation_t *p_en = wiced_bt_get_buffer(len);

    if (p_en)
    {
        WICED_MEMSET(p_en, 0, len);
        p_en->conn_id = conn_id;
        p_en->max_num_characteristics = max_characteristics;
        p_en->pfn_on_operation_complete = pfn_on_complete;
        p_en->pv_ctx = pv_ctx;
    }

    GATT_INTF_TRACE("[%s] 0x%x", __FUNCTION__, p_en);
    return p_en;
}

wiced_result_t gatt_intf_add_characteristic_to_list(gatt_intf_operation_t *p_op, gatt_intf_characteristic_handles_t *p_handle, uint16_t index)
{
    gatt_intf_characteristic_handles_t **pp_handles = (gatt_intf_characteristic_handles_t **)(p_op + 1);
    if (index < p_op->max_num_characteristics)
    {
        pp_handles[index] = p_handle;
    }
     else
    {
        GATT_INTF_TRACE("[%s] List is full, cannot add handle 0x%x", __FUNCTION__, p_handle->handle);
        return WICED_BT_ERROR;
    }
    return WICED_BT_SUCCESS;
}

wiced_bt_gatt_status_t gatt_intf_internal_enable_notification(gatt_intf_operation_t *p_en)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;
    gatt_intf_characteristic_handles_t **pp_handles = (gatt_intf_characteristic_handles_t **)(p_en + 1);

    if (p_en->iterating_loop)
    {
        GATT_INTF_TRACE("[%s] returning ...", __FUNCTION__, p_en->conn_id);
        return WICED_BT_GATT_SUCCESS;
    }

    p_en->iterating_loop = 1;
    for (int i = p_en->current_index; i < p_en->max_num_characteristics; i++)
    {
        gatt_intf_characteristic_handles_t *p_handle = pp_handles[p_en->current_index];

        if (p_handle && p_handle->descriptor_handle)
        {
            /* increment ops in progress, since the callback to decrement ops_in_progress, can be called in
             * the same context, in the operation complete */
            p_en->ops_in_progress++;
            status = gatt_intf_enable_notifications(p_en->conn_id, p_handle, GATT_CLIENT_CONFIG_NOTIFICATION);
            if (status != WICED_BT_GATT_SUCCESS)
            {
                p_en->ops_in_progress--;
                break;
            }
            else if(status == WICED_BT_GATT_SUCCESS)
            {
                /* operation is sent successfully, move to next index and break to wait for callback */
                p_en->current_index++;
                break;
            }
        }
        else
        {
            p_en->current_index++;
        }
    }
    p_en->iterating_loop = 0;

    GATT_INTF_TRACE("[%s] status %d progress %d current_index %d max %d cmpl 0x%x",
                   __FUNCTION__,
                   status,
                   p_en->ops_in_progress,
                   p_en->current_index,
                   p_en->max_num_characteristics,
                   p_en->pfn_on_operation_complete);
    if ((p_en->ops_in_progress == 0) && (p_en->current_index == p_en->max_num_characteristics))
    {
        if (p_en->pfn_on_operation_complete)
        {
            p_en->pfn_on_operation_complete(p_en->conn_id, status, p_en->pv_ctx);
        }
    }
    return status;
}


wiced_bt_gatt_status_t gatt_intf_internal_read_char_list(gatt_intf_operation_t *p_rl)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;
    gatt_intf_characteristic_handles_t **pp_handles = (gatt_intf_characteristic_handles_t **)(p_rl + 1);

    for (int i = p_rl->current_index; i < p_rl->max_num_characteristics; i++)
    {
        gatt_intf_characteristic_handles_t *p_handle = pp_handles[p_rl->current_index];
        if (p_handle && p_handle->value_handle)
        {
            status = gatt_intf_read_characteristic(p_rl->conn_id, p_handle);
            if (status != WICED_BT_GATT_SUCCESS)
            {
                p_rl->ops_in_progress--;
            }
            else
            {
                p_rl->current_index++;
                p_rl->ops_in_progress++;
            }
            break;
        }
        else
        {
            p_rl->current_index++;
        }
    }

    if ((p_rl->ops_in_progress == 0) && (p_rl->current_index == p_rl->max_num_characteristics))
    {
        if (p_rl->pfn_on_operation_complete)
        {
            p_rl->pfn_on_operation_complete(p_rl->conn_id, status, p_rl->pv_ctx);
        }
    }

    return status;
}

gatt_intf_operation_t *gatt_intf_alloc_notification_handle_list(uint16_t conn_id,
                                                         uint16_t max_characteristics,
                                                         gatt_intf_pfn_on_operation_complete_t pfn_on_complete,
                                                         void *pv_ctx)
{
    gatt_intf_operation_t *p_op =
        gatt_intf_alloc_operation_list(conn_id, max_characteristics, pfn_on_complete, pv_ctx);
    if (p_op)
    {
        p_op->pfn_operation = gatt_intf_internal_enable_notification;
    }

    return p_op;
}

gatt_intf_operation_t *gatt_intf_alloc_read_handle_list(uint16_t conn_id,
                                                 uint16_t max_characteristics,
                                                 gatt_intf_pfn_on_operation_complete_t pfn_on_complete,
                                                 void *pv_ctx)
{
    gatt_intf_operation_t *p_op =
        gatt_intf_alloc_operation_list(conn_id, max_characteristics, pfn_on_complete, pv_ctx);
    if (p_op)
    {
        p_op->pfn_operation = gatt_intf_internal_read_char_list;
    }

    return p_op;
}

void gatt_intf_free_operation_handle_list(gatt_intf_operation_t *p_op)
{
    GATT_INTF_TRACE("[%s] 0x%x", __FUNCTION__, p_op);
    wiced_bt_free_buffer(p_op);
}

wiced_bt_gatt_status_t gatt_intf_execute_handle_operations(gatt_intf_operation_t *p_op)
{
    GATT_INTF_TRACE("[%s] conn_id %d current_index %d max %d ops_in_progress %d",
                   __FUNCTION__,
                   p_op->conn_id,
                   p_op->current_index,
                   p_op->max_num_characteristics,
                   p_op->ops_in_progress);
    if (p_op && p_op->pfn_operation)
    {
        return p_op->pfn_operation(p_op);
    }
    return WICED_BT_GATT_ERROR;
}

static wiced_bool_t is_operation_complete(gatt_intf_operation_t *p_op)
{
    if ((p_op->ops_in_progress == 0) && (p_op->current_index == p_op->max_num_characteristics))
    {
        return WICED_TRUE;
    }

    return WICED_FALSE;
}


void gatt_intf_handle_gatt_operation_complete(wiced_bt_gatt_operation_complete_t *p_op_cplt,
                                              gatt_intf_operation_t *p_op)
{
    wiced_bt_gatt_status_t status = p_op_cplt->status;
    wiced_bool_t is_operation_in_progress = WICED_FALSE;


    if (p_op_cplt->op == GATTC_OPTYPE_READ_HANDLE && p_op->pfn_operation == gatt_intf_internal_read_char_list)
    {
        is_operation_in_progress = WICED_TRUE;
    }
    else if ( (p_op_cplt->op == GATTC_OPTYPE_WRITE_WITH_RSP || p_op_cplt->op == GATTC_OPTYPE_WRITE_NO_RSP) && p_op->pfn_operation == gatt_intf_internal_enable_notification)
    {
        is_operation_in_progress = WICED_TRUE;
    }
    else
    {
        GATT_INTF_TRACE("[%s] not a read or write operation", __FUNCTION__);
        return;
    }

    GATT_INTF_TRACE("[%s] conn_id %d status %d ops_in_progress count %d is_operation %s",
                   __FUNCTION__,
                   p_op->conn_id,
                   status,
                   p_op->ops_in_progress,
                   is_operation_in_progress ? "yes" : "no");


    if (is_operation_in_progress == WICED_TRUE)
    {
            p_op->ops_in_progress--;
    }

    if (is_operation_complete(p_op))
    {
        /* All operations are complete, call the application callback */
        if (p_op->pfn_on_operation_complete)
        {
            p_op->pfn_on_operation_complete(p_op->conn_id, status, p_op->pv_ctx);
        }
    }
    else if (p_op->current_index != p_op->max_num_characteristics)
    {
        gatt_intf_execute_handle_operations(p_op);
    }
}
