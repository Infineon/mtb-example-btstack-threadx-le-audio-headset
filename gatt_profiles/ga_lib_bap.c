/*
 * $ Copyright Cypress Semiconductor $
 */

#include "ga_lib_bap.h"

#ifdef BAP_DEBUG
#define BAP_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get BAP library traces */
#else
#define BAP_TRACE(...)
#endif


/*
+-------------------+---------------+--------------+
| Transport Latency | Sampling Freq | SDU Interval |
+-------------------+---------------+--------------+
| Low Latency       |               |              |
| Audio             |               |              |
| 8                 | 8, 16, 24, 32 |         7500 |
| 10                | 8, 16, 24, 32 |        10000 |
| 15                |            48 |         7500 |
| 20                |            48 |        10000 |
|                   |               |              |
| High Reliability  |               |              |
| Audio             |               |              |
| 45                | 8, 16, 24, 32 |         7500 |
| 60                | 8, 16, 24, 32 |        10000 |
| 50                |            48 |         7500 |
| 65                |            48 |        10000 |
+-------------------+---------------+--------------+
*/

const ga_bap_config_t bap_broadcast_stream_cfgs[] = {{.cc = BAP_CODEC_CONFIG_8_1_1,
                                                      .sf = 8000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 26,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 2,
                                                      .mtl = 8},
                                                     {.cc = BAP_CODEC_CONFIG_8_1_2,
                                                      .sf = 8000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 26,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 45},
                                                     {.cc = BAP_CODEC_CONFIG_8_2_1,
                                                      .sf = 8000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 30,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 2,
                                                      .mtl = 10},
                                                     {.cc = BAP_CODEC_CONFIG_8_2_2,
                                                      .sf = 8000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 30,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 60},

                                                     {.cc = BAP_CODEC_CONFIG_16_1_1,
                                                      .sf = 16000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 30,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 2,
                                                      .mtl = 8},
                                                     {.cc = BAP_CODEC_CONFIG_16_1_2,
                                                      .sf = 16000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 30,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 45},
                                                     {.cc = BAP_CODEC_CONFIG_16_2_1,
                                                      .sf = 16000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 40,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 2,
                                                      .mtl = 10},
                                                     {.cc = BAP_CODEC_CONFIG_16_2_2,
                                                      .sf = 16000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 40,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 60},

                                                     {.cc = BAP_CODEC_CONFIG_24_1_1,
                                                      .sf = 24000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 45,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 2,
                                                      .mtl = 8},
                                                     {.cc = BAP_CODEC_CONFIG_24_1_2,
                                                      .sf = 24000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 45,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 45},
                                                     {.cc = BAP_CODEC_CONFIG_24_2_1,
                                                      .sf = 24000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 60,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 2,
                                                      .mtl = 10},
                                                     {.cc = BAP_CODEC_CONFIG_24_2_2,
                                                      .sf = 24000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 60,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 60},

                                                     {.cc = BAP_CODEC_CONFIG_32_1_1,
                                                      .sf = 32000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 60,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 2,
                                                      .mtl = 8},
                                                     {.cc = BAP_CODEC_CONFIG_32_1_2,
                                                      .sf = 32000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 60,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 45},
                                                     {.cc = BAP_CODEC_CONFIG_32_2_1,
                                                      .sf = 32000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 80,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 2,
                                                      .mtl = 10},
                                                     {.cc = BAP_CODEC_CONFIG_32_2_2,
                                                      .sf = 32000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 80,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 60},

                                                     {.cc = BAP_CODEC_CONFIG_441_1_1,
                                                      .sf = 44100,
                                                      .fd = 7500,
                                                      .si = 8163,
                                                      .opcf = 97,
                                                      .fr = WICED_BLE_ISOC_FRAMED,
                                                      .rn = 4,
                                                      .mtl = 24},
                                                     {.cc = BAP_CODEC_CONFIG_441_1_2,
                                                      .sf = 44100,
                                                      .fd = 7500,
                                                      .si = 8163,
                                                      .opcf = 97,
                                                      .fr = WICED_BLE_ISOC_FRAMED,
                                                      .rn = 4,
                                                      .mtl = 54},
                                                     {.cc = BAP_CODEC_CONFIG_441_2_1,
                                                      .sf = 44100,
                                                      .fd = 10000,
                                                      .si = 10884,
                                                      .opcf = 130,
                                                      .fr = WICED_BLE_ISOC_FRAMED,
                                                      .rn = 4,
                                                      .mtl = 31},
                                                     {.cc = BAP_CODEC_CONFIG_441_2_2,
                                                      .sf = 44100,
                                                      .fd = 10000,
                                                      .si = 10884,
                                                      .opcf = 130,
                                                      .fr = WICED_BLE_ISOC_FRAMED,
                                                      .rn = 4,
                                                      .mtl = 60},

                                                     {.cc = BAP_CODEC_CONFIG_48_1_1,
                                                      .sf = 48000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 75,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 15},
                                                     {.cc = BAP_CODEC_CONFIG_48_1_2,
                                                      .sf = 48000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 75,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 50},
                                                     {.cc = BAP_CODEC_CONFIG_48_2_1,
                                                      .sf = 48000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 100,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 20},
                                                     {.cc = BAP_CODEC_CONFIG_48_2_2,
                                                      .sf = 48000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 100,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 65},
                                                     {.cc = BAP_CODEC_CONFIG_48_3_1,
                                                      .sf = 48000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 90,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 15},
                                                     {.cc = BAP_CODEC_CONFIG_48_3_2,
                                                      .sf = 48000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 90,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 50},
                                                     {.cc = BAP_CODEC_CONFIG_48_4_1,
                                                      .sf = 48000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 120,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 20},
                                                     {.cc = BAP_CODEC_CONFIG_48_4_2,
                                                      .sf = 48000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 120,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 65},
                                                     {.cc = BAP_CODEC_CONFIG_48_5_1,
                                                      .sf = 48000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 117,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 15},
                                                     {.cc = BAP_CODEC_CONFIG_48_5_2,
                                                      .sf = 48000,
                                                      .fd = 7500,
                                                      .si = 7500,
                                                      .opcf = 117,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 50},
                                                     {.cc = BAP_CODEC_CONFIG_48_6_1,
                                                      .sf = 48000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 155,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 20},
                                                     {.cc = BAP_CODEC_CONFIG_48_6_2,
                                                      .sf = 48000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 155,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 65},
#if ENABLE_LC3_PLUS
                                                     {.cc = BAP_CODEC_CONFIG_48_1_LC3plusHR_VBR,
                                                      .sf = 48000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 160,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 65},
                                                     {.cc = BAP_CODEC_CONFIG_96_1_LC3plusHR_VBR,
                                                      .sf = 96000,
                                                      .fd = 10000,
                                                      .si = 10000,
                                                      .opcf = 190,
                                                      .fr = WICED_BLE_ISOC_UNFRAMED,
                                                      .rn = 4,
                                                      .mtl = 65},
#endif /* ENABLE_LC3_PLUS */
};

