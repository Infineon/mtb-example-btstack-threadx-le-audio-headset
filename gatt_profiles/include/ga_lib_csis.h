/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Coordinate Set Identification Profile implementation header
  */
#ifndef GA_LIB_CSIS_H
#define GA_LIB_CSIS_H

#include "gatt_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GA_LIB_CSIS_CHARACTERISTIC_SIRK 0          /**< SIRK characteristic */
#define GA_LIB_CSIS_CHARACTERISTIC_SIZE 1          /**< Size characteristic */
#define GA_LIB_CSIS_CHARACTERISTIC_LOCK 2          /**< Lock characteristic */
#define GA_LIB_CSIS_CHARACTERISTIC_RANK 3          /**< Rank characteristic */
#define GA_LIB_CSIS_CHARACTERISTIC_MAX 4           /**< Max characteristic */
    typedef uint8_t ga_lib_csis_characteristics_t; /**< CSIS characteristic type */

/**
 * @brief Definition for application error codes which can be sent by CSIS profile
 */
#define GA_LIB_CSIS_ERROR_LOCK_DENIED 0x80              /**< error Lock denied */
#define GA_LIB_CSIS_ERROR_LOCK_RELEASE_NOT_ALLOWED 0x81 /**< error Lock release not allowed*/
#define GA_LIB_CSIS_ERROR_INVALID_LOCK_VALUE 0x82       /**< error invalid lock value*/
#define GA_LIB_CSIS_ERROR_OOB_SIRK_ONLY 0x83 /**< error when server supports ga_lib_csis_sirk_t only through OOB */
#define GA_LIB_CSIS_ERROR_LOCK_ALREADY_GRANTED 0x84 /**< error when the requester is already owning the lock */

    /**
 * @addtogroup Coordinate_Set_APIs
 * @{
 */

    /**
 * @addtogroup ga_lib_csis_common
 * @{
 */

#define GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN 16   /**< Length of ga_lib_csis_sirk_t */
#define GA_LIB_CSIS_PRIVATE_SET_RANDOM_IDENTIFIER_LEN 6 /**< Length of PSRI */

    typedef uint8_t ga_lib_csis_sirk_t[GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN];    /**< CSIS SIRK */
    typedef uint8_t ga_lib_csis_psri_t[GA_LIB_CSIS_PRIVATE_SET_RANDOM_IDENTIFIER_LEN]; /**< CSIS PSRI */

    /**
 * @brief Definition for lock values which can be set by application
 */
    enum ga_lib_csis_lock_val_e
    {
        GA_LIB_CSIS_UNLOCKED = 1, /**< Lock value unlocked*/
        GA_LIB_CSIS_LOCKED = 2,   /**< Lock value locked */
    };

    typedef uint8_t ga_lib_csis_lock_val_t; /**< CSIS lock values (see #ga_lib_csis_lock_val_e) */

    /**
 * @brief Definition for ga_lib_csis_sirk_t types which can be set by application
 */

