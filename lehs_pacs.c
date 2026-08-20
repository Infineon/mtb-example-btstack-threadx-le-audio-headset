/*
 * $ Copyright Cypress Semiconductor $
 */
#include "lehs.h"


// left earbud with a microphone
static const ga_lib_pacs_record_t snk_pac_records_supported[] = {{
    .codec_id =
        {
            .coding_format = LC3_CODEC_ID,
            .company_id = 0,
            .vendor_specific_codec_id = 0,
        },
    .codec_specific_capabilities = {0x03,
                                    BAP_CODEC_CAPABILITIES_SUPPORTED_SAMPLING_FREQUENCIES_TYPE,
                                    (BAP_SUPPORTED_SAMPLING_FREQ_8_KHZ | BAP_SUPPORTED_SAMPLING_FREQ_16_KHZ |
                                    BAP_SUPPORTED_SAMPLING_FREQ_24_KHZ | BAP_SUPPORTED_SAMPLING_FREQ_32_KHZ |
                                    BAP_SUPPORTED_SAMPLING_FREQ_44_1_KHZ | BAP_SUPPORTED_SAMPLING_FREQ_48_KHZ),
                                    0x00,
                                    0x02,
                                    BAP_CODEC_CAPABILITIES_SUPPORTED_FRAME_DURATIONS_TYPE,
                                    (BAP_SUPPORTED_FRAME_DURATION_10MS | BAP_SUPPORTED_FRAME_DURATION_7_5MS ),
                                    0x02,
                                    BAP_CODEC_CAPABILITIES_SUPPORTED_AUDIO_CHANNEL_COUNTS_TYPE,
    #if defined MULTIPLEX_AUDIO_SUPPORTED && (MULTIPLEX_AUDIO_SUPPORTED == 1)
                                    0x03, // Channel count greater than one to support multiplex audio
    #else
                                    0x01, // Supports mono audio
    #endif
                                    0x05,
                                    BAP_CODEC_CAPABILITIES_SUPPORTED_OCTETS_PER_CODEC_FRAME_TYPE,
                                    0x1A,
                                    0x00,
                                    MAX_SUPPORTED_OCTETS_PER_CODEC_FRAME,
                                    0x00},
    .codec_specific_capabilities_length = 16,
    .metadata_length = 4,
    .metadata =
        {0x3, BAP_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS,
        BAP_CONTEXT_TYPE_MEDIA | BAP_CONTEXT_TYPE_UNSPECIFIED,
        0x02}, //BAP_CONTEXT_TYPE_RINGTONE 0x0200
}
};

static const ga_lib_pacs_record_t src_pac_records_supported[] = {{
    .codec_id =
        {
            .coding_format = LC3_CODEC_ID,
            .company_id = 0,
            .vendor_specific_codec_id = 0,
        },
    .codec_specific_capabilities = {0x03,
                                    BAP_CODEC_CAPABILITIES_SUPPORTED_SAMPLING_FREQUENCIES_TYPE,
                                    (BAP_SUPPORTED_SAMPLING_FREQ_8_KHZ | BAP_SUPPORTED_SAMPLING_FREQ_16_KHZ |
                                     BAP_SUPPORTED_SAMPLING_FREQ_24_KHZ | BAP_SUPPORTED_SAMPLING_FREQ_32_KHZ |
                                     BAP_SUPPORTED_SAMPLING_FREQ_48_KHZ),
                                    0x00,
                                    0x02,
                                    BAP_CODEC_CAPABILITIES_SUPPORTED_FRAME_DURATIONS_TYPE,
                                    ( BAP_SUPPORTED_FRAME_DURATION_10MS | BAP_SUPPORTED_FRAME_DURATION_7_5MS ),
                                    0x02,
                                    BAP_CODEC_CAPABILITIES_SUPPORTED_AUDIO_CHANNEL_COUNTS_TYPE,
#if defined MULTIPLEX_AUDIO_SUPPORTED && (MULTIPLEX_AUDIO_SUPPORTED == 1)
                                    0x03, // Channel count greater than one to support multiplex audio
#else
                                    0x01, // Supports mono audio
#endif
                                    0x05,
                                    BAP_CODEC_CAPABILITIES_SUPPORTED_OCTETS_PER_CODEC_FRAME_TYPE,
                                    0x1A,
                                    0x00,
                                    MAX_SUPPORTED_OCTETS_PER_CODEC_FRAME,
                                    0x00},
    .codec_specific_capabilities_length = 16,
    .metadata_length = 4,
    .metadata = {0x3,
                 BAP_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS,
                 BAP_CONTEXT_TYPE_CONVERSATIONAL | BAP_CONTEXT_TYPE_UNSPECIFIED,
                 0},
}};

