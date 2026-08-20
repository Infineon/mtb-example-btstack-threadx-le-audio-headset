/*
 * $ Copyright Cypress Semiconductor $
 */

#include "ga_lib_csis.h"
#include "lehs.h"

#define LOCK_TIMER_TIMEOUT 30

lehs_csis_data_t *lehs_get_csis_data(void)
{
    return &g_lehs_gatt_cb.local_service_data.csis;
}

void lehs_csis_set_size(uint8_t size)
{
    lehs_csis_data_t *p_csis = lehs_get_csis_data();
    p_csis->size = size;
}

void lehs_csis_set_rank(uint8_t rank)
{
    lehs_csis_data_t *p_csis = lehs_get_csis_data();
    p_csis->rank = rank;
}

void lehs_csis_set_sirk(ga_lib_csis_sirk_data_t *p_sirk)
{
    lehs_csis_data_t *p_csis = lehs_get_csis_data();
    p_csis->sirk_data = *p_sirk;
}

ga_lib_csis_sirk_data_t *lehs_csis_get_sirk(void)
{
    lehs_csis_data_t *p_csis = lehs_get_csis_data();
    return &p_csis->sirk_data;
}

void csis_lock_timer_timeout(WICED_TIMER_PARAM_TYPE p_inst)
{
    lehs_csis_data_t *p_csis = lehs_get_csis_data();

    p_csis->lock = GA_LIB_CSIS_UNLOCKED;
    wiced_stop_timer(&p_csis->lock_timer);
    if (p_csis->conn_id_of_lock_owner != 0)
    {
        ga_lib_csis_notify_lock(p_csis->conn_id_of_lock_owner, HDLC_CSIS_LOCK_VALUE, p_csis->lock);
    }
}

void lehs_csis_set_lock_timeout(uint16_t handle, uint8_t timeout_in_sec)
{
    lehs_csis_data_t *p_csis = lehs_get_csis_data();

    WICED_BT_TRACE("[%s]", __FUNCTION__);
    wiced_init_timer(&p_csis->lock_timer, csis_lock_timer_timeout, NULL, WICED_SECONDS_TIMER);
}

wiced_result_t lehs_csis_initialize_data(uint8_t set_size,
                                         uint8_t member_rank,
                                         ga_lib_csis_sirk_type_t sirk_type,
                                         const ga_lib_csis_sirk_t sirk)
{
    //set initial default values
    lehs_csis_data_t *p_csis = lehs_get_csis_data();
    p_csis->lock = GA_LIB_CSIS_UNLOCKED;
    p_csis->size = set_size;
    p_csis->rank = member_rank;
    p_csis->sirk_data.is_oob = WICED_FALSE;
    p_csis->sirk_data.sirk_type = sirk_type;
    memcpy(p_csis->sirk_data.sirk, sirk, GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN);

    lehs_csis_set_lock_timeout(HDLC_CSIS_LOCK_VALUE, LOCK_TIMER_TIMEOUT);
    return WICED_SUCCESS;
}

void lehs_encrypt_sirk(wiced_bt_device_address_t bdaddr,
                       ga_lib_csis_sirk_t *p_sirk_encrypted)
{
    wiced_bt_device_link_keys_t keys = {0};
    WICED_MEMCPY(keys.bd_addr, bdaddr, sizeof(wiced_bt_device_address_t));
    lehs_nvram_read_keys(&keys);
    lehs_csis_data_t *p_csis = lehs_get_csis_data();
    ga_lib_csis_sirk_encryption_func(&p_csis->sirk_data.sirk, &keys, p_sirk_encrypted);
}

wiced_result_t lehs_csis_handle_write_req_evt(uint16_t conn_id,
                                              lehs_clcb_t *p_clcb,
                                              uint8_t *p_attr,
                                              uint16_t len_to_write)
{
    lehs_csis_data_t *p_csis = lehs_get_csis_data();
    wiced_result_t result = WICED_SUCCESS;
    ga_lib_csis_lock_val_t lock_val;

    STREAM_TO_UINT8(lock_val, p_attr);
    if (lock_val > GA_LIB_CSIS_LOCKED)
    {
        return GA_LIB_CSIS_ERROR_INVALID_LOCK_VALUE;
    }
    if (lock_val == GA_LIB_CSIS_LOCKED && p_csis->lock == GA_LIB_CSIS_LOCKED)
    {
        if (p_csis->conn_id_of_lock_owner == conn_id)
        {
            return GA_LIB_CSIS_ERROR_LOCK_ALREADY_GRANTED;
        }

        return GA_LIB_CSIS_ERROR_LOCK_DENIED;
    }

    if (lock_val == GA_LIB_CSIS_UNLOCKED && p_csis->lock == GA_LIB_CSIS_UNLOCKED)
    {
        return WICED_SUCCESS;
    }

    if (lock_val == GA_LIB_CSIS_UNLOCKED && p_csis->lock == GA_LIB_CSIS_LOCKED)
    {
        if (p_csis->conn_id_of_lock_owner != conn_id)
        {
            return GA_LIB_CSIS_ERROR_LOCK_RELEASE_NOT_ALLOWED;
        }
        else
        {
            wiced_stop_timer(&p_csis->lock_timer);
        }
    }

    if (lock_val == GA_LIB_CSIS_LOCKED)
    {
        //assign exclusive access to client
        p_csis->conn_id_of_lock_owner = conn_id;

        //start locked timer
        wiced_start_timer(&p_csis->lock_timer, LOCK_TIMER_TIMEOUT);
    }

    /* notify all registered clients except the one who initiated the change */
    {
        lehs_clcb_t *p_clcb = g_lehs_gatt_cb.clcb;
        for (int i = 0; i < LEHS_MAX_CONNECTIONS; i++, p_clcb++)
        {
            if (p_clcb->in_use && p_clcb->conn_id != conn_id)
            {
                wiced_bt_gatt_status_t result =
                    ga_lib_csis_notify_lock(p_clcb->conn_id, HDLC_CSIS_LOCK_VALUE, lock_val);
                WICED_BT_TRACE("[%s] notify lock val to conn_id %d result %d", __FUNCTION__, p_clcb->conn_id, result);
            }
        }
    }

    return result;
}