void ga_lib_bap_print_bap_stream_cfg(const ga_bap_config_t *p_s)
{
    BAP_TRACE("[%s] cc %d sf %d fd si %d opcf %d fr %d rn %d mtl %d",
              __FUNCTION__,
              p_s->cc,
              p_s->sf,
              p_s->fd,
              p_s->si,
              p_s->opcf,
              p_s->fr,
              p_s->rn,
              p_s->mtl);
}

const ga_bap_config_t *bap_lib_get_stream_cfg(const ga_bap_config_t *p_s,
                                              int limit,
                                              uint32_t cc,
                                              ga_lib_bap_stream_config_t *p_cfg)
{
    int i = limit;

    while (i--)
    {
        if (p_s->cc == cc)
        {
            ga_lib_bap_print_bap_stream_cfg(p_s);
            p_cfg->sampling_frequency = p_s->sf;
            p_cfg->frame_duration = p_s->fd;
            p_cfg->sdu_interval = p_s->si;
            p_cfg->octets_per_codec_frame = p_s->opcf;
            p_cfg->framing = p_s->fr;
            p_cfg->retransmission_number = p_s->rn;
            p_cfg->max_transport_latency = p_s->mtl;
            return p_s;
        }
        p_s++;
    }
    BAP_TRACE("[%s] bad cfg %d", __FUNCTION__, cc);
    return NULL;
}

wiced_result_t ga_lib_bap_get_broadcast_stream_config(uint32_t cc, ga_lib_bap_stream_config_t *p_cfg)
{
    const ga_bap_config_t *p_s =
        bap_lib_get_stream_cfg(bap_broadcast_stream_cfgs,
                               sizeof(bap_broadcast_stream_cfgs) / sizeof(bap_broadcast_stream_cfgs[0]),
                               cc,
                               p_cfg);

    if (!p_s)
    {
        BAP_TRACE("[%s] bad cfg %d", __FUNCTION__, cc);
        return WICED_NOT_FOUND;
    }

    return WICED_SUCCESS;
}

