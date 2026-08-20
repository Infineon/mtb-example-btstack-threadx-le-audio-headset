/*
 * $ Copyright Cypress Semiconductor $
 */

#ifndef __LEHS_NVRAM_H__
#define __LEHS_NVRAM_H__

#include "wiced_hal_nvram.h"

#define MAX_NUM_DEVICES_IN_NVRAM 10 /**< Maximum number of devices to store in NVRAM */
#define LEHS_MAX_CCCD_TO_STORE 27U  /**< Maximum number of CCCDs to store in NVRAM */
#define LEHS_MAX_DWORD_TO_STORE_CCCD(count)                                                                            \
    (((count) * 2) / 32 +                                                                                              \
     ((((count) * 2) % 32) ? 1 : 0)) /**< Calculate the number of DWORDs needed to store the CCCDs */

/**
 * @brief NVRAM IDs for storing application data
 */
enum
{
    UNICAST_APP_NVRAM_ID_START = (WICED_NVRAM_VSID_START), // 0x200  /**< Start of NVRAM ID range for the application */
    UNICAST_APP_NVRAM_ID_LOCAL_IRK,       // 0x201  /**< NVRAM ID for storing the local identity resolving key (IRK) */
    UNICAST_APP_NVRAM_ID_LAST_PAIRED_KEY, // 0x202 /**< NVRAM ID for storing the last paired device key */

    UNICAST_APP_NVRAM_ID_PAIRED_KEYS, // 0x203 /**< NVRAM ID for storing paired device keys */
    UNICAST_APP_NVRAM_ID_END = UNICAST_APP_NVRAM_ID_PAIRED_KEYS +
                               MAX_NUM_DEVICES_IN_NVRAM // 0x20D  /**< End of NVRAM ID range for the application */
};

/**
 * @brief Structure to hold the GATT service and characteristic handles for the peer device profiles
 */
typedef struct
{
    gatt_intf_service_range_t gmcs_service; /**< GMCS service range */
    gatt_intf_service_range_t gtbs_service; /**< GTBS service range */
    gatt_intf_service_range_t gmap_service; /**< GMAP service range */
    gatt_intf_service_range_t tmap_service; /**< TMAP service range */
    gatt_intf_characteristic_handles_t
        gmcs[GA_LIB_MCS_CHARACTERISTIC_MAX]; /**< Characteristic handles for GMCS profile */
    gatt_intf_characteristic_handles_t
        gtbs[GA_LIB_TBS_CHARACTERISTIC_MAX]; /**< Characteristic handles for GTBS profile */
    gatt_intf_characteristic_handles_t
        gmap[GA_LIB_GMAP_CHARACTERISTIC_MAX]; /**< Characteristic handles for GMAP profile */
    gatt_intf_characteristic_handles_t
        tmap[GA_LIB_TMAP_CHARACTERISTIC_MAX]; /**< Characteristic handles for TMAP profile */
} lehs_peer_profiles_t;

/*
 * @brief Structure to hold the paired device key information for NVRAM storage
 */

typedef struct
{
    wiced_bt_device_link_keys_t link_keys; /**< Link keys for the paired device */
    uint32_t stored_cccd_bits[LEHS_MAX_DWORD_TO_STORE_CCCD(
        LEHS_MAX_CCCD_TO_STORE)]; /**< Stored CCCD bits for the paired device */
    wiced_bt_db_hash_t db_hash;   /**< Database hash for the paired device */
    lehs_peer_profiles_t peer_profiles; /**< GATT service and characteristic handles for the paired device profiles */
} lehs_nvram_paired_device_key_t;

/*
* @brief Structure to hold the NVRAM data for the application, including local identity keys and paired device keys
*/
typedef struct
{
    wiced_bt_local_identity_keys_t local_id_keys; /**< Local identity keys for the device */
    lehs_nvram_paired_device_key_t
        paired_device_keys[MAX_NUM_DEVICES_IN_NVRAM]; /**< Array of paired device keys for NVRAM storage */
} lehs_nvram_data_t;


/**
 * @brief Read the paired device keys from NVRAM
 *
 * @param[out] p_linkkeys : Pointer to the structure to store the read link keys
 * @return nvram id on success, zero on failure
 */
int lehs_nvram_read_keys(wiced_bt_device_link_keys_t *p_linkkeys);

/*
*
 * @brief Write the paired device keys to NVRAM
 *
 * @param[in] p_linkkeys : Pointer to the structure containing the link keys to write
 * @return nvram id on success, zero on failure
 */
int lehs_nvram_write_keys(wiced_bt_device_link_keys_t *p_linkkeys);

/*
* @brief Initialize the NVRAM for the application, including reading local identity keys and paired device keys
*/
void lehs_nvram_init(void);

/*
* @brief Handle the IRK request event by reading the local identity keys from NVRAM
*
* @param[out] p_id_keys : Pointer to the structure to store the read local identity keys
* @return WICED_SUCCESS on success, otherwise an error code
*/
wiced_result_t app_handle_irk_request_evt(wiced_bt_local_identity_keys_t *p_id_keys);

/*
* @brief Handle the IRK update event by writing the local identity keys to NVRAM
*
* @param[in] p_id_keys : Pointer to the structure containing the local identity keys to write
*/
void app_handle_irk_update_evt(wiced_bt_local_identity_keys_t *p_id_keys);

/*
* @brief Handle the link keys update event by writing the paired device keys to NVRAM
*
* @param[in] nvram_id : NVRAM ID to write the paired device keys
* @param[in] p_key_data : Pointer to the structure containing the link keys to write
* @param[in] data_len : Length of the data to write
* @param[in] offset : Offset in the NVRAM to write the data
*/
void lehs_nvram_write_data(uint16_t nvram_id, uint8_t *p_key_data, uint32_t data_len, uint16_t offset);

/*
* @brief Delete the paired device keys from NVRAM for the given NVRAM ID
*
* @param[in] nvram_id : NVRAM ID to delete the paired device keys
*/
void lehs_nvram_delete_keys(uint16_t nvram_id);

/*
* @brief Get the NVRAM ID for the paired device keys for the given paired device key structure
*
* @param[in] p_pdkeys : Pointer to the structure containing the paired device keys
* @return NVRAM ID for the paired device keys, or zero if not found
*/
uint16_t lehs_nvram_get_nvram_id(lehs_nvram_paired_device_key_t *p_pdkeys);

/*
* @brief Get the paired device key information for the given Bluetooth device address
*
* @param[in] bd_addr : Bluetooth device address of the paired device
* @return Pointer to the structure containing the paired device key information, or NULL if not found
*/
lehs_nvram_paired_device_key_t *lehs_nvram_get_paired_device_key_info(wiced_bt_device_address_t bd_addr);

/*
* @brief Print the NVRAM data for debugging purposes
*
* @param[in] msg : Message to print before the NVRAM data
* @param[in] p_pdkeys : Pointer to the structure containing the paired device keys to print
*/
void lehs_print_nvram_data(char *msg, lehs_nvram_paired_device_key_t *p_pdkeys);

#endif
