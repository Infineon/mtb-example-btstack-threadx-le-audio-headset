/*
 * $ Copyright Cypress Semiconductor $
 */

#include "lehs.h"

#define UGT_SOURCE_FEATURE_SUPPORTED            (1 << 0)
#define UGT_80_KBPS_SOURCE_FEATURE_SUPPORTED    (1 << 1)
#define UGT_SINK_FEATURE_SUPPORTED              (1 << 2)
#define UGT_64_KBPS_SINK_FEATURE_SUPPORTED      (1 << 3)
#define UGT_MULTIPLEX_FEATURE_SUPPORTED         (1 << 4)
#define UGT_MULTISINK_FEATURE_SUPPORTED         (1 << 5)
#define UGT_MULTISOURCE_FEATURE_SUPPORT         (1 << 6)
#define BGR_MULTISINK_FEATURE_SUPPORTED         (1 << 0)
#define BGR_MULTIPLEX_FEATURE_SUPPORTED         (1 << 1)


wiced_result_t lehs_gmap_app_handle_read_complete(uint16_t conn_id,
                                                  lehs_clcb_t *p_clcb,
                                                  wiced_bt_gatt_data_t *p_gatt_data)
{
    lehs_gmap_data_t *p_gmap = &p_clcb->gmap;
    int index = gatt_intf_find_characteristic_type_by_value_handle(p_clcb->peer_profiles.gmap,
                                                                   GA_LIB_GMAP_CHARACTERISTIC_MAX,
                                                                   p_gatt_data->handle);
    uint8_t *p_data = p_gatt_data->p_data;

    WICED_BT_TRACE("[%s] handle %d", __FUNCTION__, p_gatt_data->handle);

    switch (index)
    {
    case GA_LIB_GMAP_CHARACTERISTIC_ROLE:
        STREAM_TO_UINT8(p_gmap->gmap_role, p_data);
        break;
    case GA_LIB_GMAP_CHARACTERISTIC_UGG_FEATURES:
        STREAM_TO_UINT8(p_gmap->ugg_features, p_data);
        break;
    case GA_LIB_GMAP_CHARACTERISTIC_UGT_FEATURES:
        STREAM_TO_UINT8(p_gmap->ugt_features, p_data);
        break;
    case GA_LIB_GMAP_CHARACTERISTIC_BGS_FEATURES:
        STREAM_TO_UINT8(p_gmap->bgs_features, p_data);
        break;
    case GA_LIB_GMAP_CHARACTERISTIC_BGR_FEATURES:
        STREAM_TO_UINT8(p_gmap->bgr_features, p_data);
        break;
    default:
        break;

    }
    return WICED_SUCCESS;
}

void lehs_gmap_init()
{
    lehs_gmap_data_t *p_gmap = &g_lehs_gatt_cb.local_service_data.gmap;
    p_gmap->gmap_role = GMAP_ROLE_UNICAST_GAME_TERMINAL | GMAP_ROLE_BROADCAST_GAME_RECEIVER;
    p_gmap->ugt_features = UGT_SOURCE_FEATURE_SUPPORTED | UGT_80_KBPS_SOURCE_FEATURE_SUPPORTED |
                           UGT_SINK_FEATURE_SUPPORTED | UGT_64_KBPS_SINK_FEATURE_SUPPORTED;

#if defined MULTIPLEX_AUDIO_SUPPORTED && (MULTIPLEX_AUDIO_SUPPORTED == 1)
    p_gmap->ugt_features |= UGT_MULTIPLEX_FEATURE_SUPPORTED;
    p_gmap->bgr_features = BGR_MULTIPLEX_FEATURE_SUPPORTED;
#else
    p_gmap->ugt_features |= UGT_MULTISINK_FEATURE_SUPPORTED;
    p_gmap->bgr_features = BGR_MULTISINK_FEATURE_SUPPORTED;
#endif
}
