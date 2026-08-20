/*
 * $ Copyright Cypress Semiconductor $
 */

#include "lehs.h"

lehs_bass_t *lehs_bass_get_bass_data()
{
    return &g_lehs_gatt_cb.local_service_data.bass;
}

static lehs_bass_data_t *lehs_bass_find_source_by_bda(uint32_t broadcast_id, wiced_bt_device_address_t bda)
{
    lehs_bass_t *p_cb = lehs_bass_get_bass_data();
    lehs_bass_data_t *p_bass = &p_cb->bass_data[0];
    for (int index = 0; index < LEHS_BROADCAST_RCV_STATE_MAX; index++, p_bass++)
    {
        if ((p_bass->is_used) &&
            !WICED_MEMCMP(p_bass->recv_state.source_addr.bda, bda, BD_ADDR_LEN)  && (broadcast_id == p_bass->recv_state.broadcast_id))
        {
            return p_bass;
        }
    }
    return NULL;
}

static void lehs_fill_char_data(ga_lib_bass_add_source_t *p_source_param, lehs_bass_data_t *p_bass)
{
    WICED_BT_TRACE("[%s]\n", __FUNCTION__);
    ga_lib_bass_common_source_data_t *p_src_data = &p_source_param->src_data;

    p_bass->recv_state.adv_sid = p_source_param->adv_sid;
    p_bass->recv_state.sub_group_data = p_bass->sub_group;
    p_bass->recv_state.broadcast_id = p_source_param->broadcast_id;
    WICED_MEMCPY(&p_bass->recv_state.source_addr, &p_source_param->source_addr, sizeof(wiced_bt_ble_address_t));
    p_bass->recv_state.num_subgroup = p_src_data->num_subgroup;

    for (int index = 0; (index < p_src_data->num_subgroup) && (index < GA_LIB_BASS_MAX_SUBGROUP_COUNT); index++)
    {
        WICED_MEMCPY(&p_bass->sub_group[index].meta_data,
                     &p_src_data->sub_group_data[index].meta_data,
                     sizeof(ga_lib_ascs_metadata_t));
    }
}

static lehs_bass_data_t *lehs_allocate_source(ga_lib_bass_add_source_t *p_source_param, uint16_t conn_id, uint8_t *char_instance)
{
    lehs_bass_t *p_bass = lehs_bass_get_bass_data();
    int index;
    WICED_BT_TRACE("[%s] \n", __FUNCTION__);
    for (index = 0; index < LEHS_BROADCAST_RCV_STATE_MAX; index++)
    {
        lehs_bass_data_t *p_bass_data = &p_bass->bass_data[index];
        if (!p_bass_data->is_used)
        {
            p_bass_data->is_used = WICED_TRUE;
            p_bass_data->conn_id = conn_id;
            p_bass_data->recv_state.source_id = index;
            p_bass_data->bis_index_bits = p_source_param->src_data.sub_group_data->bis_sync_state;
            lehs_fill_char_data(p_source_param, p_bass_data);
            *char_instance = index;
            return p_bass_data;
        }
    }
    return NULL;
}

static lehs_bass_data_t *lehs_find_source(uint8_t source_id, uint8_t *char_instance)
{
    lehs_bass_t *p_bass = lehs_bass_get_bass_data();
    int index;
    for (index = 0; index < LEHS_BROADCAST_RCV_STATE_MAX; index++)
    {
        if ((p_bass->bass_data[index].is_used) && (p_bass->bass_data[index].recv_state.source_id == source_id))
        {
            *char_instance = index;
            return &p_bass->bass_data[index];
        }
    }
    return NULL;
}

static void lehs_notify_recv_state_char(uint16_t handle, lehs_bass_data_t *p_bass)
{
    ga_lib_bass_receive_state_t data;

    WICED_MEMCPY(&data, &p_bass->recv_state, sizeof(ga_lib_bass_receive_state_t));
    data.sub_group_data = p_bass->sub_group;

    ga_lib_bass_notify_recv_state(p_bass->conn_id, handle, &data);
}

