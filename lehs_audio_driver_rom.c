/*
 * $ Copyright Cypress Semiconductor $
 */

#include "wiced_bt_trace.h"
#include "wiced_data_types.h"
#include "wiced_audio_manager.h"
#include "platform_audio_device.h"

#define DEFAULT_MIC_GAIN 80

static uint8_t prev_volume = DEFAULT_VOLUME;
static uint8_t prev_mic_gain = DEFAULT_MIC_GAIN;
static uint8_t prev_vol_mute = 0;
static uint8_t prev_mic_mute = 0;

/* Volume/Gain range on 0-255, hence mapping the received value to 0-10 */
void audio_driver_set_volume(uint8_t volume)
{
    WICED_BT_TRACE("[%s] volume %d\n", __FUNCTION__, volume);
    WICED_BT_TRACE("[%s] mute state %d\n", __FUNCTION__, prev_vol_mute);
    if (!prev_vol_mute)
        platform_audio_device_set_volume(PLATFORM_DEVICE_PLAY, volume / 25);
    prev_volume = volume;
}

void audio_driver_set_mute_state(uint8_t mute_enabled)
{
    int volume = mute_enabled ? 0 : prev_volume;
    prev_vol_mute = mute_enabled;
    WICED_BT_TRACE("[%s] mute state %d\n", __FUNCTION__, mute_enabled);
    WICED_BT_TRACE("[%s] volume %d\n", __FUNCTION__, volume);
    platform_audio_device_set_volume(PLATFORM_DEVICE_PLAY, volume / 25);
}

int32_t audio_driver_config_frequency(int32_t  sampling_rate, int32_t  no_of_channels, int32_t  bits_per_sample, am_audio_io_device_t sink, uint32_t stream_type)
{
    audio_config_t audio_config;
    int32_t stream_id;

    audio_config.sr = sampling_rate;
    audio_config.channels = no_of_channels;
    audio_config.bits_per_sample = bits_per_sample;
    audio_config.volume = prev_vol_mute ? 0 : prev_volume / 25;
    audio_config.mic_gain = prev_mic_mute ? 0 : prev_mic_gain / 25;
    audio_config.sink = sink;

    stream_id = wiced_am_stream_open(stream_type);
    WICED_BT_TRACE("[%s] volume %d\n", __FUNCTION__,  audio_config.volume );

    if (WICED_SUCCESS != wiced_am_stream_set_param(stream_id, AM_AUDIO_CONFIG, &audio_config))
    {
        WICED_BT_TRACE("wiced_am_set_param set audio config failed\n");
    }

    /* Set MIC gain. */
    if (WICED_SUCCESS != wiced_am_stream_set_param(stream_id, AM_MIC_GAIN_LEVEL, &audio_config.mic_gain))
    {
        WICED_BT_TRACE("wiced_am_set_param: AM_MIC_GAIN_LEVEL set audio config failed\n");
    }
    return stream_id;
}

void audio_driver_init_vol()
{
    int volume = prev_vol_mute ? 0 : prev_volume / 25;
    int mic_gain = prev_mic_mute ? 0 : prev_mic_gain / 25;
    platform_audio_device_set_volume(PLATFORM_DEVICE_PLAY, volume);
    platform_audio_device_set_mic_gain(PLATFORM_DEVICE_PLAY_RECORD, mic_gain);
}

void audio_driver_set_mic_gain(int32_t gain)
{
    WICED_BT_TRACE("[%s] gain in dB %d", __FUNCTION__, gain);
    prev_mic_gain = gain;
    platform_audio_device_set_mic_gain(PLATFORM_DEVICE_PLAY_RECORD, gain / 25);
}

void audio_driver_set_mic_mute_state(uint8_t mute)
{
    prev_mic_mute = mute;
    WICED_BT_TRACE("[%s] mute state %d", __FUNCTION__, mute);
    WICED_BT_TRACE("[%s] gain in dB %d", __FUNCTION__, prev_mic_gain);
    platform_audio_device_set_volume(PLATFORM_DEVICE_PLAY_RECORD, (mute) ? 0 : prev_mic_gain / 25);
}
