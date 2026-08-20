/*
 * $ Copyright Cypress Semiconductor $
 */


/* BT Stack includes */
#include "wiced_bt_trace.h"
#include "le_audio_rpc.h"
#include "gatt_interface.h"
#include "wiced_memory.h"



void le_audio_rpc_send_broadcast_status_update(uint32_t state)
{
    uint8_t tx_buf[64];
    uint8_t *p = tx_buf;

    UINT8_TO_STREAM(p, state);

    WICED_BT_TRACE("[%s]  state: %d\n", __FUNCTION__, state);

    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_BROADCAST_STATUS_UPDATE, tx_buf, (int)(p - tx_buf));
}

void le_audio_rpc_send_vcs_state_update(uint16_t conn_id,
                                        uint8_t volume_setting,
                                        uint8_t mute_state,
                                        uint8_t which_vcs_data)
{
    uint8_t tx_buf[30];
    uint8_t *p = tx_buf;

    switch (which_vcs_data)
    {
    case HCI_CONTROL_LEA_MUTE_STATUS:
        UINT16_TO_STREAM(p, conn_id);
        UINT8_TO_STREAM(p, mute_state);
        app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_MUTE_STATUS, tx_buf, (int)(p - tx_buf));
        break;

    case HCI_CONTROL_LEA_VOLUME_STATUS:
        UINT16_TO_STREAM(p, conn_id);
        UINT8_TO_STREAM(p, volume_setting);
        app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_VOLUME_STATUS, tx_buf, (int)(p - tx_buf));
        break;

    case HCI_CONTROL_LEA_MUTE_AND_VOLUME_STATUS:
        UINT16_TO_STREAM(p, conn_id);
        UINT8_TO_STREAM(p, volume_setting);
        UINT8_TO_STREAM(p, mute_state);
        app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_MUTE_AND_VOLUME_STATUS, tx_buf, (int)(p - tx_buf));
        break;
    }

}

void le_audio_rpc_update_call_state(uint16_t conn_id,
                                    uint8_t call_id,
                                    char * p_call_uri,
                                    uint8_t call_state)
{
    uint8_t tx_buf[64];
    uint8_t uri_len = 0;
    uint8_t *p = tx_buf;

    UINT16_TO_STREAM(p, conn_id);
    UINT8_TO_STREAM(p, call_id);
    UINT8_TO_STREAM(p, call_state);
    if (p_call_uri && (strlen(p_call_uri) > 0))
    {
        uri_len = (uint8_t)strlen(p_call_uri);
        #define URI_MAX_SIZE 50
        if (uri_len >= URI_MAX_SIZE) uri_len = URI_MAX_SIZE - 1;
        UINT8_TO_STREAM(p, uri_len);
        ARRAY_TO_STREAM(p, p_call_uri, uri_len);
    }
    else
    {
        UINT8_TO_STREAM(p, uri_len);
    }

    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_CALL_STATE, tx_buf, (int)(p - tx_buf));
}

void le_audio_rpc_send_call_terminated_event(uint16_t conn_id, uint8_t call_id, uint8_t termination_reason)
{
    uint8_t tx_buf[5];
    uint8_t *p = tx_buf;
    UINT16_TO_STREAM(p, conn_id);
    UINT8_TO_STREAM(p, call_id);
    UINT8_TO_STREAM(p, termination_reason);

    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_CALL_TERMINATED, tx_buf, (int)(p - tx_buf));
}

void le_audio_rpc_send_mcs_state_update(uint16_t conn_id, uint8_t state)
{
    uint8_t tx_buf[64];
    uint8_t *p = tx_buf;

    UINT16_TO_STREAM(p, conn_id);
    UINT8_TO_STREAM(p, state);

    WICED_BT_TRACE("[%s] state: %d\n", __FUNCTION__,  state);

    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_PLAY_STATUS, tx_buf, (int)(p - tx_buf));
}