const uint32_t samp_freq_index[] = {
    8000,   // BAP_SAMPLING_FREQ_8_KHz 0x01
    11025,  // BAP_SAMPLING_FREQ_11_025_KHz, // 0x02
    16000,  // BAP_SAMPLING_FREQ_16_KHz,     // 0x03
    22050,  // BAP_SAMPLING_FREQ_22_050_KHz, // 0x04
    24000,  // BAP_SAMPLING_FREQ_24_KHz,     // 0x05
    32000,  // BAP_SAMPLING_FREQ_32_KHz,     // 0x06
    44100,  // BAP_SAMPLING_FREQ_44_1_KHz,   // 0x07
    48000,  // BAP_SAMPLING_FREQ_48_KHz,     // 0x08
    88200,  // BAP_SAMPLING_FREQ_88_2_KHz,   // 0x09
    96000,  // BAP_SAMPLING_FREQ_96_KHz,     // 0x0A
    176400, // BAP_SAMPLING_FREQ_176_4_KHz,  // 0x0B
    192000, // BAP_SAMPLING_FREQ_192_KHz,    // 0x0C
    384000, // BAP_SAMPLING_FREQ_384_KHz,    // 0x0D
};

uint8_t ga_lib_bap_get_sampling_freq_index(uint32_t app_val)
{
    unsigned int i;
    const uint32_t *p_freq = samp_freq_index;

    for (i = 0; i < sizeof(samp_freq_index) / sizeof(samp_freq_index[0]); i++, p_freq++)
    {
        if (*p_freq == app_val)
        {
            return i + 1;
        }
    }

    BAP_TRACE("[%s] no val for %d", __FUNCTION__, app_val);

    return 0;
}

uint32_t ga_lib_bap_get_sampling_freq_from_index(uint8_t index)
{
    if ((index >= 1) && (index <= sizeof(samp_freq_index) / sizeof(samp_freq_index[0])))
    {
        return samp_freq_index[index - 1];
    }
    return 0;
}

wiced_result_t ga_lib_bap_parse_csc(uint8_t *p_data, int data_len, ga_lib_ascs_csc_t *p_csc)
{
    uint8_t value = 0;
    uint8_t length = 0;
    uint8_t type = 0;

    while (data_len > 0)
    {
        STREAM_TO_UINT8(length, p_data);
        STREAM_TO_UINT8(type, p_data);

        switch (type)
        {
        case BAP_CODEC_CONFIG_SAMPLING_FREQUENCY_TYPE:
            STREAM_TO_UINT8(value, p_data);
            p_csc->sampling_frequency_hz = ga_lib_bap_get_sampling_freq_from_index(value);
            if (p_csc->sampling_frequency_hz == 0)
                return WICED_ERROR;
            break;

        case BAP_CODEC_CONFIG_FRAME_DURATION_TYPE:
            STREAM_TO_UINT8(value, p_data);
            p_csc->frame_duration_us = ga_lib_bap_get_frame_duration_from_index(value);
            if (p_csc->frame_duration_us == 0)
                return WICED_ERROR;
            break;

        case BAP_CODEC_CONFIG_AUDIO_CHANNEL_ALLOCATION_TYPE:
            STREAM_TO_UINT32(p_csc->audio_channel_allocation, p_data);
            break;

        case BAP_CODEC_CONFIG_OCTETS_PER_CODEC_FRAME_TYPE:
            STREAM_TO_UINT16(p_csc->octets_per_codec_frame, p_data);
            break;

        case BAP_CODEC_CONFIG_LC3_BLOCKS_PER_SDU_TYPE:
            STREAM_TO_UINT8(p_csc->lc3_blocks_per_sdu, p_data);
            break;

        default:
            return WICED_ERROR;
        }

        /* +1 for length field itself */
        data_len -= (length + 1);
    }

    // check if csc_len matches with sum of individual lengths
    return (data_len != 0) ? WICED_ERROR : WICED_SUCCESS;
}

int ga_lib_bap_update_ltv(uint8_t *dest, int max_len, uint8_t val_length, uint8_t type, const uint8_t *value)
{
    if (max_len < (2 + val_length))
    {
        return 0;
    }

    UINT8_TO_STREAM(dest, val_length + 1);
    UINT8_TO_STREAM(dest, type);
    ARRAY_TO_STREAM(dest, value, val_length);

    return (2 + val_length);
}

