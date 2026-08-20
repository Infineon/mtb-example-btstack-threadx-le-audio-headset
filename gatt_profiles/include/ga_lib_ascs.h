/*
 * $ Copyright Cypress Semiconductor $
 */

#ifndef GA_LIB_ASCS_H
#define GA_LIB_ASCS_H

#include "gatt_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif



    /**
 * @addtogroup Stream_Control_APIs
 * @{
 */

#define GA_LIB_ASCS_CHARACTERISTIC_ASE_CONTROL_POINT 0 /**< ASCS characterisitic type control point */
#define GA_LIB_ASCS_CHARACTERISTIC_MAX_UNIQUE 1        /**< ASCS characteristic type max unique */
#define GA_LIB_ASCS_CHARACTERISTIC_SNK_ASE                                                                             \
    GA_LIB_ASCS_CHARACTERISTIC_MAX_UNIQUE    /**< ASCS characterisitic type Sink ASE */
#define GA_LIB_ASCS_CHARACTERISTIC_SRC_ASE 2 /**< ASCS characterisitic type Source ASE */
#define GA_LIB_ASCS_CHARACTERISTIC_MAX 3     /**< ASCS characterisitic type max */

    typedef uint8_t ga_lib_ascs_characteristics_t; /**< Enumeration of ASCS characteristic types */

    /**
 * @addtogroup ga_lib_ascs
 * @{
 * @brief
 ASCS can be instantiated on devices that can accept the establishment of unicast Audio Streams. Examples of such devices are speakers, headsets, hearing aids, earbuds, and wireless microphones.
	 - Two types of ASEs :
		  - Sink ASE characteristics represent Sink ASEs, to which audio data can flow. The server is said to act as Audio Sink for that ASE. There can be more than one Sink ASE characteristic on the server.
		  - Source ASE characteristics represent Source ASEs, from which audio data can flow. The server is said to act as Audio Source for that ASE. There can be more than one Source ASE characteristic on the server.
 */

