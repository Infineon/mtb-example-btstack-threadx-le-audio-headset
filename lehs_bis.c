/*
 * $ Copyright Cypress Semiconductor $
 */

#include "audio_driver.h"
#include "lehs.h"

void broadcast_sink_create_periodic_sync(wiced_ble_ext_scan_results_t *p_scan_result);

lehs_broadcast_sink_cb_t *lehs_get_broadcast_sink_cb(void)
{
    return &g_lehs_gatt_cb.broadcast_sink_cb[0];
}

broadcast_source_t *lehs_get_broadcast_source_cb(void)
{
    return &g_lehs_gatt_cb.broadcast_source;
}

wiced_bool_t lehs_broadcast_get_sync_progress(void)
{
    return g_lehs_gatt_cb.broadcast_sink_periodic_sync_in_progress;
}

void lehs_broadcast_set_sync_progress(wiced_bool_t progress)
{
    g_lehs_gatt_cb.broadcast_sink_periodic_sync_in_progress = progress;
}

lehs_broadcast_sink_cb_t *lehs_bis_get_big_by_handle(uint8_t big_handle)
{
    lehs_broadcast_sink_cb_t *p_cb = lehs_get_broadcast_sink_cb();

    for (size_t i = 0; i < LEHS_MAX_BIG; i++, p_cb++)
    {
        if (TRUE == p_cb->in_use && big_handle == p_cb->big_handle)
        {
            return p_cb;
        }
    }

    return NULL;
}

lehs_broadcast_sink_cb_t *lehs_bis_get_big_by_broadcast_id(uint32_t br_id)
{
    lehs_broadcast_sink_cb_t *p_cb = lehs_get_broadcast_sink_cb();

    for (size_t i = 0; i < LEHS_MAX_BIG; i++, p_cb++)
    {
        if (TRUE == p_cb->in_use && br_id == p_cb->base.broadcast_id)
        {
            return p_cb;
        }
    }

    return NULL;
}

static lehs_broadcast_sink_cb_t *broadcast_sink_handle_periodic_sync_established_event(
    wiced_ble_padv_sync_established_event_data_t *p_sync_estb)
{
    lehs_broadcast_sink_cb_t *p_cb = lehs_get_broadcast_sink_cb();

    WICED_BT_TRACE("[%s] [addr:%B] [adv_sid:%d] \n", __FUNCTION__, p_sync_estb->adv_addr, p_sync_estb->adv_sid);

    for (size_t i = 0; i < LEHS_MAX_BIG; i++, p_cb++)
    {
        if (p_cb->in_use && p_cb->pa_sync_in_progress && (p_cb->adv_handle == p_sync_estb->adv_sid))
        {
            p_cb->pa_sync_in_progress = FALSE;
            break;
        }
    }

    if (p_cb && (p_sync_estb->status == WICED_BT_SUCCESS))
    {
        p_cb->sync_handle = p_sync_estb->sync_handle;
        p_cb->sync_state = HCI_CONTROL_LEA_BROADCAST_PA_SYNC_ESTABLISHED;
        le_audio_rpc_send_broadcast_status_update(p_cb->sync_state);
    }
    return p_cb;
}

lehs_broadcast_sink_cb_t *lehs_bis_get_big_by_sync_handle(wiced_ble_padv_sync_handle_t sync_handle)
{
    lehs_broadcast_sink_cb_t *p_cb = lehs_get_broadcast_sink_cb();

    for (size_t i = 0; i < LEHS_MAX_BIG; i++, p_cb++)
    {
        if (TRUE == p_cb->in_use && sync_handle == p_cb->sync_handle)
        {
            return p_cb;
        }
    }

    return NULL;
}

