/*
 * $ Copyright Cypress Semiconductor $
 */


#include "ga_lib_pacs.h"

#ifdef PACS_DEBUG
#define PACS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get PACS library traces */
#else
#define PACS_TRACE(...)
#endif

const gatt_intf_characteristic_info_t pacs_characteristic_info[] = {
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_PACS_SUPPORTED_AUDIO_CONTEXT},
     .characteristic_type = GA_LIB_PACS_CHARACTERISTIC_SUPPORTED_AUDIO_CONTEXTS,
     .name = "supp_ctx"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_PACS_AUDIO_CONTEXT_AVAILABILITY},
     .uuid.uu.uuid16 = WICED_BT_UUID_PACS_AUDIO_CONTEXT_AVAILABILITY,
     .characteristic_type = GA_LIB_PACS_CHARACTERISTIC_AVAILABILE_AUDIO_CONTEXTS,
     .name = "ctx_avail"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_PACS_SINK_AUDIO_LOCATIONS},
     .characteristic_type = GA_LIB_PACS_CHARACTERISTIC_SNK_AUDIO_LOCATIONS,
     .name = "snk_loc"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_PACS_SOURCE_AUDIO_LOCATIONS},
     .uuid.uu.uuid16 = WICED_BT_UUID_PACS_SOURCE_AUDIO_LOCATIONS,
     .characteristic_type = GA_LIB_PACS_CHARACTERISTIC_SRC_AUDIO_LOCATIONS,
     .name = "src_loc"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_PACS_SINK_PAC},
     .characteristic_type = GA_LIB_PACS_CHARACTERISTIC_SNK_CAPABILITY,
     .name = "snk_pac"},
    {.uuid = {.len = 2, .uu.uuid16 = WICED_BT_UUID_PACS_SOURCE_PAC},
     .characteristic_type = GA_LIB_PACS_CHARACTERISTIC_SRC_CAPABILITY,
     .name = "src_pac"},
};

const gatt_intf_service_name_uuid_t pacs_id = {
    .name = "pacs",
    .uuid.len = 2,
    .uuid.uu.uuid16 = WICED_BT_UUID_PUBLISHED_AUDIO_CAPABILITY,
};

gatt_intf_service_discovery_ctx_t *ga_lib_pacs_discover_service(uint16_t conn_id,
                                                             pfn_on_discovery_complete_t pfn_on_pacs_complete,
                                                             uint8_t max_snk_ase,
                                                             uint8_t max_src_ase)
{
    int num_characteristics = GA_LIB_PACS_CHARACTERISTIC_MAX_UNIQUE + max_snk_ase + max_src_ase;
    gatt_intf_service_discovery_ctx_t *p_sdc =
        gatt_intf_alloc_service_discovery_ctx(num_characteristics, 0, sizeof(gatt_intf_service_discovery_ctx_t));
    if (!p_sdc)
        return NULL;


    gatt_intf_set_discovery_complete_callback(p_sdc, pfn_on_pacs_complete);

    uint8_t start =
        gatt_intf_copy_unique_characteristic_info(p_sdc, pacs_characteristic_info, GA_LIB_PACS_CHARACTERISTIC_MAX_UNIQUE);
    start = gatt_intf_repeat_characteristic_info(p_sdc,
                                                 start,
                                                 &pacs_characteristic_info[GA_LIB_PACS_CHARACTERISTIC_SNK_CAPABILITY],
                                                 max_snk_ase);
    start = gatt_intf_repeat_characteristic_info(p_sdc,
                                                 start,
                                                 &pacs_characteristic_info[GA_LIB_PACS_CHARACTERISTIC_SRC_CAPABILITY],
                                                 max_src_ase);

    gatt_intf_start_service_discovery(conn_id, &pacs_id, 1, p_sdc);

    return p_sdc;
}

uint32_t ga_lib_pacs_build_read_rsp_sink_src_cap_pkt(uint8_t num_of_records,
                                                  ga_lib_pacs_record_t *p_record_list,
                                                  uint8_t *p_ptr,
                                                  int max_len,
                                                  int *p_num_records_written)
{
    uint8_t *p_start = p_ptr;

    UINT8_TO_STREAM(p_ptr, num_of_records);

    for (uint32_t idx = 0; idx < num_of_records; idx++)
    {
        int remaining = max_len - (p_ptr - p_start);
        int required = 1 + 2 + 2 + /*coding_format + company_id + vendor_specific_codec_id */
                       1 +
                       p_record_list->codec_specific_capabilities_length + /*codec_specific_capabilities_length + len*/
                       1 + p_record_list->metadata_length;                 /*metadata_length + len */

        if (remaining < required)
        {
            break;
        }

        UINT8_TO_STREAM(p_ptr, p_record_list->codec_id.coding_format);
        UINT16_TO_STREAM(p_ptr, p_record_list->codec_id.company_id);
        UINT16_TO_STREAM(p_ptr, p_record_list->codec_id.vendor_specific_codec_id);

        UINT8_TO_STREAM(p_ptr, p_record_list->codec_specific_capabilities_length);
        if (p_record_list->codec_specific_capabilities_length)
        {
            ARRAY_TO_STREAM(p_ptr,
                            p_record_list->codec_specific_capabilities,
                            p_record_list->codec_specific_capabilities_length);
        }

        UINT8_TO_STREAM(p_ptr, p_record_list->metadata_length);
        if (p_record_list->metadata_length)
        {
            ARRAY_TO_STREAM(p_ptr, p_record_list->metadata, p_record_list->metadata_length);
        }

        p_record_list++;
        *p_num_records_written = idx + 1;
    }

    return (p_ptr - p_start);
}
