/*
 * $ Copyright Cypress Semiconductor $
 */
#include "lehs.h"

wiced_result_t lehs_mcs_play_pause(uint16_t conn_id, wiced_bool_t play)
{
    ga_lib_mcs_operation_t operation = {0};
    lehs_clcb_t *p_clcb = lehs_gatt_get_clcb_by_conn_id(conn_id);

    if (!p_clcb)
    {
        WICED_BT_TRACE_CRIT("[%s] No p_clcb", __FUNCTION__);
        return WICED_ERROR;
    }
    operation.opcode = play ? GA_LIB_MCS_PLAY : GA_LIB_MCS_PAUSE;

    return ga_lib_mcs_write_control_media(conn_id,
                                          &p_clcb->peer_profiles.gmcs[GA_LIB_MCS_CHARACTERISTIC_MEDIA_CONTROL_POINT],
                                          &operation);
}

uint8_t mcs_characteristic_sizes(ga_lib_mcs_characteristics_t type)
{
    uint8_t char_size[] = {
        MAX_MEDIA_PLAYER_NAME_LEN, /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYER_NAME */
        0,                         /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_CHANGED */
        MAX_MEDIA_TRACK_TITLE_LEN, /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_TITLE */
        4,                         /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_DURATION */
        4,                         /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_POSITION  */
        1,                         /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYBACK_SPEED */
        1,                         /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_SEEKING_SPEED */
        1,                         /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYING_ORDER */
        2,                         /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYING_ORDER_SUPPORTED */
        1,                         /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_STATE */
        1,                         /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_CONTROL_POINT */
        4                          /* GA_LIB_MCS_CHARACTERISTIC_MEDIA_OPCODE_SUPPORTED */
    };

    if (type < GA_LIB_MCS_CHARACTERISTIC_MAX)
    {
        return char_size[type];
    }

    return -1;
}

wiced_result_t lehs_gmcs_app_handle_read_complete(uint16_t conn_id,
                                                  lehs_clcb_t *p_clcb,
                                                  wiced_bt_gatt_data_t *p_gatt_data)
{
    lehs_mcs_data_t *p_mcs = &p_clcb->mcs;
    int index = gatt_intf_find_characteristic_type_by_value_handle(p_clcb->peer_profiles.gmcs,
                                                                   GA_LIB_MCS_CHARACTERISTIC_MAX,
                                                                   p_gatt_data->handle);
    uint8_t *p_data = p_gatt_data->p_data;
    int expected_len = mcs_characteristic_sizes(index);

    WICED_BT_TRACE("[%s] handle %d exp len %d got %d",
                   __FUNCTION__,
                   p_gatt_data->handle,
                   expected_len,
                   p_gatt_data->len);

    if (p_gatt_data->len < expected_len)
    {
        WICED_BT_TRACE_CRIT("[%s] Not enough data read for char %d", __FUNCTION__, index);
        return WICED_BT_ERROR;
    }

    switch (index)
    {
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYER_NAME:
    {
        int len_to_copy = p_gatt_data->len;
        if (len_to_copy > (MAX_MEDIA_PLAYER_NAME_LEN - 1))
        {
            len_to_copy = (MAX_MEDIA_PLAYER_NAME_LEN - 1);
        }
        WICED_MEMSET(p_mcs->media_player_name, 0, MAX_MEDIA_PLAYER_NAME_LEN);
        WICED_MEMCPY(p_mcs->media_player_name, p_data, len_to_copy);
        WICED_BT_TRACE("[%s]Player name %s\n", __FUNCTION__, p_mcs->media_player_name);
        lehs_rpc_send_get_players_event(conn_id, p_mcs->media_player_name);

        break;
    }
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_STATE:
    {
        STREAM_TO_UINT8(p_mcs->media_state, p_data);
        WICED_BT_TRACE("[%s] Media State %d", __FUNCTION__, p_mcs->media_state);
        lehs_rpc_send_play_status(conn_id, p_mcs->media_state);
    }
    break;
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_CONTROL_POINT:
    {
        uint8_t result, opcode;
        STREAM_TO_UINT8(opcode, p_data);
        STREAM_TO_UINT8(result, p_data);

        WICED_BT_TRACE("[%s] Media Op result 0x%x for opcode 0x%x", __FUNCTION__, result, opcode);
    }
    break;
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_CHANGED:
        WICED_BT_TRACE("[%s] Track Changed", __FUNCTION__);
        break;
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_TITLE:
    {
        int to_copy = p_gatt_data->len;
        if (to_copy > MAX_MEDIA_TRACK_TITLE_LEN)
        {
            to_copy = MAX_MEDIA_TRACK_TITLE_LEN;
        }
        WICED_MEMSET(p_mcs->track_title, 0, MAX_MEDIA_TRACK_TITLE_LEN);
        WICED_MEMCPY(p_mcs->track_title, p_data, to_copy);
        WICED_BT_TRACE("[%s] Track Title %s", __FUNCTION__, p_mcs->track_title);
    }
    break;
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_DURATION:
    {
        STREAM_TO_UINT32(p_mcs->track_duration, p_data);

        WICED_BT_TRACE("[%s] Track Duration %d", __FUNCTION__, p_mcs->track_duration);
    }
    break;
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_POSITION:
    {
        STREAM_TO_UINT32(p_mcs->track_position, p_data);
        WICED_BT_TRACE("[%s] Track Position %d", __FUNCTION__, p_mcs->track_position);
    }
    break;
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYBACK_SPEED:
    {
        STREAM_TO_UINT8(p_mcs->playback_speed, p_data);
        WICED_BT_TRACE("Track Playback Speed %d", p_mcs->playback_speed);
    }
    break;
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_SEEKING_SPEED:
    {
        STREAM_TO_UINT8(p_mcs->seeking_speed, p_data);
        WICED_BT_TRACE("Track Seeking Speed %d", p_mcs->seeking_speed);
    }
    break;
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYING_ORDER:
    {
        STREAM_TO_UINT8(p_mcs->playing_order, p_data);
        WICED_BT_TRACE("Playing Order %d", p_mcs->playing_order);
    }
    break;
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYING_ORDER_SUPPORTED:
    {
        STREAM_TO_UINT16(p_mcs->playing_order_supported, p_data);
        WICED_BT_TRACE("Playing Order Supported %d", p_mcs->playing_order_supported);
    }
    break;
    case GA_LIB_MCS_CHARACTERISTIC_MEDIA_OPCODE_SUPPORTED:
    {
        STREAM_TO_UINT32(p_mcs->media_control_supported_opcodes, p_data);
        WICED_BT_TRACE("Supported Opcode 0x%x", p_mcs->media_control_supported_opcodes);
    }
    break;
    default:
        break;
    }

    return WICED_BT_SUCCESS;
}

const uint8_t lepl_mcs_enable_notification_chars[] = {GA_LIB_MCS_CHARACTERISTIC_MEDIA_CONTROL_POINT,
                                                      GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_TITLE,
                                                      GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_CHANGED,
                                                      GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_POSITION,
                                                      GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_DURATION};

const uint8_t *lehs_mcs_get_enable_notification_chars(uint8_t *p_size)
{
    *p_size = sizeof(lepl_mcs_enable_notification_chars) / sizeof(lepl_mcs_enable_notification_chars[0]);
    return lepl_mcs_enable_notification_chars;
}
