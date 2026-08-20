/*
* $ Copyright Cypress Semiconductor $
*/

/* Application includes */
#include "lehs.h"
#include "audio_driver.h"

/* BT Stack includes */
#include "wiced_bt_audio.h"

/* App Library includes */
#include "wiced_audio_manager.h"
#include "wiced_bt_codec_cs47l35.h"
#include "wiced_bt_isoc.h"

/*
Data_Path_ID' => (Interface << 5) + slot

Bits 5:7 = hardware interface
Bits 0:6 = slot, for TDM bus slot = slot, for I2S slot 0 = left and slot 1 = right.

Hardware interfaces available are:
    0 = HCI  (illegal value for Configure_Data_Path)
    1 = ARIP_I2S master only
    2 = PCM2, I2S master only (H2 only, PCM is removed in H1)
    3 = MXTDM_0 in I2S mode (H1 only)
    4 = MXTDM_0 in TDM mode(H1 only)
    5 = MXTDM_1 in I2S mode (H1 only)
    6 = MXTDM_1 in TDM mode (H1 only)

*/
//#define ARIP_I2S_DATA_PATH_ID (1 << 5)
#define ARIP_I2S_DATA_PATH_ID 0x60

#define ARIP_MXTDM_0_DATA_PATH_ID 0x80 //4
#define ARIP_MXTDM_1_DATA_PATH_ID 0xC0 //6
#define ARIP_MXTDM_FIND_ALL_MASK 0x1f
#define ARIP_MXTDM_I2S 0xA0
#define ARIP_MXTDM_I2S_1_CH_L_DATA_PATH_ID ARIP_MXTDM_I2S       //5 1010 0000
#define ARIP_MXTDM_I2S_1_CH_R_DATA_PATH_ID (ARIP_MXTDM_I2S | 1) //5 1010 0001

#define HS_DEFAULT_VOLUME 50
#define CODEC_SPECIFIC_CONFIGURATION_LEN 11
#ifndef CTLR_DELAY
#define CTLR_DELAY 35000
#endif

int32_t stream_id;
extern int32_t audio_driver_config_frequency(int32_t sampling_rate,
                                             int32_t no_of_channels,
                                             int32_t bits_per_sample,
                                             am_audio_io_device_t sink,
                                             uint32_t stream_type);
extern void audio_driver_init_vol();

wiced_bt_audio_config_buffer_t lehs_audio_buf_config = {.role = WICED_AUDIO_SINK_ROLE,
                                                        .audio_tx_buffer_size = 0,
                                                        .audio_codec_buffer_size = 0x10000};

void lehs_isoc_dhm_init(void)
{

	/* required to call this for ARIP/I2S configuration */
    wiced_result_t res = wiced_audio_buffer_initialize(lehs_audio_buf_config);
    WICED_BT_TRACE("wiced_audio_buffer_initialize res : %d \n", res);
    wiced_am_init();

    /* Open and Close the Codec now (Boot time) to prevent DSP download delay later */
    /* Pre-initialization also can prevent pop sound for playing audio first time */

    stream_id = wiced_am_stream_open(A2DP_PLAYBACK);

    if (stream_id == WICED_AUDIO_MANAGER_STREAM_ID_INVALID)
    {
        WICED_BT_TRACE_CRIT("wiced_am_stream_open failed\n");
        return;
    }

    if (wiced_am_stream_close(stream_id) != WICED_SUCCESS)
        WICED_BT_TRACE_CRIT("Err: wiced_am_stream_close\n");
}

void lehs_isoc_dhm_reinit(uint8_t device_role)
{
    if (lehs_audio_buf_config.role == device_role) return;

    if (wiced_am_stream_close(stream_id) != WICED_SUCCESS)
        WICED_BT_TRACE_CRIT("Err: wiced_am_stream_close\n");

    lehs_audio_buf_config.role = device_role;
    lehs_audio_buf_config.audio_tx_buffer_size = (device_role == WICED_HF_ROLE) ? 0x10000 : 0;
    wiced_result_t res = wiced_audio_buffer_initialize(lehs_audio_buf_config);
    WICED_BT_TRACE("wiced_audio_buffer_initialize res : %d \n", res);
    wiced_am_init();

}

static uint32_t count_set_bits(uint32_t n)
{
    uint32_t count = 0;
    while (n)
    {
        n &= (n - 1);
        count++;
    }
    return count;
}