uint8_t ga_lib_bap_get_frame_duration_index(uint32_t app_val)
{
    switch (app_val)
    {
    case 10000:
        return BAP_FRAME_DURATION_10;
        break;

    case 7500:
        return BAP_FRAME_DURATION_7_5;
        break;

#if ENABLE_LC3_PLUS
    case 2500:
        return BAP_FRAME_DURATION_2_5;

    case 5000:
        return BAP_FRAME_DURATION_5;
#endif /* ENABLE_LC3_PLUS */

    default:
        break;
    }
    return 0xFF;
}

uint32_t ga_lib_bap_get_frame_duration_from_index(uint8_t ota_val)
{
    switch (ota_val)
    {
    case BAP_FRAME_DURATION_7_5:
        return 7500;
        break;

    case BAP_FRAME_DURATION_10:
        return 10000;
        break;

#if ENABLE_LC3_PLUS
    case BAP_FRAME_DURATION_2_5:
        return 2500;

    case BAP_FRAME_DURATION_5:
        return 5000;
#endif /* ENABLE_LC3_PLUS */

    default:
        break;
    }

    return 0;
}

int ga_lib_bap_fill_csc(uint8_t *p_dst, int max_len, const ga_lib_ascs_csc_t *p_csc)
{
    uint8_t *p_csc_len = NULL;
    uint8_t *p_dst_cache = p_dst;
    int bytes_written = 0;
    uint8_t val = 0;

    // store the location of the csc len, this will be updated based on the CSC
    p_csc_len = p_dst++;

    if (p_csc->sampling_frequency_hz)
    {
        val = ga_lib_bap_get_sampling_freq_index(p_csc->sampling_frequency_hz);
        bytes_written = ga_lib_bap_update_ltv(p_dst,
                                              max_len - (p_dst - p_dst_cache),
                                              sizeof(uint8_t),
                                              BAP_CODEC_CONFIG_SAMPLING_FREQUENCY_TYPE,
                                              &val);
        p_dst += bytes_written;
    }

    if (p_csc->frame_duration_us)
    {
        val = ga_lib_bap_get_frame_duration_index(p_csc->frame_duration_us);
        bytes_written = ga_lib_bap_update_ltv(p_dst,
                                              max_len - (p_dst - p_dst_cache),
                                              sizeof(uint8_t),
                                              BAP_CODEC_CONFIG_FRAME_DURATION_TYPE,
                                              &val);
        p_dst += bytes_written;
    }

    if (p_csc->audio_channel_allocation)
    {
        bytes_written = ga_lib_bap_update_ltv(p_dst,
                                              max_len - (p_dst - p_dst_cache),
                                              sizeof(uint32_t),
                                              BAP_CODEC_CONFIG_AUDIO_CHANNEL_ALLOCATION_TYPE,
                                              (uint8_t *)&p_csc->audio_channel_allocation);
        p_dst += bytes_written;
    }

    if (p_csc->octets_per_codec_frame)
    {
        bytes_written = ga_lib_bap_update_ltv(p_dst,
                                              max_len - (p_dst - p_dst_cache),
                                              sizeof(uint16_t),
                                              BAP_CODEC_CONFIG_OCTETS_PER_CODEC_FRAME_TYPE,
                                              (uint8_t *)&p_csc->octets_per_codec_frame);
        p_dst += bytes_written;
    }

    if (p_csc->lc3_blocks_per_sdu)
    {
        bytes_written = ga_lib_bap_update_ltv(p_dst,
                                              max_len - (p_dst - p_dst_cache),
                                              sizeof(uint8_t),
                                              BAP_CODEC_CONFIG_LC3_BLOCKS_PER_SDU_TYPE,
                                              &p_csc->lc3_blocks_per_sdu);
        p_dst += bytes_written;
    }

    /* length field should not include the its own length, hence -1 */
    *p_csc_len = ((p_dst - p_dst_cache) - 1);

    return p_dst - p_dst_cache;
}

