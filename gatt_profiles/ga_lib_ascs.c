/*
 * $ Copyright Cypress Semiconductor $
 */

#include "ga_lib_bap.h"

#ifdef ASCS_DEBUG
#define ASCS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get MICS library traces */
#else
#define ASCS_TRACE(...)
#endif


#define ASCS_MAX_HANDLES(p) ((p)->max_snk_ase_spt + (p)->max_src_ase_spt + ASCS_MAX_UNIQUE_CHARACTERISTIC)
#define ASCS_VALID_HANDLES(p) ((p)->num_sink_ase_handles + (p)->num_source_ase_handles + ASCS_MAX_UNIQUE_CHARACTERISTIC)

const gatt_intf_characteristic_info_t ascs_characteristic_info[] = {
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_ASCS_ASE_CONTROL_POINT},
     .characteristic_type = GA_LIB_ASCS_CHARACTERISTIC_ASE_CONTROL_POINT,
     .name = "cp"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_ASCS_SINK_ASE},
     .characteristic_type = GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE,
     .name = "snk"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_ASCS_SOURCE_ASE},
     .characteristic_type = GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE,
     .name = "src"}};

const gatt_intf_service_name_uuid_t ascs_id = {
    .name = "ascs",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_AUDIO_STREAM_CONTROL,
};

gatt_intf_service_discovery_ctx_t *ga_lib_ascs_discover_service(uint16_t conn_id,
                                                             pfn_on_discovery_complete_t pfn_on_ascs_complete,
                                                             uint8_t max_snk_ase,
                                                             uint8_t max_src_ase)
{
    int num_characteristics = GA_LIB_ASCS_CHARACTERISTIC_MAX_UNIQUE + max_snk_ase + max_src_ase;
    gatt_intf_service_discovery_ctx_t *p_sdc =
        gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, sizeof(gatt_intf_service_discovery_ctx_t));
    if (!p_sdc)
        return NULL;

    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_ascs_complete);

    uint8_t start =
        gatt_intf_copy_unique_characteristic_info(p_sdc, ascs_characteristic_info, GA_LIB_ASCS_CHARACTERISTIC_MAX_UNIQUE);
    start = gatt_intf_repeat_characteristic_info(p_sdc,
                                                 start,
                                                 &ascs_characteristic_info[GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE],
                                                 max_snk_ase);
    start = gatt_intf_repeat_characteristic_info(p_sdc,
                                                 start,
                                                 &ascs_characteristic_info[GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE],
                                                 max_src_ase);

    gatt_intf_start_service_discovery(conn_id, &ascs_id, 1, p_sdc);

    return p_sdc;
}

// Traces for Debugging
#define STR(x) #x
/* GA_LIB_BAP_VENDOR_SPECIFIC_CODING_FORMAT (0xFF) is now defined in ga_lib_bap.h */

const char *ascs_state_str[] = {
    STR(GA_LIB_ASCS_STATE_IDLE),
    STR(GA_LIB_ASCS_STATE_CODEC_CONFIGURED),
    STR(GA_LIB_ASCS_STATE_QOS_CONFIGURED),
    STR(GA_LIB_ASCS_STATE_ENABLING),
    STR(GA_LIB_ASCS_STATE_STREAMING),
    STR(GA_LIB_ASCS_STATE_DISABLING),
    STR(GA_LIB_ASCS_STATE_RELEASING),
    STR(GA_LIB_ASCS_STATE_MAX),
};

const char *ascs_opcode_str[] = {
    STR(GA_LIB_ASCS_OPCODE_INVALID),
    STR(GA_LIB_ASCS_OPCODE_CONFIG_CODEC),
    STR(GA_LIB_ASCS_OPCODE_CONFIG_QOS),
    STR(GA_LIB_ASCS_OPCODE_ENABLE),
    STR(GA_LIB_ASCS_OPCODE_RECEIVER_START_READY),
    STR(GA_LIB_ASCS_OPCODE_DISABLE),
    STR(GA_LIB_ASCS_OPCODE_RECEIVER_STOP_READY),
    STR(GA_LIB_ASCS_OPCODE_UPDATE_METADATA),
    STR(GA_LIB_ASCS_OPCODE_RELEASE),
    STR(GA_LIB_ASCS_OPCODE_RELEASED),
    STR(GA_LIB_ASCS_OPCODE_MAX),
};