#define GA_LIB_CSIS_SIRK_ENCR 0U             /**< ga_lib_csis_sirk_t in encrypted format */
#define GA_LIB_CSIS_SIRK_PLAIN 1U            /**< ga_lib_csis_sirk_t in plain text */
    typedef uint8_t ga_lib_csis_sirk_type_t; /**< CSIS ga_lib_csis_sirk_t values (see #ga_lib_csis_sirk_type_e) */

    /** @brief ga_lib_csis_sirk_t Data */
    typedef struct
    {
        ga_lib_csis_sirk_t sirk;           /**< ga_lib_csis_sirk_t key */
        ga_lib_csis_sirk_type_t sirk_type; /**< ga_lib_csis_sirk_t type */
        uint8_t is_oob;                    /**< is ga_lib_csis_sirk_t to be obtained via OOB methods */
    } ga_lib_csis_sirk_data_t;

    /**
     * @brief Discover Coordinated Set Identification Service on remote GATT server
     *
     * This function initiates GATT service discovery to find the Coordinated Set
     * Identification Service and its characteristics (SIRK, Size, Lock, Rank) on a
     * connected remote device.
     *
     * Discovery process:
     * 1. Discover CSIS service by UUID (range 0x0001 to 0xFFFF)
     * 2. Discover characteristics within service range
     * 3. Discover characteristic descriptors for notification/indication support
     *
     * @param[in] conn_id           Connection identifier for the GATT connection
     * @param[in] pfn_on_complete   Callback function invoked when discovery completes
     *
     * @return Pointer to the service discovery context, or NULL on failure (e.g., allocation failure)
     *
     * @note Application must save the discovered service handles for subsequent operations
     */
    gatt_intf_service_discovery_ctx_t *ga_lib_csis_discover_service(uint16_t conn_id,
                                                                    pfn_on_discovery_complete_t pfn_on_complete);

    /**
     * @brief Calculate hash value for PSRI (Private Set Random Identifier)
     *
     * This function computes the hash component of PSRI using AES-CMAC encryption.
     * The hash is calculated as: hash = AES-CMAC(k, r || padding) mod 2^24
     * where k is the SIRK (128 bits), r is prand (24 bits), and padding is 104 bits of zeros.
     *
     * @param[in] k     Pointer to 128-bit SIRK key
     * @param[in] r     Pointer to 24-bit (3-byte) random value (prand)
     * @param[out] h    Pointer to buffer to receive 24-bit (3-byte) hash value
     *
     * @note The hash is computed using AES-CMAC and reduced modulo 2^24 (16777216)
     */
    void ga_lib_csis_calc_hash(const uint8_t *k, const uint8_t *r, uint8_t *h);

    /**
     * @brief Generate prand (random part of PSRI)
     *
     * This function generates a 24-bit (3-byte) random value (prand) that meets
     * CSIS specification requirements:
     * - Most significant bit (bit 23) = 0
     * - Second most significant bit (bit 22) = 1
     * - At least one bit of the remaining random part = 1
     * - At least one bit of the remaining random part = 0
     *
     * @param[out] p_prand  Pointer to 3-byte buffer to receive generated prand value
     */
    void ga_lib_csis_generate_prand(uint8_t *p_prand);

    /**
     * @brief Generate PSRI (Private Set Random Identifier)
     *
     * This function generates a complete 6-byte PSRI for advertising data.
     * PSRI consists of: [3-byte hash][3-byte prand]
     * The hash is calculated from the SIRK and prand using AES-CMAC.
     *
     * @param[in] sirk  Pointer to 16-byte SIRK (Set Identity Resolving Key)
     * @param[out] psri Pointer to store the generated PSRI
     *
     * @return
     *
     */
    void ga_lib_csis_generate_psri(const ga_lib_csis_sirk_t *sirk, ga_lib_csis_psri_t *psri);

    /**
     * @brief Encrypt SIRK using Link Key
     *
     * This function encrypts a plaintext SIRK using the BLE Long Term Key (LTK)
     * for secure transmission over GATT. The encryption uses the formula:
     * Encrypted_SIRK = k1(LTK, s1("SIRKenc"), "csis") XOR Plaintext_SIRK
     *
     * @param[in] sirk_plain_text   Pointer to 16-byte plaintext SIRK
     * @param[in] link_keys         Pointer to device link keys containing LTK
     * @param[out] sirk_encr        Pointer to buffer to receive 16-byte encrypted SIRK
     */
    void ga_lib_csis_sirk_encryption_func(ga_lib_csis_sirk_t *sirk_plain_text,
                                          wiced_bt_device_link_keys_t *link_keys,
                                          ga_lib_csis_sirk_t *sirk_encr);

    /**
     * @brief Decrypt SIRK using Link Key
     *
     * This function decrypts an encrypted SIRK using the BLE Long Term Key (LTK)
     * received over GATT. The decryption uses the formula:
     * Plaintext_SIRK = k1(LTK, s1("SIRKenc"), "csis") XOR Encrypted_SIRK
     *
     * @param[in] sirk_enc          Pointer to 16-byte encrypted SIRK
     * @param[in] link_keys         Pointer to device link keys containing LTK
     * @param[out] sirk_plain_text  Pointer to buffer to receive 16-byte plaintext SIRK
     */
    void ga_lib_csis_sirk_decryption_func(ga_lib_csis_sirk_t *sirk_enc,
                                          wiced_bt_device_link_keys_t *link_keys,
                                          ga_lib_csis_sirk_t *sirk_plain_text);

    /**
     * @brief Send notification for SIRK characteristic
     *
     * This function sends a GATT notification to inform the client of the SIRK value.
     * The notification includes the SIRK type (encrypted or plaintext) and the 16-byte
     * SIRK value in reversed byte order.
     *
     * @param[in] conn_id   Connection identifier for the GATT connection
     * @param[in] handle    Attribute handle of the SIRK characteristic
     * @param[in] p_sirk    Pointer to SIRK data structure containing type and value
     *
     * @return WICED_BT_GATT_SUCCESS if notification was sent successfully,
     *         otherwise a GATT error code
     *
     * @note SIRK bytes are reversed before transmission (LSB first)
     */
    wiced_bt_gatt_status_t ga_lib_csis_notify_sirk(uint16_t conn_id, uint16_t handle, ga_lib_csis_sirk_data_t *p_sirk);

    /**
     * @brief Send notification for Set Size characteristic
     *
     * This function sends a GATT notification to inform the client of the coordinated
     * set size (number of set members).
     *
     * @param[in] conn_id   Connection identifier for the GATT connection
     * @param[in] handle    Attribute handle of the Size characteristic
     * @param[in] value     Set size value (number of members in the coordinated set)
     *
     * @return WICED_BT_GATT_SUCCESS if notification was sent successfully,
     *         otherwise a GATT error code
     */
    wiced_bt_gatt_status_t ga_lib_csis_notify_sirk_size(uint16_t conn_id, uint16_t handle, uint8_t value);

    /**
* \brief Read lock state
* \details Reads the lock state of the csis instance
*
* @param[in] conn_id : GATT Connection ID
* @param[in] p_handle :  instance of the coordinated set identification client
*
* @return  wiced_bt_gatt_status_t result of the read operation
*/
    wiced_bt_gatt_status_t ga_lib_csis_read_lock_value(uint16_t conn_id,
                                                       gatt_intf_characteristic_handles_t *p_lock_handle);

    /**
* \brief Set lock state
* \details Sets the lock state of the csis instance
*
* @param[in]   conn_id : GATT Connection ID
* @param[in]   p_lock_handle :  instance of the coordinated set identification client
* @param[in]   lock_val  :    lock value to be set
*
* @return  wiced_bt_gatt_status_t result of the read operation
*/
    wiced_bt_gatt_status_t ga_lib_csis_request_lock(uint16_t conn_id,
                                                    gatt_intf_characteristic_handles_t *p_lock_handle,
                                                    ga_lib_csis_lock_val_t lock_val);

    /**
     * @brief Check if advertising device belongs to a coordinated set
     *
     * This function examines advertising data to determine if the advertising device
     * is a member of a coordinated set identified by the given SIRK. It searches for
     * the PSRI (Private Set Random Identifier) in the advertising data and verifies
     * it by computing the hash from the SIRK and prand, then comparing with the
     * received hash value.
     *
     * The verification process:
     * 1. Extract hash and prand from PSRI in advertising data
     * 2. Compute local hash = AES-CMAC(SIRK, prand || padding) mod 2^24
     * 3. Compare local hash with received hash
     *
     * @param[in] p_adv_report  Pointer to extended scan results structure
     * @param[in] adv_len       Length of advertising data in bytes
     * @param[in] p_adv_data    Pointer to advertising data buffer
     * @param[in] sirk          Pointer to 16-byte SIRK to check against
     *
     * @return TRUE if device belongs to the coordinated set, FALSE otherwise
     *
     * @note This function looks for BTM_BLE_ADVERT_TYPE_PSRI advertising data type
     */
    wiced_bool_t ga_lib_csis_check_if_belongs_to_coordinated_set(const wiced_ble_ext_scan_results_t *p_adv_report,
                                                                 uint16_t adv_len,
                                                                 const uint8_t *p_adv_data,
                                                                 const ga_lib_csis_sirk_t *sirk);

    /**
 * @brief Send notification for SIRK characteristic
 *
 * This function sends a GATT notification to inform the client of the SIRK value.
 * The notification includes the SIRK type (encrypted or plaintext) and the 16-byte
 * SIRK value in reversed byte order.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the SIRK characteristic
 * @param[in] p_sirk    Pointer to SIRK data structure containing type and value
 *
 * @return WICED_BT_GATT_SUCCESS if notification was sent successfully,
 *         otherwise a GATT error code
 *
 * @note SIRK bytes are reversed before transmission (LSB first)
 */
    wiced_bt_gatt_status_t ga_lib_csis_notify_sirk(uint16_t conn_id, uint16_t handle, ga_lib_csis_sirk_data_t *p_sirk);

    /**
 * @brief Send notification for Set Size characteristic
 *
 * This function sends a GATT notification to inform the client of the coordinated
 * set size (number of set members).
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Size characteristic
 * @param[in] value     Set size value (number of members in the coordinated set)
 *
 * @return WICED_BT_GATT_SUCCESS if notification was sent successfully,
 *         otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_csis_notify_sirk_size(uint16_t conn_id, uint16_t handle, uint8_t value);

    /**
 * @brief Send notification for Lock characteristic
 *
 * This function sends a GATT notification to inform the client of the lock state
 * (locked or unlocked).
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Lock characteristic
 * @param[in] value     Lock state value (GA_LIB_CSIS_UNLOCKED or GA_LIB_CSIS_LOCKED)
 *
 * @return WICED_BT_GATT_SUCCESS if notification was sent successfully,
 *         otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_csis_notify_lock(uint16_t conn_id, uint16_t handle, uint8_t value);
    /**@} ga_lib_csis_common */
    /**@} Coordinate_Set_APIs */

#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_CSIS_H */