wiced_result_t lehs_isoc_dhm_get_csc(uint8_t *p_csc_field, const ga_lib_ascs_csc_t *p_csc)
{
    uint8_t csc_sampling_freq = 0, frame_duration = 0;

    if (!p_csc) return WICED_BADARG;

    if (!p_csc->sampling_frequency_hz || !p_csc->frame_duration_us || !p_csc->octets_per_codec_frame) return WICED_ERROR;

    WICED_BT_TRACE("[%s] sampling frequency: %d frame_duration: %d octets_per_codec_frame: %d \n",
                   __FUNCTION__,
                   p_csc->sampling_frequency_hz,
                   p_csc->frame_duration_us,
                   p_csc->octets_per_codec_frame);

    switch (p_csc->sampling_frequency_hz)
    {
    case 8000:
    case 16000:
    case 24000:
    case 32000:
    case 44100:
    case 48000:
        csc_sampling_freq = 1 << ((p_csc->sampling_frequency_hz / 8000) - 1);
        break;

    default:
        WICED_BT_TRACE("[%s] Unsupported sampling frequency: %d\n", __FUNCTION__, p_csc->sampling_frequency_hz);
        return WICED_UNSUPPORTED;
        break;
    }

    switch (p_csc->frame_duration_us)
    {
    case 7500:
    case 10000:
        frame_duration = ga_lib_bap_get_frame_duration_index(p_csc->frame_duration_us) + 1;
        break;

    default:
        WICED_BT_TRACE("[%s] Unsupported frame duration: %d\n", __FUNCTION__, p_csc->frame_duration_us);
        return WICED_UNSUPPORTED;
        break;
    }

    uint8_t csc[] = {3,                                        // Length
                     BAP_CODEC_CONFIG_SAMPLING_FREQUENCY_TYPE, // Type
                     csc_sampling_freq,                        // (bit pos + 1) * 8khz
                     0,
                     2,                                    // Length
                     BAP_CODEC_CONFIG_FRAME_DURATION_TYPE, // Type
                     frame_duration,
                     3,                                            // Length
                     BAP_CODEC_CONFIG_OCTETS_PER_CODEC_FRAME_TYPE, // Type
                     p_csc->octets_per_codec_frame,
                     0};

    WICED_MEMCPY(p_csc_field, csc, CODEC_SPECIFIC_CONFIGURATION_LEN);

    return WICED_SUCCESS;
}