void priv_periodic_adv_data_report_handler(wiced_ble_padv_report_event_data_t *p_epadv)
{
    lehs_broadcast_sink_cb_t *p_big = NULL;
    wiced_ble_padv_sync_handle_t sync_handle = p_epadv->sync_handle;
    uint8_t base_len = 0;
    uint8_t *p_base_stream = NULL;

    p_big = lehs_bis_get_big_by_sync_handle(sync_handle);
    WICED_BT_TRACE("[%s] padv %x %d sync 0x%x ds 0x%x p_big 0x%x %d\n",
                   __FUNCTION__,
                   p_epadv->p_data,
                   p_epadv->data_length,
                   sync_handle,
                   p_epadv->data_status,
                   p_big,
                   p_big ? p_big->b_base_updated : 0);
    // check if the report is processed already
    /* received incomplete packet, discard the report */
    if (!p_big || p_big->b_base_updated || (p_epadv->data_status != 0))
    {
        return;
    }

    /* Discard if the report is not Basic Announcement  */
    p_base_stream = le_audio_bap_broadcast_is_basic_announcement(p_epadv->p_data, p_epadv->data_length, &base_len);
    if (!p_base_stream)
    {
        return;
    }

    WICED_BT_TRACE("[%s] p_base_stream 0x%x [%d bytes]\n", __FUNCTION__, p_base_stream, base_len);

    // parse the received BASE info and save (adding 4 bytes to skip LTV which is already parsed)
    le_audio_bap_broadcast_parse_base_info(p_base_stream, base_len, &p_big->base);

    p_big->b_base_updated = TRUE;
    p_big->base.state = BAP_BROADCAST_STATE_CONFIGURED;
    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_BIS_INFO, p_base_stream, base_len);
}

void priv_big_adv_report_handler(wiced_ble_ext_adv_event_t event, wiced_ble_biginfo_adv_report_t *p_bigrpt)
{
    lehs_broadcast_sink_cb_t *p_big = NULL;
    wiced_ble_padv_sync_handle_t sync_handle = p_bigrpt->sync_handle;

    p_big = lehs_bis_get_big_by_sync_handle(sync_handle);
    /* Discard if BASE is not parsed yet or BIGInfo is already updated*/
    WICED_BT_TRACE("[%s] sync 0x%x p_big 0x%x %d %d",
                   __FUNCTION__,
                   sync_handle,
                   p_big,
                   p_big ? p_big->b_base_updated : 0,
                   p_big ? p_big->big_sync_in_progress : -1);
    if (!p_big || (!p_big->b_base_updated) || (p_big->big_sync_in_progress))
    {
        WICED_BT_TRACE("[%s] p_big is NULL\n", __FUNCTION__, sync_handle);
        return;
    }

    // check if the report is processed already
    {
        broadcast_source_t *p_src_cb = lehs_get_broadcast_source_cb();

        /* Enable encryption if stream is encrypted */
        p_big->b_encryption = p_bigrpt->encryption;
        p_big->number_of_subevents = p_bigrpt->number_of_subevents;
        p_big->iso_interval = p_bigrpt->iso_interval;
        p_big->b_biginfo_updated = TRUE;

        if (p_src_cb->bis_index_bits)
        {
            if (p_big->b_encryption)
            {
                WICED_MEMCPY(p_big->broadcast_code, p_src_cb->broadcast_code, BAP_BROADCAST_CODE_SIZE);
            }
            lehs_sync_to_source(p_big, p_src_cb->bis_index_bits);
            WICED_MEMSET(p_src_cb, 0, sizeof(broadcast_source_t));
        }
        else
        {
            lehs_bass_process_big_adv_report_and_sync(p_big, p_bigrpt);
        }
    }

    return;
}

