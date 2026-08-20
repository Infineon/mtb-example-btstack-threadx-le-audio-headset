/*
 * $ Copyright Cypress Semiconductor $
 */

#include "le_audio_cap.h"
#include "lehs.h"

#define MAX_CODEC_RETRANSMISSION_NUMBER 0x0D

#define MIN_SUPPORTED_PRESENTATION_DELAY 0
#define MAX_SUPPORTED_PRESENTATION_DELAY 40000 //0x9c40

typedef struct
{
    uint16_t gatt_ase_value_handle;
    uint8_t ase_id;
    ga_lib_ascs_characteristics_t ase_type;
} lehs_ase_init_data_t;

const lehs_ase_init_data_t lehs_ases[] = {{
                                              .gatt_ase_value_handle = HDLC_ASCS_ASE_SINK_VALUE,
                                              .ase_id = 10,
                                              .ase_type = GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE,
                                          },
                                          {
                                              .gatt_ase_value_handle = HDLC_ASCS_ASE_SINK_2_VALUE,
                                              .ase_id = 11,
                                              .ase_type = GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE,
                                          },
                                          {
                                              .gatt_ase_value_handle = HDLC_ASCS_ASE_SOURCE_VALUE,
                                              .ase_id = 12,
                                              .ase_type = GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE,
                                          }};

void lehs_init_local_ase_data(lehs_clcb_t *p_clcb)
{
    lehs_ase_data_t *p_lehs_ase = p_clcb->local_ase_data;
    const lehs_ase_init_data_t *p_init = lehs_ases;
    int num_ase = LEHS_ASE_INDEX_MAX;

    WICED_BT_TRACE("[%s]", __FUNCTION__);

    while (num_ase--)
    {
        ga_lib_ascs_ase_t *p_ase = &p_lehs_ase->ase;

        WICED_MEMSET(p_lehs_ase, 0, sizeof(lehs_ase_data_t));

        p_lehs_ase->gatt_ase_value_handle = p_init->gatt_ase_value_handle;
        p_lehs_ase->acl_conn_handle = wiced_bt_gatt_get_acl_conn_handle(p_clcb->conn_id);
        p_ase->ase_id = p_init->ase_id;
        p_ase->ase_type = p_init->ase_type;
        p_ase->ase_state = GA_LIB_ASCS_STATE_IDLE;
        p_ase->data_path_dir =
            (p_ase->ase_type == GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE) ? WICED_BLE_ISOC_DPD_INPUT : WICED_BLE_ISOC_DPD_OUTPUT;
        //memcpy(&p_ase->ase_cfg, &ascs_remote_codec_config, sizeof(ga_lib_ascs_ase_codec_config_data_t));

        p_ase->qos_configured.cig_id = 0xFF;
        p_ase->qos_configured.cis_id = 0xFF;

        WICED_BT_TRACE("[%s] 0x%x ase_id %d, ase_state 0x%x type %s cis_handle %d",
                       __FUNCTION__,
                       p_lehs_ase,
                       p_ase->ase_id,
                       p_ase->ase_state,
                       (p_ase->ase_type == GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE) ? "src" : "sink",
                       p_lehs_ase->cis_conn_handle);


        p_lehs_ase++;
        p_init++;
    }
}

lehs_ase_data_t *lehs_find_ase_with_ase_id(lehs_clcb_t *p_clcb, uint8_t ase_id)
{
    lehs_ase_data_t *p_lehs_ase = p_clcb->local_ase_data;
    int num_ase = LEHS_ASE_INDEX_MAX;

    while (num_ase--)
    {
        if (p_lehs_ase->ase.ase_id == ase_id)
        {
            return p_lehs_ase;
        }

        p_lehs_ase++;
    }

    return NULL;
}

#pragma pack(1)
typedef struct
{
    uint8_t opcode;
    uint8_t num_ases;
    ga_lib_ascs_cp_cmd_sts_t status[LEHS_ASE_INDEX_MAX];
} cp_notif_data_t;
#pragma pack()

