/*
 * $ Copyright Cypress Semiconductor $
 */


/* BT Stack includes */
#include "wiced_bt_trace.h"
#include "app_rpc.h"
//#include "gatt_interface.h"
#include "wiced_memory.h"


extern app_rpc_cback_t g_app_rpc_cback;

void priv_app_rpc_handle_wiced_hci_data(uint16_t opcode, uint8_t *p_data, uint32_t data_len)
{
    WICED_BT_TRACE("[%s] [cmd_opcode 0x%04x] [%d bytes]\n", __FUNCTION__, opcode, data_len);

    if (g_app_rpc_cback)
    {
        g_app_rpc_cback(opcode, p_data, data_len);
    }
}

void app_rpc_send_misc_event(uint32_t chip, uint8_t group)
{
    uint8_t tx_buf[15];
    uint8_t idx = 0;

    tx_buf[idx++] = 0;
    tx_buf[idx++] = 0;
    tx_buf[idx++] = 0;
    tx_buf[idx++] = 0;
    tx_buf[idx++] = 0;
    tx_buf[idx++] = chip & 0xFF;
    tx_buf[idx++] = (chip >> 8) & 0xFF;
    tx_buf[idx++] = (chip >> 24) & 0xFF;
    tx_buf[idx++] = 0; // not used

    /* Send MCU app the supported features */
    tx_buf[idx++] = group;

    app_rpc_send_data(HCI_CONTROL_MISC_EVENT_VERSION, tx_buf, idx);
}

void app_rpc_send_dev_role(uint32_t dev_role)
{
    app_rpc_send_misc_event(55571, HCI_CONTROL_GROUP_LE_AUDIO);

    {
        uint8_t tx_buf[8];
        uint8_t *p = tx_buf;
        UINT32_TO_STREAM(p, dev_role);
        app_rpc_send_data(HCI_CONTROL_LE_AUDIO_EVENT_DEVICE_ROLE, tx_buf, p - tx_buf);
    }
}

void app_rpc_send_connect_event(wiced_bt_gatt_connection_status_t *p_status)
{
    uint8_t tx_buf[30];
    uint8_t *p = tx_buf;

    if (p_status->connected)
    {
        WICED_BT_TRACE("[%s] connected to [%B, %d] conn %d\n", __FUNCTION__,
            p_status->bd_addr, p_status->addr_type, p_status->conn_id);
    }
    else
    {
        WICED_BT_TRACE("[%s] not connected to [%B, %d]\n", __FUNCTION__, p_status->bd_addr, p_status->addr_type);
    }

    UINT8_TO_STREAM(p, p_status->addr_type);   // 00, 01, 00
    BDADDR_TO_STREAM(p, p_status->bd_addr);    // 01, 06, 07
    UINT16_TO_STREAM(p, p_status->conn_id);    // 07, 02, 09
    UINT8_TO_STREAM(p, p_status->link_role);   // 09, 01, 10
    UINT8_TO_STREAM(p, p_status->connected);   // 10, 01, 11
    UINT8_TO_STREAM(p, p_status->reason);      // 11, 01, 12
    UINT8_TO_STREAM(p, p_status->transport);   // 12, 01, 13

    app_rpc_send_data(HCI_CONTROL_LE_EVENT_CONNECTED, tx_buf, (int)(p - tx_buf));
}

void app_rpc_send_disconnect_evt(wiced_bt_gatt_connection_status_t *p_status)
{
    uint8_t tx_buf[14];
    uint8_t *p = tx_buf;

    WICED_BT_TRACE("[%s]  conn: %d reason: %d\n", __FUNCTION__, p_status->conn_id, p_status->reason);

    UINT16_TO_STREAM(p, p_status->conn_id);
    UINT16_TO_STREAM(p, p_status->reason);
    BDADDR_TO_STREAM(p, p_status->bd_addr);  // 01, 06, 07
    UINT8_TO_STREAM(p, p_status->addr_type); // 00, 01, 00

    app_rpc_send_data(HCI_CONTROL_LE_EVENT_DISCONNECTED, tx_buf, (int)(p - tx_buf));
}


