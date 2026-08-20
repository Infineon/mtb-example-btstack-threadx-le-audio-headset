/*
 * $ Copyright Cypress Semiconductor $
 */

/* Application includes */
#include "lehs.h"

/* BT Stack includes */
#include "wiced_hal_nvram.h"

#if (SIMULATED_NVRAM == 1)
lehs_nvram_data_t local_nvram_key_store = {0};
void lehs_nvram_init(void)
{
    WICED_MEMSET(&local_nvram_key_store, 0, sizeof(local_nvram_key_store));
}

static wiced_bool_t is_bd_addr_zero(wiced_bt_device_address_t bd_addr)
{
    wiced_bt_device_address_t null_addr = {0};
    return (WICED_MEMCMP(null_addr, bd_addr, BD_ADDR_LEN) == 0);
}

wiced_result_t app_handle_irk_request_evt(wiced_bt_local_identity_keys_t *p_id_keys)
{
    // Handle IRK request event
    WICED_BT_TRACE("[%s] IRK request event received\n", __FUNCTION__);
    wiced_bt_local_identity_keys_t id_keys = {0};
    if (WICED_MEMCMP(&id_keys, &local_nvram_key_store.local_id_keys, sizeof(wiced_bt_local_identity_keys_t)) == 0)
    {
        return WICED_BT_ERROR; // No valid keys found
    }
    WICED_MEMCPY(p_id_keys, &local_nvram_key_store.local_id_keys, sizeof(wiced_bt_local_identity_keys_t));
    return WICED_BT_SUCCESS;
}

void app_handle_irk_update_evt(wiced_bt_local_identity_keys_t *p_id_keys)
{
    // Handle IRK update event
    WICED_BT_TRACE("[%s] IRK update event received\n", __FUNCTION__);
    WICED_MEMCPY(&local_nvram_key_store.local_id_keys, p_id_keys, sizeof(wiced_bt_local_identity_keys_t));
    lehs_rpc_send_identity_resolving_key(&local_nvram_key_store.local_id_keys);
}

lehs_nvram_paired_device_key_t *lehs_nvram_get_paired_device_key_info(wiced_bt_device_address_t bd_addr)
{
    lehs_nvram_paired_device_key_t *p_pdkeys = local_nvram_key_store.paired_device_keys;

    for (int i = 0; i < MAX_NUM_DEVICES_IN_NVRAM; i++, p_pdkeys++)
    {
        if (!is_bd_addr_zero(bd_addr) &&
            ((WICED_MEMCMP(p_pdkeys->link_keys.bd_addr, bd_addr, BD_ADDR_LEN) == 0) ||
             (WICED_MEMCMP(p_pdkeys->link_keys.conn_addr, bd_addr, BD_ADDR_LEN) == 0)))
        {
            return p_pdkeys;
        }
    }
    return NULL;
}

lehs_nvram_paired_device_key_t * app_free_up_paired_device_key_info(void)
{
    // If all entries in NVRAM are occupied, overwrite the not connected one.
    int index = 0;
    lehs_nvram_paired_device_key_t *p_pdkeys = local_nvram_key_store.paired_device_keys;
    for (index = 0; index < MAX_NUM_DEVICES_IN_NVRAM; index++, p_pdkeys++)
    {
        lehs_clcb_t *p_clcb = lehs_gatt_get_clcb(p_pdkeys->link_keys.bd_addr);
        if (p_clcb == NULL)
        {
            break;
        }
    }

    WICED_BT_TRACE("[%s] Free up NVRAM entry at index %d for device %B",
                    __FUNCTION__,
                    index,
                    p_pdkeys->link_keys.bd_addr);
    return p_pdkeys;
}

uint16_t lehs_nvram_get_nvram_id(lehs_nvram_paired_device_key_t *p_pdkeys)
{
    return UNICAST_APP_NVRAM_ID_PAIRED_KEYS + (p_pdkeys - local_nvram_key_store.paired_device_keys);
}

