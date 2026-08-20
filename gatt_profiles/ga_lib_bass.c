/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Broadcast Audio Scan Service init implementation.
  */

#include "ga_lib_bass.h"


#ifdef BASS_DEBUG
#define BASS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get BASS library traces */
#else
#define BASS_TRACE(...)
#endif

#define BASS_SCAN_OPCODE_DATA_SIZE 0
#define BASS_ADD_SOURCE_OPCODE_MIN_DATA_LEN 15
#define BASS_MODIFY_SOURCE_OPCODE_MIN_DATA_LEN 5
#define BASS_ADD_BROADCAST_CODE_OPCODE_DATA_LEN 17
#define BASS_REMOVE_SOURCE_OPCODE_DATA_LEN 1
#define BASS_BIS_SYNC_LEN 4
#define BASS_SUB_GROUP_MIN_DATA_LEN 5
/** Minimum Broadcast Receive state char. length (Sum of all mandatory fields data len) */
#define BASS_BROADCAST_RECV_STATE_CHAR_MIN_LEN 15
#define BASS_MAX_META_LEN 255
#define AD_FLAG_SIZE 2

const gatt_intf_characteristic_info_t bass_characteristic_info[] = {
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_BASS_CONTROL_POINT},
     .characteristic_type = GA_LIB_BASS_CHARACTERISTIC_BROADCAST_AUDIO_SCAN_CONTROL_POINT,
     .name = "cp"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_BASS_BROADCAST_RECEIVE_STATE},
     .characteristic_type = GA_LIB_BASS_CHARACTERISTIC_BROADCAST_RECEIVE_STATE,
     .name = "recv_state"},
};

const gatt_intf_service_name_uuid_t bass_id = {
    .name = "bass",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_BROADCAST_AUDIO_SCAN,
};