void app_rpc_send_scan_res_event(wiced_ble_ext_scan_results_t *p_scr, uint8_t *p_adv_data)
{
    uint8_t tx_buf[256 + sizeof(wiced_bt_ble_scan_results_t)];
    uint8_t *p = tx_buf;

    UINT16_TO_STREAM(p, p_scr->ext_evt_type);          //  0, 1, 2
    BDADDR_TO_STREAM(p, p_scr->remote_bd_addr);        //  2, 6, 8
    UINT8_TO_STREAM(p, p_scr->ble_addr_type);          //  8, 1, 9
    UINT8_TO_STREAM(p, p_scr->rssi);                   //  9, 1, 10
    UINT8_TO_STREAM(p, p_scr->primary_phy);            // 10, 1, 11
    UINT8_TO_STREAM(p, p_scr->secondary_phy);          // 11, 1, 12
    UINT8_TO_STREAM(p, p_scr->adv_sid);                // 12, 1, 13
    UINT8_TO_STREAM(p, p_scr->tx_power);               // 13, 1, 14
    UINT16_TO_STREAM(p, p_scr->periodic_adv_interval); // 14, 2, 16
    UINT8_TO_STREAM(p, p_scr->direct_addr_type);       // 16, 1, 17
    BDADDR_TO_STREAM(p, p_scr->direct_bda);            // 17, 6, 23
    UINT16_TO_STREAM(p, p_scr->adv_len);               // 23, 2, 25
    memcpy(p, p_adv_data, p_scr->adv_len); // data
    p += p_scr->adv_len;

    app_rpc_send_data(HCI_CONTROL_LE_EVENT_EXT_ADVERTISEMENT_REPORT, tx_buf, (int)(p - tx_buf));
}

void app_rpc_send_app_sub_status(uint16_t conn_id,
                                 wiced_bt_device_address_t dev_addr,
                                 uint32_t init_state,
                                 uint32_t sub_state,
                                 const wiced_bt_uuid_t *p_uuid)
{
    uint8_t tx_buf[26];
    uint8_t *p = tx_buf;

    UINT16_TO_STREAM(p, conn_id);
    BDADDR_TO_STREAM(p, dev_addr);
    UINT32_TO_STREAM(p, init_state);
    UINT32_TO_STREAM(p, sub_state);
    if (p_uuid)
    {
        UINT8_TO_STREAM(p, p_uuid->len);
        if (p_uuid->len == 2)
        {
            UINT16_TO_STREAM(p, p_uuid->uu.uuid16);
        }
        else if (p_uuid->len == 4)
        {
            UINT16_TO_STREAM(p, p_uuid->uu.uuid32);
        }
    }

    WICED_BT_TRACE("[%s] conn %d state : %d sub_state: %d  uuid: 0x%04x\n",
                   __FUNCTION__,
                   conn_id,
                   init_state,
                   sub_state,
                   p_uuid ? p_uuid->uu.uuid16 : 0);

    app_rpc_send_data(HCI_CONTROL_MISC_EVENT_APP_STATUS, tx_buf, (int)(p - tx_buf));
}

void app_rpc_send_app_status(uint16_t conn_id, wiced_bt_device_address_t dev_addr, uint32_t init_state, uint32_t sub_state)
{
    app_rpc_send_app_sub_status(conn_id, dev_addr, init_state, sub_state, NULL);
}


/*
 *  transfer advertise  event to uart
 */
void app_rpc_send_advertisement_state_event(uint8_t state)
{
    app_rpc_send_data(HCI_CONTROL_LE_EVENT_ADVERTISEMENT_STATE, &state, 1);
}

void app_rpc_send_advertisement_state(uint32_t state)
{
    uint8_t tx_buff[4];
    uint8_t *p_buff = &tx_buff[0];

    WICED_BT_TRACE("[%s] adv state %d \n", __FUNCTION__, state);
    UINT32_TO_STREAM(p_buff, state);

    app_rpc_send_data(HCI_CONTROL_LE_EVENT_EXT_ADVERTISEMENT_STATE, tx_buff, (int)(p_buff - tx_buff));
}