//extern uint8_t g_adv_duty_state;
void lehs_ext_adv_cback(wiced_ble_ext_adv_event_t event, wiced_ble_ext_adv_event_data_t *p_ed)
{
    lehs_broadcast_sink_cb_t *p_big = NULL;

    WICED_BT_TRACE("[%s] event [%d]", __FUNCTION__, event);

    switch (event)
    {
    case WICED_BLE_SET_PERIODIC_ADV_SYNC_TRANSFER_PARAM_EVENT:
    {
        WICED_BT_TRACE("[%s] status %d conn handle %d",
                       __FUNCTION__,
                       p_ed->sync_transfer_param.status,
                       p_ed->sync_transfer_param.conn_handle);

        if (p_ed->sync_transfer_param.status)
        {
            return;
        }

        lehs_bass_handle_pa_sync_transfer_param_evt(&p_ed->sync_transfer_param);
    }
    break;
    case WICED_BLE_PERIODIC_ADV_SYNC_TRANSFER_EVENT:
    {
        wiced_ble_padv_sync_established_event_data_t *p_sync_data = &p_ed->sync_transfer.sync_data;
        WICED_BT_TRACE("[%s] WICED_BLE_PERIODIC_ADV_SYNC_TRANSFER_EVENT adv sid %d sync_handle %d BDA %B",
                       __FUNCTION__,
                       p_sync_data->adv_sid,
                       p_sync_data->sync_handle,
                       p_sync_data->adv_addr);

        lehs_bass_handle_periodic_sync_transfer_evt(p_sync_data);
    }
    break;

    case WICED_BLE_PERIODIC_ADV_SYNC_ESTABLISHED_EVENT:
    {
        wiced_ble_padv_sync_established_event_data_t *p_sync_estb = &p_ed->sync_establish;
        wiced_ble_ext_scan_results_t *p_ext_adv_report = NULL;
        wiced_ble_ext_scan_results_t ext_adv_report = {0};
        WICED_BT_TRACE("[%s] PERIODIC_ADV_SYNC_ESTABLISHED status 0x%x sync_handle %d BDA %B",
                       __FUNCTION__,
                       p_sync_estb->status,
                       p_sync_estb->sync_handle,
                       p_sync_estb->adv_addr);

        p_big = broadcast_sink_handle_periodic_sync_established_event(p_sync_estb);

        /* Handle error status */
        if (p_sync_estb->status == WICED_SUCCESS)
        {
            wiced_ble_padv_alloc_segment_assembler(p_ed->sync_establish.sync_handle, 255);
        }
        else if (p_sync_estb->status == 0x3E) //Synchronization Timeout
        {
            p_ext_adv_report = &ext_adv_report;
            ext_adv_report.adv_sid = p_sync_estb->adv_sid;
            ext_adv_report.ble_addr_type = p_sync_estb->adv_addr_type;
            memcpy(ext_adv_report.remote_bd_addr, p_sync_estb->adv_addr, WICED_BT_ADDRESS_BYTE_SIZE);
            ext_adv_report.periodic_adv_interval = p_sync_estb->periodic_adv_int;
        }
        lehs_bass_notify_pa_sync_state(p_big, p_sync_estb->status ? GA_LIB_BASS_PA_FAILED_SYNC: GA_LIB_BASS_PA_SYNC);

        lehs_broadcast_set_sync_progress(FALSE);
        wiced_bt_dev_status_t status = wiced_ble_padv_remove_device_from_list(p_ed->sync_establish.adv_addr_type,
                                                                              p_ed->sync_establish.adv_addr,
                                                                              p_ed->sync_establish.adv_sid);
        WICED_BT_TRACE("[%s] Remove device from list status %d", __FUNCTION__, status);

        /* Issue further sync commands if pending */
        broadcast_sink_create_periodic_sync(p_ext_adv_report);
    }
    break;

    case WICED_BLE_PERIODIC_ADV_REPORT_EVENT:
        priv_periodic_adv_data_report_handler(&p_ed->periodic_adv_report);
        break;

    case WICED_BLE_BIGINFO_ADV_REPORT_EVENT:
        priv_big_adv_report_handler(event, &p_ed->biginfo_adv_report);
        break;

    case WICED_BLE_PERIODIC_ADV_SYNC_LOST_EVENT:
        p_big = lehs_bis_get_big_by_sync_handle(p_ed->sync_handle);
        if (p_big)
        {
            p_big->sync_state = HCI_CONTROL_LEA_BROADCAST_PA_SYNC_LOST;
            lehs_bass_notify_pa_sync_state(p_big, GA_LIB_BASS_PA_NOT_SYNC);
        }
        le_audio_rpc_send_broadcast_status_update(p_ed->sync_handle);
        break;

    case WICED_BLE_ADV_SET_TERMINATED_EVENT:
    {
        WICED_BT_TRACE("[%s] event %d adv_state %d ", __FUNCTION__, event, g_lehs_gatt_cb.adv_state);
        if (p_ed->adv_set_terminated.conn_handle)
        {
            g_lehs_gatt_cb.adv_state = ADV_STATE_IDLE;
            lehs_gatt_start_stop_adv(0, g_lehs_gatt_cb.adv_state);
        }
        else
        {
            lehs_move_to_next_adv_state(g_lehs_gatt_cb.adv_state, "adv_terminated");
        }
    }
    break;

    default:
        break;
    }
}