#define SDU_INTERVAL_LENGTH 3       /**< Length in bytes for SDU interval */
#define PRESENTATION_DELAY_LENGTH 3 /**< Length in bytes for Presentation Delay */

    /**
 * @brief defines the response code that shall be used when the server rejects or cannot successfully complete
 * a client-initiated ASE Control operation.
 */
    enum ga_lib_ascs_cp_response_e
    {
        /** The server has successfully completed the client-initiated ASE Control operation */
        GA_LIB_ASCS_CP_RESPONSE_SUCCESS,
        /** The server does not support the client-initiated ASE Control operation defined by the opcode */
        GA_LIB_ASCS_CP_RESPONSE_UNSUPPORTED_OPCODE,
        /** The server has detected a truncated operation written by the client */
        GA_LIB_ASCS_CP_RESPONSE_INVALID_LENGTH,
        /** The server has detected that the ASE_ID written by the client does not match an ASE_ID in an
         * exposed ASE characteristic value for that client
         */
        GA_LIB_ASCS_CP_RESPONSE_INVALID_ASE_ID,
        /** The server has detected that the client-initiated ASE Control operation would cause an invalid
         * ASE state machine transition
         */
        GA_LIB_ASCS_CP_RESPONSE_INVALID_ASE_STATE_MACHINE_TRANSITION,
        /** The server has detected that the client-initiated ASE Control operation would cause an invalid
         * ASE direction
         */
        GA_LIB_ASCS_CP_RESPONSE_INVALID_ASE_DIRECTION,
        /** The server has detected that the audio capabilities requested during a Config Codec operation
         * are not supported (i.e., the server has not exposed the requested configuration in any PAC record)
         */
        GA_LIB_ASCS_CP_RESPONSE_UNSUPPORTED_AUDIO_CAPABILITIES,
        /** The server has detected it does not support one or more parameter values written by the client */
        GA_LIB_ASCS_CP_RESPONSE_UNSUPPORTED_CONFIGURATION_PARAMETER_VALUE,
        /** The server has rejected one or more parameter values written by the client */
        GA_LIB_ASCS_CP_RESPONSE_REJECTED_CONFIGURATION_PARAMETER_VALUE,
        /** The server has detected one or more invalid parameter values written by the client */
        GA_LIB_ASCS_CP_RESPONSE_INVALID_CONFIGURATION_PARAMETER_VALUE,
        /** The server has detected an unsupported Metadata Type written by the profile. */
        GA_LIB_ASCS_CP_RESPONSE_UNSUPPORTED_METADATA,
        /** The server has rejected an unsupported Metadata Type written by the client */
        GA_LIB_ASCS_CP_RESPONSE_REJECTED_METADATA,
        /** This Response_Code is used to inform the client that the Metadata Value is incorrectly formatted. */
        GA_LIB_ASCS_CP_RESPONSE_INVALID_METADATA,
        /** The server is unable to successfully complete the client-initiated ASE Control operation because of
          * insufficient resources
          */
        GA_LIB_ASCS_CP_RESPONSE_INSUFFICIENT_RESOURCES,
        /** The server has encountered an unspecified error */
        GA_LIB_ASCS_CP_RESPONSE_UNSPECIFIED_ERROR
    };

    typedef uint8_t ga_lib_ascs_cp_response_t; /**< ASCS Response code (see \ref ga_lib_ascs_cp_response_e) */

    /**
 * @brief defines the reason values that shall be used when the server rejects or cannot successfully complete a
 * client-initiated ASE Control operation.
 */
    enum ga_lib_ascs_cp_reason_e
    {
        /** Reason value is not applicable */
        GA_LIB_ASCS_CP_REASON_NOT_APPLICABLE,
        /** The server has detected that the codec ID value used in configuration parameters by the client is invalid
         */
        GA_LIB_ASCS_CP_REASON_CODEC_ID,
        /** The server has detected that the codec specific configuration value used by the client is invalid */
        GA_LIB_ASCS_CP_REASON_CODEC_SPECIFIC_CONFIGURATION,
        /** The server has detected that the SDU interval used in configuration parameters by the client is out of
          * range
          */
        GA_LIB_ASCS_CP_REASON_SDU_INTERVAL,
        /** The server has detected that the framing value used in configuration parameters by the client is invalid */
        GA_LIB_ASCS_CP_REASON_FRAMING,
        /** The server has detected that the phy value used in configuration parameters by the client is invalid */
        GA_LIB_ASCS_CP_REASON_PHY,
        /** The server has detected that the maximum sdu value used in configuration parameters by the client is
          * invalid
          */
        GA_LIB_ASCS_CP_REASON_MAXIMUM_SDU_SIZE,
        /** The server has detected that the retransmission number used in configuration parameters by the client is
          * invalid
          */
        GA_LIB_ASCS_CP_REASON_RETRANSMISSION_NUMBER,
        /** The server has detected that the transport latency used in configuration parameters by the client is
          * invalid
          */
        GA_LIB_ASCS_CP_REASON_MAX_TRANSPORT_LATENCY,
        /** The server has detected that the presentation delay used in configuration parameters by the client is
          * invalid
          */
        GA_LIB_ASCS_CP_REASON_PRESENTATION_DELAY,
        /** The server has detected that the cis value used in configuration parameters by the client is invalid */
        GA_LIB_ASCS_CP_REASON_INVALID_ASE_CIS_MAPPING,
    };
    typedef uint8_t ga_lib_ascs_cp_reason_t; /**< ASCS Reason code (see \ref ga_lib_ascs_cp_reason_e) */

    /**
 * @brief list of opcodes supported by ASCS
 */
    enum ga_lib_ascs_opcode_e
    {
        GA_LIB_ASCS_OPCODE_INVALID = 0,      /**< Invalid opcode */
        GA_LIB_ASCS_OPCODE_CONFIG_CODEC = 1, /**< Configures codec parameters for one or more ASEs.*/
        GA_LIB_ASCS_OPCODE_CONFIG_QOS,       /**< Configures preferred CIS parameters for one or more ASEs.*/
        GA_LIB_ASCS_OPCODE_ENABLE, /**< Applies codec parameters and preferred CIS parameters, applies any Metadata, and starts coupling an ASE to a CIS for one or more ASEs.*/
        GA_LIB_ASCS_OPCODE_RECEIVER_START_READY, /**< Signals that the Audio Sink is ready to receive audio data transmitted by the Audio Source, and completes coupling an ASE to a CIS*/
        GA_LIB_ASCS_OPCODE_DISABLE, /**< Starts decoupling a Source ASE from a CIS for one or more Source ASEs.*/
        GA_LIB_ASCS_OPCODE_RECEIVER_STOP_READY, /**< Signals that the Audio Sink is ready to stop receiving audio data transmitted by the Audio Source, and completes decoupling a Source ASE from a CIS.*/
        GA_LIB_ASCS_OPCODE_UPDATE_METADATA,     /**< Updates Metadata for one or more ASEs */
        GA_LIB_ASCS_OPCODE_RELEASE,             /**< Releases resources associated with an ASE */
        GA_LIB_ASCS_OPCODE_RELEASED, /**< Transitions an ASE from Releasing state to the Idle state or the Codec Configured state*/
        GA_LIB_ASCS_OPCODE_MAX, /**< Max opcode, not applicable*/
    };

    typedef uint8_t ga_lib_ascs_opcode_t; /**< ASCS Opcode (see #ga_lib_ascs_opcode_e) */

    /**
 * @brief list of states supported by the ASE
 */
    enum ga_lib_ascs_state_e
    {
        GA_LIB_ASCS_STATE_IDLE,             /**< ASE state is idle  */
        GA_LIB_ASCS_STATE_CODEC_CONFIGURED, /**< ASE state is codec configured  */
        GA_LIB_ASCS_STATE_QOS_CONFIGURED,   /**< ASE state is qos configured  */
        GA_LIB_ASCS_STATE_ENABLING,         /**< ASE state is in enabling */
        GA_LIB_ASCS_STATE_STREAMING,        /**< ASE state is in streaming state */
        GA_LIB_ASCS_STATE_DISABLING,        /**< ASE state is in disabling */
        GA_LIB_ASCS_STATE_RELEASING,        /**< ASE state is in releasing */
        GA_LIB_ASCS_STATE_MAX,              /**< Invalid ASE state */
    };

    typedef uint8_t ga_lib_ascs_state_t; /**< ASCS Opcode (see #ga_lib_ascs_state_e) */

    /**
 * @brief list of framing values used by ASCS
 */
    enum ga_lib_ascs_framing_e
    {
        GA_LIB_ASCS_UNFRAMED,       /**< Unframed ISOAL PDUs preferred */
        GA_LIB_ASCS_FRAMED,         /**< Framed ISOAL PDUs preferred */
        GA_LIB_ASCS_INVALID_FRAMING /**< Invalid value for Framing */
    };

    typedef uint8_t ga_lib_ascs_framing_t; /**< ASCS Framing values (see #ga_lib_ascs_framing_e) */

    enum ga_lib_ascs_supported_framing_e
    {
        GA_LIB_ASCS_UNFRAMED_ISOAL_PDUS_SUPPORTED,     /**< Unframed ISOAL PDUs Supported*/
        GA_LIB_ASCS_UNFRAMED_ISOAL_PDUS_NOT_SUPPORTED, /**< Framed ISOAL PDUs Not Supported */
    };

    typedef uint8_t
        ga_lib_ascs_supported_framing_e; /**< ASCS supported Framing (see #ga_lib_ascs_supported_framing_e) */
    /**
 * @brief list of PHY values used by ASCS
 */
    enum ga_lib_ascs_phy_e
    {
        GA_LIB_ASCS_PHY_1M = 1, /**< LE 1M PHY preferred */
        GA_LIB_ASCS_PHY_2M,     /**< LE 2M PHY preferred */
        GA_LIB_ASCS_PHY_CODED,  /**< LE Coded PHY preferred */
        GA_LIB_ASCS_INVALID_PHY /**< Invalid PHY value */
    };

    typedef uint8_t ga_lib_ascs_phy_t; /**< ASCS Phy values (see #ga_lib_ascs_phy_e) */

    /**
 * @brief list of announcement type values used by ASCS
 */

    enum ga_lib_ascs_announcement_type_e
    {
        GA_LIB_ASCS_GENERAL_ANNOUNCEMENT,  /**< General Announcement Type */
        GA_LIB_ASCS_TARGETED_ANNOUNCEMENT, /**< Targetted Announcement Type */
    };

    typedef uint8_t
        ga_lib_ascs_announcement_type_t; /**< ASCS Announcement values (see #ga_lib_ascs_announcement_type_e) */