static void set_pa_sync_state(lehs_bass_data_t *p_bass, uint8_t state)
{
    WICED_BT_TRACE("[%s] pa sync state %d updated state %d", __FUNCTION__, p_bass->recv_state.pa_sync_state, state);
    p_bass->recv_state.pa_sync_state = state;
}

static void set_bis_sync_state(lehs_bass_data_t *p_bass, uint32_t bis_sync_state)
{
    WICED_BT_TRACE("[%s] bis sync state %d updated state %d",
                   __FUNCTION__,
                   p_bass->recv_state.sub_group_data->bis_sync_state,
                   bis_sync_state);
    p_bass->recv_state.sub_group_data->bis_sync_state = bis_sync_state; // BIS bit fields
}

static uint8_t get_pa_sync_state(lehs_bass_data_t *p_bass)
{
    return p_bass->recv_state.pa_sync_state;
}

static uint32_t get_bis_sync_state(lehs_bass_data_t *p_bass)
{
    return p_bass->recv_state.sub_group_data->bis_sync_state;
}

static void set_big_encryption_state(lehs_bass_data_t *p_bass, uint8_t state)
{
    WICED_BT_TRACE("[%s] big encryption state %d updated state %d",
                   __FUNCTION__,
                   p_bass->recv_state.big_encryption_state,
                   state);
    p_bass->recv_state.big_encryption_state = state;
}

static void lehs_handle_modify_source(lehs_bass_data_t *p_bass, ga_lib_bass_modify_source_t *p_modify_src)
{
    WICED_BT_TRACE("[%s] conn id %x source id %d pa_sync_param %d bis_sync_state %d",
                   __FUNCTION__,
                   p_bass->conn_id,
                   p_modify_src->source_id,
                   p_modify_src->src_data.pa_sync_param,
                   p_modify_src->src_data.sub_group_data->bis_sync_state);

    uint8_t pa_sync_state = get_pa_sync_state(p_bass);
    uint32_t bis_sync_state = get_bis_sync_state(p_bass);
    ga_lib_bass_pa_sync_param_t pa_sync_param = p_modify_src->src_data.pa_sync_param;
    uint32_t req_bis_sync_state = p_modify_src->src_data.sub_group_data->bis_sync_state;

    // Terminate PA sync if the client requests no sync and the server is currently synchronized to the PA
    if ((pa_sync_param == GA_LIB_BASS_PA_NO_SYNC) && (pa_sync_state == GA_LIB_BASS_PA_SYNC))
    {
        wiced_ble_padv_terminate_sync(p_bass->sync_handle);
        set_pa_sync_state(p_bass, GA_LIB_BASS_PA_NOT_SYNC);
        lehs_notify_recv_state_char(HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE, p_bass);
    }
    // If the server is not synchronized to the PA and the client requests synchronization to the PA, initiate synchronization to the PA
    // If the server is already synchronized to BIS, avoid synchronizing to the PA again.
    else if (!bis_sync_state && (pa_sync_param != GA_LIB_BASS_PA_NO_SYNC) && (pa_sync_state == GA_LIB_BASS_PA_NOT_SYNC))
    {
        lehs_bass_synchronize_to_pa(pa_sync_param, p_bass);
        // BIS synchronization is dependent on PA synchronization, wait for PA synchronization to complete before synchronizing to BIS.
        if (req_bis_sync_state)
        {
            p_bass->bis_index_bits = req_bis_sync_state;
        }
        return;
    }


    if (bis_sync_state != req_bis_sync_state)
    {
        WICED_BT_TRACE("[%s] bis sync state %d updated state %d",
                       __FUNCTION__,
                       bis_sync_state,
                       req_bis_sync_state);

        // If the server is synchronized to BIS and the client requests no synchronization to BIS, notify the client of the current state.
        if (bis_sync_state && (req_bis_sync_state == 0xFFFFFFFF))
        {
            lehs_notify_recv_state_char(HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE, p_bass);
        }

        // If the server is synchronized to BIS and the client requests no synchronization to BIS, terminate synchronization to BIS.
        if (bis_sync_state)
        {
            lehs_bis_terminate_sync(p_bass->recv_state.broadcast_id);
        }
        else
        {
            // If the server is synchronized to the PA and the client requests synchronization to BIS, initiate synchronization to BIS.
            if (req_bis_sync_state && (pa_sync_state == GA_LIB_BASS_PA_SYNC))
            {
                p_bass->bis_index_bits = req_bis_sync_state;
                lehs_bass_synchronize_to_source(p_bass);
            }
        }
    }
    else
    {
        lehs_notify_recv_state_char(HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE, p_bass);
    }

}

