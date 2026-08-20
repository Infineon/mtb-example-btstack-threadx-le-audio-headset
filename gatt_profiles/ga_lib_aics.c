/*
 * $ Copyright Cypress Semiconductor $
 */

#include "ga_lib_aics.h"

#ifdef AICS_DEBUG
#define AICS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get AICS library traces */
#else
#define AICS_TRACE(...)
#endif


const gatt_intf_characteristic_info_t aics_characteristic_info[] =
{
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_INPUT_STATE},
     .characteristic_type = GA_LIB_AICS_CHARACTERISTIC_INPUT_STATE,
     .name = "state"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_GAIN_SETTING_ATTRIBUTE},
     .characteristic_type = GA_LIB_AICS_CHARACTERISTIC_GAIN_SETTING_PROPERTIES,
     .name = "gain"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_INPUT_TYPE},
     .characteristic_type = GA_LIB_AICS_CHARACTERISTIC_INPUT_TYPE,
     .name = "input_type"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_INPUT_STATUS},
     .characteristic_type = GA_LIB_AICS_CHARACTERISTIC_INPUT_STATUS,
     .name = "status"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_AUDIO_INPUT_CONTROL_POINT},
     .characteristic_type = GA_LIB_AICS_CHARACTERISTIC_INPUT_CONTROL_POINT,
     .name = "cp"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_AUDIO_INPUT_DESCRIPTION},
     .characteristic_type = GA_LIB_AICS_CHARACTERISTIC_INPUT_DESCRIPTION,
     .name = "desc"},
};

const gatt_intf_service_name_uuid_t aics_id = {
    .name = "aics",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_AUDIO_INPUT_CONTROL,
};

/**< Starts AICS service discovery
* Invokes \p pfn_on_complete callback with the service handles on discovering the AICS service and its characteristics.
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
gatt_intf_service_discovery_ctx_t *ga_lib_aics_discover_service(uint16_t conn_id,
                                                                pfn_on_discovery_complete_t pfn_on_complete,
                                                                uint16_t start_handle,
                                                                uint16_t end_handle)
{
    uint8_t num_characteristics = sizeof(aics_characteristic_info) / sizeof(aics_characteristic_info[0]);
    gatt_intf_service_discovery_ctx_t *p_sdc = gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, 0);
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);

    gatt_intf_copy_unique_characteristic_info(p_sdc, aics_characteristic_info, num_characteristics);

    gatt_intf_start_secondary_service_discovery(conn_id, &aics_id, start_handle, end_handle, p_sdc);

    return p_sdc;
}

wiced_bt_gatt_status_t ga_lib_aics_notify_description(uint16_t conn_id,
                                                   uint16_t handle,
                                                   uint8_t *p_desc,
                                                   uint16_t desc_len)
{
    return gatt_intf_send_notification(conn_id, handle, p_desc, desc_len);
}

wiced_bt_gatt_status_t ga_lib_aics_notify_input_state(uint16_t conn_id,
                                                   uint16_t handle,
                                                   ga_lib_aics_input_state_t *p_state)
{
    uint8_t buff[4];
    uint8_t *ptr = buff ;

    //Increment change counter
    if (p_state->change_counter == 0xFF)
        p_state->change_counter = 0;
    else
        p_state->change_counter++;

    UINT8_TO_STREAM(ptr, p_state->gain_setting);
    UINT8_TO_STREAM(ptr, p_state->mute_mode);
    UINT8_TO_STREAM(ptr, p_state->gain_mode);
    UINT8_TO_STREAM(ptr, p_state->change_counter);

    return gatt_intf_send_notification(conn_id, handle, buff, ptr - buff);
}

wiced_bt_gatt_status_t ga_lib_aics_notify_input_status(uint16_t conn_id, uint16_t handle, ga_lib_aics_input_status_t input_status)
{
    return gatt_intf_send_notification(conn_id, handle, &input_status, 1);
}

wiced_bt_gatt_status_t ga_lib_aics_write_cp(uint16_t conn_id,
                                         gatt_intf_characteristic_handles_t *p_handle,
                                         uint8_t opcode,
                                         ga_lib_aics_input_state_t *p_state)
{
    uint8_t *p_write_data = gatt_intf_method_get_buffer(3);
    uint8_t *p_data = p_write_data;

    if (!p_write_data)
    {
        return WICED_BT_GATT_NO_RESOURCES;
    }
    UINT8_TO_STREAM(p_data, opcode);
    UINT8_TO_STREAM(p_data, p_state->change_counter);
    if (opcode == GA_LIB_AICS_OPCODE_SET_GAIN_SETTINGS)
    {
        UINT8_TO_STREAM(p_data, p_state->gain_setting);
    }

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

wiced_bt_gatt_status_t ga_lib_aics_write_input_description(uint16_t conn_id,
                                                        gatt_intf_characteristic_handles_t *p_handle,
                                                        uint8_t *name,
                                                        uint16_t name_len)
{
    uint8_t *p_write_data = gatt_intf_method_get_buffer(name_len);
    uint8_t *p_data = p_write_data;

    if (!p_write_data)
    {
        return WICED_BT_GATT_NO_RESOURCES;
    }
    ARRAY_TO_STREAM(p_data, name, name_len);
    {
        wiced_bt_gatt_write_hdr_t write_hdr = {.handle = p_handle->value_handle,
                                               .offset = 0,
                                               .len = (uint16_t)(p_data - p_write_data)};
        return wiced_bt_gatt_client_send_write(conn_id,
                                               GATT_CMD_WRITE,
                                               &write_hdr,
                                               p_write_data,
                                               (wiced_bt_gatt_app_context_t)gatt_intf_method_free_buffer);
    }
}