void lehs_bis_isoc_cb(wiced_ble_isoc_event_t event, wiced_ble_isoc_event_data_t *p_ed)
{
    lehs_broadcast_sink_cb_t *p_big = NULL;
    wiced_ble_isoc_terminated_evt_t *p_big_sync_lost = NULL;
    wiced_ble_isoc_big_sync_established_evt_t *p_big_sync_established = &p_ed->big_sync_established;

    WICED_BT_TRACE("[%s] event %d ", __FUNCTION__, event);

    switch (event)
    {
    case WICED_BLE_ISOC_BIG_SYNC_ESTABLISHED_EVT:
    {
        if (p_big_sync_established->status)
            return;

        p_big = lehs_bis_get_big_by_handle(p_big_sync_established->big_handle);
        if (p_big == NULL)
            return;

        p_big->sync_state = HCI_CONTROL_LEA_BROADCAST_BIG_SYNC_ESTABLISHED;
        le_audio_rpc_send_broadcast_status_update(p_big->sync_state);

        // Stop any scan operations
        wiced_ble_ext_scan_enable(0, NULL);
        p_big->big_sync_in_progress = WICED_FALSE;

        wiced_ble_padv_terminate_sync(p_big->sync_handle);

        memcpy(p_big->bis_conn_id_list,
               p_big_sync_established->bis_conn_hdl_list,
               p_big_sync_established->num_bis * sizeof(uint16_t));

        p_big->bis_conn_id_count = p_big_sync_established->num_bis;

        lehs_isoc_dhm_setup_bis_stream(p_big, p_big_sync_established->num_bis);
        lehs_bass_notify_big_sync_state(p_big);
    }
    break;

    case WICED_BLE_ISOC_BIG_SYNC_LOST_EVT:
    {
        p_big_sync_lost = &p_ed->big_sync_lost;
        p_big = lehs_bis_get_big_by_handle(p_big_sync_lost->big_handle);
        if (p_big == NULL)
            return;

        p_big->sync_state = HCI_CONTROL_LEA_BROADCAST_BIG_SYNC_LOST;
        lehs_isoc_dhm_free_bis_stream(p_big->bis_conn_id_list, p_big->bis_conn_id_count);
        lehs_bass_notify_big_sync_state(p_big);
        lehs_bis_free_big(p_big);
        WICED_BT_TRACE("[%s] BASE State [%d] \n", __FUNCTION__, p_big->base.state);
    }
    break;
    case WICED_BLE_ISOC_BIG_TERMINATED_SYNC_EVT:
        WICED_BT_TRACE("[%s] status %d sync handle %d",
                       __FUNCTION__,
                       p_ed->big_sync_terminated.status,
                       p_ed->big_sync_terminated.big_handle);

        p_big = lehs_bis_get_big_by_handle(p_ed->big_sync_terminated.big_handle);
        if (p_big == NULL)
            return;

        p_big->sync_state = HCI_CONTROL_LEA_BROADCAST_BIG_SYNC_TERMINATED;
        le_audio_rpc_send_broadcast_status_update(p_big->sync_state);
        lehs_bass_notify_big_sync_state(p_big);
        lehs_bis_free_big(p_big);
        break;

    default:
        break;
    }
}

lehs_broadcast_sink_cb_t *lehs_bis_alloc_big(uint32_t broadcast_id, wiced_bt_device_address_t bd_addr, uint8_t adv_sid)
{
    lehs_broadcast_sink_cb_t *p_cb = lehs_get_broadcast_sink_cb();
    lehs_broadcast_sink_cb_t *p_big = NULL;

    /* If BIG already exists do not allocate a new one */
    p_big = lehs_bis_get_big_by_broadcast_id(broadcast_id);
    if (p_big)
        return p_big;

    /* find a free slot */
    for (size_t i = 0; i < LEHS_MAX_BIG; i++, p_cb++)
    {
        if (TRUE != p_cb->in_use)
        {
            p_big = p_cb;
            break;
        }
    }

    if (!p_big)
    {
        return p_big;
    }

    p_big->in_use = TRUE;
    p_big->base.broadcast_id = broadcast_id;
    p_big->adv_handle = adv_sid;
    /* Adv. set ID will be unique per BASE, so it should be ok to use the same as BIG handle */
    p_big->big_handle = adv_sid;
    memcpy(p_big->bd_addr, bd_addr, BD_ADDR_LEN);

    p_big->sync_handle = 0xFF;
    p_big->base.state = BAP_BROADCAST_STATE_IDLE;
    p_big->b_encryption = FALSE;

    WICED_BT_TRACE("[%s] Initializing [State:%d] [br_id:0x%x] [adv_sid:%d] [sync_handle:0xFF]\n",
                   __FUNCTION__,
                   p_big->base.state,
                   p_big->base.broadcast_id,
                   adv_sid);

    return p_big;
}