wiced_result_t lehs_bass_handle_write_req_evt(uint16_t conn_id,
                                              lehs_clcb_t *p_clcb,
                                              uint8_t *p_evt_data,
                                              uint16_t len_to_write)
{
    wiced_result_t result = WICED_SUCCESS;
    lehs_bass_t *p_data = lehs_bass_get_bass_data();
    ga_lib_bass_operation_t *p_op_data = &p_data->op_data;

    result = ga_lib_bass_parse_control_point_data(p_op_data, p_evt_data, len_to_write);

    if (result != WICED_BT_SUCCESS)
        return result;

    switch (p_op_data->opcode)
    {
    case GA_LIB_BASS_OP_ADD_SOURCE:
    {
        uint8_t char_instance = 0;

        ga_lib_bass_add_source_t *p_add_src = &p_op_data->data.add_source_param;

        WICED_BT_TRACE("[%s] conn id %x boradcast id %x source addr %B adv sid %d pa_sync_param %d\n",
                       __FUNCTION__,
                       conn_id,
                       p_add_src->broadcast_id,
                       p_add_src->source_addr.bda,
                       p_add_src->adv_sid,
                       p_add_src->src_data.pa_sync_param);
        lehs_bass_data_t *p_bass = lehs_allocate_source(p_add_src, conn_id, &char_instance);

        uint8_t *br_name = NULL;
        lehs_rpc_send_new_stream_info(p_add_src->broadcast_id, br_name);

        if (p_bass)
        {
            set_big_encryption_state(p_bass, GA_LIB_BASS_BIG_NOT_ENCRYPTED);
            // Notify
            lehs_notify_recv_state_char(HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE, p_bass);
            lehs_bass_synchronize_to_pa(p_add_src->src_data.pa_sync_param, p_bass);
        }
    }
    break;
    case GA_LIB_BASS_OP_MODIFY_SOURCE:
    {

        uint8_t char_instance = 0;
        ga_lib_bass_modify_source_t *p_modify_src = &p_op_data->data.modify_source_param;
        lehs_bass_data_t *p_bass = lehs_find_source(p_modify_src->source_id, &char_instance);
        if (p_bass)
        {
            lehs_handle_modify_source(p_bass, p_modify_src);
        }
        else
        {
            result = (wiced_result_t)GA_LIB_BASS_ERROR_INVALID_SOURCE_ID;
        }
    }
    break;
    case GA_LIB_BASS_OP_REMOVE_SOURCE:
    {
        uint8_t char_instance = 0;
        lehs_bass_data_t *p_bass = lehs_find_source(p_op_data->data.remove_source_id, &char_instance);
        /*
            * The server shall not accept a Remove Source operation for a Source_ID value that matches the Source_ID written by the client
            *  in the Remove Source operation if the server is synchronized to the PA and/or any BIS as defined by the values of the PA_Sync_State
            *  and BIS_Sync_State fields in the Broadcast Receive State characteristic containing that Source_ID value.
            */
        if (p_bass)
        {
            if ((get_pa_sync_state(p_bass) == GA_LIB_BASS_PA_SYNC) || get_bis_sync_state(p_bass))
            {
                WICED_BT_TRACE_CRIT("[%s]  source id %d pa_sync_state %d bis_sync_state %d",
                                    __FUNCTION__,
                                    p_bass->recv_state.source_id,
                                    p_bass->recv_state.pa_sync_state,
                                    p_bass->recv_state.sub_group_data->bis_sync_state);
                return WICED_UNSUPPORTED;
            }
            lehs_broadcast_sink_cb_t *p_big = lehs_bis_get_big_by_broadcast_id(p_bass->recv_state.broadcast_id);
            if (p_big)
            {
                WICED_MEMSET(p_big, 0, sizeof(lehs_broadcast_sink_cb_t));
            }

            memset(&p_bass->recv_state, 0, sizeof(p_bass->recv_state));
            lehs_notify_recv_state_char(HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE, p_bass);
            WICED_MEMSET(p_bass, 0, sizeof(lehs_bass_data_t));
        }
        else
        {
            result = (wiced_result_t)GA_LIB_BASS_ERROR_INVALID_SOURCE_ID;
        }
    }
    break;
    case GA_LIB_BASS_OP_SET_BROADCAST_CODE:
    {
        uint8_t char_instance = 0;
        lehs_bass_data_t *p_bass = lehs_find_source(p_op_data->data.set_broadcast_param.source_id, &char_instance);
        if (p_bass)
        {
            WICED_MEMCPY(p_bass->recv_state.broadcast_code,
                         p_op_data->data.set_broadcast_param.broadcast_code,
                         BAP_BROADCAST_CODE_SIZE);
            p_bass->waiting_broadcast_code = WICED_FALSE;
            set_big_encryption_state(p_bass, GA_LIB_BASS_BIG_DECRPTING);
            lehs_notify_recv_state_char(HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE, p_bass);

            //sync to stream
            lehs_broadcast_sink_cb_t *p_big = lehs_bis_get_big_by_broadcast_id(p_bass->recv_state.broadcast_id);
            if (p_big)
            {
                WICED_MEMCPY(p_big->broadcast_code,
                             p_op_data->data.set_broadcast_param.broadcast_code,
                             BAP_BROADCAST_CODE_SIZE);

                lehs_bass_synchronize_to_source(p_bass);
            }
            else
            {
                WICED_BT_TRACE_CRIT("[%s] p_big is NULL!", __FUNCTION__);
            }
        }
    }
    break;
    default:
        break;
    }
    return result;
}

