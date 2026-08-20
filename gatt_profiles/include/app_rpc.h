/*
 * $ Copyright Cypress Semiconductor $
 */

#ifndef APP_RPC_H
#define APP_RPC_H

#include "wiced_data_types.h"
#include "wiced_bt_ble.h"
#include "wiced_bt_gatt.h"
#include "hci_control_api.h"

typedef wiced_bool_t (*app_rpc_cback_t)(uint16_t opcode, uint8_t *p_data, uint32_t data_len);

void priv_app_rpc_handle_wiced_hci_data(uint16_t opcode, uint8_t *p_data, uint32_t data_len);

void app_rpc_init(int host_instance, app_rpc_cback_t app_rpc_cback, wiced_bool_t b_route_traces_to_CC);

typedef void (*route_data_to_client_control_t)(uint8_t type, uint8_t *buffer, uint16_t length, uint8_t spy_instance);

void app_rpc_send_data(int type, uint8_t *p_data, uint16_t data_size);

void app_rpc_send_connect_event(wiced_bt_gatt_connection_status_t *p_status);
void app_rpc_send_disconnect_evt(wiced_bt_gatt_connection_status_t *p_status);

void app_rpc_send_misc_event(uint32_t chip, uint8_t group);
void app_rpc_send_dev_role(uint32_t dev_role);

void app_rpc_le_send_advertisement_state_event(uint8_t state);
void app_rpc_send_advertisement_state(uint32_t state);
void app_rpc_send_scan_res_event(wiced_ble_ext_scan_results_t *p_scr, uint8_t *p_adv_data);
void app_rpc_send_app_status(uint16_t conn_id,
                             wiced_bt_device_address_t dev_addr,
                             uint32_t init_state,
                             uint32_t sub_state);
void app_rpc_send_app_sub_status(uint16_t conn_id,
                                 wiced_bt_device_address_t dev_addr,
                                 uint32_t init_state,
                                 uint32_t sub_state,
                                 const wiced_bt_uuid_t *p_uuid);

uint16_t nvram_memory_read(uint16_t id, uint16_t buf_len, uint8_t *p_buf);

uint16_t nvram_memory_write(uint16_t id, uint16_t data_len, const uint8_t *p_data);

#endif /* APP_RPC_H */