#define GA_LIB_ASCS_VALID_PHY_MASK                                                                                     \
    ~(GA_LIB_ASCS_PHY_1M | GA_LIB_ASCS_PHY_2M | GA_LIB_ASCS_PHY_CODED) /**< Valid Phy Mask */

    /** @brief Codec Id structure */
    typedef struct
    {
        uint8_t coding_format;             /**< 0xFF if vendor specific */
        uint16_t company_id;               /**< company ID values if coding format is 0xFF otherwise 0 */
        uint16_t vendor_specific_codec_id; /**< vendor specific codec id if codec format is 0xFF otherwise 0 */
    } ga_lib_ascs_codec_id_t;

    /** @brief Codec Specific Configuration */
    typedef struct
    {
        uint32_t sampling_frequency_hz;    /**< sampling frequency in Hz */
        uint32_t frame_duration_us;        /**< framing duration in microseconds */
        uint32_t audio_channel_allocation; /**< audio channel allocation in codec specific configuration*/
        uint16_t octets_per_codec_frame;   /**< octects per codec frame in codec specific configuration*/
        uint8_t lc3_blocks_per_sdu;        /**< lc3 blocks per sdu in codec specific configuration*/
    } ga_lib_ascs_csc_t;

    /**
 * @brief defines the data received on config codec event (when peer initiates config codec operation)
 */
    typedef struct
    {
        /** Codec ID to be used for config codec  */
        ga_lib_ascs_codec_id_t codec_id;
        /** Provides context for the server to return meaningful values for QoS preferences in Codec Configured state*/
        uint8_t target_latency;
        /** PHY parameter target to achieve the Target_Latency value  */
        uint8_t target_phy;
        /**< Codec specific configuration value */
        ga_lib_ascs_csc_t csc;
    } ga_lib_ascs_config_codec_args_t;

    /**
 * @brief defines the QoS info
 *      1) received through config QoS command
 *      2) to be exposed to peer upon accepting config qos from client
 */
    typedef struct
    {
        uint8_t cig_id;                 /**< CIG ID set by the central */
        uint8_t cis_id;                 /**< CIS ID set by the central */
        uint32_t sdu_interval;          /**< SDU_Interval written by the client*/
        uint8_t framing;                /**< Framing written by the client*/
        uint8_t phy;                    /**< PHY written by the client*/
        uint16_t max_sdu;               /**< Max_SDU written by the client*/
        uint8_t retransmission_number;  /**< Retransmission_Number written by the client*/
        uint16_t max_transport_latency; /**< Max_Transport_Latency written by the client*/
        uint32_t presentation_delay;    /**< Presentation_Delay written by the client*/
    } ga_lib_ascs_config_qos_args_t;

    /** @brief Metadata information */
    typedef struct
    {
        uint8_t metadata_present;            /**< true if metadata is present */
        uint16_t preferred_audio_ctx;        /**< prefered audio context */
        uint16_t streaming_audio_ctx;        /**< streaming audio context */
        uint8_t vendor_specific_data_length; /**< vendor specifc data length if vendor data is present else 0 */
        uint8_t *p_vendor_specific_data;     /**< vendor specifc data */
        uint8_t upper_layer_data_length;     /**< upper layer data length if upper layer data is present else 0 */
        uint8_t *p_upper_layer_data;         /**< upper layer data */
        uint8_t program_info_len;            /**< program info length if program info is present else 0 */
        uint8_t *p_program_info;             /**< program info */
    } ga_lib_ascs_metadata_t;

    /**
 * @brief defines the information to be provided by the application when rejecting a request from client
 */
    typedef struct
    {
        uint8_t ase_id;                          /**< ASE id of the status */
        ga_lib_ascs_cp_response_t response_code; /**< Response code of the status */
        ga_lib_ascs_cp_reason_t reason;          /**< Error reason if any */
    } ga_lib_ascs_cp_cmd_sts_t;

    /**
 * @brief ASCS CP notification
 */
    typedef struct
    {
        ga_lib_ascs_opcode_t opcode;        /**< Opcode of control point notification */
        uint8_t num_of_ase;                 /**< Number of ASE considered in this notification */
        ga_lib_ascs_cp_cmd_sts_t *p_status; /**< Status of the control point operation */
    } ga_lib_ascs_cp_notif_t;

    /**
 * @brief ASE Preference values
 */

    typedef struct
    {
        ga_lib_ascs_supported_framing_e supported_framing; /**< Preferred server value for the Framing */
        ga_lib_ascs_phy_t preferred_phy;                   /**< Preferred server value for the PHY */
        uint8_t preferred_retransmission_number;           /**< Preferred server value for the Retransmission_Number */
        uint16_t max_transport_latency;                    /**< Preferred server value for the Max_Transport_Latency */
        uint32_t presentation_delay_in_us_min;             /**< Preferred minimum server supported Presentation_Delay */
        uint32_t presentation_delay_in_us_max;             /**< Preferred maximum server supported Presentation_Delay */
        uint32_t preferred_presentation_delay_in_us_min;   /**< Preferred minimum server supported Presentation_Delay */
        uint32_t preferred_presentation_delay_in_us_max;   /**< Preferred maximum server supported Presentation_Delay */
        uint16_t target_latency;                           /**< Target latency for the ASE in milliseconds */
        uint8_t target_phy;              /**< Target PHY for the ASE to achieve the Target_Latency value */
        ga_lib_ascs_codec_id_t codec_id; /**< Codec ID to be used for config codec  */
        ga_lib_ascs_csc_t csc;           /**< Client remote codec configuration data */
    } ga_lib_ascs_ase_codec_config_data_t;

    /**
 * @brief ASE data structure
 */
    typedef struct
    {
        uint8_t ase_id;                                     /**< Unique identifier for the ASE */
        uint8_t ase_type;                                   /**< ASE type : Source or Sink */
        wiced_ble_isoc_data_path_direction_t data_path_dir; /**< ASE data direction, H2C(0),  C2H(1) */
        ga_lib_ascs_state_t ase_state;                      /**< State of the ase for the client */
        //ga_lib_ascs_config_codec_args_t codec_configured;   /**< Client configured Codec params  */
        ga_lib_ascs_config_qos_args_t qos_configured; /**< Client configured QOS params    */
        ga_lib_ascs_metadata_t metadata;              /**< Metadata information for the ASE */
        ga_lib_ascs_ase_codec_config_data_t ase_cfg;  /**< Client remote codec configuration data */
    } ga_lib_ascs_ase_t;

    /** ASCS init data for service object */
    typedef struct
    {
        uint8_t max_snk_ase_spt; /**< MAX Number of Sink ASEs supported for ASCS service instance */
        uint8_t max_src_ase_spt; /**< MAX Number of Source ASEs supported for ASCS service instance */
    } ga_lib_ascs_init_data_t;

    /**
 * @brief Initialize the ASCS service/profile
 * @param[in] num_instances: Number of ASCS instances to be created.
 * @param[in] pv_ini: see \ref ga_lib_ascs_init_data_t
 *
 */
    wiced_result_t ga_lib_ascs_init(uint8_t num_instances, ga_lib_ascs_init_data_t *pv_ini);

    /**
 * @brief Enable ASCS server module
 */
    void ga_lib_ascs_enable_server(void);
    /**
 * @brief Enable ASCS client module
 */
    void ga_lib_ascs_enable_client(void);

    /**
     * @brief Parse Config Codec control point data from a byte stream
     *
     * This function parses the codec configuration data received from a Config Codec
     * control point operation, validating the parameters and extracting codec ID,
     * target latency, target PHY, and codec-specific configuration.
     *
     * @param[in] p_data_stream     Pointer to the byte stream containing codec config data
     * @param[in] length            Length of the data stream in bytes
     * @param[out] p_cc             Pointer to structure to receive parsed codec configuration
     * @param[out] p_sts            Pointer to status/reason code indicating any parsing errors
     *
     * @return GA_LIB_ASCS_CP_RESPONSE_SUCCESS on success, or error response code if parsing fails
     */
    ga_lib_ascs_cp_response_t ga_lib_ascs_parse_config_codec(uint8_t *p_data_stream,
                                                             int length,
                                                             ga_lib_ascs_config_codec_args_t *p_cc,
                                                             ga_lib_ascs_cp_reason_t *p_sts);

    /**
 * @brief Get the ASCS opcode and number of ASE from the stream
 *
 * @param[in] data :  data stream to be parsed
 * @param[out] opcode :  opcode for the control point operation
 * @param[out] num_of_ase :  number of ASE in the control point operation
 */

    int ga_lib_ascs_get_cp_header(uint8_t *data, uint8_t *opcode, uint8_t *num_of_ase);

    /**
 * @brief Return a printable ascs state string
 *
 * @param[in] ascs_state : see #ga_lib_ascs_state_e
 *
 * return A valid string or "unknown"
 */
    const char *ga_lib_ascs_get_state_string(ga_lib_ascs_state_t ascs_state);

    /**
 * @brief Return a printable ascs opcode string
 *
 * @param[in] ascs_opcode : see #ga_lib_ascs_opcode_e
 *
 * return A valid string or "unknown"
 */
    const char *ga_lib_ascs_get_opcode_string(ga_lib_ascs_opcode_t ascs_opcode);

    /**
 * @brief Return a printable ascs characteristic name string
 *
 * @param[in] ascs_char : see #ascs_characteristics_t
 *
 */
    char *ga_lib_ascs_get_characteristic_name(ga_lib_ascs_characteristics_t char_type);

    /**
     * @brief Discover Audio Stream Control Service on the remote GATT server
     *
     * This function initiates GATT service discovery to find the Audio Stream Control Service
     * and its characteristics (control point, sink ASEs, and source ASEs) on a connected remote device.
     * Memory is allocated for the specified number of ASE characteristics.
     *
     * @param[in] conn_id           Connection identifier for the GATT connection
     * @param[in] pfn_on_complete   Callback function to be invoked when service discovery completes
     * @param[in] max_snk_ase       Maximum number of Sink ASE characteristics to discover
     * @param[in] max_src_ase       Maximum number of Source ASE characteristics to discover
     *
     * @return Pointer to the service discovery context, or NULL on failure (e.g., allocation failure)
     */
    gatt_intf_service_discovery_ctx_t *ga_lib_ascs_discover_service(uint16_t conn_id,
                                                                    pfn_on_discovery_complete_t pfn_on_complete,
                                                                    uint8_t max_snk_ase,
                                                                    uint8_t max_src_ase);

    /**
     * @brief Build ASE response data in OTA format
     *
     * This function serializes an ASE structure into the over-the-air (OTA) format
     * suitable for GATT read responses or notifications. The format varies dynamically
     * based on the current ASE state (e.g., codec configured, QoS configured, streaming).
     *
     * @param[in] p_ase     Pointer to the ASE structure to serialize
     * @param[out] p_dest   Pointer to destination buffer for serialized data
     * @param[in] max_len   Maximum length of destination buffer in bytes
     *
     * @return Number of bytes written to destination buffer
     */
    uint16_t ga_lib_ascs_build_ase_response(ga_lib_ascs_ase_t *p_ase, uint8_t *p_dest, int max_len);

    /**
     * @brief Send notification for ASE state change
     *
     * This function sends a GATT notification to inform the client of changes to an
     * ASE characteristic. The ASE structure is serialized into OTA format before sending.
     *
     * @param[in] conn_id   Connection identifier for the GATT connection
     * @param[in] handle    Attribute handle of the ASE characteristic
     * @param[in] p_ase     Pointer to the ASE structure containing current state
     *
     * @return WICED_BT_GATT_SUCCESS if notification was sent successfully,
     *         otherwise a GATT error code
     */
    wiced_bt_gatt_status_t ga_lib_ascs_notify_ase_state(uint16_t conn_id, uint16_t handle, ga_lib_ascs_ase_t *p_ase);

    /**
     * @brief Send Receiver Start Ready command to ASE Control Point
     *
     * This function writes a Receiver Start Ready command to the ASE Control Point
     * characteristic to signal that the Audio Sink is ready to receive audio data.
     *
     * @param[in] conn_id   Connection identifier for the GATT connection
     * @param[in] p_handles Pointer to characteristic handles structure containing control point handle
     * @param[in] ase_id    ASE identifier for the target ASE
     *
     * @return WICED_BT_SUCCESS if command was sent successfully, otherwise error code
     */
    wiced_result_t ga_lib_ascs_send_receiver_start_ready(uint16_t conn_id,
                                                         gatt_intf_characteristic_handles_t *p_handles,
                                                         uint8_t ase_id);

    /**
     * @brief Send Receiver Stop Ready command to ASE Control Point
     *
     * This function writes a Receiver Stop Ready command to the ASE Control Point
     * characteristic to signal that the Audio Sink is ready to stop receiving audio data.
     *
     * @param[in] conn_id   Connection identifier for the GATT connection
     * @param[in] p_handles Pointer to characteristic handles structure containing control point handle
     * @param[in] ase_id    ASE identifier for the target ASE
     *
     * @return WICED_BT_SUCCESS if command was sent successfully, otherwise error code
     */
    wiced_result_t ga_lib_ascs_send_receiver_stop_ready(uint16_t conn_id,
                                                        gatt_intf_characteristic_handles_t *p_handles,
                                                        uint8_t ase_id);

    /**
     * @brief Send Release command to ASE Control Point
     *
     * This function writes a Release command to the ASE Control Point characteristic
     * to release resources associated with the specified ASE.
     *
     * @param[in] conn_id   Connection identifier for the GATT connection
     * @param[in] p_handles Pointer to characteristic handles structure containing control point handle
     * @param[in] ase_id    ASE identifier for the target ASE
     *
     * @return WICED_BT_SUCCESS if command was sent successfully, otherwise error code
     */
    wiced_result_t ga_lib_ascs_send_release(uint16_t conn_id,
                                            gatt_intf_characteristic_handles_t *p_handles,
                                            uint8_t ase_id);

    /**
     * @brief Send Disable command to ASE Control Point
     *
     * This function writes a Disable command to the ASE Control Point characteristic
     * to start decoupling a Source ASE from a CIS.
     *
     * @param[in] conn_id   Connection identifier for the GATT connection
     * @param[in] p_handles Pointer to characteristic handles structure containing control point handle
     * @param[in] ase_id    ASE identifier for the target ASE
     *
     * @return WICED_BT_SUCCESS if command was sent successfully, otherwise error code
     */
    wiced_result_t ga_lib_ascs_send_disable(uint16_t conn_id,
                                            gatt_intf_characteristic_handles_t *p_handles,
                                            uint8_t ase_id);

    /**
     * @brief Parse codec configuration from byte stream
     *
     * This function parses codec configuration parameters from a control point data stream,
     * including target latency, target PHY, codec ID, and codec-specific configuration.
     * Validation is performed and errors are reported via the status parameter.
     *
     * @param[in] p_stream      Pointer to byte stream containing codec config data
     * @param[in] length        Length of the stream in bytes
     * @param[out] p_cc         Pointer to structure to receive parsed codec configuration
     * @param[out] p_sts        Pointer to status structure for error reporting
     *
     * @return Number of bytes consumed from the stream
     */
    uint16_t ga_lib_ascs_parse_codec_config(uint8_t *p_stream,
                                            uint16_t length,
                                            ga_lib_ascs_config_codec_args_t *p_cc,
                                            ga_lib_ascs_cp_cmd_sts_t *p_sts);

    /**
     * @brief Parse QoS configuration from byte stream
     *
     * This function parses QoS configuration parameters from a control point data stream,
     * including CIG/CIS IDs, SDU interval, framing, PHY, max SDU, retransmission number,
     * max transport latency, and presentation delay. All parameters are validated against
     * specification requirements.
     *
     * @param[in] p_stream      Pointer to byte stream containing QoS config data
     * @param[in] length        Length of the stream in bytes (must be at least 15)
     * @param[out] p_qos        Pointer to structure to receive parsed QoS configuration
     * @param[out] p_sts        Pointer to status structure for error reporting
     *
     * @return Number of bytes consumed from the stream
     */
    uint16_t ga_lib_ascs_parse_config_qos(uint8_t *p_stream,
                                          uint16_t length,
                                          ga_lib_ascs_config_qos_args_t *p_qos,
                                          ga_lib_ascs_cp_cmd_sts_t *p_sts);

    /**
     * @brief Parse metadata from byte stream
     *
     * This function parses metadata parameters from a control point data stream.
     * The metadata includes preferred audio context, streaming audio context, vendor
     * specific data, upper layer data, and program info.
     *
     * @param[in] p_stream      Pointer to byte stream containing metadata
     * @param[in] length        Length of the stream in bytes
     * @param[out] p_metadata   Pointer to structure to receive parsed metadata
     * @param[out] p_sts        Pointer to status structure for error reporting
     *
     * @return Number of bytes consumed from the stream (including metadata length field)
     */
    uint16_t ga_lib_ascs_parse_metadata(uint8_t *p_stream,
                                        uint16_t length,
                                        ga_lib_ascs_metadata_t *p_metadata,
                                        ga_lib_ascs_cp_cmd_sts_t *p_sts);

    /**
     * @brief Build Config Codec control point command
     *
     * This function serializes codec configuration parameters into a byte stream
     * suitable for writing to the ASE Control Point characteristic. The output includes
     * ASE ID, target latency, target PHY, codec ID, and codec-specific configuration.
     *
     * @param[out] p_start  Pointer to destination buffer for serialized command
     * @param[in] len       Maximum length of destination buffer in bytes
     * @param[in] ase_id    ASE identifier for the target ASE
     * @param[in] p_cc      Pointer to codec configuration structure to serialize
     *
     * @return Number of bytes written to destination buffer
     */
    int ga_lib_ascs_build_cp_config_codec(uint8_t *p_start,
                                          int len,
                                          uint8_t ase_id,
                                          ga_lib_ascs_config_codec_args_t *p_cc);

    /**
     * @brief Build Config QoS control point command
     *
     * This function serializes QoS configuration parameters into a byte stream
     * suitable for writing to the ASE Control Point characteristic. The output includes
     * ASE ID and all QoS parameters.
     *
     * @param[out] p_start  Pointer to destination buffer for serialized command
     * @param[in] len       Maximum length of destination buffer in bytes
     * @param[in] ase_id    ASE identifier for the target ASE
     * @param[in] p_qos     Pointer to QoS configuration structure to serialize
     *
     * @return Number of bytes written to destination buffer
     */
    int ga_lib_ascs_build_cp_config_qos(uint8_t *p_start,
                                        int len,
                                        uint8_t ase_id,
                                        ga_lib_ascs_config_qos_args_t *p_qos);

    /**
     * @brief Build Update Metadata control point command
     *
     * This function serializes metadata into a byte stream suitable for writing to the
     * ASE Control Point characteristic. The output includes ASE ID, metadata length, and
     * LTV-formatted metadata fields (preferred audio context, streaming audio context,
     * program info, vendor specific data).
     *
     * @param[out] p_start      Pointer to destination buffer for serialized command
     * @param[in] len           Maximum length of destination buffer in bytes
     * @param[in] ase_id        ASE identifier for the target ASE
     * @param[in] p_metadata    Pointer to metadata structure to serialize
     *
     * @return Number of bytes written to destination buffer
     */
    int ga_lib_ascs_build_cp_metadata_cmd(uint8_t *p_start,
                                          int len,
                                          uint8_t ase_id,
                                          ga_lib_ascs_metadata_t *p_metadata);

    /**@} ga_lib_ascs */
    /**@} Stream_Control_APIs */

#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_ASCS_H */