void lehs_bass_handle_pa_sync_transfer_param_evt(wiced_ble_set_padv_sync_transfer_param_event_data_t *p_sync_param)
{
    lehs_clcb_t *p_clcb = lehs_gatt_get_clcb_by_conn_handle(p_sync_param->conn_handle);
    if (p_clcb == NULL)
    {
        WICED_BT_TRACE_CRIT("[%s] p_clcb is NULL!", __FUNCTION__);
        return;
    }

    lehs_bass_t *p_cb = lehs_bass_get_bass_data();
    lehs_bass_data_t *p_bass = &p_cb->bass_data[0];
    for (int i = 0; i < LEHS_BROADCAST_RCV_STATE_MAX; i++, p_bass++)
    {
        if (p_bass->is_used && p_bass->conn_id == p_clcb->conn_id &&
            p_bass->recv_state.pa_sync_state == GA_LIB_BASS_PA_SYNC_INFO_REQUEST)
        {
            lehs_notify_recv_state_char(HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE, p_bass);
            break;
        }
    }
}

void lehs_bass_handle_periodic_sync_transfer_evt(wiced_ble_padv_sync_established_event_data_t *p_sync_evt)
{
    lehs_bass_t *p_cb = lehs_bass_get_bass_data();
    lehs_bass_data_t *p_bass = &p_cb->bass_data[0];
    for (int index = 0; index < LEHS_BROADCAST_RCV_STATE_MAX; index++, p_bass++)
    {
        if (p_bass->is_used && (p_bass->recv_state.pa_sync_state == GA_LIB_BASS_PA_SYNC_INFO_REQUEST) &&
            p_bass->recv_state.adv_sid == p_sync_evt->adv_sid)
        {
            lehs_broadcast_sink_cb_t *p_big = lehs_bis_get_big_by_broadcast_id(p_bass->recv_state.broadcast_id);
            if (p_big)
            {
                p_big->sync_handle = (p_sync_evt->status == WICED_BT_SUCCESS) ? p_sync_evt->sync_handle : 0xFF;
            }
            p_bass->sync_handle = (p_sync_evt->status == WICED_BT_SUCCESS) ? p_sync_evt->sync_handle : 0xFF;
            set_pa_sync_state(p_bass,
                              (p_sync_evt->status == WICED_BT_SUCCESS) ? GA_LIB_BASS_PA_SYNC
                                                                       : GA_LIB_BASS_PA_FAILED_SYNC);
            lehs_notify_recv_state_char(HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE, p_bass);
            break;
        }
    }
}

