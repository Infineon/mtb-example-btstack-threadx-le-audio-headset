/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
 *
 * Broadcast Audio Scan Service Profile implementation header
 */

#ifndef GA_LIB_BASS_H
#define GA_LIB_BASS_H

#include "gatt_interface.h"
#include "ga_lib_bap.h"

#ifdef __cplusplus
extern "C"
{
#endif


    /**
 * @addtogroup Stream_Control_APIs
 * @{
 */

    /**
 * @addtogroup ga_lib_bass
 * @{

@brief - BASS can be instantiated on servers to solicit for clients to scan on behalf of the server for broadcast Audio Streams and associated data that are transmitted by Broadcast Sources.
 - Clients scanning on behalf of the server can help reduce the need to scan by the server and reduce power consumption on the server.
 - Servers can receive information from clients that is associated with broadcast Audio Streams, including decryption keys known as Broadcast_Codes necessary to decrypt encrypted BISes.
 */

/** BASS char control point */
#define GA_LIB_BASS_CHARACTERISTIC_BROADCAST_AUDIO_SCAN_CONTROL_POINT 0
/** BASS char max unique */
#define GA_LIB_BASS_CHARACTERISTIC_MAX_UNIQUE 1
/** BASS char broadcast receive state */
#define GA_LIB_BASS_CHARACTERISTIC_BROADCAST_RECEIVE_STATE GA_LIB_BASS_CHARACTERISTIC_MAX_UNIQUE
typedef uint8_t ga_lib_bass_characteristics_t; /**< BASS characteristic type */

#define GA_LIB_BASS_MAX_SUBGROUP_COUNT 4  /**< Max subgroups possible */
#define GA_LIB_BASS_MEDATA_MAX_LEN 255    /**< Max len of metadata allowed */
#define GA_LIB_BASS_ADV_DATA_SIZE 6       /**< BASS adv data length */
#define GA_LIB_BASS_SYNC_EST_MASK (1)     /**< BASS sync establishment mask */
#define GA_LIB_BASS_SYNC_LOST_MASK (uint32_t)(~1u)   /**< BASS sync lost mask */
#define GA_LIB_BASS_MAX_ADV_SID 0x0F      /**< MAX adv sid value */
#define GA_LIB_BASS_BIS_INDEX_MASK 0x7FFF /**< BIS index mask */

/** @brief PA Sync State. Refer BASS SPEC Broadcast Receive State Characteristic Section 3.2 */
#define GA_LIB_BASS_PA_NOT_SYNC 0                /**< Not synchronized to PA */
#define GA_LIB_BASS_PA_SYNC_INFO_REQUEST 1       /**< SyncInfo Request */
#define GA_LIB_BASS_PA_SYNC 2                    /**< Synchronized to PA */
#define GA_LIB_BASS_PA_FAILED_SYNC 3             /**< Failed to synchronize to PA */
#define GA_LIB_BASS_PA_NO_PAST 4                 /**< No PAST */
    typedef uint8_t ga_lib_bass_pa_sync_state_t; /**< PA Sync states */

/** PA Sync PARAM. Refer BASS SPEC Section 3.1.1.4 Add Source operation */
#define GA_LIB_BASS_PA_NO_SYNC 0                 /**< Do not synchronize to PA  */
#define GA_LIB_BASS_PA_SYNC_USING_PAST 1         /**< Synchronize to PA, PAST available */
#define GA_LIB_BASS_PA_SYNC_NO_PAST 2            /**< Synchronize to PA,  PAST not available */
    typedef uint8_t ga_lib_bass_pa_sync_param_t; /**< PA Sync param */

/** BIG Encryption state. Refer BASS SPEC Broadcast Receive State Characteristic Section 3.2 */
#define GA_LIB_BASS_BIG_NOT_ENCRYPTED 0                 /**< Not encrypted */
#define GA_LIB_BASS_BIG_BROADCAST_CODE_REQUIRED 1       /**< Broadcast_Code required */
#define GA_LIB_BASS_BIG_DECRPTING 2                     /**< Decrypting */
#define GA_LIB_BASS_BIG_BAD_BROADCAST_CODE 3            /**< Bad_Code (incorrect encryption key) */
    typedef uint8_t ga_lib_bass_big_encryption_state_t; /**<  BIG encryption states  */

/** BASS Control Point Opcode. Refer BASS SPEC Broadcast Audio Scan Control Point Characteristic Section 3.1*/
/** Informs the server that the client is not scanning for Broadcast Sources on behalf of the server */
#define GA_LIB_BASS_OP_REMOTE_SCAN_STOPPED 0
    /** Informs the server that the client is scanning for Broadcast Sources on behalf of the server. */
#define GA_LIB_BASS_OP_REMOTE_SCAN_STARTED 1
/** Requests the server to add information including Metadata for a Broadcast Source, and requests the server to
* synchronize to a PA and/or BIS transmitted by the Broadcast Source
*/
#define GA_LIB_BASS_OP_ADD_SOURCE 2
/** Requests the server to update Metadata, to synchronize to, or to stop synchronizing to a PA and/or BIS transmitted
 * by the Broadcast Source identified by the Source_ID
 */
#define GA_LIB_BASS_OP_MODIFY_SOURCE 3

/** Provides the server with the Broadcast_Code to decrypt a BIS transmitted by a Broadcast Source identified by the
 * Source_ID
 */
#define GA_LIB_BASS_OP_SET_BROADCAST_CODE 4
/** Requests the server to remove all information for a Broadcast Source identified by the Source_ID */
#define GA_LIB_BASS_OP_REMOVE_SOURCE 5
    typedef uint8_t ga_lib_bass_opcode_t; /**< BASS Opcodes */

/**
 * @brief BASS error code values
 */
#define GA_LIB_BASS_ERROR_OPCODE_NOT_SUPPORTED 0x80 /**< Opcode is invalid*/
#define GA_LIB_BASS_ERROR_INVALID_SOURCE_ID 0x81    /**< Invalid Source ID*/

    /** @brief Broadcast Subgroup data. BASS Control Point Opcode.
     * Refer BASS SPEC Broadcast Audio Scan Control Point Characteristic Section 3.1 */
    typedef struct
    {
        uint32_t bis_sync_state;          /**< BIG Sync State. Bit 0-30 = BIS_index[1-31]
                                                                 0b0 = Not synchronized to BIS
                                                                 0b1 = Synchronized to BIS */
        ga_lib_ascs_metadata_t meta_data; /**< LTV-formatted Metadata  */
    } ga_lib_bass_sub_group_data_t;

    /** @brief BASS Common Source Data */
    typedef struct
    {
        ga_lib_bass_pa_sync_param_t pa_sync_param; /**< PA Sync Parameter */
        uint16_t pa_interval; /**< SyncInfo field Interval parameter value. 0xFFFF: PA_Interval unknown*/
        uint8_t num_subgroup; /**< Number of subgroups  */
        ga_lib_bass_sub_group_data_t sub_group_data[GA_LIB_BASS_MAX_SUBGROUP_COUNT]; /**< Subgroup data array */
    } ga_lib_bass_common_source_data_t;

    /** @brief BASS Add Source Operation parameter */
    typedef struct
    {
        wiced_bt_ble_address_t source_addr; /**< Broadcast Source address */
        uint8_t adv_sid;                    /**< Advertising_SID subfield of the ADI field of the AUX_ADV_IND PDU or
    the LL_PERIODIC_SYNC_IND containing the SyncInfo that points to the PA transmitted by the Broadcast Source */
        uint32_t broadcast_id;              /**< Broadcast ID */
        ga_lib_bass_common_source_data_t src_data; /**< Broadcast Source Data */
    } ga_lib_bass_add_source_t;

    /** @brief BASS Modify Source Operation parameter */
    typedef struct
    {
        uint8_t source_id; /**< Source_ID assigned by the server to a Broadcast Receive State characteristic */
        ga_lib_bass_common_source_data_t src_data; /**< Broadcast Source Data */
    } ga_lib_bass_modify_source_t;

    /** @brief BASS Set Broadcast Code Operation parameter */
    typedef struct
    {
        uint8_t source_id; /**< Source_ID assigned by the server to a Broadcast Receive State characteristic */
        ga_lib_bap_broadcast_code_t broadcast_code; /**< Broadcast_Code for the Source_ID assigned to
        a Broadcast Receive State characteristic */
    } ga_lib_bass_set_broadcast_code_t;

    /** @brief BASS Operation data */
    typedef union
    {
        ga_lib_bass_add_source_t add_source_param;            /**< Add Source Parameter */
        ga_lib_bass_modify_source_t modify_source_param;      /**< Modifu Source Parameter */
        ga_lib_bass_set_broadcast_code_t set_broadcast_param; /**< Set Broadcast Code Parameter */
        uint8_t remove_source_id; /**< Source_ID assigned by the server to a Broadcast Receive State characteristic */
    } ga_lib_bass_operation_data_t;

    /** @brief BASS Receive State Data */
    typedef struct
    {
        /** Assigned by the server */
        uint8_t source_id;
        /** Source Address */
        wiced_bt_ble_address_t source_addr;
        /** Advertising_SID subfield of the ADI field of the AUX_ADV_IND PDU or the LL_PERIODIC_SYNC_IND containing
         * the SyncInfo that points to the PA transmitted by the Broadcast Source
         */
        uint8_t adv_sid;

        /** Broadcast_ID of the Broadcast Source */
        uint32_t broadcast_id;
        /** PA Sync State */
        ga_lib_bass_pa_sync_state_t pa_sync_state;
        /** BIG Encryption State */
        ga_lib_bass_big_encryption_state_t big_encryption_state;
        /** Broadcast_Code that fails to decrypt the BIG.
         * Valid Only if big_encryption_state=GA_LIB_BASS_BIG_BAD_BROADCAST_CODE */
        ga_lib_bap_broadcast_code_t broadcast_code;
        /** Number of subgroups */
        uint8_t num_subgroup;
        /** Subgroup Data */
        ga_lib_bass_sub_group_data_t *sub_group_data;
    } ga_lib_bass_receive_state_t;

    /** @brief BASS Operation Data */
    typedef struct
    {
        ga_lib_bass_opcode_t opcode;       /**< BASS Control Opcode */
        ga_lib_bass_operation_data_t data; /**< BASS Control Operation data */
    } ga_lib_bass_operation_t;

    /** BASS init data for service object */
    typedef struct
    {
        uint8_t max_broadcast_receive_state; /**< Max number of Broadcast Receive States per BASS service instance */
    } ga_lib_bass_init_data_t;

    /**
 * @brief Parse Broadcast Receive State characteristic header from byte stream
 *
 * This function parses the header portion of the Broadcast Receive State characteristic,
 * extracting source ID, source address, advertising SID, broadcast ID, PA sync state,
 * BIG encryption state, and number of subgroups. The subgroup data itself must be
 * parsed separately using ga_lib_bass_parse_sub_group_data().
 *
 * @param[in]   p_data          Pointer to byte stream received from peer device
 * @param[out]  p_recv_state    Pointer to structure to receive parsed receive state data
 *
 * @return Number of bytes consumed from the stream
 */
    uint8_t ga_lib_bass_parse_receive_state_char_header(const uint8_t *p_data, ga_lib_bass_receive_state_t *p_recv_state);

    /**
 * @brief Parse subgroup data from byte stream
 *
 * This function parses a single subgroup's data from the Broadcast Receive State
 * characteristic, extracting BIS sync state and LTV-formatted metadata.
 *
 * @param[in]   p_data          Pointer to byte stream received from peer device
 * @param[out]  sub_group_data  Pointer to structure to receive parsed subgroup data
 *
 * @return Number of bytes consumed from the stream
 */
    uint8_t ga_lib_bass_parse_sub_group_data(const uint8_t *p_data, ga_lib_bass_sub_group_data_t *sub_group_data);

    /**
 * @brief Parse BASS Control Point data from byte stream
 *
 * This function parses control point write data, extracting the opcode and
 * operation-specific parameters. It validates the opcode, length, and parameters
 * (e.g., advertising SID must be <= 0x0F, address type must be valid).
 *
 * Supported operations:
 * - Remote Scan Started/Stopped (no parameters)
 * - Add Source (source address, advertising SID, broadcast ID, PA sync params, subgroups)
 * - Modify Source (source ID, PA sync params, subgroups)
 * - Set Broadcast Code (source ID, 16-byte broadcast code)
 * - Remove Source (source ID)
 *
 * @param[out] p_operation_data Pointer to structure to receive parsed control point data
 * @param[in]  p_data           Pointer to byte stream received from peer device
 * @param[in]  total_len        Length of the stream in bytes
 *
 * @return WICED_BT_GATT_SUCCESS if parsing succeeded,
 *         GA_LIB_BASS_ERROR_OPCODE_NOT_SUPPORTED if opcode is invalid,
 *         WICED_BT_GATT_WRITE_REQ_REJECTED if parameters are invalid
 */
    wiced_bt_gatt_status_t ga_lib_bass_parse_control_point_data(ga_lib_bass_operation_t *p_operation_data,
                                                                const uint8_t *p_data,
                                                                uint16_t total_len);

    /**
 * @brief Check if advertising data contains BASS solicitation request
 *
 * This function examines advertising data to determine if the peer device is a
 * scan delegator (BASS server) soliciting for broadcast scan assistance. It looks
 * for the BASS service UUID in the solicitation request advertising data type.
 *
 * @param[in]  adv_len      Length of advertising data in bytes
 * @param[in]  p_adv_data   Pointer to advertising data buffer
 *
 * @return TRUE if peer device is a scan delegator, FALSE otherwise
 */

    wiced_bool_t ga_lib_bass_broadcast_is_solicitation_request(uint16_t adv_len, const uint8_t *p_adv_data);

    /**
 * @brief Discover Broadcast Audio Scan Service on remote GATT server
 *
 * This function initiates GATT service discovery to find the Broadcast Audio Scan
 * Service and its characteristics (control point and multiple receive state instances)
 * on a connected remote device. Memory is allocated for the specified number of
 * Broadcast Receive State characteristics.
 *
 * Discovery process:
 * 1. Discover BASS service by UUID (range 0x0001 to 0xFFFF)
 * 2. Discover characteristics within service range
 * 3. Discover characteristic descriptors for notification/indication support
 *
 * @param[in] conn_id               Connection identifier for the GATT connection
 * @param[in] pfn_on_complete       Callback function invoked when discovery completes
 * @param[in] max_receive_states    Maximum number of Broadcast Receive State characteristics
 *
 * @return Pointer to the service discovery context, or NULL on failure (e.g., allocation failure)
 *
 * @note Application must save the discovered service handles for subsequent operations
 */
    gatt_intf_service_discovery_ctx_t *ga_lib_bass_discover_service(uint16_t conn_id,
                                                                    pfn_on_discovery_complete_t pfn_on_complete,
                                                                    uint8_t max_receive_states);

    /**
 * @brief Build Broadcast Receive State characteristic data in OTA format
 *
 * This function serializes a Broadcast Receive State structure into the over-the-air
 * format suitable for GATT read responses or notifications. The format includes
 * source ID, address, advertising SID, broadcast ID, PA sync state, BIG encryption
 * state, optional bad broadcast code (if decryption failed), and subgroup data
 * with metadata.
 *
 * @param[in] p_recv_data   Pointer to receive state structure to serialize
 * @param[out] ptr          Pointer to destination buffer for serialized data
 * @param[in] max_len       Maximum length of destination buffer in bytes
 *
 * @return Number of bytes written to destination buffer, or 0 if no source is added
 *         (broadcast_id is 0 and source address is all zeros)
 *
 * @note If BIG encryption state is GA_LIB_BASS_BIG_BAD_BROADCAST_CODE, a 16-byte
 *       field of 0xFF is written to indicate the bad broadcast code
 */
    int ga_lib_bass_build_receive_state_char_data(ga_lib_bass_receive_state_t *p_recv_data, uint8_t *ptr, int max_len);

    /**
 * @brief Send notification for Broadcast Receive State characteristic
 *
 * This function sends a GATT notification to inform the client of changes to a
 * Broadcast Receive State characteristic. The receive state structure is serialized
 * into OTA format before sending.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Broadcast Receive State characteristic
 * @param[in] p_bass    Pointer to receive state structure containing current state
 *
 * @return WICED_BT_GATT_SUCCESS if notification was sent successfully,
 *         otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_bass_notify_recv_state(uint16_t conn_id,
                                                         uint16_t handle,
                                                         ga_lib_bass_receive_state_t *p_bass);

    /**@} ga_lib_bass */
    /**@} Stream_Control_APIs */

#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_BASS_H */