// Allocate entry in NVRAM for new paired device and write the link keys to it.
lehs_nvram_paired_device_key_t *lehs_nvram_allocate_paired_device_key_info(wiced_bt_device_link_keys_t *p_linkkeys)
{
    lehs_nvram_paired_device_key_t *p_pdkeys = local_nvram_key_store.paired_device_keys;
    for (int i = 0; i < MAX_NUM_DEVICES_IN_NVRAM; i++, p_pdkeys++)
    {
        if (is_bd_addr_zero(p_pdkeys->link_keys.bd_addr))
        {
            break;
        }
    }

    /* free up one entry */
    if (p_pdkeys == NULL)
    {
        p_pdkeys = app_free_up_paired_device_key_info();
    }
    WICED_MEMCPY(&p_pdkeys->link_keys, p_linkkeys, sizeof(wiced_bt_device_link_keys_t));
    return p_pdkeys;
}

int lehs_nvram_write_keys(wiced_bt_device_link_keys_t *p_linkkeys)
{
    lehs_nvram_paired_device_key_t *p_pdkeys = lehs_nvram_get_paired_device_key_info(p_linkkeys->bd_addr);

    WICED_BT_TRACE("[%s] bda %B len %d %A",
                   __FUNCTION__,
                   p_linkkeys->bd_addr,
                   sizeof(wiced_bt_device_link_keys_t),
                   p_linkkeys,
                   sizeof(wiced_bt_device_link_keys_t));

    /* free up one entry */
    if (p_pdkeys == NULL)
    {
        p_pdkeys = lehs_nvram_allocate_paired_device_key_info(p_linkkeys);
    }
    else
    {
        if (WICED_MEMCMP(&p_pdkeys->link_keys.key_data, &p_linkkeys->key_data, sizeof(wiced_bt_device_sec_keys_t)) != 0)
        {
            wiced_bt_dev_remove_device_from_address_resolution_db(&p_pdkeys->link_keys);
        }
        WICED_MEMCPY(&p_pdkeys->link_keys, p_linkkeys, sizeof(wiced_bt_device_link_keys_t));
    }

    return lehs_nvram_get_nvram_id(p_pdkeys);
}

int lehs_nvram_read_keys(wiced_bt_device_link_keys_t *p_linkkeys)
{
    lehs_nvram_paired_device_key_t *p_pdkeys = lehs_nvram_get_paired_device_key_info(p_linkkeys->bd_addr);
    if (p_pdkeys == NULL)
    {
        return 0;
    }
    WICED_BT_TRACE("[%s] bda %B len %d",
                    __FUNCTION__,
                    p_linkkeys->bd_addr, sizeof(wiced_bt_device_link_keys_t));
    WICED_MEMCPY(p_linkkeys, &p_pdkeys->link_keys, sizeof(wiced_bt_device_link_keys_t));
    return lehs_nvram_get_nvram_id(p_pdkeys);
}

int app_read_paired_key_nvram_data(uint16_t nvram_id, lehs_nvram_paired_device_key_t *p_key_data)
{
    lehs_nvram_paired_device_key_t *p_pdkeys = local_nvram_key_store.paired_device_keys;
    for (int i = 0; i < MAX_NUM_DEVICES_IN_NVRAM; i++, p_pdkeys++)
    {
        if (lehs_nvram_get_nvram_id(p_pdkeys) == nvram_id)
        {
            WICED_MEMCPY(p_key_data, p_pdkeys, sizeof(lehs_nvram_paired_device_key_t));
            return sizeof(lehs_nvram_paired_device_key_t);
        }
    }
    return 0;
}