/**< Starts BASS service discovery
* Invokes \p pfn_store callback with the service handles on discovering the BASS service and its characteristics.
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
gatt_intf_service_discovery_ctx_t *ga_lib_bass_discover_service(uint16_t conn_id,
                                                             pfn_on_discovery_complete_t pfn_on_complete,
                                                             uint8_t max_receive_states)
{
    int num_characteristics = GA_LIB_BASS_CHARACTERISTIC_MAX_UNIQUE + max_receive_states;
    gatt_intf_service_discovery_ctx_t *p_sdc =
        gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, sizeof(gatt_intf_service_discovery_ctx_t));
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_complete);
    uint8_t start =
        gatt_intf_copy_unique_characteristic_info(p_sdc, bass_characteristic_info, GA_LIB_BASS_CHARACTERISTIC_MAX_UNIQUE);
    gatt_intf_repeat_characteristic_info(p_sdc,
                                         start,
                                         &bass_characteristic_info[GA_LIB_BASS_CHARACTERISTIC_MAX_UNIQUE],
                                         max_receive_states);

    gatt_intf_start_service_discovery(conn_id, &bass_id, 1, p_sdc);

    return p_sdc;
}

int ga_lib_bass_build_receive_state_char_data(ga_lib_bass_receive_state_t *p_recv_data, uint8_t *ptr, int max_len)
{
    int index = 0;
    uint8_t *ptr_start = ptr;
    wiced_bt_ble_address_t addr = {0};

    //if there is no source added send empty notification
    if (p_recv_data->broadcast_id == 0 && memcmp(&addr, &p_recv_data->source_addr, BD_ADDR_LEN) == 0)
    {
        return 0;
    }

    UINT8_TO_STREAM(ptr, p_recv_data->source_id);
    UINT8_TO_STREAM(ptr, p_recv_data->source_addr.type);
    BDADDR_TO_STREAM(ptr, p_recv_data->source_addr.bda);
    UINT8_TO_STREAM(ptr, p_recv_data->adv_sid);
    ARRAY_TO_STREAM(ptr, &p_recv_data->broadcast_id, BAP_BROADCAST_ID_SIZE);
    UINT8_TO_STREAM(ptr, p_recv_data->pa_sync_state);
    UINT8_TO_STREAM(ptr, p_recv_data->big_encryption_state);
    if (p_recv_data->big_encryption_state == GA_LIB_BASS_BIG_BAD_BROADCAST_CODE)
    {
        WICED_MEMSET(ptr, 0XFF, BAP_BROADCAST_CODE_SIZE);
        ptr += BAP_BROADCAST_CODE_SIZE;
    }
    UINT8_TO_STREAM(ptr, p_recv_data->num_subgroup);
    if (p_recv_data->sub_group_data)
    {
        for (index = 0; index < p_recv_data->num_subgroup; index++)
        {
            UINT32_TO_STREAM(ptr, p_recv_data->sub_group_data[index].bis_sync_state);
            ptr += ga_lib_bap_fill_metadata(ptr,
                                            max_len - (ptr - ptr_start),
                                            &p_recv_data->sub_group_data[index].meta_data);
        }
    }
    return ptr - ptr_start;
}

wiced_bt_gatt_status_t ga_lib_bass_notify_recv_state(uint16_t conn_id, uint16_t handle, ga_lib_bass_receive_state_t *p_bass)
{
    uint8_t buff[256];
    uint8_t *ptr = buff;

    BASS_TRACE("[%s] \n", __FUNCTION__);

    ptr += ga_lib_bass_build_receive_state_char_data(p_bass, ptr, sizeof(buff));

    return gatt_intf_send_notification(conn_id, handle, buff, ptr - buff);
}

static wiced_bool_t bass_validate_num_subgroup_len(const uint8_t *p_data, uint16_t len)
{
    uint8_t num_subgroup;
    uint8_t metadata_len;
    STREAM_TO_UINT8(num_subgroup, p_data);
    len--;
    while (num_subgroup)
    {
        if (len < BASS_SUB_GROUP_MIN_DATA_LEN)
        {
            return WICED_FALSE;
        }
        // skip bis sync
        p_data += BASS_BIS_SYNC_LEN;
        len -= BASS_BIS_SYNC_LEN;

        // read metadata len
        STREAM_TO_UINT8(metadata_len, p_data);
        len--;

        // validate metadata length
        if (len < metadata_len)
        {
            return WICED_FALSE;
        }
        len -= metadata_len;
        p_data += metadata_len;

        //move to next subgroup
        num_subgroup--;
    }
    if (len)
        return WICED_FALSE;
    return WICED_TRUE;
}

static wiced_bool_t bass_validate_opcode_length(const uint8_t *p_data, ga_lib_bass_opcode_t opcode, uint16_t len)
{
    if (opcode > GA_LIB_BASS_OP_REMOVE_SOURCE)
    {
        return WICED_FALSE;
    }

    switch (opcode)
    {
    case GA_LIB_BASS_OP_REMOTE_SCAN_STOPPED:
    case GA_LIB_BASS_OP_REMOTE_SCAN_STARTED:
        if (len != BASS_SCAN_OPCODE_DATA_SIZE)
        {
            return WICED_FALSE;
        }
        break;
    case GA_LIB_BASS_OP_ADD_SOURCE:
        if (len < BASS_ADD_SOURCE_OPCODE_MIN_DATA_LEN)
        {
            return WICED_FALSE;
        }
        len -= BASS_ADD_SOURCE_OPCODE_MIN_DATA_LEN;
        p_data += BASS_ADD_SOURCE_OPCODE_MIN_DATA_LEN;
        return bass_validate_num_subgroup_len(p_data - 1, len + 1); // pass data from number of subgroup
        break;
    case GA_LIB_BASS_OP_MODIFY_SOURCE:
        if (len < BASS_MODIFY_SOURCE_OPCODE_MIN_DATA_LEN)
        {
            return WICED_FALSE;
        }
        len -= BASS_MODIFY_SOURCE_OPCODE_MIN_DATA_LEN;
        p_data += BASS_MODIFY_SOURCE_OPCODE_MIN_DATA_LEN;
        return bass_validate_num_subgroup_len(p_data - 1, len + 1); // pass data from number of subgroup
        break;
    case GA_LIB_BASS_OP_SET_BROADCAST_CODE:
        if (len != BASS_ADD_BROADCAST_CODE_OPCODE_DATA_LEN)
        {
            return WICED_FALSE;
        }
        break;
    case GA_LIB_BASS_OP_REMOVE_SOURCE:
        if (len != BASS_REMOVE_SOURCE_OPCODE_DATA_LEN)
        {
            return WICED_FALSE;
        }
        break;
    }
    return WICED_TRUE;
}

static wiced_bool_t bass_parse_common_param(ga_lib_bass_common_source_data_t *src_param, const uint8_t *p_data)
{
    int index;
    int j;
    uint8_t metadata_length;

    STREAM_TO_UINT8(src_param->pa_sync_param, p_data);
    STREAM_TO_UINT16(src_param->pa_interval, p_data);
    STREAM_TO_UINT8(src_param->num_subgroup, p_data);
    if (src_param->pa_sync_param > GA_LIB_BASS_PA_SYNC_NO_PAST)
        return WICED_FALSE;
    for (index = 0; (index < src_param->num_subgroup) && (index < GA_LIB_BASS_MAX_SUBGROUP_COUNT); index++)
    {
        STREAM_TO_UINT32(src_param->sub_group_data[index].bis_sync_state, p_data);
        STREAM_TO_UINT8(metadata_length, p_data);
        for (j = index - 1; j >= 0; j--)
        {
            if ((src_param->sub_group_data[index].bis_sync_state & GA_LIB_BASS_BIS_INDEX_MASK) ==
                (src_param->sub_group_data[j].bis_sync_state & GA_LIB_BASS_BIS_INDEX_MASK))
                return WICED_FALSE;
        }
        ga_lib_ascs_cp_cmd_sts_t sts;
        ga_lib_bap_get_metadata(p_data, metadata_length, &src_param->sub_group_data[index].meta_data, &sts);
    }
    return WICED_TRUE;
}

wiced_bt_gatt_status_t ga_lib_bass_parse_control_point_data(ga_lib_bass_operation_t *p_operation_data,
                                                         const uint8_t *p_data,
                                                         uint16_t total_len)
{
    // handle control point write
    STREAM_TO_UINT8(p_operation_data->opcode, p_data);
    // Validate Opcode
    if (p_operation_data->opcode > GA_LIB_BASS_OP_REMOVE_SOURCE)
        return GA_LIB_BASS_ERROR_OPCODE_NOT_SUPPORTED;
    total_len--; // don't count opcode as its already read
    // Validate length based on opcode
    if (!bass_validate_opcode_length(p_data, p_operation_data->opcode, total_len))
    {
        return WICED_BT_GATT_WRITE_REQ_REJECTED;
    }

    switch (p_operation_data->opcode)
    {
    case GA_LIB_BASS_OP_ADD_SOURCE:
    {
        ga_lib_bass_add_source_t *p_add_source_param = &p_operation_data->data.add_source_param;

        STREAM_TO_UINT8(p_add_source_param->source_addr.type, p_data);
        STREAM_TO_BDADDR(p_add_source_param->source_addr.bda, p_data);
        STREAM_TO_UINT8(p_add_source_param->adv_sid, p_data);
        STREAM_TO_UINT24(p_add_source_param->broadcast_id, p_data);
        if ((p_add_source_param->adv_sid > GA_LIB_BASS_MAX_ADV_SID) ||
            (p_add_source_param->source_addr.type > BLE_ADDR_RANDOM) ||
            (!bass_parse_common_param(&p_add_source_param->src_data, p_data)))
        {
            return WICED_BT_GATT_WRITE_REQ_REJECTED;
        }
    }
    break;
    case GA_LIB_BASS_OP_MODIFY_SOURCE:
    {
        ga_lib_bass_modify_source_t *p_modify_source_param = &p_operation_data->data.modify_source_param;
        STREAM_TO_UINT8(p_modify_source_param->source_id, p_data);
        if (!bass_parse_common_param(&p_modify_source_param->src_data, p_data))
            return WICED_BT_GATT_WRITE_REQ_REJECTED;
    }
    break;
    case GA_LIB_BASS_OP_SET_BROADCAST_CODE:
    {
        ga_lib_bass_set_broadcast_code_t *p_broadcast_code_param = &p_operation_data->data.set_broadcast_param;

        STREAM_TO_UINT8(p_broadcast_code_param->source_id, p_data);
        STREAM_TO_ARRAY16(p_broadcast_code_param->broadcast_code, p_data);
    }
    break;
    case GA_LIB_BASS_OP_REMOVE_SOURCE:
    {
        STREAM_TO_UINT8(p_operation_data->data.remove_source_id, p_data);
    }
    break;
    default:
        BASS_TRACE("[%s] Unknown event\n", __FUNCTION__);
        break;
    }
    return WICED_BT_GATT_SUCCESS;
}