void lehs_bis_free_big(lehs_broadcast_sink_cb_t *p_big)
{
    memset(p_big, 0, sizeof(lehs_broadcast_sink_cb_t));
}

void broadcast_sink_create_periodic_sync(wiced_ble_ext_scan_results_t *p_ext_adv_report)
{
    /* Queue to hold "sync to periodic adv" requests (since controller rejects more than one request at a time) */
    static wiced_bt_buffer_q_t broadcast_sink_pending_scan_q = {0};
    wiced_ble_ext_scan_results_t *p_cached_scan_result = NULL;
    uint16_t sync_timeout = 0;

    WICED_BT_TRACE("[%s] %x", __FUNCTION__, p_ext_adv_report);

    /* If previous sync is pending still add this req to queue else controller will reject the command */
    if (lehs_broadcast_get_sync_progress() && p_ext_adv_report)
    {
        wiced_ble_ext_scan_results_t *p_ext_adv_report_cpy =
            (wiced_ble_ext_scan_results_t *)wiced_bt_get_buffer(sizeof(wiced_ble_ext_scan_results_t));
        if (!p_ext_adv_report_cpy)
        {
            WICED_BT_TRACE_CRIT("[%s] memory allocation failed !", __FUNCTION__);
            return;
        }

        *p_ext_adv_report_cpy = *p_ext_adv_report;

        /* add the request to queue */
        wiced_bt_enqueue(&broadcast_sink_pending_scan_q, p_ext_adv_report_cpy);
        WICED_BT_TRACE("[%s] Queuing periodic adv sync request\n", __FUNCTION__);
        return;
    }

    if (!p_ext_adv_report)
    {
        /* get pending sync request from queue */
        p_cached_scan_result = (wiced_ble_ext_scan_results_t *)wiced_bt_dequeue(&broadcast_sink_pending_scan_q);
        if (!p_cached_scan_result)
        {
            lehs_broadcast_set_sync_progress(FALSE);

            // Stop any scan operations
            wiced_ble_ext_scan_enable(0, NULL);
            return;
        }

        p_ext_adv_report = p_cached_scan_result;
    }

    WICED_BT_TRACE("[%s] Trying to sync to periodic adv\n", __FUNCTION__);
    /* The Sync timeout should at least be 6 times the interval to accommodate for 6 opportunities to catch the peer. */
    sync_timeout = (p_ext_adv_report->periodic_adv_interval * 3) / 4;
    {
        wiced_ble_padv_create_sync_params_t csp;
        csp.options = WICED_BLE_PADV_CREATE_SYNC_OPTION_USE_PA_LIST;
        csp.adv_sid = p_ext_adv_report->adv_sid;
        csp.adv_addr_type = p_ext_adv_report->ble_addr_type;
        WICED_MEMCPY(csp.adv_addr, p_ext_adv_report->remote_bd_addr, BD_ADDR_LEN);
        csp.skip = 0;
        csp.sync_timeout = sync_timeout;
        csp.sync_cte_type = 0;

         wiced_bt_dev_status_t status = wiced_ble_padv_add_device_to_list(p_ext_adv_report->ble_addr_type,
                                                                         p_ext_adv_report->remote_bd_addr,
                                                                         p_ext_adv_report->adv_sid);
        WICED_BT_TRACE("[%s] Add device to periodic adv list status %d\n", __FUNCTION__, status);
        wiced_ble_padv_create_sync(&csp);
    }

    lehs_broadcast_set_sync_progress(TRUE);

    if (p_cached_scan_result)
        wiced_bt_free_buffer(p_cached_scan_result);
}