void lehs_nvram_write_data(uint16_t nvram_id, uint8_t *p_key_data, uint32_t data_len, uint16_t offset)
{
    lehs_nvram_paired_device_key_t *p_pdkeys = local_nvram_key_store.paired_device_keys;
    if (nvram_id == UNICAST_APP_NVRAM_ID_LOCAL_IRK)
    {
        if (wiced_ble_init_ctlr_private_addr_generation((wiced_bt_local_identity_keys_t *)p_key_data) == WICED_BT_SUCCESS)
        {
            WICED_MEMCPY(&local_nvram_key_store.local_id_keys, p_key_data, sizeof(wiced_bt_local_identity_keys_t));
        }
        return;
    }
    for (int i = 0; i < MAX_NUM_DEVICES_IN_NVRAM; i++, p_pdkeys++)
    {
        if (lehs_nvram_get_nvram_id(p_pdkeys) == nvram_id)
        {
            WICED_MEMCPY((uint8_t *)p_pdkeys + offset, p_key_data, data_len);
            wiced_bt_dev_add_device_to_address_resolution_db(&p_pdkeys->link_keys);

            break;
        }
    }

}

void lehs_nvram_delete_keys(uint16_t nvram_id)
{
    lehs_nvram_paired_device_key_t *p_pdkeys = local_nvram_key_store.paired_device_keys;
    if (nvram_id == UNICAST_APP_NVRAM_ID_LOCAL_IRK)
    {
        WICED_MEMSET(&local_nvram_key_store.local_id_keys, 0, sizeof(wiced_bt_local_identity_keys_t));
        return;
    }
    for (int i = 0; i < MAX_NUM_DEVICES_IN_NVRAM; i++, p_pdkeys++)
    {
        if (lehs_nvram_get_nvram_id(p_pdkeys) == nvram_id)
        {
            wiced_bt_dev_remove_device_from_address_resolution_db(&p_pdkeys->link_keys);
            lehs_clcb_t *p_clcb = lehs_gatt_get_clcb(p_pdkeys->link_keys.conn_addr);
            if (p_clcb)
            {
                lehs_disconnect_device(p_clcb->conn_id);
            }
            WICED_MEMSET(&p_pdkeys->link_keys, 0, sizeof(wiced_bt_device_link_keys_t));
            break;
        }
    }
}

void lehs_print_nvram_data(char *msg, lehs_nvram_paired_device_key_t *p_pdkeys)
{
    WICED_BT_TRACE("[%s] %s", __FUNCTION__, msg);
    WICED_BT_TRACE("[%s] NVRAM ID 0x%x %B size %d",
                   __FUNCTION__,
                   lehs_nvram_get_nvram_id(p_pdkeys),
                   p_pdkeys->link_keys.bd_addr,
                   sizeof(lehs_nvram_paired_device_key_t));
    uint8_t *p_link_keys = (uint8_t *)&p_pdkeys->link_keys;
    for (int i = 0; i < sizeof(wiced_bt_device_link_keys_t); i += 32)
    {
        int end = (i + 31) > sizeof(wiced_bt_device_link_keys_t) ? sizeof(wiced_bt_device_link_keys_t) : (i + 31);
        WICED_BT_TRACE("[%s] Link Keys[%d:%d]: %A", __FUNCTION__, i, end, p_link_keys + i, end - i + 1);
    }
    WICED_BT_TRACE("[dbhash] %A", p_pdkeys->db_hash, sizeof(wiced_bt_db_hash_t));
    {
        gatt_intf_characteristic_handles_t *p = p_pdkeys->peer_profiles.gmcs;
        WICED_BT_TRACE("[gmcs %d-%d] state %d cp %d",
                       p_pdkeys->peer_profiles.gmcs_service.start_handle,
                       p_pdkeys->peer_profiles.gmcs_service.end_handle,
                       p[GA_LIB_MCS_CHARACTERISTIC_MEDIA_STATE].value_handle,
                       p[GA_LIB_MCS_CHARACTERISTIC_MEDIA_CONTROL_POINT].value_handle);
    }
    {
        gatt_intf_characteristic_handles_t *p = p_pdkeys->peer_profiles.gtbs;
        WICED_BT_TRACE("[gtbs %d-%d] state %d cp %d",
                       p_pdkeys->peer_profiles.gtbs_service.start_handle,
                       p_pdkeys->peer_profiles.gtbs_service.end_handle,
                       p[GA_LIB_TBS_CHARACTERISTIC_CALL_STATE].value_handle,
                       p[GA_LIB_TBS_CHARACTERISTIC_CALL_CONTROL_POINT].value_handle);
    }
    {
        gatt_intf_characteristic_handles_t *p = p_pdkeys->peer_profiles.gmap;
        WICED_BT_TRACE("[gmap %d-%d] role %d",
                       p_pdkeys->peer_profiles.gmap_service.start_handle,
                       p_pdkeys->peer_profiles.gmap_service.end_handle,
                       p[GA_LIB_GMAP_CHARACTERISTIC_ROLE].value_handle);
    }
    {
        gatt_intf_characteristic_handles_t *p = p_pdkeys->peer_profiles.tmap;
        WICED_BT_TRACE("[tmap %d-%d] role %d",
                       p_pdkeys->peer_profiles.tmap_service.start_handle,
                       p_pdkeys->peer_profiles.tmap_service.end_handle,
                       p[GA_LIB_TMAP_CHARACTERISTIC_ROLE].value_handle);
    }
    WICED_BT_TRACE("[stored_cccd_bits] %08x-%08x", p_pdkeys->stored_cccd_bits[0], p_pdkeys->stored_cccd_bits[1]);
}

