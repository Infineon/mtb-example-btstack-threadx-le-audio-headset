/*
* $ Copyright Cypress Semiconductor $
*/

#include "lehs.h"

lehs_ase_data_t *lehs_find_ase_with_cis_id(uint8_t cig_id, uint8_t cis_id, uint8_t char_type, uint16_t *p_conn_id)
{
    lehs_clcb_t *p_clcb = g_lehs_gatt_cb.clcb;
    int limit = sizeof(g_lehs_gatt_cb.clcb) / sizeof(g_lehs_gatt_cb.clcb[0]);
    lehs_ase_data_t *p_lehs_ase = NULL;
    int num_ase = 0;

    while (limit--)
    {
        p_lehs_ase = &p_clcb->local_ase_data[0];

        num_ase = LEHS_ASE_INDEX_MAX;
        for (; num_ase--; p_lehs_ase++)
        {
            ga_lib_ascs_ase_t *p_ase = &p_lehs_ase->ase;
            ga_lib_ascs_config_qos_args_t *p_qos = &p_ase->qos_configured;
            if (p_ase->ase_state < GA_LIB_ASCS_STATE_QOS_CONFIGURED)
            {
                continue;
            }
            if ((p_qos->cig_id == cig_id) && (p_qos->cis_id == cis_id) && (char_type == p_ase->ase_type))
            {
                if (p_conn_id)
                {
                    *p_conn_id = p_clcb->conn_id;
                }
                return p_lehs_ase;
            }
        }
        p_clcb++;
    }

    return NULL;
}

lehs_ase_data_t *lehs_get_ase_app_data_ptr_by_cis_conn_hdl(uint16_t cis_conn_hdl, uint16_t *p_conn_id)
{
    lehs_clcb_t *p_clcb = g_lehs_gatt_cb.clcb;
    int limit = sizeof(g_lehs_gatt_cb.clcb) / sizeof(g_lehs_gatt_cb.clcb[0]);
    lehs_ase_data_t *p_lehs_ase = NULL;
    int num_ase = 0;
    while (limit--)
    {
        p_lehs_ase = &p_clcb->local_ase_data[0];

        num_ase = LEHS_ASE_INDEX_MAX;
        for (; num_ase--; p_lehs_ase++)
        {
            ga_lib_ascs_ase_t *p_ase = &p_lehs_ase->ase;
            ga_lib_ascs_config_qos_args_t *p_qos = &p_ase->qos_configured;
            uint16_t cis_conn_hdl =
                wiced_ble_isoc_get_cis_conn_handle(p_qos->cig_id, p_qos->cis_id, p_lehs_ase->acl_conn_handle);

            if (p_ase->ase_state < GA_LIB_ASCS_STATE_QOS_CONFIGURED)
            {
                continue;
            }
            if (p_lehs_ase->cis_conn_handle == cis_conn_hdl)
            {
                if (p_conn_id)
                {
                    *p_conn_id = p_clcb->conn_id;
                }
                return p_lehs_ase;
            }
        }
        p_clcb++;
    }

    return NULL;
}

static void lehs_cis_handle_connection(wiced_ble_isoc_cis_t *p_cis, uint8_t char_type)
{
    wiced_result_t res = WICED_ERROR;
    uint16_t conn_id;
    lehs_ase_data_t *p_lehs_ase = lehs_find_ase_with_cis_id(p_cis->cig_id, p_cis->cis_id, char_type, &conn_id);
    ga_lib_ascs_ase_t *p_ase = &p_lehs_ase->ase;

    CHECK_FOR_NULL_AND_RETURN(p_lehs_ase);

    WICED_BT_TRACE("[%s] ase_id %d, state : %d char_type %d",
                   __FUNCTION__,
                   p_ase->ase_id,
                   p_ase->ase_state,
                   p_ase->ase_type);

    //assign CIS to ASE
    p_lehs_ase->cis_conn_handle = p_cis->cis_conn_handle;

    if (char_type == GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE && p_ase->ase_state == GA_LIB_ASCS_STATE_ENABLING)
        res = lehs_isoc_dhm_setup_cis_stream(p_lehs_ase);
    else if (char_type == GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE && p_ase->ase_state == GA_LIB_ASCS_STATE_STREAMING)
        res = lehs_isoc_dhm_setup_cis_stream(p_lehs_ase);

    if (res)
    {
        WICED_BT_TRACE_CRIT("[%s] data path setup unsuccessful...(err:%d)\n", __FUNCTION__, res);
    }
}

