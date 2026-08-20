/*
 * $ Copyright Cypress Semiconductor $
 */
#include "lehs.h"
#include "audio_driver.h"

#define MAX_PRESET_RECORDS 5
#define UNIVERSAL_PRESET "Universal"
#define OUTDOOR_PRESET "Outdoor"
#define NOISY_ENV_PRESET "Noisy Environment"

lehs_has_preset_rec_t lehs_has_preset_records[] = {{.rec.preset_index = 1,
                                                    .rec.properties = GA_LIB_HAS_PRESET_PROPERTIES_AVAILABLE,
                                                    .rec.name = UNIVERSAL_PRESET,
                                                    .rec.name_len = sizeof(UNIVERSAL_PRESET)},
                                                   {.rec.preset_index = GA_LIB_HAS_PRESET_PROPERTIES_AVAILABLE,
                                                    .rec.properties = 2,
                                                    .rec.name = OUTDOOR_PRESET,
                                                    .rec.name_len = sizeof(OUTDOOR_PRESET)},
                                                   {.rec.preset_index = 3,
                                                    .rec.properties = GA_LIB_HAS_PRESET_PROPERTIES_AVAILABLE,
                                                    .rec.name = NOISY_ENV_PRESET,
                                                    .rec.name_len = sizeof(NOISY_ENV_PRESET),
                                                    .rec.is_last = 1}};

#define LEHS_HAS_FEATURE (GA_LIB_HAS_FEATURES(GA_LIB_HAS_FEATURES_HEARING_AID_TYPE_BINAURAL, GA_LIB_HAS_FEATURES_PRESET_SYNC_NOT_SUPPORTED, \
                                         GA_LIB_HAS_FEATURES_INDEPENDENT_PRESETS_NOT_SUPPORTED, GA_LIB_HAS_FEATURES_DYNAMIC_PRESETS_NOT_SUPPORTED, \
                                         GA_LIB_HAS_FEATURES_WRITABLE_PRESETS_NOT_SUPPORTED))

lehs_has_data_t lehs_has_data = {.procedure_in_progress = 0,
                                 .active_preset_index = 0,
                                 .hearing_aid_features = LEHS_HAS_FEATURE,
                                 };


lehs_has_preset_rec_t * get_preset_rec(uint8_t preset_index)
{
    lehs_has_preset_rec_t *p_rec = lehs_has_preset_records;
    for (int index = 0; index < sizeof(lehs_has_preset_records) / sizeof(lehs_has_preset_records[0]); index++)
    {
        if (p_rec->rec.preset_index == preset_index)
        {
            return p_rec;
        }
        p_rec++;
    }
    return NULL;
}

wiced_result_t lehs_has_handle_read_preset_record(uint16_t conn_id, uint8_t start_preset, uint8_t num_presets)
{
    if (lehs_has_data.procedure_in_progress)
    {
        return (wiced_result_t)WICED_BT_GATT_PRC_IN_PROGRESS;
    }

    lehs_has_preset_rec_t *p_rec = get_preset_rec(start_preset);
    if (!p_rec)
    {
        return (wiced_result_t)WICED_BT_GATT_OUT_OF_RANGE;
    }

    lehs_has_data.procedure_in_progress = GA_LIB_HAS_OPCODE_READ_PRESETS_REQUEST;
    int limit = sizeof(lehs_has_preset_records) / sizeof(lehs_has_preset_records[0]);

    for (int i = p_rec - lehs_has_preset_records; num_presets && (i < limit); i++, p_rec++, num_presets--)
    {
        ga_lib_has_notify_read_preset_rsp(conn_id, HDLC_HAS_HEARING_AID_PRESET_CONTROL_POINT_VALUE, &p_rec->rec);
    }
    lehs_has_data.procedure_in_progress = GA_LIB_HAS_OPCODE_INVALID;

    return WICED_SUCCESS;
}