void lehs_bass_synchronize_to_pa(ga_lib_bass_pa_sync_param_t pa_sync_param, lehs_bass_data_t *p_bass)
{
    WICED_BT_TRACE("[%s] broadcast id %x pa_sync_param %d\n",
                   __FUNCTION__,
                   p_bass->recv_state.broadcast_id,
                   pa_sync_param);

    lehs_bis_alloc_big(p_bass->recv_state.broadcast_id, p_bass->recv_state.source_addr.bda, p_bass->recv_state.adv_sid);
    if (pa_sync_param == GA_LIB_BASS_PA_SYNC_USING_PAST)
    {
        wiced_ble_padv_sync_transfer_param_t sync_transfer_param = {
            .mode = WICED_BLE_PERIODIC_ENABLE_PA_SYNC_TRANSFER_ENABLE_PA_REPORT_EVT,
            .skip = 0,
            .sync_cte_type = 0,
            .sync_timeout = 0x1770};
        wiced_bt_device_address_t bdaddr;
        wiced_bt_gatt_get_device_address(p_bass->conn_id, &bdaddr, NULL, NULL);
        wiced_bt_dev_status_t status = wiced_ble_padv_set_sync_transfer_params(bdaddr, &sync_transfer_param);
        WICED_BT_TRACE("[%s] set sync transfer param %d", __FUNCTION__, status);
        set_pa_sync_state(p_bass, GA_LIB_BASS_PA_SYNC_INFO_REQUEST);
    }
    else if (pa_sync_param == GA_LIB_BASS_PA_SYNC_NO_PAST)
    {
        lehs_sync_to_pa(p_bass->recv_state.broadcast_id);
    }
}

void lehs_bass_synchronize_to_source(lehs_bass_data_t *p_bass)
{
    WICED_BT_TRACE("[%s] broadcast id %x bis index bits %x\n",
                   __FUNCTION__,
                   p_bass->recv_state.broadcast_id,
                   p_bass->bis_index_bits);

    lehs_broadcast_sink_cb_t *p_big = lehs_bis_get_big_by_broadcast_id(p_bass->recv_state.broadcast_id);
    if (!p_big)
    {
        WICED_BT_TRACE("[%s] p_big is null", __FUNCTION__);
        return;
    }

    if (p_bass->bis_index_bits == 0xffffffff)
    {
        p_bass->bis_index_bits = 0;
        for (int i = 0; i < p_big->base.sub_group->bis_cnt; i++)
        {
            WICED_BT_TRACE("[%s] aca %d snk lc %d idx %d",
                           __FUNCTION__,
                           p_big->base.sub_group->bis_config[i].bis_csc.audio_channel_allocation,
                           g_lehs_pacs_app_data.snk_audio_location,
                           p_big->base.sub_group->bis_config[i].bis_idx);
            if (p_big->base.sub_group->bis_config[i].bis_csc.audio_channel_allocation &
                g_lehs_pacs_app_data.snk_audio_location)
            {
                p_bass->bis_index_bits |= p_big->base.sub_group->bis_config[i].bis_idx;
            }
        }
    }
    lehs_sync_to_source(p_big, p_bass->bis_index_bits);
}

