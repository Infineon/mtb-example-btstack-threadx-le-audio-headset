/*
 * $ Copyright Cypress Semiconductor $
 */
#include "audio_driver.h"
#include "lehs.h"

static void lehs_vcs_notify_volume_state(void)
{
    int i = 0;
    lehs_clcb_t *p_clcb = &g_lehs_gatt_cb.clcb[0];
    lehs_vcs_t *p_vcs = &g_lehs_gatt_cb.local_service_data.vcs;

    for (i = 0; i < LEHS_MAX_CONNECTIONS; i++, p_clcb++)
    {
        if (p_clcb->in_use)
        {
            ga_lib_vcs_notify_volume_state(p_clcb->conn_id, HDLC_VCS_VOLUME_STATE_VALUE, &p_vcs->state);
        }
    }
    return;
}

wiced_bt_gatt_status_t lehs_vcs_set_volume(uint16_t conn_id,
                                           ga_lib_vcs_volume_control_opcodes_t vcs_opcode,
                                           uint8_t abs_vol)
{
    lehs_vcs_t *p_vcs = &g_lehs_gatt_cb.local_service_data.vcs;
    uint8_t char_type = 0;

    if (ga_lib_vcs_check_volume_state_validity(vcs_opcode, &p_vcs->state, abs_vol, VCS_STEP_SIZE) != WICED_SUCCESS)
    {
        return WICED_BT_GATT_SUCCESS;
    }

    audio_driver_set_volume(p_vcs->state.volume_setting);
    audio_driver_set_mute_state(p_vcs->state.mute_state);
    lehs_vcs_notify_volume_state();


    switch (vcs_opcode)
    {
    case VOLUME_CONTROL_OPCODE_RELATIVE_VOLUME_DOWN:
    case VOLUME_CONTROL_OPCODE_RELATIVE_VOLUME_UP:
    case VOLUME_CONTROL_OPCODE_SET_ABSOLUTE_VOLUME:
        char_type = HCI_CONTROL_LEA_VOLUME_STATUS;
        break;

    case VOLUME_CONTROL_OPCODE_UNMUTE_RELATIVE_VOLUME_DOWN:
    case VOLUME_CONTROL_OPCODE_UNMUTE_RELATIVE_VOLUME_UP:
        char_type = HCI_CONTROL_LEA_MUTE_AND_VOLUME_STATUS;
        break;

    case VOLUME_CONTROL_OPCODE_UNMUTE:
    case VOLUME_CONTROL_OPCODE_MUTE:
        char_type = HCI_CONTROL_LEA_MUTE_STATUS;
        break;

    default:
        break;
    }
    le_audio_rpc_send_vcs_state_update(conn_id, p_vcs->state.volume_setting, p_vcs->state.mute_state, char_type);
    return WICED_BT_GATT_SUCCESS;
}

wiced_bt_gatt_status_t lehs_handle_vcs_cp_write(uint16_t conn_id, const uint8_t *p_data, uint16_t len_to_write)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;
    ga_lib_vcs_volume_control_opcodes_t opcode;
    uint8_t change_counter;
    lehs_vcs_t *p_vcs = &g_lehs_gatt_cb.local_service_data.vcs;

    STREAM_TO_UINT8(opcode, p_data);

    // Check Opcode
    if (opcode > VOLUME_CONTROL_OPCODE_MUTE)
    {
        WICED_BT_TRACE("[%s] unsupported opcode %d", __FUNCTION__, opcode);
        return GA_LIB_VCS_ERROR_OPCODE_NOT_SUPPORTED;
    }

    // Check length
    if (((opcode == VOLUME_CONTROL_OPCODE_SET_ABSOLUTE_VOLUME) && (len_to_write != 3)) ||
        ((opcode != VOLUME_CONTROL_OPCODE_SET_ABSOLUTE_VOLUME) && (len_to_write != 2)))
    {
        WICED_BT_TRACE("[%s] invalid attribute length %d for opcode %d", __FUNCTION__, len_to_write, opcode);
        return WICED_BT_GATT_INVALID_ATTR_LEN;
    }

    STREAM_TO_UINT8(change_counter, p_data);

    // Check change counter
    if (p_vcs->state.change_counter != change_counter)
    {
        WICED_BT_TRACE("[%s] invalid change counter local %d peer %d",
                       __FUNCTION__,
                       p_vcs->state.change_counter,
                       change_counter);
        return GA_LIB_VCS_ERROR_INVALID_CHANGE_COUNTER;
    }

    uint8_t abs_vol = p_vcs->state.volume_setting;
    if (opcode == VOLUME_CONTROL_OPCODE_SET_ABSOLUTE_VOLUME)
    {
        STREAM_TO_UINT8(abs_vol, p_data);
    }

    status = lehs_vcs_set_volume(conn_id, opcode, abs_vol);

    if ((opcode < VOLUME_CONTROL_OPCODE_UNMUTE) && (p_vcs->flag != GA_LIB_VOLUME_FLAG_VOLUME_SETTING_PERSISTED))
    {
        p_vcs->flag = GA_LIB_VOLUME_FLAG_VOLUME_SETTING_PERSISTED;
        WICED_BT_TRACE("[%s] notifying flag %d", __FUNCTION__, p_vcs->flag);
        ga_lib_vcs_notify_volume_flag(conn_id, HDLC_VCS_VOLUME_FLAGS_VALUE, p_vcs->flag);
    }

    return status;
}

void lehs_vcs_initialize_data(void)
{
    lehs_vcs_t *p_vcs = &g_lehs_gatt_cb.local_service_data.vcs;

    p_vcs->flag = 1;
    p_vcs->state.mute_state = GA_LIB_MUTE_STATE_NOT_MUTED;
    p_vcs->state.volume_setting = DEFAULT_VOL;

    // initialize volume in alsa driver
    audio_driver_set_volume(p_vcs->state.volume_setting);
    audio_driver_set_mute_state(p_vcs->state.mute_state);
}
