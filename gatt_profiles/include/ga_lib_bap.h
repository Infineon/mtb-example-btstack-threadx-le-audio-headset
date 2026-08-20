/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 * Basic Audio Profile (BAP) Application Programming Interface
 */

#ifndef GA_LIB_BAP_H
#define GA_LIB_BAP_H

#include "ga_lib_ascs.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @addtogroup Stream_Control_APIs
 * @{
 */

/**
 * @addtogroup wiced_bt_ga_bap
 * @{
 */

/** Macros for NULL checking */
#define CHECK_FOR_NULL_AND_RETURN_VALUE(x, error_return_value)                                                         \
    if (!x)                                                                                                            \
    {                                                                                                                  \
        WICED_BT_TRACE("[%s] %s is NULL\n", __FUNCTION__, #x);                                                         \
        return error_return_value;                                                                                     \
    }

/** Macros for NULL checking */
#define CHECK_FOR_NULL_AND_RETURN(x)                                                                                   \
    if (!x)                                                                                                            \
    {                                                                                                                  \
        WICED_BT_TRACE("[%s] %s is NULL\n", __FUNCTION__, #x);                                                         \
        return;                                                                                                        \
    }

#define BAP_BROADCAST_CODE_SIZE 16 /**< Length in bytes for Broadcast Code */
#define BAP_BROADCAST_ID_SIZE 3    /**< Length in bytes for Broadcast ID */

    // Context Types
    /**
 * @addtogroup  BAP_CONTEXT_TYPE
 * @{
 *  @brief List of Context type values which can be used
 */

#define BAP_CONTEXT_TYPE_PROHIBITED 0x0000  /**< Prohibited */
#define BAP_CONTEXT_TYPE_UNSPECIFIED 0x0001 /**< Unspecified Matches any audio context */
#define BAP_CONTEXT_TYPE_CONVERSATIONAL                                                                                \
    0x0002 /**< Conversation between humans, for example, in telephony or video calls, including traditional cellular as well as VoIP and Push-to-Talk */
#define BAP_CONTEXT_TYPE_MEDIA                                                                                         \
    0x0004 /**< Media, for example, music playback, radio, podcast or movie soundtrack, or tv audio */
#define BAP_CONTEXT_TYPE_GAME                                                                                          \
    0x0008 /**< Audio associated with video gaming, for example gaming media; gaming effects; music and in-game voice chat between participants; or a mix of all the above */
#define BAP_CONTEXT_TYPE_INSTRUCTIONAL                                                                                 \
    0x0010 /**< Instructional audio, for example, in navigation, announcements, or user guidance */
#define BAP_CONTEXT_TYPE_VOICE_ASSISTANTS                                                                              \
    0x0020 /**< Man-machine communication, for example, with voice recognition or virtual assistants */
#define BAP_CONTEXT_TYPE_LIVE                                                                                          \
    0x0040 /**< Live audio, for example, from a microphone where audio is perceived both through a direct acoustic path and through an LE Audio Stream */
#define BAP_CONTEXT_TYPE_SOUND_EFFECT                                                                                  \
    0x0080 /**< Sound effects including keyboard and touch feedback; menu and user interface sounds; and other system sounds */
#define BAP_CONTEXT_TYPE_NOTIFICATIONS                                                                                 \
    0x100 /**< Notification and reminder sounds; attention-seeking audio, for example, in beeps signaling the arrival of a message */
#define BAP_CONTEXT_TYPE_RINGTONE                                                                                      \
    0x0200 /**< Alerts the user to an incoming call, for example, an incoming telephony or video call, including traditional cellular as well as VoIP and Push-to-Talk */
#define BAP_CONTEXT_TYPE_ALERTS                                                                                        \
    0x0400 /**< Alarms and timers; immediate alerts, for example, in a critical battery alarm, timer expiry or alarm clock, toaster, cooker, kettle, microwave, etc */
#define BAP_CONTEXT_TYPE_EMERGENCY_ALARM                                                                               \
    0x0800                         /**< Emergency sounds, for example, fire alarms or other urgent alerts */
#define BAP_CONTEXT_TYPE_TV 0x0200 /**< Metadata conforming to the Bluetooth Broadcast TV profile */

#define VALID_CONTEXT_TYPE_MASK 0x0FFF   /**< Mask for valid context types */
#define INVALID_CONTEXT_TYPE_MASK 0xF000 /**< Mask for invalid context types */

    typedef uint16_t ga_lib_bap_context_type_t; /**< BAP audio context types */

/**@}  BAP_CONTEXT_TYPE */

/**
 * @addtogroup  BAP_AUDIO_LOCATIONS
 * @{
 *  @brief List of Audio location values which can be used
 */
#define BAP_AUDIO_LOCATION_NOT_ALLOWED 0x00000000             /**< This audio location value is not allowed */
#define BAP_AUDIO_LOCATION_FRONT_LEFT 0x00000001              /**< Audio location value for Front Left */
#define BAP_AUDIO_LOCATION_FRONT_RIGHT 0x00000002             /**< Audio location value for Front Right */
#define BAP_AUDIO_LOCATION_FRONT_CENTER 0x00000004            /**< Audio location value for Front Center */
#define BAP_AUDIO_LOCATION_LOW_FREQUENCY_EFFECTS_1 0x00000008 /**< Audio location value for Low Frequency effect 1 */
#define BAP_AUDIO_LOCATION_BACK_LEFT 0x00000010               /**< Audio location value for Back Left */
#define BAP_AUDIO_LOCATION_BACK_RIGHT 0x00000020              /**< Audio location value for Back Right */
#define BAP_AUDIO_LOCATION_FRONT_LEFT_OF_CENTER 0x00000040    /**< Audio location value for Front Left of Center */
#define BAP_AUDIO_LOCATION_FRONT_RIGHT_OF_CENTER 0x00000080   /**< Audio location value for Front Right of Center */
#define BAP_AUDIO_LOCATION_BACK_CENTER 0x00000100             /**< Audio location value for BAck Center */
#define BAP_AUDIO_LOCATION_LOW_FREQUENCY_EFFECTS_2 0x00000200 /**< Audio location value for Low Frequency effect 2 */
#define BAP_AUDIO_LOCATION_SIDE_LEFT 0x00000400               /**< Audio location value for Side Left */
#define BAP_AUDIO_LOCATION_SIDE_RIGHT 0x00000800              /**< Audio location value for Side Right */
#define BAP_AUDIO_LOCATION_TOP_FRONT_LEFT 0x00001000          /**< Audio location value for Top Front Left */
#define BAP_AUDIO_LOCATION_TOP_FRONT_RIGHT 0x00002000         /**< Audio location value for Top Front Right */
#define BAP_AUDIO_LOCATION_TOP_FRONT_CENTER 0x00004000        /**< Audio location value for Front Center */
#define BAP_AUDIO_LOCATION_TOP_CENTER 0x00008000              /**< Audio location value for Top Center */
#define BAP_AUDIO_LOCATION_TOP_BACK_LEFT 0x00010000           /**< Audio location value for Back Left */
#define BAP_AUDIO_LOCATION_TOP_BACK_RIGHT 0x00020000          /**< Audio location value for Back Right */
#define BAP_AUDIO_LOCATION_TOP_SIDE_LEFT 0x00040000           /**< Audio location value for Side Left */
#define BAP_AUDIO_LOCATION_TOP_SIDE_RIGHT 0x00080000          /**< Audio location value for Side Right */
#define BAP_AUDIO_LOCATION_TOP_BACK_CENTER 0x00100000         /**< Audio location value for Back Center */
#define BAP_AUDIO_LOCATION_BOTTOM_FRONT_CENTER 0x00200000     /**< Audio location value for Bottom Front Center */
#define BAP_AUDIO_LOCATION_BOTTOM_FRONT_LEFT 0x00400000       /**< Audio location value for Bottom Front Left */
#define BAP_AUDIO_LOCATION_BOTTOM_FRONT_RIGHT 0x00800000      /**< Audio location value for Bottom Front Right */
#define BAP_AUDIO_LOCATION_FRONT_LEFT_WIDE 0x01000000         /**< Audio location value for Front Left Wide */
#define BAP_AUDIO_LOCATION_FRONT_RIGHT_WIDE 0x02000000        /**< Audio location value for Front Right Wide */
#define BAP_AUDIO_LOCATION_LEFT_SURROUND 0x04000000           /**< Audio location value for Left Surround */
#define BAP_AUDIO_LOCATION_RIGHT_SURROUND 0x08000000          /**< Audio location value for Right Surround */
#define BAP_AUDIO_LOCATION_INVALID_MASK 0xF0000000            /**<Invalid Mask for audio location */
    /**@}  BAP_AUDIO_LOCATIONS */

    // sampling freq

    /**
 * @addtogroup  BAP_SUPPORTED_SAMPLING_FREQ
 * @{
 *  @brief List of Supported sampling frequency values which can be used
 */

#define BAP_SUPPORTED_SAMPLING_FREQ_8_KHZ (1 << 0)        /**< Bit 0: 8,000 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_11_025_KHZ (1 << 1)   /**< Bit 1: 11,025 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_16_KHZ (1 << 2)       /**< Bit 2: 16,000 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_22_050_KHZ (1 << 3)   /**< Bit 3: 22,050 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_24_KHZ (1 << 4)       /**< Bit 4: 24,000 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_32_KHZ (1 << 5)       /**< Bit 5: 32,000 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_44_1_KHZ (1 << 6)     /**< Bit 6: 44,100 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_48_KHZ (1 << 7)       /**< Bit 7: 48,000 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_88_200_KHZ (1 << 8)   /**< Bit 8: 88,200 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_96_KHZ (1 << 9)       /**< Bit 9: 96,000 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_176_400_KHZ (1 << 10) /**< Bit 10: 176,400 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_192_KHZ (1 << 11)     /**< Bit 11: 192,000 Hz */
#define BAP_SUPPORTED_SAMPLING_FREQ_384_KHZ (1 << 12)     /**< Bit 12: 384,000 Hz */
/**@}  BAP_SUPPORTED_SAMPLING_FREQ */

// frame durations
#define BAP_SUPPORTED_FRAME_DURATION_7_5MS (1 << 0) /**< Bit 0: 7.5 ms frame duration supported. */
#define BAP_SUPPORTED_FRAME_DURATION_10MS (1 << 1)  /**< Bit 1: 10 ms frame duration supported. */

#define BAP_SUPPORTED_FRAME_DURATION_7_5MS_PREFERRED                                                                   \
    0x10 /**< Bit 4: 7.5 ms preferred. Valid only when 7.5 ms is supported and 10 ms is supported. Shall not be set to 0b1 any of (bit 5 or bit 6 or bit 7) is set to 0b1.*/
#define BAP_SUPPORTED_FRAME_DURATION_10MS_PREFERRED                                                                    \
    0x20 /**< Valid only when 7.5 ms is supported and 10 ms is supported. Shall not be set to 0b1 if any of (bit 4 or bit 5 or bit 7) is set to 0b1. Bit 6: 10 ms preferred.*/

    /**
 * @addtogroup  BAP_SAMPLING_FREQ
 * @{
 *  @brief List of Sampling frequency values from Assigned numbers which can be used
 */

#define BAP_SAMPLING_FREQ_8_KHz 0x01      /**< 8000Hz Frequency */
#define BAP_SAMPLING_FREQ_11_025_KHz 0x02 /**< 11025 Hz Frequency */
#define BAP_SAMPLING_FREQ_16_KHz 0x03     /**< 16000 Hz Frequency */
#define BAP_SAMPLING_FREQ_22_050_KHz 0x04 /**< 22050 Hz Frequency */
#define BAP_SAMPLING_FREQ_24_KHz 0x05     /**< 24000 Hz Frequency */
#define BAP_SAMPLING_FREQ_32_KHz 0x06     /**< 32000 Hz Frequency */
#define BAP_SAMPLING_FREQ_44_1_KHz 0x07   /**< 44100 Hz Frequency */
#define BAP_SAMPLING_FREQ_48_KHz 0x08     /**< 48000 Hz Frequency */
#define BAP_SAMPLING_FREQ_88_2_KHz 0x09   /**< 88200 Hz Frequency */
#define BAP_SAMPLING_FREQ_96_KHz 0x0A     /**< 96000 Hz Frequency */
#define BAP_SAMPLING_FREQ_176_4_KHz 0x0B  /**< 176400 Hz Frequency */
#define BAP_SAMPLING_FREQ_192_KHz 0x0C    /**< 192000 Hz Frequency */
#define BAP_SAMPLING_FREQ_384_KHz 0x0D    /**< 384000 Hz Frequency */
/**@}  BAP_SAMPLING_FREQ */

/**
 * @addtogroup  FRAME_DURATION
 * @{
 *  @brief List of Frame duration values which can be used
 */

// frame durations
#define BAP_FRAME_DURATION_7_5 0x00   /**< 7.5 ms frame duration */
#define BAP_FRAME_DURATION_10 0x01    /**< 10ms frame duration */
#define BAP_FRAME_DURATION_8163 0x02  /**< 8.16327 ms frame duration */
#define BAP_FRAME_DURATION_10884 0x03 /**< 10.88435 ms frame duration */
#if ENABLE_LC3_PLUS
#define BAP_FRAME_DURATION_2_5 0x04 /**< 2.5 ms frame duration */
#define BAP_FRAME_DURATION_5   0x05 /**< 5 ms frame duration */
#endif /* ENABLE_LC3_PLUS */
    /**@}  FRAME_DURATION */

/**
 * @addtogroup  LC3_PLUS
 * @{
 *  @brief LC3plus vendor-specific codec definitions
 */
#define GA_LIB_BAP_VENDOR_SPECIFIC_CODING_FORMAT 0xFF
#if ENABLE_LC3_PLUS
#define GA_LIB_BAP_FRAUNHOFER_ISS_COMPANY_ID        0x08A9
#define GA_LIB_BAP_LC3PLUSHR_VBR_ID   0x01
/* Selected codec frame durations (for CSC LTV) */
#define LC3PLUSHR_FRAME_DURATION_10_MS   0x01 /**< 10 ms */
#define LC3PLUSHR_FRAME_DURATION_7_5_MS  0x02 /**< 7.5 ms */
#define LC3PLUSHR_FRAME_DURATION_5_MS    0x03 /**< 5 ms */
#define LC3PLUSHR_FRAME_DURATION_2_5_MS  0x04 /**< 2.5 ms */
/* Bitfield for LC3plusHR_Supported_Frame_Durations LTV */
#define LC3PLUSHR_SUPPORTED_FRAME_DURATION_10_MS            (0x0001 << 0)
#define LC3PLUSHR_SUPPORTED_FRAME_DURATION_7_5_MS           (0x0001 << 1)
#define LC3PLUSHR_SUPPORTED_FRAME_DURATION_5_MS             (0x0001 << 2)
#define LC3PLUSHR_SUPPORTED_FRAME_DURATION_2_5_MS           (0x0001 << 3)
#define LC3PLUSHR_SUPPORTED_FRAME_DURATION_10_MS_PREFERRED  (0x0001 << 8)
#define LC3PLUSHR_SUPPORTED_FRAME_DURATION_7_5_MS_PREFERRED (0x0001 << 9)
#define LC3PLUSHR_SUPPORTED_FRAME_DURATION_5_MS_PREFERRED   (0x0001 << 10)
#define LC3PLUSHR_SUPPORTED_FRAME_DURATION_2_5_MS_PREFERRED (0x0001 << 11)
/** @brief LC3plus codec-specific capabilities types (vendor-specific LTV) */
typedef enum {
    BAP_CODEC_CAPABILITIES_LC3PLUSHR_SUPPORTED_FRAME_DURATIONS_TYPE = 0xF1,
    BAP_CODEC_CAPABILITIES_LC3PLUSHR_SUPPORTED_OCTETS_PER_CODEC_FRAME_10MS_TYPE,
    BAP_CODEC_CAPABILITIES_LC3PLUSHR_SUPPORTED_OCTETS_PER_CODEC_FRAME_7_5MS_TYPE,
    BAP_CODEC_CAPABILITIES_LC3PLUSHR_SUPPORTED_OCTETS_PER_CODEC_FRAME_5MS_TYPE,
    BAP_CODEC_CAPABILITIES_LC3PLUSHR_SUPPORTED_OCTETS_PER_CODEC_FRAME_2_5MS_TYPE,
    BAP_CODEC_CAPABILITIES_LC3PLUSHR_MAX_TYPE = BAP_CODEC_CAPABILITIES_LC3PLUSHR_SUPPORTED_OCTETS_PER_CODEC_FRAME_2_5MS_TYPE
} ga_lib_bap_codec_capabilities_lc3plushr_type_t;
#endif /* ENABLE_LC3_PLUS */
/**@} LC3_PLUS */

    /** @brief Codec Specific Capabilities */
    typedef enum
    {
        BAP_CODEC_CAPABILITIES_INVALID_TYPE,
        BAP_CODEC_CAPABILITIES_SUPPORTED_SAMPLING_FREQUENCIES_TYPE,
        BAP_CODEC_CAPABILITIES_SUPPORTED_FRAME_DURATIONS_TYPE,
        BAP_CODEC_CAPABILITIES_SUPPORTED_AUDIO_CHANNEL_COUNTS_TYPE,
        BAP_CODEC_CAPABILITIES_SUPPORTED_OCTETS_PER_CODEC_FRAME_TYPE,
        BAP_CODEC_CAPABILITIES_SUPPORTED_MAX_CODEC_FRAMES_PER_SDU_TYPE,
        BAP_CODEC_CAPABILITIES_MAX_TYPE = BAP_CODEC_CAPABILITIES_SUPPORTED_MAX_CODEC_FRAMES_PER_SDU_TYPE
    } ga_lib_bap_codec_capabilities_type_t;

    /** @brief Codec Specific Configuration */
    typedef enum
    {
        BAP_CODEC_CONFIG_INVALID_TYPE,
        BAP_CODEC_CONFIG_SAMPLING_FREQUENCY_TYPE,
        BAP_CODEC_CONFIG_FRAME_DURATION_TYPE,
        BAP_CODEC_CONFIG_AUDIO_CHANNEL_ALLOCATION_TYPE,
        BAP_CODEC_CONFIG_OCTETS_PER_CODEC_FRAME_TYPE,
        BAP_CODEC_CONFIG_LC3_BLOCKS_PER_SDU_TYPE,
        BAP_CODEC_CONFIG_MAX_TYPE = BAP_CODEC_CONFIG_LC3_BLOCKS_PER_SDU_TYPE
    } ga_lib_bap_codec_configuration_type_t;

    /** @brief Metadata types */
    typedef enum
    {
        BAP_METADATA_TYPE_INVALID,
        BAP_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS,
        BAP_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS,
        BAP_METADATA_TYPE_PROGRAM_INFO,
        BAP_METADATA_TYPE_LANGUAGE,
        BAP_METADATA_TYPE_CCID_LIST,
        BAP_METADATA_TYPE_PARENTAL_RATING,
        BAP_METADATA_TYPE_PROGRAM_INFO_URI,
        BAP_METADATA_MAX_TYPE = BAP_METADATA_TYPE_PROGRAM_INFO_URI,
        BAP_METADATA_TYPE_EXTENDED_METADATA = 0xFE,
        BAP_METADATA_TYPE_VENDOR_SPECIFIC = 0xFF,
    } ga_lib_bap_metadata_type_t;

    /** @brief Stream configuration */
    typedef struct
    {
        uint32_t sampling_frequency;      /**< Sampling Frequency to be used for the configuration */
        uint32_t frame_duration;          /**< Frame duration to be used */
        uint32_t sdu_interval;            /**< SDU interval */
        uint16_t octets_per_codec_frame;  /**< Octets per codec frame */
        wiced_ble_isoc_framing_t framing; /**< Framed or unframed */
        uint8_t retransmission_number;    /**< Retransmission number */
        uint8_t max_transport_latency;    /**< maximum transport latency */
    } ga_lib_bap_stream_config_t;

    /** @brief Codec configuration list */
    typedef enum
    {
        BAP_CODEC_CONFIG_8_1_1,   /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_8_1_2,   /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_8_2_1,   /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_8_2_2,   /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_16_1_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_16_1_2,  /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_16_2_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_16_2_2,  /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_24_1_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_24_1_2,  /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_24_2_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_24_2_2,  /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_32_1_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_32_1_2,  /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_32_2_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_32_2_2,  /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_441_1_1, /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_441_1_2, /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_441_2_1, /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_441_2_2, /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_48_1_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_48_1_2,  /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_48_2_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_48_2_2,  /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_48_3_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_48_3_2,  /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_48_4_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_48_4_2,  /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_48_5_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_48_5_2,  /**< Config setting for high quality audio data */
        BAP_CODEC_CONFIG_48_6_1,  /**< Config setting for low latency audio data */
        BAP_CODEC_CONFIG_48_6_2,  /**< Config setting for high quality audio data */
#if ENABLE_LC3_PLUS
        /* LC3plus High Resolution VBR codec configurations */
        BAP_CODEC_CONFIG_48_1_LC3plusHR_VBR,  /* 48K, FD=10ms, mandatory for Broadcast */
        BAP_CODEC_CONFIG_48_2_LC3plusHR_VBR,  /* 48K, FD=10ms, high quality */
        BAP_CODEC_CONFIG_48_3_LC3plusHR_VBR,  /* 48K, FD=7.5ms, low latency */
        BAP_CODEC_CONFIG_48_4_LC3plusHR_VBR,  /* 48K, FD=7.5ms, high quality */
        BAP_CODEC_CONFIG_48_5_LC3plusHR_VBR,  /* 48K, FD=5ms */
        BAP_CODEC_CONFIG_96_1_LC3plusHR_VBR,  /* 96K, FD=10ms, mandatory for Unicast */
        BAP_CODEC_CONFIG_96_2_LC3plusHR_VBR,  /* 96K, FD=10ms, high quality */
        BAP_CODEC_CONFIG_96_3_LC3plusHR_VBR,  /* 96K, FD=7.5ms, low latency */
        BAP_CODEC_CONFIG_96_4_LC3plusHR_VBR,  /* 96K, FD=7.5ms, high quality */
        BAP_CODEC_CONFIG_96_5_LC3plusHR_VBR,  /* 96K, FD=5ms */
#endif /* ENABLE_LC3_PLUS */
    } ga_lib_bap_codec_config_t;

    typedef uint8_t ga_lib_bap_broadcast_code_t[BAP_BROADCAST_CODE_SIZE]; /**< Broadcast code value */

    typedef struct
    {
        ga_lib_bap_codec_config_t cc;
        uint32_t sf;                 /**< Sampling Frequency to be used for the configuration */
        uint32_t fd;                 /**< Frame duration to be used */
        uint32_t si;                 /**< SDU interval */
        uint16_t opcf;               /**< Octets per codec frame */
        wiced_ble_isoc_framing_t fr; /**< Framed or unframed */
        uint8_t rn;                  /**< Retransmission number */
        uint8_t mtl;                 /**< maximum transport latency */
    } ga_bap_config_t;

    /**
 * @brief Check if the transition results in a valid state
 *
 * This function validates whether a control point opcode can be applied to an ASE
 * in its current state according to the ASCS state machine. It uses a state transition
 * table to determine validity and calculates the resulting next state. Special handling
 * is provided for Sink ASEs which skip the Disabling state.
 *
 * @param[in] char_type     Characteristic type (Sink ASE or Source ASE)
 * @param[in] ase_state     Current ASE state (see #ga_lib_ascs_state_e)
 * @param[in] opcode        Control point opcode (see #ga_lib_ascs_opcode_e)
 * @param[out] next_state   Pointer to receive the next state if transition is valid
 *
 * @return TRUE if transition is valid, FALSE otherwise
 *
 * @note For Sink ASEs, the Disable opcode transitions directly to QoS Configured state
 */
    wiced_bool_t ga_lib_bap_is_state_transition_valid(ga_lib_ascs_characteristics_t char_type,
                                                      uint8_t ase_state,
                                                      uint8_t opcode,
                                                      uint8_t *next_state);

    /**
 * @brief Update ltv to stream
 *
 * @param[out] dest : pointer to stream to store ltv data the caller has to allocate the memory before calling
 * @param[in] length : GATT Connection ID
 * @param[in] type : ltv type which is being updated
 * @param[in] value : value of ltv which is being filled
 */
    int ga_lib_bap_update_ltv(uint8_t *dest, int max_len, uint8_t length, uint8_t type, const uint8_t *value);

    /**
 * @brief Fill codec specifc configuration
 *
 * @param[out] p_dst : pointer to stream to store codec specific configuration the caller has to allocate the memory before calling
 * @param[in] args_ptr : pointer to codec specific configuration to be filled to the stream
 */
    int ga_lib_bap_fill_csc(uint8_t *p_dst, int max_len, const ga_lib_ascs_csc_t *args_ptr);

    /**
 * @brief Parse codec specifc configuration
 *
 * @param[in] stream : pointer to stream to be parsed
 * @param[in] stream_len : length of the stream
 * @param[out] p_csc : pointer to the codec specific config structure which has to be filled, memory has to be allocated before calling
 */
    wiced_result_t ga_lib_bap_parse_csc(uint8_t *stream, int stream_len, ga_lib_ascs_csc_t *p_csc);

    /**
 * @brief Parse metadata from the stream
 *
 * @param[in] stream : pointer to stream to be parsed
 * @param[in] stream_len : length of the stream
 * @param[out] p_metadata : pointer to the metadata structure which has to be filled, memory has to be allocated before calling
 * @param[out] response : response of the parsing
 * @param[out] reason : reason if parsing was not successful
 */
    wiced_result_t ga_lib_bap_get_metadata(const uint8_t *stream,
                                           int stream_len,
                                           ga_lib_ascs_metadata_t *p_metadata,
                                           ga_lib_ascs_cp_cmd_sts_t *p_sts);

    /**
 * @brief Fill metadata to the stream
 *
 * @param[out] p_dst : pointer to stream to be filled, memory has to be allocated before calling
 * @param[in] p_metadata : pointer to the metadata structure
 */

    int ga_lib_bap_fill_metadata(uint8_t *p_dst, int max_len, ga_lib_ascs_metadata_t *p_metadata);

    /**
 * @brief Get the bit index for the specific sampling frequency
 *
 * @param[in] sampling_freq : Sampling frequency for which the bit index has to be found
 * @return bit_index : bit index for the sampling frequency provided
 */

    uint8_t ga_lib_bap_get_sampling_freq_index(uint32_t sampling_freq);

    uint32_t ga_lib_bap_get_sampling_freq_from_index(uint8_t index);

    /**
 * @brief Get the bit index for the frame duration
 *
 * @param[in] frame_duration : frame duration for which the bit index has to be found
 * @return bit_index : bit index for the frame duration provided
 */

    uint8_t ga_lib_bap_get_frame_duration_index(uint32_t frame_duration);

    uint32_t ga_lib_bap_get_frame_duration_from_index(uint8_t ota_val);

    /**
 * @brief Get Unicast Stream configuration for the codec configuration provided
 *
 * @param[in] codec_config : Codec config for which configuration values has to be provided
 * @param[out] p_stream_config : stream configuration values for the given config
 */

    wiced_result_t ga_lib_bap_get_unicast_stream_config(uint32_t codec_config,
                                                        ga_lib_bap_stream_config_t *p_stream_config);

    /**
 * @brief Get Broadcast Stream configuration for the codec configuration provided
 *
 * @param[in] codec_config : Codec config for which configuration values has to be provided
 * @param[out] p_stream_config : stream configuration values for the given config
 */

    wiced_result_t ga_lib_bap_get_broadcast_stream_config(uint32_t codec_config,
                                                          ga_lib_bap_stream_config_t *p_stream_config);

    /** Calculates output sample buffer size
 *
 * @param[in]  sampling_frequency    : Supported sampling rate
 * @param[in]  frame_duration        : Supported frame duration
 * @return  frame size : No. of samples in decoded data
 */
    uint32_t ga_lib_bap_get_decoded_data_size(uint32_t sampling_frequency, uint16_t frame_duration);

    /**@} wiced_bt_ga_bap */
    /**@} Stream_Control_APIs */

#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_BAP_H */
