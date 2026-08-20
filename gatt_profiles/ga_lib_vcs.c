/*
 * $ Copyright Cypress Semiconductor $
 */
 /** @file
  *
  * Volume Control Service Init
  */

#include "ga_lib_vcs.h"

#ifdef VCS_DEBUG
#define VCS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get VCS library traces */
#else
#define VCS_TRACE(...)
#endif



const gatt_intf_characteristic_info_t vcs_characteristic_info[] = {
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_VOLUME_STATE},
     .characteristic_type = GA_LIB_VCS_CHARACTERISTIC_VOLUME_STATE,
     .name = "state"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_CONTROL_POINT},
     .characteristic_type = GA_LIB_VCS_CHARACTERISTIC_CONTROL_POINT,
     .name = "cp"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_VOLUME_FLAG},
     .characteristic_type = GA_LIB_VCS_CHARACTERISTIC_VOLUME_FLAG,
     .name = "flag"}};

const gatt_intf_service_name_uuid_t vcs_id = {
    .name = "vcs",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_VOLUME_CONTROL,
};

const gatt_intf_included_service_info_t vcs_included[] = {
    {.name = "aics", .uuid.len = 2, .uuid.uu.uuid16 = WICED_BT_UUID_AUDIO_INPUT_CONTROL},
    {.name = "vocs", .uuid.len = 2, .uuid.uu.uuid16 = WICED_BT_UUID_VOLUME_OFFSET_CONTROL},
};

gatt_intf_service_discovery_ctx_t *ga_lib_vcs_discover_service(uint16_t conn_id,
                                                            pfn_on_discovery_complete_t pfn_on_complete,
                                                            uint8_t num_aics_instances,
                                                            uint8_t num_vocs_instances)
{
    uint8_t num_characteristics = sizeof(vcs_characteristic_info) / sizeof(vcs_characteristic_info[0]);
    gatt_intf_service_discovery_ctx_t *p_sdc =
        gatt_intf_alloc_service_discovery_ctx(num_characteristics,
                                              num_aics_instances + num_vocs_instances,
                                              sizeof(gatt_intf_service_discovery_ctx_t));
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);

    /* ignore return */
    gatt_intf_copy_unique_characteristic_info(p_sdc, vcs_characteristic_info, num_characteristics);
    uint8_t inc_start = gatt_intf_repeat_included_service_info(p_sdc,
                                                               0,
                                                               &vcs_included[0],
                                                               num_aics_instances);
    inc_start = gatt_intf_repeat_included_service_info(p_sdc,
                                                       inc_start,
                                                       &vcs_included[1],
                                                       num_vocs_instances);

    gatt_intf_start_service_discovery(conn_id, &vcs_id, 1, p_sdc);

    return p_sdc;
}

wiced_result_t ga_lib_vcs_check_volume_state_validity(ga_lib_vcs_volume_control_opcodes_t opcode,
                                                   ga_lib_vcs_volume_state_t *p_state,
                                                   uint8_t new_volume_setting, int step_size)
{
    ga_lib_vcs_volume_state_t new_state = {0};

    switch (opcode)
    {
    case VOLUME_CONTROL_OPCODE_RELATIVE_VOLUME_DOWN:
        new_state.volume_setting = MAX((int)(p_state->volume_setting - step_size), 0);
        new_state.mute_state = p_state->mute_state;
        break;
    case VOLUME_CONTROL_OPCODE_RELATIVE_VOLUME_UP:
        new_state.volume_setting = MIN((int)(p_state->volume_setting + step_size), GA_LIB_VCS_MAXIMUM_VOLUME);
        new_state.mute_state = p_state->mute_state;
        break;
    case VOLUME_CONTROL_OPCODE_UNMUTE_RELATIVE_VOLUME_DOWN:
        new_state.volume_setting = MAX((int)(p_state->volume_setting - step_size), 0);
        new_state.mute_state = GA_LIB_MUTE_STATE_NOT_MUTED;
        break;
    case VOLUME_CONTROL_OPCODE_UNMUTE_RELATIVE_VOLUME_UP:
        new_state.volume_setting = MIN((int)(p_state->volume_setting + step_size), GA_LIB_VCS_MAXIMUM_VOLUME);
        new_state.mute_state = GA_LIB_MUTE_STATE_NOT_MUTED;
        break;
    case VOLUME_CONTROL_OPCODE_SET_ABSOLUTE_VOLUME:
        new_state.volume_setting = new_volume_setting;
        new_state.mute_state = p_state->mute_state;
        break;
    case VOLUME_CONTROL_OPCODE_UNMUTE:
        new_state.mute_state = GA_LIB_MUTE_STATE_NOT_MUTED;
        new_state.volume_setting = p_state->volume_setting;
        break;
    case VOLUME_CONTROL_OPCODE_MUTE:
        new_state.volume_setting = p_state->volume_setting;
        new_state.mute_state = GA_LIB_MUTE_STATE_MUTED;
        break;
    default:
        return WICED_ERROR;
    }

    if ((new_state.volume_setting != p_state->volume_setting) || (new_state.mute_state != p_state->mute_state))
    {
        p_state->mute_state = new_state.mute_state;
        p_state->volume_setting = new_state.volume_setting;
        return WICED_SUCCESS;
    }
    return WICED_ERROR;
}