void lehs_save_device_data_to_nvram(lehs_clcb_t *p_clcb)
{
    lehs_nvram_paired_device_key_t *p_pdkeys = lehs_nvram_get_paired_device_key_info(p_clcb->identity_bd_address);
    if (p_pdkeys == NULL)
    {
        WICED_BT_TRACE("[%s] No paired device found for %B", __FUNCTION__, p_clcb->identity_bd_address);
        return;
    }
    WICED_MEMCPY(&p_pdkeys->peer_profiles, &p_clcb->peer_profiles, sizeof(lehs_peer_profiles_t));
    WICED_MEMCPY(p_pdkeys->db_hash, &p_clcb->db_hash, sizeof(wiced_bt_db_hash_t));
    WICED_MEMCPY(p_pdkeys->stored_cccd_bits, p_clcb->cccd_bits, sizeof(p_clcb->cccd_bits));

    lehs_print_nvram_data("Saving paired device data to NVRAM", p_pdkeys);
    lehs_rpc_send_link_keys(lehs_nvram_get_nvram_id(p_pdkeys), p_pdkeys);
}

#else

void lehs_nvram_init(void)
{
}

static int get_next_nvram_index(int current_nvram_index)
{
    if (current_nvram_index >= UNICAST_APP_NVRAM_ID_PAIRED_KEYS &&
        (current_nvram_index < UNICAST_APP_NVRAM_ID_END))
    {
        return current_nvram_index + 1;
    }

    if (current_nvram_index == 0)
    {
        return UNICAST_APP_NVRAM_ID_PAIRED_KEYS;
    }

    return 0;
}

int lehs_nvram_write(int nvram_id, wiced_bt_device_address_t bdaddr, uint8_t *p_data, uint32_t len)
{
    uint32_t write_len;
    wiced_result_t result;

    WICED_BT_TRACE_CRIT("[%s] nvram id 0x%x", __FUNCTION__, nvram_id);

    if ((nvram_id <= UNICAST_APP_NVRAM_ID_START) || (nvram_id >= UNICAST_APP_NVRAM_ID_END))
    {
        WICED_BT_TRACE_CRIT("[%s] bad id 0x%x", __FUNCTION__, nvram_id);
        return 0;
    }
    write_len = wiced_hal_write_nvram(nvram_id, len, (uint8_t *)p_data, &result);

    WICED_BT_TRACE_CRIT("[%s] write nvram result 0x%x", __FUNCTION__, result);

    if (write_len != len)
    {
        return 0;
    }

    WICED_BT_TRACE_CRIT("[%s] bytes written 0x%x", __FUNCTION__, write_len);
    return write_len;
}