void lehs_bass_notify_big_sync_state(lehs_broadcast_sink_cb_t* p_big)
{
    if (p_big)
    {
        lehs_bass_data_t *p_bass = lehs_bass_find_source_by_bda(p_big->base.broadcast_id, p_big->bd_addr);
        if (p_bass == NULL)
        {
            return;
        }

        if (p_big->sync_state == HCI_CONTROL_LEA_BROADCAST_BIG_SYNC_ESTABLISHED)
        {
            set_bis_sync_state(p_bass, p_bass->bis_index_bits);
            set_pa_sync_state(p_bass, GA_LIB_BASS_PA_NOT_SYNC);
        }
        else
        {
            set_bis_sync_state(p_bass, 0);
        }

        p_bass->recv_state.big_encryption_state = 0;
        lehs_notify_recv_state_char(HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE, p_bass);
    }
}

void lehs_bass_notify_pa_sync_state(lehs_broadcast_sink_cb_t *p_big, uint8_t pa_sync_state)
{
    if (p_big)
    {
        lehs_bass_data_t *p_bass = lehs_bass_find_source_by_bda(p_big->base.broadcast_id, p_big->bd_addr);
        if (p_bass == NULL)
        {
            return;
        }

        set_pa_sync_state(p_bass, pa_sync_state);
        lehs_notify_recv_state_char(HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE, p_bass);
    }
}

void lehs_bass_process_big_adv_report_and_sync(lehs_broadcast_sink_cb_t *p_big,
                                               wiced_ble_biginfo_adv_report_t *p_bigrpt)
{
    if (p_big)
    {
        lehs_bass_data_t *p_bass = lehs_bass_find_source_by_bda(p_big->base.broadcast_id, p_big->bd_addr);
        if (p_bass == NULL)
        {
            return;
        }
        WICED_BT_TRACE("[%s]\n", __FUNCTION__);

        // Handle BIG advertising report event
        if (p_bigrpt->encryption == WICED_BLE_ISOC_ENCRYPTED)
        {
            if (!p_bass->waiting_broadcast_code)
            {
                WICED_BT_TRACE("[%s] encryption is enabled broadcast code required!\n", __FUNCTION__);
                p_bass->waiting_broadcast_code = WICED_TRUE;
                set_big_encryption_state(p_bass, GA_LIB_BASS_BIG_BROADCAST_CODE_REQUIRED);

                // Notify Broadcast Code required
                lehs_notify_recv_state_char(HDLC_BASS_BROADCAST_RECEIVE_STATE_1_VALUE, p_bass);
            }
        }
        else
        {
            lehs_bass_synchronize_to_source(p_bass);
        }
    }
}

void lehs_bass_handle_acl_disconnection(uint16_t conn_id)
{
    lehs_bass_t *p_cb = lehs_bass_get_bass_data();
    lehs_bass_data_t *p_bass = &p_cb->bass_data[0];
    for (int index = 0; index < LEHS_BROADCAST_RCV_STATE_MAX; index++, p_bass++)
    {
        if (p_bass->is_used && (p_bass->conn_id == conn_id))
        {
            WICED_BT_TRACE("[%s] conn id %x source id %d\n",
                           __FUNCTION__,
                           p_bass->conn_id,
                           p_bass->recv_state.source_id);
            memset(&p_bass->recv_state, 0, sizeof(p_bass->recv_state));
            WICED_MEMSET(p_bass, 0, sizeof(lehs_bass_data_t));
        }
    }
}