void lehs_rpc_send_new_stream_info(uint32_t broadcast_id, const uint8_t *broadcast_name)
{
    size_t name_len = 0;
    uint8_t tx_buff[MAX_BROADCAST_NAME_SIZE + 5];
    uint8_t *p = tx_buff;
    UINT32_TO_STREAM(p, broadcast_id);

    if (!broadcast_name)
        broadcast_name = (uint8_t *)"UNKNOWN SOURCE";
    WICED_BT_TRACE("[%s] broadcastid [%x] source_name %s\n", __FUNCTION__, broadcast_id, broadcast_name);

    name_len = strlen((char *)broadcast_name);
    UINT8_TO_STREAM(p, name_len);
    ARRAY_TO_STREAM(p, broadcast_name, name_len);

    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_BROADCAST_STREAM_RSP, tx_buff, (int)(p - tx_buff));
}

void broadcast_sink_bis_ext_adv_scan_cback(wiced_ble_ext_scan_results_t *p_scan_result,
                                           uint16_t adv_len,
                                           uint8_t *p_adv_data)
{
    uint32_t br_id = 0;
    uint16_t adv_evt_type = (p_scan_result) ? p_scan_result->ext_evt_type : 0xffff;

    WICED_BT_TRACE("[%s] type 0x%x len %d\n", __FUNCTION__, adv_evt_type, adv_len);

    if (!p_scan_result || p_scan_result->periodic_adv_interval == 0)
    {
        return;
    }

    if (0 != (p_scan_result->ext_evt_type >> 5))
    {

        return;
    }

    if (!le_audio_bap_broadcast_is_broadcast_announcement(adv_len, p_adv_data, &br_id))
    {
        return;
    }
    le_audio_rcv_public_broadcast_t p_rcv_br;
    if (le_audio_pbp_is_public_broadcast(adv_len, p_adv_data, &p_rcv_br))
    {
        WICED_BT_TRACE("%s", p_rcv_br.broadcast_name);
        WICED_BT_TRACE("%d", p_rcv_br.audio_config);
        WICED_BT_TRACE("%d", p_rcv_br.encryption);
        WICED_BT_TRACE("%d", p_rcv_br.metadata_length);
        WICED_BT_TRACE("%d", p_rcv_br.source_appearance_value);
    }

    WICED_BT_TRACE("[%s] broadcast id found %x\n", __FUNCTION__, br_id);

    lehs_rpc_send_new_stream_info(br_id, p_rcv_br.broadcast_name);
}

void broadcast_sink_bis_ext_adv_scan_to_sync_cback(wiced_ble_ext_scan_results_t *p_scan_result,
                                                   uint16_t adv_len,
                                                   uint8_t *p_adv_data)
{
    uint32_t br_id = 0;
    lehs_broadcast_sink_cb_t *p_big = NULL;
    broadcast_source_t *p_src_cb = lehs_get_broadcast_source_cb();
    uint16_t adv_evt_type = (p_scan_result) ? p_scan_result->ext_evt_type : 0xffff;

    WICED_BT_TRACE("[%s] type 0x%x len %d\n", __FUNCTION__, adv_evt_type, adv_len);

    if (!p_scan_result || p_scan_result->periodic_adv_interval == 0)
    {
        return;
    }

    if (!le_audio_bap_broadcast_is_broadcast_announcement(adv_len, p_adv_data, &br_id))
    {
        return;
    }
    le_audio_rcv_public_broadcast_t p_rcv_br;

    if (le_audio_pbp_is_public_broadcast(adv_len, p_adv_data, &p_rcv_br))
    {

        WICED_BT_TRACE("%s", p_rcv_br.broadcast_name);
        WICED_BT_TRACE("%d", p_rcv_br.audio_config);
        WICED_BT_TRACE("%d", p_rcv_br.encryption);
        WICED_BT_TRACE("%d", p_rcv_br.metadata_length);
        WICED_BT_TRACE("%d", p_rcv_br.source_appearance_value);
    }
    if (br_id != p_src_cb->broadcast_id)
    {
        WICED_BT_TRACE("[%s] broadcast id found %x looking for %x \n", __FUNCTION__, br_id, p_src_cb->broadcast_id);
        return;
    }

    // check for duplicate reports
    //TODO: Check for addr and br_id ?
    p_big = lehs_bis_get_big_by_broadcast_id(br_id);
    if (p_big == NULL)
    {
        // Alloc a slot to store BASE, do not sync to PA if unsuccessful
        p_big = lehs_bis_alloc_big(br_id, p_scan_result->remote_bd_addr, p_scan_result->adv_sid);
    }
    if (!p_big)
        return;

    WICED_BT_TRACE("[%s] sync_handle %d progress %d \n", __FUNCTION__, p_big->sync_handle, p_big->big_sync_in_progress);

    if ((0xFF == p_big->sync_handle) && (p_big->big_sync_in_progress == WICED_FALSE))
    {
        p_big->pa_sync_in_progress = TRUE;
        broadcast_sink_create_periodic_sync(p_scan_result);
        wiced_ble_ext_scan_register_cb(NULL);
    }
}

