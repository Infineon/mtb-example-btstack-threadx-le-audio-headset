/*
 * $ Copyright Cypress Semiconductor $
 */

#ifndef __LEHS_RPC_H__
#define __LEHS_RPC_H__

wiced_bool_t lehs_rpc_rx_callback(uint16_t opcode, uint8_t *p_data, uint32_t payload_len);

void lehs_rpc_send_play_status(uint16_t conn_id, uint8_t play_status);
void lehs_rpc_send_device_role_event(uint8_t dev_role);
void lehs_rpc_send_get_players_event(uint16_t conn_id, char *name);
wiced_bool_t lehs_rpc_is_dev_role_sink(void);
void lehs_rpc_send_new_stream_info(uint32_t broadcast_id, const uint8_t *br_name);

void lehs_terminate_incoming_call(uint16_t conn_id,
                                  uint8_t call_id,
                                  uint8_t reason);

void lehs_handle_call_control_point_action(uint16_t conn_id,
                                           uint8_t call_id,
                                           uint8_t opcode);

void lehs_rpc_update_call_friendly_name(uint16_t conn_id, char * f_name, uint8_t len);

#ifdef SIMULATED_NVRAM
void lehs_rpc_send_identity_resolving_key(wiced_bt_local_identity_keys_t *p_id_keys);
void lehs_rpc_send_link_keys(uint16_t nvram_id, lehs_nvram_paired_device_key_t *p_key_data);
#endif



#endif