static uint32_t fill_qos_params(const ga_lib_ascs_ase_t *p_ase, uint8_t *p_dst, int max_len);
static uint32_t fill_common_params(const ga_lib_ascs_ase_t *p_ase, uint8_t *p_dst, int max_len);


static uint32_t fill_codec_params(const ga_lib_ascs_ase_t *p_ase, uint8_t *p_dst, int max_len)
{
    uint8_t *p_dst_cache = p_dst;
    const ga_lib_ascs_ase_codec_config_data_t *p_pref = &p_ase->ase_cfg;

    UINT8_TO_STREAM(p_dst, p_pref->supported_framing);
    UINT8_TO_STREAM(p_dst, p_pref->preferred_phy);
    UINT8_TO_STREAM(p_dst, p_pref->preferred_retransmission_number);
    UINT16_TO_STREAM(p_dst, p_pref->max_transport_latency);

    ARRAY_TO_STREAM(p_dst, &p_pref->presentation_delay_in_us_min, PRESENTATION_DELAY_LENGTH);
    ARRAY_TO_STREAM(p_dst, &p_pref->presentation_delay_in_us_max, PRESENTATION_DELAY_LENGTH);
    ARRAY_TO_STREAM(p_dst, &p_pref->preferred_presentation_delay_in_us_min, PRESENTATION_DELAY_LENGTH);
    ARRAY_TO_STREAM(p_dst, &p_pref->preferred_presentation_delay_in_us_max, PRESENTATION_DELAY_LENGTH);

    UINT8_TO_STREAM(p_dst, p_pref->codec_id.coding_format);
    UINT16_TO_STREAM(p_dst, p_pref->codec_id.company_id);
    UINT16_TO_STREAM(p_dst, p_pref->codec_id.vendor_specific_codec_id);

    p_dst += ga_lib_bap_fill_csc(p_dst, max_len - (p_dst - p_dst_cache), &p_ase->ase_cfg.csc);
    return (p_dst - p_dst_cache);
}

static uint32_t fill_qos_params(const ga_lib_ascs_ase_t *p_ase, uint8_t *p_dst, int max_len)
{
    uint8_t *p_dst_cache = p_dst;
    const ga_lib_ascs_config_qos_args_t *qos_configured = &p_ase->qos_configured;

    ASCS_TRACE("[%s]\n", __FUNCTION__);

    UINT8_TO_STREAM(p_dst, qos_configured->cig_id);
    UINT8_TO_STREAM(p_dst, qos_configured->cis_id);

    ARRAY_TO_STREAM(p_dst, (uint8_t *)&qos_configured->sdu_interval, SDU_INTERVAL_LENGTH);

    UINT8_TO_STREAM(p_dst, qos_configured->framing);
    UINT8_TO_STREAM(p_dst, qos_configured->phy);
    UINT16_TO_STREAM(p_dst, qos_configured->max_sdu);
    UINT8_TO_STREAM(p_dst, qos_configured->retransmission_number);
    UINT16_TO_STREAM(p_dst, qos_configured->max_transport_latency);

    ARRAY_TO_STREAM(p_dst, &qos_configured->presentation_delay, PRESENTATION_DELAY_LENGTH);

    return (p_dst - p_dst_cache);
}