int lehs_nvram_write_keys(wiced_bt_device_link_keys_t *p_linkkeys)
{
    int nvram_id = 0;
    lehs_nvram_data_t bonded_dev_info = {0};
    uint32_t last_used_nvram_id = 0;
    uint32_t write_last_used_nvram_id = 0;

    if (p_linkkeys == NULL)
    {
        WICED_BT_TRACE("[%s] p_linkkeys is null", __FUNCTION__, p_linkkeys);
        return 0;
    }

    WICED_BT_TRACE("[%s] bda %B %A",
                   __FUNCTION__,
                   p_linkkeys->bd_addr,
                   p_linkkeys,
                   sizeof(wiced_bt_device_link_keys_t));

    nvram_id = lehs_nvram_read_peer_device(p_linkkeys->bd_addr, &bonded_dev_info);
    if (nvram_id == 0)
    {
        WICED_BT_TRACE("[%s] nvram_id 0x%x", __FUNCTION__, nvram_id);
        if (!lehs_nvram_read(UNICAST_APP_NVRAM_ID_LAST_PAIRED_KEY,
                             p_linkkeys->bd_addr,
                             (uint8_t *)&last_used_nvram_id,
                             sizeof(last_used_nvram_id)))
        {
            // no last used nvram entry
            last_used_nvram_id = UNICAST_APP_NVRAM_ID_PAIRED_KEYS;
            WICED_BT_TRACE("[%s] no last used nvram id 0x%x", __FUNCTION__, last_used_nvram_id);
        }
        else
        {
            // found last used nvram entry, increment it
            WICED_BT_TRACE("[%s] last_used_nvram_id 0x%x", __FUNCTION__, last_used_nvram_id);
            last_used_nvram_id += (last_used_nvram_id + 1) % MAX_NUM_DEVICES_IN_NVRAM;
        }

        //create entry
        nvram_id = last_used_nvram_id;
        write_last_used_nvram_id = 1;
    }

    bonded_link_keys = *p_linkkeys;
    lehs_nvram_write(nvram_id, p_linkkeys->bd_addr, (uint8_t *)&bonded_dev_info, sizeof(bonded_dev_info));

    if (write_last_used_nvram_id)
    {
        lehs_nvram_write(UNICAST_APP_NVRAM_ID_LAST_PAIRED_KEY,
                         p_linkkeys->bd_addr,
                         (uint8_t *)&last_used_nvram_id,
                         sizeof(last_used_nvram_id));
    }

    return nvram_id;
}

int lehs_nvram_read_keys(wiced_bt_device_link_keys_t *p_linkkeys)
{
    int nvram_id = 0;
    lehs_nvram_data_t bonded_dev_info;

    if (p_linkkeys == NULL)
    {
        WICED_BT_TRACE("[%s] p_linkkeys is null", __FUNCTION__, p_linkkeys);
        return 0;
    }

    nvram_id = lehs_nvram_read_peer_device(p_linkkeys->bd_addr, &bonded_dev_info);
    if (nvram_id == 0)
    {
        return nvram_id;
    }

    *p_linkkeys = bonded_link_keys;

    WICED_BT_TRACE("[%s] %d %A", __FUNCTION__, nvram_id, p_linkkeys, sizeof(wiced_bt_device_link_keys_t));

    return nvram_id;
}

void lehs_nvram_delete(int nvram_id, wiced_bt_device_address_t bdaddr)
{
    wiced_result_t result = WICED_SUCCESS;
    wiced_hal_delete_nvram(nvram_id, &result);
    WICED_BT_TRACE("[%s] result %d", __FUNCTION__, result);
}
#endif