wiced_result_t lehs_isoc_dhm_setup_cis_stream(lehs_ase_data_t *p_ase)
{
    uint8_t csc[CODEC_SPECIFIC_CONFIGURATION_LEN];
    wiced_result_t ret;
    ga_lib_ascs_csc_t *p_csc = &p_ase->ase.ase_cfg.csc;
    uint8_t datapath_dir = p_ase->ase.data_path_dir;
    uint32_t numOfChannels = 0;
    uint8_t device_role = WICED_HF_ROLE;
    uint32_t stream_type = HFP;

    if (p_csc->audio_channel_allocation)
    {
        numOfChannels = count_set_bits(p_csc->audio_channel_allocation);
        if (numOfChannels > 2)
        {
            return WICED_UNSUPPORTED;
        }
    }

    ret = lehs_isoc_dhm_get_csc(csc, p_csc);
    if (ret)
        return ret;
    if (p_ase->ase.metadata.streaming_audio_ctx == BAP_CONTEXT_TYPE_MEDIA)
    {
        device_role = WICED_AUDIO_SINK_ROLE;
        stream_type = A2DP_PLAYBACK;
    }

    lehs_isoc_dhm_reinit(device_role);
    stream_id = audio_driver_config_frequency(p_csc->sampling_frequency_hz,
                                              numOfChannels,
                                              DEFAULT_BITSPSAM,
                                              AM_HEADPHONES,
                                              stream_type);

    /* audio channel : If one bit is set then its treated as mono,
		                       if 2 bits are set then its stereo
		Eg: 1 - mono; 3 - stereo
		ref: BAP_AUDIO_LOCATIONS for all the available locations */
    wiced_bt_codec_cs47l35_set_sink_mono2stereo(p_csc->audio_channel_allocation);

    wiced_ble_isoc_setup_data_path_info_t iso_audio_param_data = {0};
    uint8_t codec_id[5] = {0x06, 0x00, 0x00, 0x00, 0x00};

    iso_audio_param_data.isoc_conn_hdl = p_ase->cis_conn_handle;
    iso_audio_param_data.p_app_ctx = p_ase;
    iso_audio_param_data.data_path_dir = datapath_dir;
    iso_audio_param_data.controller_delay = CTLR_DELAY;
    iso_audio_param_data.csc_length = sizeof(csc);
    iso_audio_param_data.p_csc = csc;
    memcpy(iso_audio_param_data.codec_id, codec_id, sizeof(iso_audio_param_data.codec_id));
    lehs_csis_data_t *p_csis = lehs_get_csis_data();
    uint32_t aca =
        (stream_type == HFP && (p_csis->size > 1)) ? BAP_AUDIO_LOCATION_FRONT_LEFT : p_csc->audio_channel_allocation;

    switch (aca)
    {
    case BAP_AUDIO_LOCATION_FRONT_LEFT:
        iso_audio_param_data.data_path_id = ARIP_MXTDM_I2S_1_CH_L_DATA_PATH_ID;
        wiced_ble_isoc_configure_data_path(datapath_dir, ARIP_MXTDM_I2S_1_CH_L_DATA_PATH_ID);
        break;
    case BAP_AUDIO_LOCATION_FRONT_RIGHT:
        iso_audio_param_data.data_path_id = ARIP_MXTDM_I2S_1_CH_R_DATA_PATH_ID;
        wiced_ble_isoc_configure_data_path(datapath_dir, ARIP_MXTDM_I2S_1_CH_R_DATA_PATH_ID);
        break;
    case BAP_AUDIO_LOCATION_FRONT_LEFT | BAP_AUDIO_LOCATION_FRONT_RIGHT:
        iso_audio_param_data.data_path_id = (ARIP_MXTDM_I2S | ARIP_MXTDM_FIND_ALL_MASK);
        wiced_ble_isoc_configure_data_path(datapath_dir, ARIP_MXTDM_I2S_1_CH_L_DATA_PATH_ID);
        wiced_ble_isoc_configure_data_path(datapath_dir, ARIP_MXTDM_I2S_1_CH_R_DATA_PATH_ID);
        break;
    }

    wiced_result_t res = wiced_ble_isoc_setup_data_path(&iso_audio_param_data);
    WICED_BT_TRACE("[%s] Datapath setup res 0x%x", __FUNCTION__, res);
    // setup ISO data path and LC3 codec for INPUT from controller or OUTPUT to controller

    return res;
}
uint32_t get_stream_audio_location(uint8_t bis_index, lehs_broadcast_sink_cb_t *p_big)
{
    for (int i = 0; i < p_big->base.sub_group[0].bis_cnt; i++)
    {
        if ((p_big->base.sub_group[0].bis_config[i].bis_idx == bis_index) &&
            p_big->base.sub_group[0].bis_config[i].bis_csc.audio_channel_allocation)
            return p_big->base.sub_group[0].bis_config[i].bis_csc.audio_channel_allocation;
    }
    return p_big->base.sub_group[0].csc.audio_channel_allocation;
}