static uint32_t fill_common_params(const ga_lib_ascs_ase_t *p_ase, uint8_t *p_dst, int max_len)
{
    uint8_t *p_dst_cache = p_dst;
    uint8_t *p_metadata_len = NULL;
    uint8_t bytes_written = 0;
    const ga_lib_ascs_metadata_t *p_meta = &p_ase->metadata;
    const ga_lib_ascs_config_qos_args_t *p_qos = &p_ase->qos_configured;

    UINT8_TO_STREAM(p_dst, p_qos->cig_id);
    UINT8_TO_STREAM(p_dst, p_qos->cis_id);

    p_metadata_len = p_dst++;
    *p_metadata_len = 0;

    ASCS_TRACE("[%s] pref %d str %d vs 0x%x %d",
               __FUNCTION__,
               p_meta->preferred_audio_ctx,
               p_meta->streaming_audio_ctx,
               p_meta->p_vendor_specific_data,
               p_meta->vendor_specific_data_length);

    if (p_meta->preferred_audio_ctx != 0)
    {
        bytes_written += ga_lib_bap_update_ltv(p_dst,
                                               max_len - (p_dst - p_dst_cache),
                                               sizeof(uint16_t),
                                               BAP_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS,
                                               (const uint8_t *)&p_meta->preferred_audio_ctx);
        p_dst += bytes_written;
        *p_metadata_len += bytes_written;
    }

    if (p_meta->streaming_audio_ctx != 0)
    {
        bytes_written += ga_lib_bap_update_ltv(p_dst,
                                               max_len - (p_dst - p_dst_cache),
                                               sizeof(uint16_t),
                                               BAP_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS,
                                               (const uint8_t *)&p_meta->streaming_audio_ctx);
        p_dst += bytes_written;
        *p_metadata_len += bytes_written;
    }

    return (p_dst - p_dst_cache);
}

// to be used to create packets in the OTA format from data structures for read response and notifications
uint16_t ga_lib_ascs_build_ase_response(ga_lib_ascs_ase_t *p_ase, uint8_t *p_dest, int max_len)
{
    uint8_t *ptr = p_dest;

    // multiple ASE per server possible and each client gets its own instance
    // the format of ASE value changes dynamically based on the ase_state machine
    // ASE_ID + ASE_State + Additional parameters
    UINT8_TO_STREAM(ptr, p_ase->ase_id);
    UINT8_TO_STREAM(ptr, p_ase->ase_state);

    // get the additional parameter based on the current ase_state and the client
    switch (p_ase->ase_state)
    {
    case GA_LIB_ASCS_STATE_CODEC_CONFIGURED:
        ptr += fill_codec_params(p_ase, ptr, max_len - (ptr - p_dest));
        break;
    case GA_LIB_ASCS_STATE_QOS_CONFIGURED:
        ptr += fill_qos_params(p_ase, ptr, max_len - (ptr - p_dest));
        break;
    case GA_LIB_ASCS_STATE_ENABLING:
    case GA_LIB_ASCS_STATE_STREAMING:
    case GA_LIB_ASCS_STATE_DISABLING:
        ptr += fill_common_params(p_ase, ptr, max_len - (ptr - p_dest));
        break;
    default:
        break;
    }

    return (ptr - p_dest);
}

wiced_bt_gatt_status_t ga_lib_ascs_notify_ase_state(uint16_t conn_id, uint16_t handle, ga_lib_ascs_ase_t *p_ase)
{
    uint8_t buff[256];
    uint8_t *p_ptr = buff;

    ASCS_TRACE("[%s] conn_id %d ase_id %d", __FUNCTION__, conn_id, p_ase->ase_id);

    p_ptr += ga_lib_ascs_build_ase_response(p_ase, p_ptr, sizeof(buff) - (p_ptr - buff));

    return gatt_intf_send_notification(conn_id, handle, buff, p_ptr - buff);
}

const char *ga_lib_ascs_get_opcode_string(ga_lib_ascs_opcode_t ascs_opcode)
{
    if (ascs_opcode < sizeof(ascs_opcode_str) / sizeof(ascs_opcode_str[0]))
    {
        return ascs_opcode_str[ascs_opcode];
    }

    return "unknown";
}