void lehs_ascs_notify_control_point_response(uint16_t conn_id, cp_notif_data_t *p_n)
{
    uint8_t buff[64];
    uint8_t *ptr = buff;
    int max_len = sizeof(buff);

    // if the response code is Unsupported Opcode / Invalid Length num_ases is set to 0xFF
    if ((GA_LIB_ASCS_CP_RESPONSE_UNSUPPORTED_OPCODE == p_n->status[0].response_code) ||
        (GA_LIB_ASCS_CP_RESPONSE_INVALID_LENGTH == p_n->status[0].response_code))
    {
        p_n->num_ases = 0xff;
        p_n->status[0].ase_id = 0;
    }

    UINT8_TO_STREAM(ptr, p_n->opcode);
    UINT8_TO_STREAM(ptr, p_n->num_ases);

    for (int ase_index = 0; ase_index < p_n->num_ases; ase_index++)
    {
        int remaining = max_len - (ptr - buff);
        int required = 1 + 1 + 1; /* ase_id + response_code + reason */
        ga_lib_ascs_cp_cmd_sts_t *p_status = &p_n->status[ase_index];

        if (remaining < required)
        {
            break;
        }

        UINT8_TO_STREAM(ptr, p_status->ase_id);
        UINT8_TO_STREAM(ptr, p_status->response_code);
        UINT8_TO_STREAM(ptr, p_status->reason);

        // if the response code is Unsupported Opcode / Invalid Length num_ases is set to 0xFF
        if (p_n->num_ases == 0xff)
        {
            break;
        }
    }

    gatt_intf_send_notification(conn_id, HDLC_ASCS_ASE_CONTROL_POINT_VALUE, buff, ptr - buff);
}

wiced_bool_t lehs_is_valid_ase_id(uint8_t ase_id)
{
    if(ase_id == 0 )
    {
        WICED_BT_TRACE_CRIT("[%s] invalid ASE ID %d \n", __FUNCTION__, ase_id);
        return WICED_FALSE;
    }

    const lehs_ase_init_data_t *p_init = lehs_ases;
    int limit = sizeof(lehs_ases) / sizeof(lehs_ase_init_data_t);

    while (limit--)
    {
        if(p_init->ase_id == ase_id)
        {
            return WICED_TRUE;
        }
        p_init++;
    }
    return WICED_FALSE;
}


void get_preferred_ascs_data(lehs_ase_data_t *p_lehs_ase)
{
    ga_lib_ascs_ase_codec_config_data_t *p_cfg = &p_lehs_ase->ase.ase_cfg;

    if (p_cfg->target_latency == 1)
    {
        p_cfg->max_transport_latency = (p_cfg->csc.frame_duration_us == 7500) ? 15 : 20;
        p_cfg->presentation_delay_in_us_max =
            (p_lehs_ase->ase.ase_type == GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE) ? 60000 : 10000;
        p_cfg->preferred_retransmission_number = 1;
        p_cfg->preferred_presentation_delay_in_us_max = p_cfg->presentation_delay_in_us_max;
    }
    else
    {
        p_cfg->max_transport_latency = 0x64;
        p_cfg->presentation_delay_in_us_max = 40000;
        p_cfg->preferred_retransmission_number = 4;
        p_cfg->preferred_presentation_delay_in_us_max = p_cfg->presentation_delay_in_us_max;
    }
    p_cfg->preferred_phy = GA_LIB_ASCS_PHY_2M;
    p_cfg->supported_framing = (p_cfg->csc.sampling_frequency_hz == 44100)
                                   ? GA_LIB_ASCS_UNFRAMED_ISOAL_PDUS_NOT_SUPPORTED
                                   : GA_LIB_ASCS_UNFRAMED_ISOAL_PDUS_SUPPORTED;
}