void lehs_sync_to_source(lehs_broadcast_sink_cb_t *p_big, uint32_t bis_index_bits)
{
    wiced_ble_isoc_big_create_sync_t create_sync = {0};
    if (!p_big)
        return;
    create_sync.big_handle = p_big->big_handle;
    create_sync.sync_handle = p_big->sync_handle;
    create_sync.max_sub_events = p_big->number_of_subevents;
    create_sync.big_sync_timeout = p_big->iso_interval * 10;
    create_sync.bis_idx_list = p_big->bis_index_list;

    uint8_t index = 0;
    for (int i = 0; i < p_big->base.sub_group[0].bis_cnt; i++)
    {
        if (bis_index_bits & 1 << (p_big->base.sub_group[0].bis_config[i].bis_idx - 1))
        {
            create_sync.bis_idx_list[index] = p_big->base.sub_group[0].bis_config[i].bis_idx;
            index++;
        }
    }

    create_sync.num_bis = index;
    create_sync.encrypt = p_big->b_encryption;
    if (p_big->b_encryption)
    {
        memcpy(create_sync.broadcast_code, p_big->broadcast_code, sizeof(create_sync.broadcast_code));
    }
    p_big->base.state = BAP_BROADCAST_STATE_STREAMING;
    WICED_BT_TRACE("[%s] calling wiced_ble_isoc_peripheral_big_create_sync\n", __FUNCTION__);
    wiced_result_t res = wiced_ble_isoc_peripheral_big_create_sync(&create_sync);
    WICED_BT_TRACE("[%s] result %x\n", __FUNCTION__, res);
    WICED_BT_TRACE("[%s] event sync handle[%d]\n", __FUNCTION__, p_big->sync_handle);
    p_big->big_sync_in_progress = WICED_TRUE;
}

const wiced_ble_ext_scan_params_t scan_params_default = {
    .own_addr_type = BLE_ADDR_PUBLIC,
    .scanning_phys = WICED_BLE_EXT_ADV_PHY_1M_BIT,
    .scan_filter_policy = WICED_BLE_SCAN_BASIC_UNFILTERED_SP,
    .sp_1m.scan_type = BTM_BLE_SCAN_MODE_PASSIVE,
    .sp_1m.scan_interval = WICED_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_INTERVAL,
    .sp_1m.scan_window = WICED_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_WINDOW};

const wiced_ble_ext_scan_enable_params_t scan_enable_params = {
    .filter_duplicates = 0, .scan_period = 0, .scan_duration = 0};

void lehs_bis_discover_sources(uint8_t start)
{
    wiced_ble_ext_scan_params_t scan_params = scan_params_default;
    wiced_ble_ext_scan_enable_params_t scan_enable = scan_enable_params;
    if (!start)
    {
        wiced_ble_ext_scan_enable(0, NULL);
        return;
    }
    wiced_ble_ext_scan_register_cb(broadcast_sink_bis_ext_adv_scan_cback);
    wiced_ble_ext_scan_configure_reassembly(255, 5);
    scan_params.sp_1m.scan_type = BTM_BLE_SCAN_MODE_ACTIVE;
    wiced_ble_ext_scan_set_params(&scan_params);
    wiced_ble_ext_scan_enable(1, &scan_enable);
}

void lehs_bis_sync_to_source(broadcast_source_t source)
{
    broadcast_source_t *p_src_cb = lehs_get_broadcast_source_cb();
    WICED_MEMSET(p_src_cb, 0, sizeof(broadcast_source_t));
    memcpy(p_src_cb, &source, sizeof(broadcast_source_t));

    lehs_broadcast_sink_cb_t *p_big = lehs_bis_get_big_by_broadcast_id(p_src_cb->broadcast_id);
    if (!p_big)
    {
        lehs_sync_to_pa(source.broadcast_id);
        return;
    }
    if (!p_big->b_biginfo_updated || !p_big->b_base_updated)
    {
        return;
    }
    if (p_big->b_encryption)
    {
        WICED_MEMCPY(p_big->broadcast_code, p_src_cb->broadcast_code, BAP_BROADCAST_CODE_SIZE);
    }
    lehs_sync_to_source(p_big, p_src_cb->bis_index_bits);
    WICED_MEMSET(p_src_cb, 0, sizeof(broadcast_source_t));
}