ga_lib_pacs_data_t g_lehs_pacs_app_data = {
    .src_pac_list = {.num_records = 1, .p_records = (ga_lib_pacs_record_t *)src_pac_records_supported},
    .src_audio_location = BAP_AUDIO_LOCATION_FRONT_LEFT,
    .snk_pac_list = {.num_records = 1, .p_records = (ga_lib_pacs_record_t *)snk_pac_records_supported},
    .snk_audio_location = (BAP_AUDIO_LOCATION_FRONT_LEFT | BAP_AUDIO_LOCATION_FRONT_RIGHT),
    .supported = {.src_contexts = 0x0FFF, .snk_contexts = 0x0FFF}, // 0x0FFF : Supports all valid context type
    .available = {.src_contexts = 0x0FFF, .snk_contexts = 0x0FFF},
};

void lehs_set_audio_location(uint32_t audio_location)
{
    g_lehs_pacs_app_data.snk_audio_location= audio_location;
    g_lehs_pacs_app_data.src_audio_location = audio_location;
}

wiced_bool_t lehs_pacs_verify_context_type(uint8_t ase_type, uint16_t req_context)
{
    uint16_t supported_contexts = (ase_type == GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE)
                                      ? g_lehs_pacs_app_data.supported.snk_contexts
                                      : g_lehs_pacs_app_data.supported.src_contexts;
    uint16_t available_contexts = (ase_type == GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE)
                                      ? g_lehs_pacs_app_data.available.snk_contexts
                                      : g_lehs_pacs_app_data.available.src_contexts;
    if (!(req_context & supported_contexts) || !(req_context & available_contexts))
    {
        WICED_BT_TRACE_CRIT(
            "[%s] req context %d is not supported or not available supported context %d available context %d\n",
            __FUNCTION__,
            req_context,
            supported_contexts,
            available_contexts);
        return WICED_FALSE;
    }

    return WICED_TRUE;
}

wiced_bool_t lehs_pacs_verify_audio_location(uint8_t ase_type, ga_lib_pacs_audio_location_t req)
{
    ga_lib_pacs_audio_location_t supported_location = (ase_type == GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE)
                                                               ? g_lehs_pacs_app_data.snk_audio_location
                                                               : g_lehs_pacs_app_data.src_audio_location;
    if (req && supported_location)
    {
        if (!(req & supported_location))
        {
            WICED_BT_TRACE_CRIT("[%s] req location %d is not supported supported location %d\n",
                                __FUNCTION__,
                                req,
                                supported_location);
            return WICED_FALSE;
        }
    }

    return WICED_TRUE;
}

void fill_lc3_codec_param(uint8_t *data, uint8_t len, lehs_utils_pacs_t *output)
{
    uint8_t type = 0;
    while (len > 2)
    {
        data++;
        len--;
        type = *data;
        data++;
        len--;
        switch (type)
        {
        case BAP_CODEC_CAPABILITIES_SUPPORTED_SAMPLING_FREQUENCIES_TYPE:
            output->sf = *data;
            data++;
            output->sf |= ((*data) << 8);
            data++;
            len -= 2;
            break;
        case BAP_CODEC_CAPABILITIES_SUPPORTED_FRAME_DURATIONS_TYPE:
            output->frame_duration = *data;
            data++;
            len--;
            break;
        case BAP_CODEC_CAPABILITIES_SUPPORTED_AUDIO_CHANNEL_COUNTS_TYPE:
            output->audio_ch_count = *data;
            data++;
            len--;
            break;
        case BAP_CODEC_CAPABILITIES_SUPPORTED_OCTETS_PER_CODEC_FRAME_TYPE:
            output->min_data_per_frame = *data;
            data++;
            output->min_data_per_frame |= ((*data) << 8);
            data++;
            output->max_data_per_frame = *data;
            data++;
            output->max_data_per_frame |= ((*data) << 8);
            data++;
            len -= 4;
            break;
        case BAP_CODEC_CAPABILITIES_SUPPORTED_MAX_CODEC_FRAMES_PER_SDU_TYPE:
            output->frame_per_sdu = *data;
            data++;
            len--;
            break;
        default:
            break;
        }
    }
    WICED_BT_TRACE("[%s] sf %d frame_duration %d ch_count %d octet_min %d octet_max %d frame_per_sdu %d",
              __FUNCTION__,
              output->sf,
              output->frame_duration,
              output->audio_ch_count,
              output->min_data_per_frame,
              output->max_data_per_frame,
              output->frame_per_sdu);
}

static uint8_t get_num_bit_set(uint32_t val)
{
    uint8_t count = 0;

    while (val > 0)
    {
        val &= (val - 1);
        count++;
    }
    return count;
}

