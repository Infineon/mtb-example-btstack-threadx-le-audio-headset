/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
 *
 * Published Audio Capability Service implementation header
 */

#ifndef GA_LIB_PACS_H
#define GA_LIB_PACS_H

#include "gatt_interface.h"
#include "ga_lib_bap.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @addtogroup Published_Audio_Capability_Service_APIs
 * @{
 * @brief  PACS can be instantiated on devices that can accept the establishment of unicast Audio Streams or devices that can receive broadcast Audio Streams. Examples of such devices are speakers, headsets, hearing aids, and microphones.
 - Servers expose one or more sets of audio capabilities and audio availability. Sets of audio capabilities,known as Published Audio Capability (PAC) records, are exposed by using either the Sink PAC characteristic or Source PAC characteristic. Clients can discover and read these characteristics, and servers can notify these characteristics.
 */

    /**
 * @brief PACS characteristic enumeration
 *
 * Defines the characteristics exposed by PACS. The first GA_LIB_PACS_CHARACTERISTIC_MAX_UNIQUE
 * entries are unique (singleton), while Sink and Source PAC capability characteristics
 * may have multiple instances.
 */
#define GA_LIB_PACS_CHARACTERISTIC_SUPPORTED_AUDIO_CONTEXTS 0     /**< Supported Audio Contexts (singleton) */
#define GA_LIB_PACS_CHARACTERISTIC_AVAILABILE_AUDIO_CONTEXTS 1 /**< Available Audio Contexts (singleton) */
#define GA_LIB_PACS_CHARACTERISTIC_SNK_AUDIO_LOCATIONS 2       /**< Sink Audio Locations (singleton) */
#define GA_LIB_PACS_CHARACTERISTIC_SRC_AUDIO_LOCATIONS 3       /**< Source Audio Locations (singleton) */
#define GA_LIB_PACS_CHARACTERISTIC_MAX_UNIQUE 4                /**< Number of unique characteristics */
    /** Sink PAC capability (one or more instances) */
#define GA_LIB_PACS_CHARACTERISTIC_SNK_CAPABILITY  GA_LIB_PACS_CHARACTERISTIC_MAX_UNIQUE
#define GA_LIB_PACS_CHARACTERISTIC_SRC_CAPABILITY  5 /**< Source PAC capability (one or more instances) */
#define GA_LIB_PACS_CHARACTERISTIC_MAX             6 /**< Maximum characteristic index */
    typedef uint8_t ga_lib_pacs_characteristics_t;          /**< Type for PACS characteristics */

#define MAX_CODEC_SPECIFIC_CAPABILITIES_LENGTH 128 /**< Length of codec specific capabilities */
#define MAX_METADATA_LENGTH 128                    /**< Maximum length for metadata */

    /**
 * @brief PACS codec id information
 *
 */
    typedef struct
    {
        uint8_t coding_format;             /**< Coding format  */
        uint16_t company_id;               /**< Company ID from the assigned value document */
        uint16_t vendor_specific_codec_id; /**< Vendor specific codec id value */
    } ga_lib_pacs_codec_id_t;

    /**
 * @brief Defines PAC record
 *
 */
    typedef struct
    {
        ga_lib_pacs_codec_id_t codec_id;            /**< codec id information for the pac record */
        uint8_t codec_specific_capabilities_length; /**< Length of the codec specific capabilities */
        uint8_t codec_specific_capabilities[MAX_CODEC_SPECIFIC_CAPABILITIES_LENGTH]; /**< Codec specific capabilities */
        uint8_t metadata_length;               /**< Length of the metadata information */
        uint8_t metadata[MAX_METADATA_LENGTH]; /**< Metadata for the pacs record */
    } ga_lib_pacs_record_t;

    /**
 * @brief PACS characteristic data
 *
 */
    typedef struct
    {
        uint8_t num_records;             /**< Number of PACS records */
        ga_lib_pacs_record_t *p_records; /**< List of PACS records */
    } ga_lib_pacs_char_data_t;

    /**
 * @brief PACS audio context information
 *
 */
    typedef struct
    {
        ga_lib_bap_context_type_t src_contexts; /**< Source audio contexts */
        ga_lib_bap_context_type_t snk_contexts; /**< Sink audio contexts */
    } ga_lib_pacs_audio_contexts_t;

    typedef uint32_t
        ga_lib_pacs_audio_location_t; /**<Device-wide bitmap of supported Audio Location values for all PAC records */

    /**
 * @brief Defines all the fields required for Published Audio Capability Service
 *
 */
    typedef struct
    {
        uint8_t char_instance;                           /**< Characteristic instance of the pacs data */
        ga_lib_pacs_char_data_t src_pac_list;            /**< List of source pac records */
        ga_lib_pacs_char_data_t snk_pac_list;            /**< List of Sink pac records */
        ga_lib_pacs_audio_location_t src_audio_location; /**< Audio location for the source ase from assigned doc */
        ga_lib_pacs_audio_location_t snk_audio_location; /**< Audio location for the sink ase from assigned doc */
        ga_lib_pacs_audio_contexts_t supported;          /**< Supported audio contexts */
        ga_lib_pacs_audio_contexts_t available;          /**< Available audio contexts */
    } ga_lib_pacs_data_t;

    /**
     * @brief Discover Published Audio Capability Service on remote GATT server
     *
     * This function initiates GATT service discovery to find the PACS service and its
     * characteristics on a connected remote device. The discovery includes:
     * - Singleton characteristics: Supported/Available Audio Contexts, Sink/Source Audio Locations
     * - Sink PAC capability characteristics (one or more instances)
     * - Source PAC capability characteristics (one or more instances)
     *
     * Memory is allocated for the specified number of Sink and Source PAC characteristics.
     *
     * @param[in] conn_id           Connection identifier for the GATT connection
     * @param[in] pfn_on_complete   Callback function invoked when discovery completes
     * @param[in] max_snk_ase       Maximum number of Sink PAC capability characteristics to discover
     * @param[in] max_src_ase       Maximum number of Source PAC capability characteristics to discover
     *
     * @return Pointer to the service discovery context, or NULL on failure (e.g., allocation failure)
     *
     * @note Application must save the discovered handles to read PAC records and
     *       receive notifications of capability changes
     */
    gatt_intf_service_discovery_ctx_t *ga_lib_pacs_discover_service(uint16_t conn_id,
                                                                    pfn_on_discovery_complete_t pfn_on_complete,
                                                                    uint8_t max_snk_ase,
                                                                    uint8_t max_src_ase);

    /**
     * @brief Build read response packet for Sink/Source PAC characteristic
     *
     * This function serializes a list of PAC records into the over-the-air format
     * suitable for GATT read responses on the Sink PAC or Source PAC characteristic.
     * Each record is written as: codec_id (5 bytes) + codec_specific_capabilities (LTV)
     * + metadata (LTV). The function stops writing if the destination buffer would
     * overflow and reports the actual number of records written via p_num_records_written.
     *
     * Packet format:
     * - 1 byte: number of records
     * - For each record:
     *   - 1 byte: coding format
     *   - 2 bytes: company ID
     *   - 2 bytes: vendor specific codec ID
     *   - 1 byte: codec specific capabilities length (N)
     *   - N bytes: codec specific capabilities
     *   - 1 byte: metadata length (M)
     *   - M bytes: metadata
     *
     * @param[in] num_of_records         Number of records in p_record_list to serialize
     * @param[in] p_record_list          Pointer to array of PAC records
     * @param[out] p_ptr                 Pointer to destination buffer (must be pre-allocated)
     * @param[in] max_len                Maximum length of destination buffer in bytes
     * @param[out] p_num_records_written Pointer to receive the actual number of records written
     *                                   (may be less than num_of_records if buffer is too small)
     *
     * @return Number of bytes written to destination buffer
     */
    uint32_t ga_lib_pacs_build_read_rsp_sink_src_cap_pkt(uint8_t num_of_records,
                                                         ga_lib_pacs_record_t *p_record_list,
                                                         uint8_t *p_ptr,
                                                         int max_len,
                                                         int *p_num_records_written);

    /**@} Published_Audio_Capability_Service_APIs */
#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_PACS_H */