void lehs_sync_to_pa(uint32_t broadcast_id)
{
    wiced_ble_ext_scan_params_t scan_params = scan_params_default;

    broadcast_source_t *p_src_cb = lehs_get_broadcast_source_cb();
    p_src_cb->broadcast_id = broadcast_id;
    lehs_broadcast_sink_cb_t *p_big = lehs_bis_get_big_by_broadcast_id(broadcast_id);
    if (p_big && p_big->b_base_updated)
    {
        WICED_BT_TRACE("[%s] Already synced to PA", __FUNCTION__);
        return;
    }

    // Make sure to stop previous scan
    wiced_ble_ext_scan_enable(0, NULL);

    wiced_ble_ext_scan_register_cb(broadcast_sink_bis_ext_adv_scan_to_sync_cback);
    wiced_ble_ext_scan_configure_reassembly(255, 5);
    scan_params.sp_1m.scan_type = BTM_BLE_SCAN_MODE_ACTIVE;
    wiced_ble_ext_scan_set_params(&scan_params);
    wiced_ble_ext_scan_enable(1, &scan_enable_params);
}

void lehs_bis_terminate_sync(uint32_t broadcast_id)
{
    lehs_broadcast_sink_cb_t *p_big = lehs_bis_get_big_by_broadcast_id(broadcast_id);
    if (p_big == NULL)
    {
        WICED_BT_TRACE("[%s] p_big is null", __FUNCTION__);
        return;
    }

    lehs_isoc_dhm_free_bis_stream(p_big->bis_conn_id_list, p_big->bis_conn_id_count);
    if ((p_big->sync_state == HCI_CONTROL_LEA_BROADCAST_BIG_SYNC_ESTABLISHED) || p_big->big_sync_in_progress)
        wiced_ble_isoc_peripheral_big_terminate_sync(p_big->big_handle);
    if (p_big->sync_state == HCI_CONTROL_LEA_BROADCAST_PA_SYNC_ESTABLISHED)
    {
        wiced_ble_padv_terminate_sync(p_big->sync_handle);
        lehs_bis_free_big(p_big);
    }
    if (lehs_broadcast_get_sync_progress())
    {
        wiced_ble_padv_cancel_sync();
        lehs_bis_free_big(p_big);
    }

    broadcast_source_t *p_src_cb = lehs_get_broadcast_source_cb();
    WICED_MEMSET(p_src_cb, 0, sizeof(broadcast_source_t));
}

void broadcast_sink_clear_data()
{
    lehs_broadcast_sink_cb_t *p_cb = lehs_get_broadcast_sink_cb();

    WICED_BT_TRACE("[%s] \n", __FUNCTION__);
    while (p_cb)
    {
        if (p_cb->in_use == TRUE)
        {
            WICED_BT_TRACE("[%s]terminating sync to  %B big handle %d\n",
                           __FUNCTION__,
                           p_cb->bd_addr,
                           p_cb->big_handle);

            if ((p_cb->sync_state == HCI_CONTROL_LEA_BROADCAST_BIG_SYNC_ESTABLISHED) || p_cb->big_sync_in_progress)
            {
                wiced_ble_isoc_peripheral_big_terminate_sync(p_cb->big_handle);
                lehs_isoc_dhm_free_bis_stream(p_cb->bis_conn_id_list, p_cb->bis_conn_id_count);
            }
            if (p_cb->sync_state == HCI_CONTROL_LEA_BROADCAST_PA_SYNC_ESTABLISHED)
                wiced_ble_padv_terminate_sync(p_cb->sync_handle);
            if (lehs_broadcast_get_sync_progress())
                wiced_ble_padv_cancel_sync();
        }
        memset(p_cb, 0, sizeof(lehs_broadcast_sink_cb_t));
        p_cb++;
    }

    lehs_broadcast_set_sync_progress(FALSE);
}