int ga_lib_ascs_build_cp_config_codec(uint8_t *p_start, int len, uint8_t ase_id, ga_lib_ascs_config_codec_args_t *p_cc)
{
    uint8_t *p_data = p_start;

    UINT8_TO_STREAM(p_data, ase_id);
    UINT8_TO_STREAM(p_data, p_cc->target_latency);
    UINT8_TO_STREAM(p_data, p_cc->target_phy);
    UINT8_TO_STREAM(p_data, p_cc->codec_id.coding_format);
    UINT16_TO_STREAM(p_data, p_cc->codec_id.company_id);
    UINT16_TO_STREAM(p_data, p_cc->codec_id.vendor_specific_codec_id);
    p_data += ga_lib_bap_fill_csc(p_data, len - (p_data - p_start), &p_cc->csc);

    return (p_data - p_start);
}

int ga_lib_ascs_build_cp_config_qos(uint8_t *p_start, int len, uint8_t ase_id, ga_lib_ascs_config_qos_args_t *p_qos)
{
    uint8_t *p_data = p_start;

    UINT8_TO_STREAM(p_data, ase_id);
    UINT8_TO_STREAM(p_data, p_qos->cig_id);
    UINT8_TO_STREAM(p_data, p_qos->cis_id);
    UINT24_TO_STREAM(p_data, p_qos->sdu_interval);
    UINT8_TO_STREAM(p_data, p_qos->framing);
    UINT8_TO_STREAM(p_data, p_qos->phy);
    UINT16_TO_STREAM(p_data, p_qos->max_sdu);
    UINT8_TO_STREAM(p_data, p_qos->retransmission_number);
    UINT16_TO_STREAM(p_data, p_qos->max_transport_latency);
    UINT24_TO_STREAM(p_data, p_qos->presentation_delay);

    return (p_data - p_start);
}

int ga_lib_ascs_build_cp_metadata_cmd(uint8_t *p_start, int len, uint8_t ase_id, ga_lib_ascs_metadata_t *p_metadata)
{
    uint8_t *p_data = p_start;
    const ga_lib_ascs_metadata_t *p_meta = p_metadata;

    UINT8_TO_STREAM(p_data, ase_id);
    // reserve space for metadata length and fill the actual metadata after filling the rest of the parameters
    uint8_t *p_metadata_len = p_data++;
    *p_metadata_len = 0;

    if (p_meta->preferred_audio_ctx != 0)
    {
        int ltv_len = ga_lib_bap_update_ltv(p_data,
                                            len - (p_data - p_start),
                                            sizeof(uint16_t),
                                            BAP_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS,
                                            (const uint8_t *)&p_meta->preferred_audio_ctx);
        *p_metadata_len += ltv_len;
        p_data += ltv_len;
    }
    if (p_meta->streaming_audio_ctx != 0)
    {
        int ltv_len = ga_lib_bap_update_ltv(p_data,
                                            len - (p_data - p_start),
                                            sizeof(uint16_t),
                                            BAP_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS,
                                            (const uint8_t *)&p_meta->streaming_audio_ctx);
        *p_metadata_len += ltv_len;
        p_data += ltv_len;
    }
    if (p_meta->program_info_len)
    {
        int ltv_len = ga_lib_bap_update_ltv(p_data,
                                            len - (p_data - p_start),
                                            p_meta->program_info_len,
                                            BAP_METADATA_TYPE_PROGRAM_INFO,
                                            p_meta->p_program_info);
        *p_metadata_len += ltv_len;
        p_data += ltv_len;
    }
    if (p_meta->vendor_specific_data_length)
    {
        int ltv_len = ga_lib_bap_update_ltv(p_data,
                                            len - (p_data - p_start),
                                            p_meta->vendor_specific_data_length,
                                            BAP_METADATA_TYPE_VENDOR_SPECIFIC,
                                            p_meta->p_vendor_specific_data);
        *p_metadata_len += ltv_len;
        p_data += ltv_len;
    }
    return (p_data - p_start);
}

wiced_result_t ga_lib_ascs_send_ase_id_cmd(uint16_t conn_id,
                                        gatt_intf_characteristic_handles_t *p_handles,
                                        uint8_t opcode,
                                        uint8_t ase_id)
{
    uint8_t cp_data[3], *p_data = cp_data;

    UINT8_TO_STREAM(p_data, opcode);
    UINT8_TO_STREAM(p_data, 1); // num of ASEs
    UINT8_TO_STREAM(p_data, ase_id);

    return gatt_intf_write_no_rsp(conn_id, p_handles, cp_data, p_data - cp_data);
}