wiced_result_t lehs_isoc_dhm_setup_bis_stream(lehs_broadcast_sink_cb_t *p_big, uint8_t bis_count)
{
    uint8_t csc[CODEC_SPECIFIC_CONFIGURATION_LEN];
    uint32_t numOfChannels = 0;
    wiced_result_t ret;
    ga_lib_ascs_csc_t *p_csc = &p_big->base.sub_group[0].csc;

    lehs_isoc_dhm_reinit(WICED_AUDIO_SINK_ROLE);
    stream_id =
        audio_driver_config_frequency(p_csc->sampling_frequency_hz, numOfChannels, DEFAULT_BITSPSAM, AM_HEADPHONES, A2DP_PLAYBACK);
    wiced_bt_codec_cs47l35_set_sink_mono2stereo(p_csc->audio_channel_allocation);

    for (int i = 0; i < bis_count; i++)
    {
        ret = lehs_isoc_dhm_get_csc(csc, p_csc);
        if (ret != WICED_SUCCESS)
        {
            WICED_BT_TRACE_CRIT("[%s] Could not get csc", __FUNCTION__, ret);
            return ret;
        }
        p_csc->audio_channel_allocation = get_stream_audio_location(p_big->bis_index_list[i], p_big);
        if (p_csc->audio_channel_allocation)
        {
            numOfChannels = count_set_bits(p_csc->audio_channel_allocation);
            if (numOfChannels > 2)
            {
                WICED_BT_TRACE_CRIT("[%s] not supporting more than 2 %d", __FUNCTION__, numOfChannels);
                return WICED_UNSUPPORTED;
            }
        }

        wiced_ble_isoc_setup_data_path_info_t iso_bis_audio_param_data = {0};
        uint8_t codec_id[5] = {0x06, 0x00, 0x00, 0x00, 0x00};

        iso_bis_audio_param_data.p_app_ctx = NULL;
        iso_bis_audio_param_data.isoc_conn_hdl = p_big->bis_conn_id_list[i];
        iso_bis_audio_param_data.data_path_dir = WICED_BLE_ISOC_DPD_OUTPUT;
        iso_bis_audio_param_data.controller_delay = CTLR_DELAY;
        iso_bis_audio_param_data.csc_length = sizeof(csc);
        iso_bis_audio_param_data.p_csc = csc;
        memcpy(iso_bis_audio_param_data.codec_id, codec_id, sizeof(codec_id));
        switch (p_csc->audio_channel_allocation)
        {
        case BAP_AUDIO_LOCATION_FRONT_LEFT:
            iso_bis_audio_param_data.data_path_id = ARIP_MXTDM_I2S_1_CH_L_DATA_PATH_ID;
            wiced_ble_isoc_configure_data_path(WICED_BLE_ISOC_DPD_OUTPUT, ARIP_MXTDM_I2S_1_CH_L_DATA_PATH_ID);
            break;
        case BAP_AUDIO_LOCATION_FRONT_RIGHT:
            iso_bis_audio_param_data.data_path_id = ARIP_MXTDM_I2S_1_CH_R_DATA_PATH_ID;
            wiced_ble_isoc_configure_data_path(WICED_BLE_ISOC_DPD_OUTPUT, ARIP_MXTDM_I2S_1_CH_R_DATA_PATH_ID);
            break;
        case BAP_AUDIO_LOCATION_FRONT_LEFT | BAP_AUDIO_LOCATION_FRONT_RIGHT:
            iso_bis_audio_param_data.data_path_id = (ARIP_MXTDM_I2S | ARIP_MXTDM_FIND_ALL_MASK);
            wiced_ble_isoc_configure_data_path(WICED_BLE_ISOC_DPD_OUTPUT, ARIP_MXTDM_I2S_1_CH_L_DATA_PATH_ID);
            wiced_ble_isoc_configure_data_path(WICED_BLE_ISOC_DPD_OUTPUT, ARIP_MXTDM_I2S_1_CH_R_DATA_PATH_ID);
            break;
        }

        ret = wiced_ble_isoc_setup_data_path(&iso_bis_audio_param_data);
        if (ret != WICED_SUCCESS)
        {
            WICED_BT_TRACE_CRIT("[%s] 2. setup error 0x%x", __FUNCTION__, ret);
            return WICED_ERROR;
        }

    }

    return ret;
}

void lehs_isoc_dhm_start_stream(uint16_t conn_hdl, uint8_t ase_type)
{

	if (WICED_SUCCESS != wiced_am_stream_start(stream_id))
	{
	    WICED_BT_TRACE_CRIT("wiced_am_stream_start failed\n");
	}

	audio_driver_init_vol();
}

static void isoc_dhm_free_stream(void)
{
    if (wiced_am_stream_stop(stream_id) != WICED_SUCCESS)
    {
        WICED_BT_TRACE_CRIT("Err: wiced_am_stream_stop\n");
    }
    if (wiced_am_stream_close(stream_id) != WICED_SUCCESS)
    {
        WICED_BT_TRACE_CRIT("Err: wiced_am_stream_close\n");
    }
}

void lehs_isoc_dhm_free_cis_stream(uint16_t conn_hdl, wiced_ble_isoc_data_path_bit_t dir)
{
    WICED_BT_TRACE("[%s] [dir %d] [conn_hdl 0x%x]", __FUNCTION__, dir, conn_hdl);
    isoc_dhm_free_stream();
    wiced_result_t ret = wiced_ble_isoc_remove_data_path(conn_hdl, dir, NULL);
    if (ret)
    {
        WICED_BT_TRACE_CRIT("[%s] ret %d\n", __FUNCTION__, ret);
    }
}

void lehs_isoc_dhm_free_bis_stream(uint16_t *conn_hdl_list, uint8_t bis_count)
{
    isoc_dhm_free_stream();
    for (int i = 0; i < bis_count; i++)
    {
        WICED_BT_TRACE("[%s] [conn_hdl 0x%x]", __FUNCTION__, conn_hdl_list[i]);
        wiced_result_t ret = wiced_ble_isoc_remove_data_path(conn_hdl_list[i], WICED_BLE_ISOC_DPD_OUTPUT_BIT, NULL);
        if (ret)
        {
            WICED_BT_TRACE_CRIT("[%s] ret %d\n", __FUNCTION__, ret);
        }
    }
}

void lehs_isoc_audio_stop_stream(uint16_t conn_id)
{
}