void le_audio_rpc_send_mic_state_update(uint16_t conn_id, uint8_t state)
{
    uint8_t tx_buf[64];
    uint8_t *p = tx_buf;

    UINT16_TO_STREAM(p, conn_id);
    UINT8_TO_STREAM(p, state);

    WICED_BT_TRACE("[%s]  state: %d\n", __FUNCTION__, state);

    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_MIC_STATUS, tx_buf, (int)(p - tx_buf));
}

void le_audio_rpc_send_convo_stream_state_update(uint16_t conn_id, uint8_t state)
{
    uint8_t tx_buf[64];
    uint8_t *p = tx_buf;

    UINT16_TO_STREAM(p, conn_id);
    UINT8_TO_STREAM(p, state);

    WICED_BT_TRACE("[%s]  state: %d\n", __FUNCTION__, state);

    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_CONVO_STREAM_STATE, tx_buf, (int)(p - tx_buf));
}


void le_audio_rpc_send_app_status(uint16_t conn_id, wiced_bt_device_address_t bd_addr, uint32_t init_state, uint32_t sub_state)
{
    app_rpc_send_app_sub_status(conn_id, bd_addr, init_state, sub_state, NULL);
}

#define MAX_AICS_DESCRIPTION_LEN 50
void le_audio_rpc_send_mics_aics_description(uint16_t conn_id, uint32_t instance, char *p_desc)
{
    uint8_t tx_buff[2 + 4 + 4 + MAX_AICS_DESCRIPTION_LEN + 1] = {0};
    uint8_t *p_buff = &tx_buff[0];
    uint32_t len = strlen(p_desc) +1;

    if (len > MAX_AICS_DESCRIPTION_LEN){
        len = MAX_AICS_DESCRIPTION_LEN;
    }
    UINT16_TO_STREAM(p_buff, conn_id);
    UINT32_TO_STREAM(p_buff, instance);
    UINT32_TO_STREAM(p_buff, strlen(p_desc)+1);
    ARRAY_TO_STREAM(p_buff, p_desc, strlen(p_desc)+1);

    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_MICS_AICS_DESC, tx_buff, (int)(p_buff - tx_buff));
}

void le_audio_rpc_send_mics_mute_state(uint16_t conn_id, uint8_t mute_state)
{

    uint8_t tx_buff[5] = {0};
    uint8_t *p_buff = &tx_buff[0];

    UINT16_TO_STREAM(p_buff, conn_id);
    UINT8_TO_STREAM(p_buff, mute_state);

    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_MICS_MUTE_STATUS, tx_buff, (int)(p_buff - tx_buff));
}

void le_audio_rpc_send_mics_aics_input_state(uint16_t conn_id,
                                             uint32_t instance,
                                             ga_lib_aics_input_state_t *p_input_state)
{

    uint8_t tx_buff[10] = {0};
    uint8_t *p_buff = &tx_buff[0];

    UINT16_TO_STREAM(p_buff, conn_id);
    UINT32_TO_STREAM(p_buff, instance);
    INT8_TO_STREAM(p_buff, p_input_state->gain_setting);
    UINT8_TO_STREAM(p_buff, p_input_state->mute_mode);
    UINT8_TO_STREAM(p_buff, p_input_state->gain_mode);

    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_MICS_AICS_INPUT_STATUS, tx_buff, (int)(p_buff - tx_buff));
}

void le_audio_rpc_send_preset_record(uint16_t conn_id, uint8_t preset_index, char * p_name, uint8_t name_len)
{
    uint8_t tx_buf[45];
    uint8_t *p = tx_buf;
    UINT16_TO_STREAM(p, conn_id);
    UINT8_TO_STREAM(p, preset_index);
    ARRAY_TO_STREAM(p, p_name, name_len);
    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_PRESET_RECORD, tx_buf, (int)(p - tx_buf));
}

void le_audio_rpc_update_active_preset(uint16_t conn_id, uint8_t preset_index)
{
    uint8_t tx_buf[5];
    uint8_t *p = tx_buf;
    UINT16_TO_STREAM(p, conn_id);
    UINT8_TO_STREAM(p, preset_index);
    app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_ACTIVE_PRESET, tx_buf, (int)(p - tx_buf));
}