/*
* Validate the write request received on ASCS control point characteristic, this will be called from GATT write handler
*/
ga_lib_ascs_cp_response_t lehs_ascs_check_write_req(lehs_clcb_t *p_clcb,
                                                       uint8_t *p_stream,
                                                       int length,
                                                       cp_notif_data_t * p_n,
                                                       wiced_bool_t apply)
{
    ga_lib_ascs_cp_cmd_sts_t *p_sts = p_n->status;
    uint8_t *p_data = p_stream;

    STREAM_TO_UINT8(p_n->opcode, p_data);
    STREAM_TO_UINT8(p_n->num_ases, p_data);

    if (p_n->opcode > GA_LIB_ASCS_OPCODE_MAX)
    {
        WICED_BT_TRACE_CRIT("[%s] invalid opcode %d \n", __FUNCTION__, p_n->opcode);
        return GA_LIB_ASCS_CP_RESPONSE_UNSUPPORTED_OPCODE;
    }

    if (!p_n->num_ases || (p_n->num_ases > LEHS_ASE_INDEX_MAX))
    {
        WICED_BT_TRACE_CRIT("[%s] invalid number of ASEs %d \n", __FUNCTION__, p_n->num_ases);
        return GA_LIB_ASCS_CP_RESPONSE_INVALID_ASE_ID;
    }

    uint8_t num_ases = 0;
    /* Ase specific errors */
    while (length > (p_data - p_stream) )
    {
        num_ases++;
        STREAM_TO_UINT8(p_sts->ase_id, p_data); // read ase_id

        lehs_ase_data_t *p_lehs_ase = lehs_find_ase_with_ase_id(p_clcb, p_sts->ase_id);
        if (!p_lehs_ase)
        {
            /* break on invalid ASE id */
            p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_ASE_ID;
            break;
        }
        ga_lib_ascs_ase_t *p_ase = &p_lehs_ase->ase;
        uint8_t next_state;
        if(!ga_lib_bap_is_state_transition_valid(p_ase->ase_type, p_ase->ase_state, p_n->opcode, &next_state))
        {
            p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_ASE_STATE_MACHINE_TRANSITION;
            return p_sts->response_code;
        }

        WICED_BT_TRACE("[%s] bytes used %d of %d st %d next %d", __FUNCTION__, p_data - p_stream, length, p_ase->ase_state, next_state);

        switch (p_n->opcode)
        {
        case GA_LIB_ASCS_OPCODE_CONFIG_CODEC:
        {
            ga_lib_ascs_config_codec_args_t cc = {0};

            p_data += ga_lib_ascs_parse_codec_config(p_data, length - (p_data - p_stream), &cc, p_sts);

            if(p_sts->response_code != GA_LIB_ASCS_CP_RESPONSE_SUCCESS)
            {
                return p_sts->response_code;
            }

            {
                wiced_bool_t is_valid_cfg = WICED_FALSE;
                if(p_ase->ase_type == GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE)
                {
                    is_valid_cfg = lehs_pacs_verify_snk_codec_config(&cc);
                }
                else {
                    is_valid_cfg = lehs_pacs_verify_src_codec_config(&cc);
                }
                if (is_valid_cfg == WICED_FALSE)
                {
                    p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_UNSUPPORTED_CONFIGURATION_PARAMETER_VALUE;
                    p_sts->reason = GA_LIB_ASCS_CP_REASON_CODEC_SPECIFIC_CONFIGURATION;
                    return p_sts->response_code;
                }

                if(apply)
                {
                    p_ase->ase_cfg.target_latency = cc.target_latency;
                    p_ase->ase_cfg.target_phy = cc.target_phy;
                    memcpy(&p_ase->ase_cfg.codec_id, &cc.codec_id, sizeof(p_ase->ase_cfg.codec_id));


                    if (cc.csc.audio_channel_allocation == 0)
                    {
                        cc.csc.audio_channel_allocation = 1;
                    }


                    memcpy(&p_ase->ase_cfg.csc, &cc.csc, sizeof(p_ase->ase_cfg.csc));
                    get_preferred_ascs_data(p_lehs_ase);
                }
            }
        }
        break;
        case GA_LIB_ASCS_OPCODE_CONFIG_QOS:
        {
            ga_lib_ascs_config_qos_args_t qos;
            p_data += ga_lib_ascs_parse_config_qos(p_data, length - (p_data - p_stream), &qos, p_sts);
            if(p_sts->response_code == GA_LIB_ASCS_CP_RESPONSE_SUCCESS)
            {
                memcpy(&p_ase->qos_configured, &qos, sizeof(p_ase->qos_configured));
                p_lehs_ase->cis_conn_handle =
                    wiced_ble_isoc_get_cis_conn_handle(qos.cig_id, qos.cis_id, p_lehs_ase->acl_conn_handle);

            }
        }
        break;

        case GA_LIB_ASCS_OPCODE_ENABLE:
        case GA_LIB_ASCS_OPCODE_UPDATE_METADATA:
        {
            p_data += ga_lib_ascs_parse_metadata(p_data,
                                 length - (p_data - p_stream),
                                 &p_ase->metadata,
                                 p_sts);
            if (!lehs_pacs_verify_context_type(p_ase->ase_type, p_ase->metadata.streaming_audio_ctx))
            {
                p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_UNSUPPORTED_METADATA;
                p_sts->reason = GA_LIB_ASCS_CP_REASON_NOT_APPLICABLE;
                return p_sts->response_code;
             }
        }
        break;
        case GA_LIB_ASCS_OPCODE_RECEIVER_START_READY:
        {
            if(apply)
            {
                wiced_result_t data_path_setup_sts = WICED_ERROR;
                data_path_setup_sts = lehs_isoc_dhm_setup_cis_stream(p_lehs_ase);
                if (data_path_setup_sts)
                {
                    WICED_BT_TRACE_CRIT("[%s] data path setup unsuccessful..(err:%d)\n",
                                        __FUNCTION__,
                                        data_path_setup_sts);
                    return WICED_ERROR;
                }
            }
        }break;
        case GA_LIB_ASCS_OPCODE_DISABLE:
        case GA_LIB_ASCS_OPCODE_RELEASE:
        {
            if (apply)
            {
                lehs_isoc_dhm_free_cis_stream(p_lehs_ase->cis_conn_handle, WICED_BLE_ISOC_DPD_OUTPUT_BIT);
            }
        }
        break;
        case GA_LIB_ASCS_OPCODE_RECEIVER_STOP_READY:
        {
            // If server is source stop sending audio data
            // The Unicast Server in the Audio Source role should not stop transmitting audio
            // data for a Source ASE in the Disabling state until the Unicast Server transitions
            // the ASE to the QoS Configured state.
            if (WICED_BLE_ISOC_DPD_INPUT_BIT == p_ase->ase_type)
            {
                lehs_isoc_audio_stop_stream(p_lehs_ase->cis_conn_handle);
            }
        }break;

        default:
        {
            p_n->status[0].ase_id = 0xff;
            p_n->status[0].response_code = GA_LIB_ASCS_CP_RESPONSE_UNSUPPORTED_OPCODE;
            return p_n->status[0].response_code;
        }
        break;
        }


        WICED_BT_TRACE("[%s] len %d %d rsp %d\n", __FUNCTION__, length, (p_data - p_stream), p_sts->response_code);
        if(p_sts->response_code == GA_LIB_ASCS_CP_RESPONSE_INVALID_LENGTH)
        {
                return p_sts->response_code;
        }
        if(p_sts->response_code == GA_LIB_ASCS_CP_RESPONSE_SUCCESS)
        {
            if(apply)
            {
                p_ase->ase_state = next_state;
            }
        }

        p_sts++;
    }

    WICED_BT_TRACE("[%s] opcode %d %s num_ases %d %d",
        __FUNCTION__, p_n->opcode, ga_lib_ascs_get_opcode_string(p_n->opcode), p_n->num_ases, p_sts - p_n->status);

    return GA_LIB_ASCS_CP_RESPONSE_SUCCESS;
}