wiced_result_t ga_lib_ascs_send_receiver_start_ready(uint16_t conn_id,
                                                  gatt_intf_characteristic_handles_t *p_handles,
                                                  uint8_t ase_id)
{
    return ga_lib_ascs_send_ase_id_cmd(conn_id, p_handles, GA_LIB_ASCS_OPCODE_RECEIVER_START_READY, ase_id);
}

wiced_result_t ga_lib_ascs_send_receiver_stop_ready(uint16_t conn_id,
                                                 gatt_intf_characteristic_handles_t *p_handles,
                                                 uint8_t ase_id)
{
    return ga_lib_ascs_send_ase_id_cmd(conn_id, p_handles, GA_LIB_ASCS_OPCODE_RECEIVER_STOP_READY, ase_id);
}

wiced_result_t ga_lib_ascs_send_disable(uint16_t conn_id, gatt_intf_characteristic_handles_t *p_handles, uint8_t ase_id)
{
    return ga_lib_ascs_send_ase_id_cmd(conn_id, p_handles, GA_LIB_ASCS_OPCODE_DISABLE, ase_id);
}

wiced_result_t ga_lib_ascs_send_release(uint16_t conn_id, gatt_intf_characteristic_handles_t *p_handles, uint8_t ase_id)
{
    return ga_lib_ascs_send_ase_id_cmd(conn_id, p_handles, GA_LIB_ASCS_OPCODE_RELEASE, ase_id);
}

int count_bits(uint32_t n)
{
    int count = 0;
    while (n > 0)
    {
        n &= (n - 1); // Clears the least significant set bit
        count++;
    }
    return count;
}

const uint8_t csc_type_len[] = {
    0, // 0th index is reserved for invalid type
    2, // BAP_CODEC_CONFIG_SAMPLING_FREQUENCY_TYPE,
    2, // BAP_CODEC_CONFIG_FRAME_DURATION_TYPE,
    5, // BAP_CODEC_CONFIG_AUDIO_CHANNEL_ALLOCATION_TYPE,
    3, // BAP_CODEC_CONFIG_OCTETS_PER_CODEC_FRAME_TYPE,
    2  // BAP_CODEC_CONFIG_LC3_BLOCKS_PER_SDU_TYPE
};