static wiced_bool_t lehs_pacs_compare_codec_param(lehs_utils_pacs_t *codec_param,
                                        ga_lib_ascs_config_codec_args_t *codec_arg)
{
    uint8_t num_ch = 1;
    uint32_t block_per_sdu = 0;

    if (codec_arg->csc.octets_per_codec_frame)
    {
        // verify octed per frame
        if ((codec_param->min_data_per_frame > codec_arg->csc.octets_per_codec_frame) ||
            (codec_param->max_data_per_frame < codec_arg->csc.octets_per_codec_frame))
        {
            WICED_BT_TRACE("BAP_CODEC_CONFIG_OCTET_PER_CODEC_FRAME_TYPE Error. OPF %d requested min %d max %d ",
                      codec_arg->csc.octets_per_codec_frame,
                      codec_param->min_data_per_frame,
                      codec_param->max_data_per_frame);
            return WICED_FALSE;
        }
    }

    if (codec_arg->csc.sampling_frequency_hz)
    {
        uint8_t bit_index = ga_lib_bap_get_sampling_freq_index(codec_arg->csc.sampling_frequency_hz);
        if (!bit_index)
        {
            WICED_BT_TRACE("Invalid sampling freq %d ", codec_arg->csc.sampling_frequency_hz);
            return WICED_FALSE;
        }
        bit_index -= 1; // convert to supported sf LTV structures
        if (!(codec_param->sf & (1 << bit_index)))
        {
            WICED_BT_TRACE("BAP_CODEC_CONFIG_SAMPLING_FREQUENCY_TYPE Error. ");
            return WICED_FALSE;
        }
    }

    if (codec_arg->csc.frame_duration_us)
    {
        // verify frame duration
        uint8_t bit_index = ga_lib_bap_get_frame_duration_index(codec_arg->csc.frame_duration_us);
        if (bit_index == 0xFF)
        {
            WICED_BT_TRACE("Invalid frame duration %d ", codec_arg->csc.frame_duration_us);
            return WICED_FALSE;
        }
        if (!(codec_param->frame_duration & (1 << bit_index)))
        {
            WICED_BT_TRACE("BAP_CODEC_CONFIG_FRAME_DURATION_TYPE Error. ");
            return WICED_FALSE;
        }
    }

    if (codec_arg->csc.audio_channel_allocation)
    {
        num_ch = get_num_bit_set(codec_arg->csc.audio_channel_allocation);
    }

    if (codec_arg->csc.lc3_blocks_per_sdu)
    {
        block_per_sdu = codec_arg->csc.lc3_blocks_per_sdu;
    }

    if (num_ch && block_per_sdu)
    {
        // check lc3 block per sdu & channel allocation
        if ((block_per_sdu * num_ch) < codec_param->frame_per_sdu)
            return WICED_FALSE;
    }

    return WICED_TRUE;
}

static wiced_bool_t lehs_pacs_verify_audio_config_param(ga_lib_ascs_config_codec_args_t *p_cc, ga_lib_pacs_record_t *p_record)
{
    lehs_utils_pacs_t record_param;

    if (p_record->codec_id.coding_format != p_cc->codec_id.coding_format)
    {
        WICED_BT_TRACE_CRIT("[%s] req_id %d peer_id %d  \n",
                            __FUNCTION__,
                            p_record->codec_id.coding_format,
                            p_cc->codec_id.coding_format);
        WICED_BT_TRACE_CRIT("Codec id didn't match ");
        return WICED_FALSE;
    }

    // go through the codec capabilities
    memset(&record_param, 0, sizeof(lehs_utils_pacs_t));
    fill_lc3_codec_param(p_record->codec_specific_capabilities,
                         p_record->codec_specific_capabilities_length,
                         &record_param);
    return lehs_pacs_compare_codec_param(&record_param, p_cc);
}


wiced_bool_t lehs_pacs_verify_src_codec_config(ga_lib_ascs_config_codec_args_t *p_cc)
{
    wiced_bool_t is_valid = WICED_FALSE;
    for (int i = 0; i < g_lehs_pacs_app_data.src_pac_list.num_records; i++)
    {
        is_valid = lehs_pacs_verify_audio_config_param(p_cc, &g_lehs_pacs_app_data.src_pac_list.p_records[i]);
        if (is_valid)
        {
            return lehs_pacs_verify_audio_location(GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE, p_cc->csc.audio_channel_allocation);
        }
    }
    return WICED_FALSE;
}

wiced_bool_t lehs_pacs_verify_snk_codec_config(ga_lib_ascs_config_codec_args_t *p_cc)
{
    wiced_bool_t is_valid = WICED_FALSE;
    for (int i = 0; i < g_lehs_pacs_app_data.snk_pac_list.num_records; i++)
    {
        is_valid = lehs_pacs_verify_audio_config_param(p_cc, &g_lehs_pacs_app_data.snk_pac_list.p_records[i]);
        if (is_valid)
        {
            return lehs_pacs_verify_audio_location(GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE, p_cc->csc.audio_channel_allocation);
        }
    }
    return WICED_FALSE;
}