wiced_result_t lehs_ascs_handle_write_req_evt(uint16_t conn_id, lehs_clcb_t *p_clcb, uint8_t *p_stream, int length)
{
    cp_notif_data_t cp_notif_data = {0}, *p_n = &cp_notif_data;

    ga_lib_ascs_cp_response_t response_code =
        lehs_ascs_check_write_req(p_clcb, p_stream, length, &cp_notif_data, WICED_FALSE);

    if (response_code != GA_LIB_ASCS_CP_RESPONSE_SUCCESS)
    {
        WICED_BT_TRACE_CRIT("[%s] parse failed %d \n", __FUNCTION__, response_code);

        if (response_code == GA_LIB_ASCS_CP_RESPONSE_UNSUPPORTED_OPCODE ||
            response_code == GA_LIB_ASCS_CP_RESPONSE_INVALID_LENGTH)
        {
            cp_notif_data.num_ases = 0xff;
            cp_notif_data.status[0].response_code = response_code;
        }
        lehs_ascs_notify_control_point_response(conn_id, &cp_notif_data);

        return WICED_ERROR;
    }

    lehs_ascs_notify_control_point_response(conn_id, &cp_notif_data);

    lehs_ascs_check_write_req(p_clcb, p_stream, length, &cp_notif_data, WICED_TRUE);

    WICED_BT_TRACE("[%s] opcode %d %s num_ases %d", __FUNCTION__, p_n->opcode, ga_lib_ascs_get_opcode_string(p_n->opcode), p_n->num_ases);

    for (int i = 0; i < p_n->num_ases; i++)
    {
        lehs_ase_data_t *p_lehs_ase = lehs_find_ase_with_ase_id(p_clcb, p_n->status[i].ase_id);
        if (p_lehs_ase)
        {
            ga_lib_ascs_notify_ase_state(conn_id, p_lehs_ase->gatt_ase_value_handle, &p_lehs_ase->ase);

            if (wiced_ble_isoc_is_cis_connected_with_conn_hdl(p_lehs_ase->cis_conn_handle))
            {
                continue;
            }

            if (p_lehs_ase->ase.ase_state == GA_LIB_ASCS_STATE_RELEASING)
            {
                p_lehs_ase->ase.ase_state = GA_LIB_ASCS_STATE_IDLE;
                ga_lib_ascs_notify_ase_state(conn_id, p_lehs_ase->gatt_ase_value_handle, &p_lehs_ase->ase);
            }
            else if (p_lehs_ase->ase.ase_state == GA_LIB_ASCS_STATE_DISABLING)
            {
                p_lehs_ase->ase.ase_state = GA_LIB_ASCS_STATE_QOS_CONFIGURED;
                ga_lib_ascs_notify_ase_state(conn_id, p_lehs_ase->gatt_ase_value_handle, &p_lehs_ase->ase);
            }
        }
        else
        {
            WICED_BT_TRACE_CRIT("[%s] no ase found for %d",__FUNCTION__, p_n->status[i].ase_id);
        }
    }

    return WICED_SUCCESS;
}