static void lehs_cis_handle_data_path_setup(uint16_t cis_conn_hdl, lehs_ase_data_t *p_lehs_ase)
{
    uint16_t conn_id = 0;
    uint8_t ase_type;

    if (!lehs_get_ase_app_data_ptr_by_cis_conn_hdl(cis_conn_hdl, &conn_id))
    {
        WICED_BT_TRACE("[%s] did not get conn_id for 0x%x ", __FUNCTION__, cis_conn_hdl);
        return;
    }
    ga_lib_ascs_ase_t *p_ase = &p_lehs_ase->ase;
    ase_type = p_ase->ase_type;
    WICED_BT_TRACE("[%s] ase_type %s \n", __FUNCTION__, (ase_type == GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE) ? "SNK" : "SRC");

    // Update ASE data to indicate data path is setup successfully
    p_lehs_ase->data_path_established = 1;

    lehs_isoc_dhm_start_stream(cis_conn_hdl, p_ase->ase_type);
    // if server + source and in streaming state, start audio streaming
    // if server + sink and in enabling state, transition to streaming state
    if (GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE == ase_type && GA_LIB_ASCS_STATE_ENABLING == p_ase->ase_state)
    {
        p_ase->ase_state = GA_LIB_ASCS_STATE_STREAMING;
        ga_lib_ascs_notify_ase_state(conn_id, p_lehs_ase->gatt_ase_value_handle, p_ase);
    }
    else
    {
        WICED_BT_TRACE_CRIT("[%s] sr unexpected state %d char_type %d\n",
                            __FUNCTION__,
                            p_ase->ase_state,
                            ase_type);
    }
}

static void lehs_cis_handle_disconnection(uint8_t cig_id, uint8_t cis_id, uint8_t char_type)
{
    uint16_t conn_id = 0;
    lehs_ase_data_t *p_lehs_ase = lehs_find_ase_with_cis_id(cig_id, cis_id, char_type, &conn_id);

    CHECK_FOR_NULL_AND_RETURN(p_lehs_ase);

    ga_lib_ascs_ase_t *p_ase = &p_lehs_ase->ase;

    lehs_isoc_dhm_free_cis_stream(p_lehs_ase->cis_conn_handle, p_ase->data_path_dir);

    if (!conn_id)
    {
        WICED_BT_TRACE_CRIT("[%s] conn_id is 0", __FUNCTION__);
        conn_id = 0x8000;
        //return;
    }

    p_lehs_ase->data_path_established = 0;

    if (GA_LIB_ASCS_STATE_RELEASING == p_ase->ase_state)
    {
        p_ase->ase_state = GA_LIB_ASCS_STATE_IDLE;

        p_ase->qos_configured.cis_id = 0xFF;
        p_ase->qos_configured.cig_id = 0xFF;
    }
    else if (GA_LIB_ASCS_STATE_STREAMING == p_ase->ase_state)
    {
        if (GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE != p_ase->ase_type)
        {
            p_ase->ase_state = GA_LIB_ASCS_STATE_QOS_CONFIGURED;
        }
        else
        {
            p_ase->ase_state = GA_LIB_ASCS_STATE_DISABLING;
        }
    }
    else if (GA_LIB_ASCS_STATE_DISABLING == p_ase->ase_state)
    {
        p_ase->ase_state = GA_LIB_ASCS_STATE_QOS_CONFIGURED;
    }
    else if (GA_LIB_ASCS_STATE_QOS_CONFIGURED == p_ase->ase_state)
    {
        // wiced_ble_isoc_peripheral_remove_cig(p_ase->data.qos_configured.cig_id);
    }
    else
    {
        return;
    }

    ga_lib_ascs_notify_ase_state(conn_id, p_lehs_ase->gatt_ase_value_handle, p_ase);
}