uint16_t ga_lib_ascs_parse_codec_config(uint8_t *p_stream,
                                     uint16_t length,
                                     ga_lib_ascs_config_codec_args_t *p_cc,
                                     ga_lib_ascs_cp_cmd_sts_t *p_sts)
{
    // codec_specific_configuration_length is at offset 8 from ase_id
    uint8_t *p_data = p_stream;
    uint8_t csc_len;

    STREAM_TO_UINT8(p_cc->target_latency, p_data);                     // 0, 1, 1
    STREAM_TO_UINT8(p_cc->target_phy, p_data);                         // 1, 2, 2
    STREAM_TO_UINT8(p_cc->codec_id.coding_format, p_data);             // 2, 1, 3
    STREAM_TO_UINT16(p_cc->codec_id.company_id, p_data);               // 3, 2, 5
    STREAM_TO_UINT16(p_cc->codec_id.vendor_specific_codec_id, p_data); // 5, 2, 7
    STREAM_TO_UINT8(csc_len, p_data);                                  // 7, 1, 8

    WICED_BT_TRACE("[%s] tl %d tp %d cf %d c_id %d vcid %d csc_len %d %d %d",
                   __FUNCTION__,
                   p_cc->target_latency,
                   p_cc->target_phy,
                   p_cc->codec_id.coding_format,
                   p_cc->codec_id.company_id,
                   p_cc->codec_id.vendor_specific_codec_id,
                   csc_len,
                   length,
                   (p_data - p_stream));

    if ((length - (p_data - p_stream)) < csc_len)
    {
        p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_LENGTH;
        return p_data - p_stream;
    }

    while (csc_len)
    {
        uint8_t type_len, type;
        uint8_t *p_data_start = p_data;
        STREAM_TO_UINT8(type_len, p_data);
        STREAM_TO_UINT8(type, p_data);

        WICED_BT_TRACE("[%s] type %d type_len %d remaining csc_len %d", __FUNCTION__, type, type_len, csc_len);

        if (type_len > csc_len)
        {
            p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_LENGTH;
            return p_data - p_stream;
        }
        if ((type == 0) || (type > (sizeof(csc_type_len) / sizeof(csc_type_len[0]))) ||
            (type_len != csc_type_len[type]))
        {
            int expected_len = (type < (sizeof(csc_type_len) / sizeof(csc_type_len[0]))) ? csc_type_len[type] : 0;

            WICED_BT_TRACE("[%s] bad type %d type_len %d expected_len %d", __FUNCTION__, type, type_len, expected_len);
            p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_CONFIGURATION_PARAMETER_VALUE;
            p_sts->reason = GA_LIB_ASCS_CP_REASON_CODEC_SPECIFIC_CONFIGURATION;
            return p_data - p_stream;
        }

        switch (type)
        {
        case BAP_CODEC_CONFIG_SAMPLING_FREQUENCY_TYPE:
        {
            uint8_t selected_sampling_freq;
            uint32_t sampling_freq;

            STREAM_TO_UINT8(selected_sampling_freq, p_data);

            sampling_freq = ga_lib_bap_get_sampling_freq_from_index(selected_sampling_freq);

            p_cc->csc.sampling_frequency_hz = sampling_freq;
        }
        break;
        case BAP_CODEC_CONFIG_FRAME_DURATION_TYPE:
        {
            uint8_t selected_frame_duration;
            STREAM_TO_UINT8(selected_frame_duration, p_data);

            p_cc->csc.frame_duration_us = ga_lib_bap_get_frame_duration_from_index(selected_frame_duration);
        }
        break;
        case BAP_CODEC_CONFIG_AUDIO_CHANNEL_ALLOCATION_TYPE:
        {
            STREAM_TO_UINT32(p_cc->csc.audio_channel_allocation, p_data);
        }
        break;
        case BAP_CODEC_CONFIG_OCTETS_PER_CODEC_FRAME_TYPE:
        {
            STREAM_TO_UINT16(p_cc->csc.octets_per_codec_frame, p_data);
        }
        break;
        case BAP_CODEC_CONFIG_LC3_BLOCKS_PER_SDU_TYPE:
        {
            STREAM_TO_UINT8(p_cc->csc.lc3_blocks_per_sdu, p_data);
        }
        break;
        default:
            p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_UNSUPPORTED_CONFIGURATION_PARAMETER_VALUE;
            p_sts->reason = GA_LIB_ASCS_CP_REASON_CODEC_SPECIFIC_CONFIGURATION;
            return p_data - p_stream;
        }
        csc_len -= (type_len + 1);            // 1 byte for type
        p_data = p_data_start + type_len + 1; // move the pointer to the end of current type data
    }

    ga_lib_ascs_csc_t *p_csc = &p_cc->csc;
    WICED_BT_TRACE("[%s] cc sf %d fr %d aca 0x%x ocf %d bl %d ",
                   __FUNCTION__,
                   p_csc->sampling_frequency_hz,
                   p_csc->frame_duration_us,
                   p_csc->audio_channel_allocation,
                   p_csc->octets_per_codec_frame,
                   p_csc->lc3_blocks_per_sdu);

    WICED_BT_TRACE("[%s] ASE ID %d lat %d phy %d coding_fmt %d compid %d vencdc_id %d",
                   __FUNCTION__,
                   p_sts->ase_id,
                   p_cc->target_latency,
                   p_cc->target_phy,
                   p_cc->codec_id.coding_format,
                   p_cc->codec_id.company_id,
                   p_cc->codec_id.vendor_specific_codec_id);

    return p_data - p_stream;
}

