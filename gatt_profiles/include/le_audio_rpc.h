/*
 * $ Copyright Cypress Semiconductor $
 */

#ifndef LE_AUDIO_RPC_H
#define LE_AUDIO_RPC_H

#include "wiced_data_types.h"
#include "wiced_bt_ble.h"
#include "wiced_bt_gatt.h"
#include "hci_control_api.h"
#include "app_rpc.h"
#include "gatt_interface.h"
#include "ga_lib_aics.h"

void le_audio_rpc_send_data(int type, uint8_t *p_data, uint16_t data_size);

void le_audio_rpc_send_misc_event(uint32_t chip, uint8_t group);
void le_audio_rpc_send_dev_role(uint32_t dev_role);

void le_audio_rpc_send_broadcast_status_update(uint32_t status);

void le_audio_rpc_send_vcs_state_update(uint16_t conn_id,
                                        uint8_t volume_setting,
                                        uint8_t mute_state,
                                        uint8_t which_vcs_data);
void le_audio_rpc_update_call_state(uint16_t conn_id, uint8_t call_id, char *p_call_uri, uint8_t call_state);
void le_audio_rpc_send_call_terminated_event(uint16_t conn_id, uint8_t call_id, uint8_t termination_reason);

void le_audio_rpc_send_micp_aics_description(uint16_t conn_id, uint32_t instance, char *p_desc);
void le_audio_rpc_send_micp_mute_state(uint16_t conn_id, uint8_t mute_state);
void le_audio_rpc_send_micp_aics_input_state(uint16_t conn_id,
                                             uint32_t instance,
                                             ga_lib_aics_input_state_t *p_input_state);
void le_audio_rpc_send_preset_record(uint16_t conn_id, uint8_t preset_index, char *p_name, uint8_t name_len);
void le_audio_rpc_update_active_preset(uint16_t conn_id, uint8_t preset_index);

void le_audio_rpc_send_mcs_state_update(uint16_t conn_id, uint8_t state);
void le_audio_rpc_send_mic_state_update(uint16_t conn_id, uint8_t state);
void le_audio_rpc_send_convo_stream_state_update(uint16_t conn_id, uint8_t state);

void le_audio_rpc_send_mics_mute_state(uint16_t conn_id, uint8_t mute_state);
void le_audio_rpc_send_mics_aics_description(uint16_t conn_id, uint32_t instance, char *p_desc);
void le_audio_rpc_send_mics_aics_input_state(uint16_t conn_id,
                                             uint32_t instance,
                                             ga_lib_aics_input_state_t *p_input_state);

#endif /* LE_AUDIO_RPC_H */