wiced_result_t ga_lib_bap_get_metadata(const uint8_t *p_stream,
                                       int stream_len,
                                       ga_lib_ascs_metadata_t *p_metadata,
                                       ga_lib_ascs_cp_cmd_sts_t *p_sts)
{
    const uint8_t *p_data = p_stream;
    uint8_t ltv_length = 0;
    uint8_t type = 0;

    p_sts->response_code = 0; // initialize with success

    while (stream_len && (p_sts->response_code == 0))
    {
        STREAM_TO_UINT8(ltv_length, p_data);
        STREAM_TO_UINT8(type, p_data);

        // validate the ltv_length
        if (ltv_length > stream_len)
            break;

        switch (type)
        {
        case BAP_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS:
            STREAM_TO_UINT16(p_metadata->preferred_audio_ctx, p_data);
            if (INVALID_CONTEXT_TYPE_MASK & p_metadata->preferred_audio_ctx)
            {
                p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_REJECTED_METADATA;
                p_sts->reason = BAP_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS;
            }
            break;

        case BAP_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS:
            STREAM_TO_UINT16(p_metadata->streaming_audio_ctx, p_data);
            if (INVALID_CONTEXT_TYPE_MASK & p_metadata->streaming_audio_ctx)
            {
                p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_REJECTED_METADATA;
                p_sts->reason = BAP_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS;
            }
            break;

        case BAP_METADATA_TYPE_CCID_LIST:
            // TODO: store data and provide to app
            p_data += ltv_length;
            break;

        default:
            BAP_TRACE("[%s] Unsupported Metadata type [%d]\n", __FUNCTION__, type);
            p_sts->response_code = GA_LIB_ASCS_CP_RESPONSE_REJECTED_METADATA;
            p_sts->reason = type;
        }

        stream_len -= (ltv_length + 1);
    }

    return p_data - p_stream;
}

int ga_lib_bap_fill_metadata(uint8_t *p_dst, int max_len, ga_lib_ascs_metadata_t *p_meta)
{
    uint8_t *ptr_start = p_dst;
    uint8_t *p_metadata_len = p_dst++;
    int bytes_written = 0;
    *p_metadata_len = 0;

    BAP_TRACE("[%s] pref %d stream %d upper %x %d vsc %x %d",
              __FUNCTION__,
              p_meta->preferred_audio_ctx,
              p_meta->streaming_audio_ctx,
              p_meta->p_upper_layer_data,
              p_meta->upper_layer_data_length,
              p_meta->p_vendor_specific_data,
              p_meta->vendor_specific_data_length);

    if (p_meta->preferred_audio_ctx)
    {
        bytes_written = ga_lib_bap_update_ltv(p_dst,
                                              max_len - (p_dst - ptr_start),
                                              sizeof(uint16_t),
                                              BAP_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS,
                                              (const uint8_t *)&p_meta->preferred_audio_ctx);
        p_dst += bytes_written;
        *p_metadata_len += bytes_written;
    }

    if (p_meta->streaming_audio_ctx)
    {
        bytes_written = ga_lib_bap_update_ltv(p_dst,
                                              max_len - (p_dst - ptr_start),
                                              sizeof(uint16_t),
                                              BAP_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS,
                                              (const uint8_t *)&p_meta->streaming_audio_ctx);
        p_dst += bytes_written;
        *p_metadata_len += bytes_written;
    }

    if (p_meta->p_program_info)
    {
        bytes_written = ga_lib_bap_update_ltv(p_dst,
                                              max_len - (p_dst - ptr_start),
                                              p_meta->program_info_len,
                                              BAP_METADATA_TYPE_PROGRAM_INFO,
                                              (const uint8_t *)p_meta->p_program_info);
        p_dst += bytes_written;
        *p_metadata_len += bytes_written;
    }

    if (p_meta->p_upper_layer_data)
    {
        if ((max_len - (p_dst - ptr_start)) >= p_meta->upper_layer_data_length)
        {
            memcpy(p_dst, p_meta->p_upper_layer_data, p_meta->upper_layer_data_length);
            p_dst += p_meta->upper_layer_data_length;
            *p_metadata_len += p_meta->upper_layer_data_length;
        }
    }

    return p_dst - ptr_start;
}

uint32_t ga_lib_bap_get_decoded_data_size(uint32_t sampling_frequency, uint16_t frame_duration)
{
    uint32_t data_size = ((sampling_frequency * frame_duration) + 500000) / 1000000;

    /*For 44.1 kHz operation, the output sample buffer size is same as the 48kHz operation, 480 samples for the
    10 ms frame duration and 360 samples for the 7.5 ms frame duration*/
    if (sampling_frequency == 44100)
        data_size = ((data_size - 1) * 12) / 11;
    return data_size;
}

