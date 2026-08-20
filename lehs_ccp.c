/*
 * $ Copyright Cypress Semiconductor $
 */

#include "lehs.h"
#include "le_audio_rpc.h"
#include "ga_lib_tbs.h"
#include "ga_lib_tbs.h"

#include "wiced_bt_gatt.h"
#include "wiced_bt_trace.h"
#include "wiced_memory.h"

void lehs_rpc_call_control_point_action(uint16_t conn_id, lehs_clcb_t *p_clcb, uint8_t call_id, uint8_t opcode)
{
    gatt_intf_characteristic_handles_t *p_handle = &p_clcb->peer_profiles.gtbs[GA_LIB_TBS_CHARACTERISTIC_CALL_CONTROL_POINT];
    wiced_bt_gatt_status_t status = WICED_ERROR;

    switch (opcode)
    {
    case GA_LIB_CCP_ACTION_ACCEPT_CALL:
        status = ga_lib_tbs_write_cp_action_accept_call(conn_id, p_handle, call_id);
        break;
    case GA_LIB_CCP_ACTION_TERMINATE_CALL:
        status = ga_lib_tbs_write_cp_action_terminate_call(conn_id, p_handle, call_id);
        break;
    case GA_LIB_CCP_ACTION_HOLD_CALL:
        status = ga_lib_tbs_write_cp_action_hold_call(conn_id, p_handle, call_id);
        break;
    case GA_LIB_CCP_ACTION_RETRIEVE_CALL:
        status = ga_lib_tbs_write_cp_action_retrieve_call(conn_id, p_handle, call_id);
        break;
    default:
        break;
    }

    WICED_BT_TRACE("[%s] status %d ", __FUNCTION__, status);
}

