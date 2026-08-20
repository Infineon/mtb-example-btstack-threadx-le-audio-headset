/*
 * $ Copyright Cypress Semiconductor $
 */


#include "lehs.h"
#include "audio_driver.h"

#define AICS_AUDIO_INPUT_DESCRIPTION "MICP AICS"

lehs_aics_t aics = {.description = AICS_AUDIO_INPUT_DESCRIPTION,
                    .description_len = sizeof(AICS_AUDIO_INPUT_DESCRIPTION),
                    .gain_setting = {.gain_setting_units = 15, .max_gain_setting = 127, .min_gain_setting = -128},
                    .input_state =
                        {
                            .gain_mode = GA_LIB_AICS_GAIN_MODE_MANUAL,
                            .gain_setting = DEFAULT_MIC_GAIN,
                        },
                    .input_type = GA_LIB_AICS_INPUT_TYPE_BLUETOOTH,
                    .input_status = GA_LIB_AICS_INPUT_STATUS_ACTIVE};

wiced_bt_gatt_status_t lehs_mics_mute(uint16_t conn_id, uint8_t mute)
{
    lehs_mics_t *p_mics = &g_lehs_gatt_cb.local_service_data.mics;

    audio_driver_set_mic_mute_state(mute);

    p_mics->mute_state = mute;

    le_audio_rpc_send_mics_mute_state(conn_id, mute);

    return ga_lib_mics_notify_mute_state(conn_id, HDLC_MICS_MUTE_STATE_VALUE, p_mics->mute_state);
}

wiced_bt_gatt_status_t lehs_mics_aics_mute(uint16_t conn_id, uint32_t aics_instance, uint8_t mute)
{
    lehs_aics_t *p_aics = &g_lehs_gatt_cb.local_service_data.mics_aics[aics_instance];

    if (p_aics->input_state.mute_mode == mute)
    {
        WICED_BT_TRACE_CRIT("[%s] No change in mute state %d", __FUNCTION__, mute);
        return WICED_BT_GATT_SUCCESS;
    }

    audio_driver_set_mic_mute_state(mute);
    p_aics->input_state.mute_mode = mute;
    le_audio_rpc_send_mics_aics_input_state(conn_id,
                                            aics_instance,
                                            &p_aics->input_state);

    return ga_lib_aics_notify_input_state(conn_id, HDLC_MICS_AICS_INPUT_STATE_VALUE, &p_aics->input_state);
}

wiced_bt_gatt_status_t lehs_mics_aics_set_gain(uint16_t conn_id,
                                               uint32_t aics_instance,
                                               uint8_t opcode,
                                               int8_t input_gain)
{
    lehs_aics_t *p_aics = &g_lehs_gatt_cb.local_service_data.mics_aics[aics_instance];

    if (opcode == HCI_CONTROL_LEA_MICS_AICS_GAIN_INCREMENT)
    {
        int gain = (int)(p_aics->input_state.gain_setting + p_aics->gain_setting.gain_setting_units);
        input_gain = (gain > 127) ? 127 : gain;
    }
    else if (opcode == HCI_CONTROL_LEA_MICS_AICS_GAIN_DECREMENT)
    {
        int gain = (int)(p_aics->input_state.gain_setting - p_aics->gain_setting.gain_setting_units);
        input_gain = (gain < -128) ? -128 : gain;
    }

    if (input_gain > p_aics->gain_setting.max_gain_setting)
        input_gain = p_aics->gain_setting.max_gain_setting;
    else if (input_gain < p_aics->gain_setting.min_gain_setting)
        input_gain = p_aics->gain_setting.min_gain_setting;

    if (p_aics->input_state.gain_setting == input_gain)
    {
        WICED_BT_TRACE_CRIT("[%s] No change in gain setting %d", __FUNCTION__, input_gain);
        return WICED_BT_GATT_SUCCESS;
    }

    audio_driver_set_mic_gain(input_gain);

    p_aics->input_state.gain_setting = input_gain;

    le_audio_rpc_send_mics_aics_input_state(conn_id, aics_instance, &p_aics->input_state);

    return ga_lib_aics_notify_input_state(conn_id, HDLC_MICS_AICS_INPUT_STATE_VALUE, &p_aics->input_state);
}

void lehs_mics_initialize_data(void)
{
    lehs_mics_t *p_mics = &g_lehs_gatt_cb.local_service_data.mics;
    p_mics->mute_state = GA_LIB_AICS_UNMUTE;
    lehs_aics_t *p_aics = &g_lehs_gatt_cb.local_service_data.mics_aics[0];
    WICED_MEMCPY(p_aics, &aics, sizeof(lehs_aics_t));
}