static int set_conn_on_ase_id(wiced_ble_isoc_cis_t *p_cis_req, int type)
{
    uint16_t conn_id;
    lehs_ase_data_t *p_lehs_ase = lehs_find_ase_with_cis_id(p_cis_req->cig_id, p_cis_req->cis_id, type, &conn_id);
    int ase_id = (p_lehs_ase) ? p_lehs_ase->ase.ase_id : 0xFF;

    WICED_BT_TRACE("[%s] cis_id %d cig_id %d cis 0x%x acl 0x%x p_ase %x ase_id %d\n",
                   __FUNCTION__,
                   p_cis_req->cis_id,
                   p_cis_req->cig_id,
                   p_cis_req->cis_conn_handle,
                   p_cis_req->acl_conn_handle,
                   p_lehs_ase,
                   ase_id);

    if (p_lehs_ase)
    {
        p_lehs_ase->cis_conn_handle = p_cis_req->cis_conn_handle;
    }

    return ase_id;
}

void lehs_isoc_event_handler(wiced_ble_isoc_event_t event, wiced_ble_isoc_event_data_t *p_event_data)
{
    WICED_BT_TRACE("[%s] event %d ", __FUNCTION__, event);

    switch (event)
    {
    case WICED_BLE_ISOC_CIS_REQUEST_EVT:
    {
        wiced_ble_isoc_cis_t *p_cis_req = &p_event_data->cis_request;
        int snk_ase_id = set_conn_on_ase_id(p_cis_req, GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE);

        WICED_BT_TRACE("ase id %d cis_id %d cig_id %d cis_conn_handle %d acl_handle %d \n",
                       snk_ase_id,
                       p_cis_req->cis_id,
                       p_cis_req->cig_id,
                       p_cis_req->cis_conn_handle,
                       p_cis_req->acl_conn_handle);

        wiced_ble_isoc_peripheral_accept_cis(p_cis_req);
    }
    break;

    case WICED_BLE_ISOC_CIS_ESTABLISHED_EVT:
        if (p_event_data->cis_established_data.status)
        {
            WICED_BT_TRACE_CRIT("[%s] status %d \n", __FUNCTION__, p_event_data->cis_established_data.status);
            return;
        }

        // Stup data path after CIS establishment for sink role as server/client,
        // Data path is setup upon receiving streaming notification / receiver start ready
        // as client and server
        lehs_cis_handle_connection(&p_event_data->cis_established_data.cis, GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE);
        lehs_cis_handle_connection(&p_event_data->cis_established_data.cis, GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE);
        break;

    case WICED_BLE_ISOC_CIS_DISCONNECTED_EVT:
        // in case of bi-directional CIS handle for both the ASE's attached to the CIS
        lehs_cis_handle_disconnection(p_event_data->cis_disconnect.cis.cig_id,
                                      p_event_data->cis_disconnect.cis.cis_id,
                                      GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE);

        lehs_cis_handle_disconnection(p_event_data->cis_disconnect.cis.cig_id,
                                      p_event_data->cis_disconnect.cis.cis_id,
                                      GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE);

        break;
    case WICED_BLE_ISOC_BIG_SYNC_ESTABLISHED_EVT:
    case WICED_BLE_ISOC_BIG_SYNC_LOST_EVT:
    case WICED_BLE_ISOC_BIG_TERMINATED_SYNC_EVT:
        lehs_bis_isoc_cb(event, p_event_data);
        break;

    case WICED_BLE_ISOC_DATA_PATH_SETUP_EVT:
        if (p_event_data->datapath.status)
        {
            WICED_BT_TRACE_CRIT("[%s] Data path setup not successful\n", __FUNCTION__);
            return;
        }

        if (wiced_ble_isoc_is_cis_connected_with_conn_hdl(p_event_data->datapath.conn_hdl))
        {
            lehs_cis_handle_data_path_setup(p_event_data->datapath.conn_hdl, p_event_data->datapath.p_app_ctx);
        }
        else if (wiced_ble_isoc_is_bis_created(p_event_data->datapath.conn_hdl))
        {
            lehs_isoc_dhm_start_stream(p_event_data->datapath.conn_hdl, GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE);
        }
        break;

    case WICED_BLE_ISOC_DATA_PATH_REMOVED_EVT:
        if (p_event_data->datapath.status)
        {
            WICED_BT_TRACE_CRIT("[%s] Data path removal not successful\n", __FUNCTION__);
        }
        break;

    default:
        break;
    }
}