wiced_result_t lehs_gtbs_app_handle_read_complete(uint16_t conn_id,
                                                  lehs_clcb_t *p_clcb,
                                                  wiced_bt_gatt_data_t *p_gatt_data)
{
    int index = gatt_intf_find_characteristic_type_by_value_handle(p_clcb->peer_profiles.gtbs,
                                                                   GA_LIB_TBS_CHARACTERISTIC_MAX,
                                                                   p_gatt_data->handle);
    uint8_t *p_data = p_gatt_data->p_data;
    uint16_t data_len = p_gatt_data->len;

    WICED_BT_TRACE("[%s] handle %d index %d", __FUNCTION__, p_gatt_data->handle, index);

    switch (index)
    {
    case GA_LIB_TBS_CHARACTERISTIC_BEARER_PROVIDER_NAME:
        break;
    case GA_LIB_TBS_CHARACTERISTIC_BEARER_UCI:
        break;
    case GA_LIB_TBS_CHARACTERISTIC_BEARER_TECHNOLOGY:
    {
        uint8_t bearer_technology;
        STREAM_TO_UINT8(bearer_technology, p_data);
        WICED_BT_TRACE("[%s] bearer technology:%d ", __FUNCTION__, bearer_technology);
    }
    break;
    case GA_LIB_TBS_CHARACTERISTIC_BEARER_URI_SUPPORTED_SCHEMES:
        break;
    case GA_LIB_TBS_CHARACTERISTIC_BEARER_SIGNAL_STRENGTH:
    {
        uint8_t bearer_signal_strength;
        STREAM_TO_UINT8(bearer_signal_strength, p_data);
        WICED_BT_TRACE("[%s] bearer signal strength %d  ", __FUNCTION__, bearer_signal_strength);
    }
    break;
    case GA_LIB_TBS_CHARACTERISTIC_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL:
    {
        uint8_t bearer_signal_strength_reporting_interval;
        STREAM_TO_UINT8(bearer_signal_strength_reporting_interval, p_data);
        WICED_BT_TRACE("[%s] bearer signal strength interval %d",
                       __FUNCTION__,
                       bearer_signal_strength_reporting_interval);
    }
    break;
    case GA_LIB_TBS_CHARACTERISTIC_BEARER_LIST_CURRENT_CALLS:
    {
        uint8_t list_len;
        ga_lib_tbs_current_call_t call;

        WICED_BT_TRACE("[%s] ---- current call list begin----", __FUNCTION__);
        while (data_len >= 4)
        {
            STREAM_TO_UINT8(list_len, p_data);
            STREAM_TO_UINT8(call.call_id, p_data);
            STREAM_TO_UINT8(call.call_state, p_data);
            STREAM_TO_UINT8(call.call_flags, p_data);

            call.p_remote_caller_id = (char *)p_data;
            call.remote_caller_id_len = list_len - 3;

            WICED_BT_TRACE("[%s] call id %d state %d flags %d %s",
                           __FUNCTION__,
                           call.call_id,
                           call.call_state,
                           call.call_flags,
                           call.p_remote_caller_id);
            data_len -= list_len + 1;
        }
        WICED_BT_TRACE("[%s] ---- current call list end ----", __FUNCTION__);
    }
    break;
    case GA_LIB_TBS_CHARACTERISTIC_CONTENT_CONTROL_ID:
    {
        uint8_t content_control_id;
        STREAM_TO_UINT8(content_control_id, p_data);
        WICED_BT_TRACE("[%s] content control id %x ", __FUNCTION__, content_control_id);
    }
    break;
    case GA_LIB_TBS_CHARACTERISTIC_INCOMING_CALL_TG_BEARER_URI:
    {
        uint8_t call_id;
        uint8_t uri[50] = {0};
        int uri_len = sizeof(uri) - 1;

        STREAM_TO_UINT8(call_id, p_data);
        if (uri_len > data_len - 1)
        {
            uri_len = data_len - 1;
        }
        strncpy((char *)uri, (char *)p_data, uri_len);
        WICED_BT_TRACE("[%s] incoming call target caller id ", __FUNCTION__);
        WICED_BT_TRACE("[%s] callid %d %s", __FUNCTION__, call_id, uri);
    }
    break;
    case GA_LIB_TBS_CHARACTERISTIC_STATUS_FLAGS:
    {
        uint16_t status_flag;
        STREAM_TO_UINT16(status_flag, p_data);
        WICED_BT_TRACE("[%s] bearer status_flags %d", __FUNCTION__, status_flag);
    }
    break;
    case GA_LIB_TBS_CHARACTERISTIC_CALL_STATE:
    {
        WICED_BT_TRACE("[%s] ---- call state begin----", __FUNCTION__);
        while (data_len >= 3)
        {
            ga_lib_tbs_call_state_data_t cs;

            STREAM_TO_UINT8(cs.call_id, p_data);
            STREAM_TO_UINT8(cs.call_state, p_data);
            STREAM_TO_UINT8(cs.call_flags, p_data);
            WICED_BT_TRACE("[%s] call id %d st %d flags 0x%x ", __FUNCTION__, cs.call_id, cs.call_state, cs.call_flags);

            le_audio_rpc_update_call_state(conn_id, cs.call_id, NULL, cs.call_state);
            data_len -= 3;
        }
        WICED_BT_TRACE("[%s] ---- call state end----", __FUNCTION__);
    }
    break;
    case GA_LIB_TBS_CHARACTERISTIC_CALL_CONTROL_POINT_OPTIONAL_OPCODE:
    {
        uint16_t ccp_supported_opcode;
        STREAM_TO_UINT16(ccp_supported_opcode, p_data);
        WICED_BT_TRACE("[%s] control point supported opcode %d ", __FUNCTION__, ccp_supported_opcode);
    }
    break;
    case GA_LIB_TBS_CHARACTERISTIC_CALL_TERMINATION_REASON:
    {
        uint8_t call_id;
        uint8_t termination_reason;
        STREAM_TO_UINT8(call_id, p_data);
        STREAM_TO_UINT8(termination_reason, p_data);

        WICED_BT_TRACE("[%s] call %d termination reason %d ", __FUNCTION__, call_id, termination_reason);
        le_audio_rpc_send_call_terminated_event(conn_id, call_id, termination_reason);
    }
    break;
    case GA_LIB_TBS_CHARACTERISTIC_INCOMING_CALL:
    {
        uint8_t call_id;
        char caller_id[50] = {0};
        STREAM_TO_UINT8(call_id, p_data);
        data_len -= 1;
        if (data_len  > (sizeof(caller_id) -1))
        {
            data_len = sizeof(caller_id) - 1;
        }
        memcpy(caller_id, p_data, data_len);

        WICED_BT_TRACE("[%s] bearer incoming caller id %d %s", __FUNCTION__, call_id, caller_id);

        le_audio_rpc_update_call_state(conn_id, call_id, caller_id, GA_LIB_TBS_CALL_STATE_INCOMING);
    }
    break;
    case GA_LIB_TBS_CHARACTERISTIC_CALL_FRIENDLY_NAME:
    {
        uint8_t call_id;
        char caller_id[50] = {0};
        STREAM_TO_UINT8(call_id, p_data);
        data_len -= 1;
        if (data_len > (sizeof(caller_id) - 1))
        {
            data_len = sizeof(caller_id) - 1;
        }
        memcpy(caller_id, p_data, data_len);

        WICED_BT_TRACE("[%s] call friendly name id %d %s", __FUNCTION__, call_id, caller_id);
        lehs_rpc_update_call_friendly_name(conn_id, caller_id, data_len);
    }
    break;
    default:
        break;
    }

    return WICED_BT_SUCCESS;
}