wiced_result_t lehs_has_handle_write_preset_name(uint16_t conn_id, uint8_t preset_index, uint8_t *p_data, uint8_t len_to_write)
{
    lehs_has_preset_rec_t *p_lehs_rec;
    ga_lib_has_preset_records_t *p_rec;

    if (lehs_has_data.procedure_in_progress)
    {
        return (wiced_result_t)WICED_BT_GATT_PRC_IN_PROGRESS;
    }

    if (!GA_LIB_HAS_WRITABLE_PRESETS_SUPPORTED(lehs_has_data.hearing_aid_features))
    {
        return (wiced_result_t)GA_LIB_HAS_ERROR_WRITE_NAME_NOT_ALLOWED;
    }

    p_lehs_rec = get_preset_rec(preset_index);
    if (!p_lehs_rec)
    {
        return (wiced_result_t)WICED_BT_GATT_OUT_OF_RANGE;
    }

    p_rec = &p_lehs_rec->rec;

    if (!(p_rec->properties & 1))
    {
        return (wiced_result_t)GA_LIB_HAS_ERROR_WRITE_NAME_NOT_ALLOWED;
    }

    if (len_to_write > (sizeof(p_rec->name) - 1))
    {
        return (wiced_result_t)GA_LIB_HAS_ERROR_INVALID_PARAMETERS_LENGTH;
    }

    WICED_MEMSET(p_rec->name, 0, sizeof(p_rec->name));
    STREAM_TO_ARRAY(p_rec->name, p_data, len_to_write);

    ga_lib_has_cp_rsp_preset_changed_t preset_changed;

    preset_changed.change_id = GA_LIB_HAS_GENERIC_UPDATE;
    preset_changed.p_rec = p_rec;
    if (p_rec->preset_index == 1)
    {
        preset_changed.prev_index = 1;
    }
    else
    {
        preset_changed.prev_index = p_rec->preset_index - 1;
    }

    ga_lib_has_notify_preset_changed(conn_id, HDLC_HAS_HEARING_AID_PRESET_CONTROL_POINT_VALUE, &preset_changed);

    return WICED_SUCCESS;
}


wiced_result_t lehs_has_set_active_preset(uint16_t conn_id, uint8_t preset_index)
{
    lehs_has_preset_rec_t *p_rec = get_preset_rec(preset_index);

    if (!p_rec)
    {
        return (wiced_result_t)WICED_BT_GATT_OUT_OF_RANGE;
    }

    if (!(p_rec->rec.properties & 2))
    {
        return (wiced_result_t)GA_LIB_HAS_ERROR_PRESET_OPERATION_NOT_POSSIBLE;
    }

    lehs_has_data.active_preset_index = preset_index;
    ga_lib_has_notify_active_preset_index(conn_id, HDLC_HAS_ACTIVE_PRESET_INDEX_VALUE, preset_index);

    return WICED_SUCCESS;
}

wiced_result_t lehs_has_set_next_preset(uint16_t conn_id, int incr)
{
    lehs_has_preset_rec_t *p_rec = get_preset_rec(lehs_has_data.active_preset_index + incr);

    if (!p_rec)
    {
        int last = sizeof(lehs_has_preset_records) / sizeof(lehs_has_preset_records[0]) - 1;
        p_rec = (incr > 0) ? lehs_has_preset_records : &lehs_has_preset_records[last];
    }
    return lehs_has_set_active_preset(conn_id, p_rec->rec.preset_index);
}


wiced_result_t lehs_has_set_preset_synchronization(uint8_t opcode)
{
    if (!GA_LIB_HAS_PRESET_SYNC_SUPPORTED(lehs_has_data.hearing_aid_features))
    {
        return (wiced_result_t)GA_LIB_HAS_ERROR_PRESET_SYNCHRONIZATION_NOT_SUPPORTED;
    }

    return WICED_UNSUPPORTED;
}

void lehs_has_initialize_data(void)
{
    lehs_local_service_data_t *p_local = &g_lehs_gatt_cb.local_service_data;
    WICED_MEMCPY(&p_local->has, &lehs_has_data, sizeof(lehs_has_data));
}