uint16_t ga_lib_ascs_parse_config_qos(uint8_t *p_stream,
                                   uint16_t length,
                                   ga_lib_ascs_config_qos_args_t *p_qos,
                                   ga_lib_ascs_cp_cmd_sts_t *p_sts)
{
    uint8_t *p_data = p_stream;
    // received command should atleast be (9*num_of_ase) bytes (number_of_ASE (1) + min. size of ASE data (8))
    if (length < 15)
    {
        p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_LENGTH;
        p_sts->reason = GA_LIB_ASCS_CP_REASON_NOT_APPLICABLE;
        return p_data - p_stream;
    }

    STREAM_TO_UINT8(p_qos->cig_id, p_data);                 // 0, 1, 1
    STREAM_TO_UINT8(p_qos->cis_id, p_data);                 // 1, 1, 2
    STREAM_TO_UINT24(p_qos->sdu_interval, p_data);          // 2, 3, 5
    STREAM_TO_UINT8(p_qos->framing, p_data);                // 5, 1, 6
    STREAM_TO_UINT8(p_qos->phy, p_data);                    // 6, 1, 7
    STREAM_TO_UINT16(p_qos->max_sdu, p_data);               // 7, 2, 9
    STREAM_TO_UINT8(p_qos->retransmission_number, p_data);  // 9, 1, 10
    STREAM_TO_UINT16(p_qos->max_transport_latency, p_data); // 10, 2, 12
    STREAM_TO_UINT24(p_qos->presentation_delay, p_data);    // 12, 3, 15

    if (p_qos->sdu_interval < 0xFF || p_qos->sdu_interval > 0xFFFFFF)
    {
        p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_CONFIGURATION_PARAMETER_VALUE;
        p_sts->reason = GA_LIB_ASCS_CP_REASON_SDU_INTERVAL;
        return p_data - p_stream;
    }

    // validate Framing
    if (p_qos->framing >= GA_LIB_ASCS_INVALID_FRAMING)
    {
        p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_CONFIGURATION_PARAMETER_VALUE;
        p_sts->reason = GA_LIB_ASCS_CP_REASON_FRAMING;
        return p_data - p_stream;
    }

    // validate PHY
    if (0 != (p_qos->phy & GA_LIB_ASCS_VALID_PHY_MASK))
    {
        p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_CONFIGURATION_PARAMETER_VALUE;
        p_sts->reason = GA_LIB_ASCS_CP_REASON_PHY;
        return p_data - p_stream;
    }

    // validate Max_SDU
    if (p_qos->max_sdu > 0xFFF)
    {
        p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_CONFIGURATION_PARAMETER_VALUE;
        p_sts->reason = GA_LIB_ASCS_CP_REASON_MAXIMUM_SDU_SIZE;
        return p_data - p_stream;
    }

    // validate Max_Transport_Latency
    if (p_qos->max_transport_latency < 0x5 || p_qos->max_transport_latency > 0xFA0)
    {
        p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_CONFIGURATION_PARAMETER_VALUE;
        p_sts->reason = GA_LIB_ASCS_CP_REASON_MAX_TRANSPORT_LATENCY;
        return p_data - p_stream;
    }

    return p_data - p_stream;
}

uint16_t ga_lib_ascs_parse_metadata(uint8_t *p_stream,
                                 uint16_t length,
                                 ga_lib_ascs_metadata_t *p_metadata,
                                 ga_lib_ascs_cp_cmd_sts_t *p_sts)
{
    uint8_t *p_data = p_stream;
    uint8_t metadata_len;
    if (length < 1)
    {
        p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_LENGTH;
        p_sts->reason = GA_LIB_ASCS_CP_REASON_NOT_APPLICABLE;
        return 0;
    }
    STREAM_TO_UINT8(metadata_len, p_data);
    if (metadata_len > (length - 1))
    {
        p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_INVALID_LENGTH;
        p_sts->reason = GA_LIB_ASCS_CP_REASON_NOT_APPLICABLE;
        return p_data - p_stream;
    }
    ga_lib_ascs_cp_cmd_sts_t sts;
    p_data += ga_lib_bap_get_metadata(p_data, metadata_len, p_metadata, &sts);
    return metadata_len + 1; // +1 for metadata_len field
}