wiced_bool_t ga_lib_bap_is_state_transition_valid(ga_lib_ascs_characteristics_t char_type,
                                                  uint8_t ase_state,
                                                  uint8_t opcode,
                                                  uint8_t *next_state)
{
    static const uint8_t state_transition_table[GA_LIB_ASCS_STATE_MAX][GA_LIB_ASCS_OPCODE_MAX] = {
        [GA_LIB_ASCS_STATE_IDLE] = {GA_LIB_ASCS_STATE_MAX,
                                    GA_LIB_ASCS_STATE_CODEC_CONFIGURED,
                                    GA_LIB_ASCS_STATE_MAX,
                                    GA_LIB_ASCS_STATE_MAX,
                                    GA_LIB_ASCS_STATE_MAX,
                                    GA_LIB_ASCS_STATE_MAX,
                                    GA_LIB_ASCS_STATE_MAX,
                                    GA_LIB_ASCS_STATE_MAX,
                                    GA_LIB_ASCS_STATE_MAX,
                                    GA_LIB_ASCS_STATE_MAX},

        [GA_LIB_ASCS_STATE_CODEC_CONFIGURED] = {GA_LIB_ASCS_STATE_MAX,
                                                GA_LIB_ASCS_STATE_CODEC_CONFIGURED,
                                                GA_LIB_ASCS_STATE_QOS_CONFIGURED,
                                                GA_LIB_ASCS_STATE_MAX,
                                                GA_LIB_ASCS_STATE_MAX,
                                                GA_LIB_ASCS_STATE_MAX,
                                                GA_LIB_ASCS_STATE_MAX,
                                                GA_LIB_ASCS_STATE_MAX,
                                                GA_LIB_ASCS_STATE_RELEASING,
                                                GA_LIB_ASCS_STATE_MAX},

        [GA_LIB_ASCS_STATE_QOS_CONFIGURED] = {GA_LIB_ASCS_STATE_MAX,
                                              GA_LIB_ASCS_STATE_CODEC_CONFIGURED,
                                              GA_LIB_ASCS_STATE_QOS_CONFIGURED,
                                              GA_LIB_ASCS_STATE_ENABLING,
                                              GA_LIB_ASCS_STATE_MAX,
                                              GA_LIB_ASCS_STATE_MAX,
                                              GA_LIB_ASCS_STATE_MAX,
                                              GA_LIB_ASCS_STATE_MAX,
                                              GA_LIB_ASCS_STATE_RELEASING,
                                              GA_LIB_ASCS_STATE_MAX},

        [GA_LIB_ASCS_STATE_ENABLING] = {GA_LIB_ASCS_STATE_MAX,
                                        GA_LIB_ASCS_STATE_MAX,
                                        GA_LIB_ASCS_STATE_MAX,
                                        GA_LIB_ASCS_STATE_MAX,
                                        GA_LIB_ASCS_STATE_STREAMING,
                                        GA_LIB_ASCS_STATE_DISABLING,
                                        GA_LIB_ASCS_STATE_MAX,
                                        GA_LIB_ASCS_STATE_ENABLING,
                                        GA_LIB_ASCS_STATE_RELEASING,
                                        GA_LIB_ASCS_STATE_MAX},

        [GA_LIB_ASCS_STATE_STREAMING] = {GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_DISABLING,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_STREAMING,
                                         GA_LIB_ASCS_STATE_RELEASING,
                                         GA_LIB_ASCS_STATE_MAX},

        [GA_LIB_ASCS_STATE_DISABLING] = {GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_QOS_CONFIGURED,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_RELEASING,
                                         GA_LIB_ASCS_STATE_MAX},

        [GA_LIB_ASCS_STATE_RELEASING] = {GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_MAX,
                                         GA_LIB_ASCS_STATE_RELEASING,
                                         GA_LIB_ASCS_STATE_MAX},
    };

    uint8_t current_state = ase_state;

    BAP_TRACE("[%s] current state : %d, opcode: %d\n", __FUNCTION__, current_state, opcode);

    if (!opcode || opcode >= GA_LIB_ASCS_OPCODE_MAX)
    {
        BAP_TRACE("[%s] Invalid opcode", __FUNCTION__);
        return FALSE;
    }

    if (GA_LIB_ASCS_STATE_MAX == state_transition_table[current_state][opcode])
    {
        BAP_TRACE("[%s] Invalid transition\n", __FUNCTION__);
        return FALSE;
    }

    *next_state = state_transition_table[current_state][opcode];

    // there is no disabling state for sink ASE
    if (GA_LIB_ASCS_OPCODE_DISABLE == opcode && GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE == char_type)
    {
        *next_state = GA_LIB_ASCS_STATE_QOS_CONFIGURED;
    }

    return TRUE;
}