wiced_bt_gatt_status_t ga_lib_vcs_notify_volume_state(uint16_t conn_id,
                                                   uint16_t handle,
                                                   ga_lib_vcs_volume_state_t *p_state)
{
    uint8_t buff[3];
    uint8_t *ptr = buff;


    if (p_state->change_counter == 0xFF)
        p_state->change_counter = 0;
    else
        p_state->change_counter++;

    UINT8_TO_STREAM(ptr, p_state->volume_setting);
    UINT8_TO_STREAM(ptr, p_state->mute_state);
    UINT8_TO_STREAM(ptr, p_state->change_counter);


    return gatt_intf_send_notification(conn_id, handle, buff, ptr - buff);
}

wiced_bt_gatt_status_t ga_lib_vcs_notify_volume_flag(uint16_t conn_id, uint16_t handle, uint8_t flag)
{
    return gatt_intf_send_notification(conn_id, handle, &flag, 1);
}

wiced_bt_gatt_status_t ga_lib_vcs_write_control_point(uint16_t conn_id,
                                                   gatt_intf_characteristic_handles_t *p_handle,
                                                   ga_lib_vcs_volume_control_opcodes_t opcode,
                                                   ga_lib_vcs_volume_state_t *p_state)
{
    uint8_t *p_write_data = gatt_intf_method_get_buffer(3);
    uint8_t *p_data = p_write_data;
    wiced_bt_gatt_status_t res = WICED_BT_GATT_SUCCESS;

    if (!p_write_data)
    {
        return WICED_BT_GATT_NO_RESOURCES;
    }

    // execute read procedure
    UINT8_TO_STREAM(p_data, opcode);
    UINT8_TO_STREAM(p_data, p_state->change_counter);

    switch (opcode)
    {
    case VOLUME_CONTROL_OPCODE_RELATIVE_VOLUME_DOWN:
    case VOLUME_CONTROL_OPCODE_RELATIVE_VOLUME_UP:
    case VOLUME_CONTROL_OPCODE_UNMUTE_RELATIVE_VOLUME_DOWN:
    case VOLUME_CONTROL_OPCODE_UNMUTE_RELATIVE_VOLUME_UP:
    case VOLUME_CONTROL_OPCODE_MUTE:
    case VOLUME_CONTROL_OPCODE_UNMUTE:
        break;
    case VOLUME_CONTROL_OPCODE_SET_ABSOLUTE_VOLUME:
        UINT8_TO_STREAM(p_data, p_state->volume_setting);
        break;
    default:
        res = WICED_BT_GATT_ERROR;
    }

    if (res == WICED_BT_GATT_SUCCESS)
    {
        wiced_bt_gatt_write_hdr_t write_hdr = {.handle = p_handle->value_handle,
                                               .offset = 0,
                                               .len = (uint16_t)(p_data - p_write_data)};

        res = wiced_bt_gatt_client_send_write(conn_id,
                                              GATT_REQ_WRITE,
                                              &write_hdr,
                                              p_write_data,
                                              (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);
    }

    return res;
}