const ga_bap_config_t bap_unicast_stream_cfgs[] = {{.cc = BAP_CODEC_CONFIG_8_1_1,
                                                    .sf = 8000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 26,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 2,
                                                    .mtl = 8},
                                                   {.cc = BAP_CODEC_CONFIG_8_1_2,
                                                    .sf = 8000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 26,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 75},
                                                   {.cc = BAP_CODEC_CONFIG_8_2_1,
                                                    .sf = 8000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 30,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 2,
                                                    .mtl = 10},
                                                   {.cc = BAP_CODEC_CONFIG_8_2_2,
                                                    .sf = 8000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 30,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 95},

                                                   {.cc = BAP_CODEC_CONFIG_16_1_1,
                                                    .sf = 16000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 30,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 2,
                                                    .mtl = 10},
                                                   {.cc = BAP_CODEC_CONFIG_16_1_2,
                                                    .sf = 16000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 30,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 95},
                                                   {.cc = BAP_CODEC_CONFIG_16_2_1,
                                                    .sf = 16000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 40,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 2,
                                                    .mtl = 10},
                                                   {.cc = BAP_CODEC_CONFIG_16_2_2,
                                                    .sf = 16000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 40,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 60},

                                                   {.cc = BAP_CODEC_CONFIG_24_1_1,
                                                    .sf = 24000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 45,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 2,
                                                    .mtl = 8},
                                                   {.cc = BAP_CODEC_CONFIG_24_1_2,
                                                    .sf = 24000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 45,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 75},
                                                   {.cc = BAP_CODEC_CONFIG_24_2_1,
                                                    .sf = 24000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 60,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 2,
                                                    .mtl = 10},
                                                   {.cc = BAP_CODEC_CONFIG_24_2_2,
                                                    .sf = 24000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 60,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 95},

                                                   {.cc = BAP_CODEC_CONFIG_32_1_1,
                                                    .sf = 32000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 60,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 2,
                                                    .mtl = 8},
                                                   {.cc = BAP_CODEC_CONFIG_32_1_2,
                                                    .sf = 32000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 60,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 75},
                                                   {.cc = BAP_CODEC_CONFIG_32_2_1,
                                                    .sf = 32000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 80,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 2,
                                                    .mtl = 10},
                                                   {.cc = BAP_CODEC_CONFIG_32_2_2,
                                                    .sf = 32000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 80,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 95},

                                                   {.cc = BAP_CODEC_CONFIG_441_1_1,
                                                    .sf = 44100,
                                                    .fd = 7500,
                                                    .si = 8163,
                                                    .opcf = 97,
                                                    .fr = WICED_BLE_ISOC_FRAMED,
                                                    .rn = 5,
                                                    .mtl = 24},
                                                   {.cc = BAP_CODEC_CONFIG_441_1_2,
                                                    .sf = 44100,
                                                    .fd = 7500,
                                                    .si = 8163,
                                                    .opcf = 97,
                                                    .fr = WICED_BLE_ISOC_FRAMED,
                                                    .rn = 13,
                                                    .mtl = 80},
                                                   {.cc = BAP_CODEC_CONFIG_441_2_1,
                                                    .sf = 44100,
                                                    .fd = 10000,
                                                    .si = 10884,
                                                    .opcf = 130,
                                                    .fr = WICED_BLE_ISOC_FRAMED,
                                                    .rn = 5,
                                                    .mtl = 31},
                                                   {.cc = BAP_CODEC_CONFIG_441_2_2,
                                                    .sf = 44100,
                                                    .fd = 10000,
                                                    .si = 10884,
                                                    .opcf = 130,
                                                    .fr = WICED_BLE_ISOC_FRAMED,
                                                    .rn = 13,
                                                    .mtl = 85},

                                                   {.cc = BAP_CODEC_CONFIG_48_1_1,
                                                    .sf = 48000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 75,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 5,
                                                    .mtl = 15},
                                                   {.cc = BAP_CODEC_CONFIG_48_1_2,
                                                    .sf = 48000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 75,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 75},
                                                   {.cc = BAP_CODEC_CONFIG_48_2_1,
                                                    .sf = 48000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 100,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 5,
                                                    .mtl = 20},
                                                   {.cc = BAP_CODEC_CONFIG_48_2_2,
                                                    .sf = 48000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 100,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 95},
                                                   {.cc = BAP_CODEC_CONFIG_48_3_1,
                                                    .sf = 48000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 90,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 5,
                                                    .mtl = 15},
                                                   {.cc = BAP_CODEC_CONFIG_48_3_2,
                                                    .sf = 48000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 90,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 75},
                                                   {.cc = BAP_CODEC_CONFIG_48_4_1,
                                                    .sf = 48000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 120,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 5,
                                                    .mtl = 20},
                                                   {.cc = BAP_CODEC_CONFIG_48_4_2,
                                                    .sf = 48000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 120,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 100},
                                                   {.cc = BAP_CODEC_CONFIG_48_5_1,
                                                    .sf = 48000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 117,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 5,
                                                    .mtl = 15},
                                                   {.cc = BAP_CODEC_CONFIG_48_5_2,
                                                    .sf = 48000,
                                                    .fd = 7500,
                                                    .si = 7500,
                                                    .opcf = 117,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 75},
                                                   {.cc = BAP_CODEC_CONFIG_48_6_1,
                                                    .sf = 48000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 155,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 5,
                                                    .mtl = 20},
                                                   {.cc = BAP_CODEC_CONFIG_48_6_2,
                                                    .sf = 48000,
                                                    .fd = 10000,
                                                    .si = 10000,
                                                    .opcf = 155,
                                                    .fr = WICED_BLE_ISOC_UNFRAMED,
                                                    .rn = 13,
                                                    .mtl = 100},
#if ENABLE_LC3_PLUS
    /* LC3plus HR configs per LC3plus_HighResolution_VendorSpecific_BluetoothLEAudio.pdf */
    {.cc = BAP_CODEC_CONFIG_48_1_LC3plusHR_VBR, .sf = 48000, .fd = 10000, .si = 10000, .opcf = 160, .fr = WICED_BLE_ISOC_UNFRAMED, .rn = 13, .mtl = 100},
    {.cc = BAP_CODEC_CONFIG_48_2_LC3plusHR_VBR, .sf = 48000, .fd = 10000, .si = 10000, .opcf = 310, .fr = WICED_BLE_ISOC_UNFRAMED, .rn = 13, .mtl = 100},
    {.cc = BAP_CODEC_CONFIG_48_3_LC3plusHR_VBR, .sf = 48000, .fd = 7500,  .si = 7500,  .opcf = 117, .fr = WICED_BLE_ISOC_UNFRAMED, .rn = 13, .mtl = 75},
    {.cc = BAP_CODEC_CONFIG_48_4_LC3plusHR_VBR, .sf = 48000, .fd = 7500,  .si = 7500,  .opcf = 180, .fr = WICED_BLE_ISOC_UNFRAMED, .rn = 13, .mtl = 75},
    {.cc = BAP_CODEC_CONFIG_48_5_LC3plusHR_VBR, .sf = 48000, .fd = 5000,  .si = 5000,  .opcf = 120, .fr = WICED_BLE_ISOC_UNFRAMED, .rn = 4,  .mtl = 5},
    {.cc = BAP_CODEC_CONFIG_96_1_LC3plusHR_VBR, .sf = 96000, .fd = 10000, .si = 10000, .opcf = 190, .fr = WICED_BLE_ISOC_UNFRAMED, .rn = 13, .mtl = 100},
    {.cc = BAP_CODEC_CONFIG_96_2_LC3plusHR_VBR, .sf = 96000, .fd = 10000, .si = 10000, .opcf = 310, .fr = WICED_BLE_ISOC_UNFRAMED, .rn = 13, .mtl = 100},
    {.cc = BAP_CODEC_CONFIG_96_3_LC3plusHR_VBR, .sf = 96000, .fd = 7500,  .si = 7500,  .opcf = 141, .fr = WICED_BLE_ISOC_UNFRAMED, .rn = 13, .mtl = 75},
    {.cc = BAP_CODEC_CONFIG_96_4_LC3plusHR_VBR, .sf = 96000, .fd = 7500,  .si = 7500,  .opcf = 225, .fr = WICED_BLE_ISOC_UNFRAMED, .rn = 13, .mtl = 75},
    {.cc = BAP_CODEC_CONFIG_96_5_LC3plusHR_VBR, .sf = 96000, .fd = 5000,  .si = 5000,  .opcf = 120, .fr = WICED_BLE_ISOC_UNFRAMED, .rn = 4,  .mtl = 5}
#endif /* ENABLE_LC3_PLUS */
};

wiced_result_t ga_lib_bap_get_unicast_stream_config(uint32_t cc, ga_lib_bap_stream_config_t *p_cfg)
{
    const ga_bap_config_t *p_s =
        bap_lib_get_stream_cfg(bap_unicast_stream_cfgs,
                               sizeof(bap_unicast_stream_cfgs) / sizeof(bap_unicast_stream_cfgs[0]),
                               cc,
                               p_cfg);

    if (!p_s)
    {
        return WICED_NOT_FOUND;
    }

    return WICED_SUCCESS;
}
