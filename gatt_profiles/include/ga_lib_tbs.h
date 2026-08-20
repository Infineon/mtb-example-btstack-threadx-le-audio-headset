/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Call Control Profile implementation.
  */

#ifndef GA_LIB_TBS_H
#define GA_LIB_TBS_H

#include "gatt_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GA_LIB_TBS_CHARACTERISTIC_BEARER_PROVIDER_NAME 0         /**< Bearer provider name characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_BEARER_UCI 1                   /**< Bearer UCI characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_BEARER_TECHNOLOGY 2            /**< Bearer technology characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_BEARER_URI_SUPPORTED_SCHEMES 3 /**< Bearer URI supported schemes characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_BEARER_SIGNAL_STRENGTH 4       /**< Bearer signal strength characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL                                            \
    5 /**< Bearer signal strength reporting interval characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_BEARER_LIST_CURRENT_CALLS 6   /**< Bearer list current calls characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_CONTENT_CONTROL_ID 7          /**< Content control ID characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_INCOMING_CALL_TG_BEARER_URI 8 /**< Incoming call TG bearer URI characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_STATUS_FLAGS 9                /**< Status flags characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_CALL_STATE 10                 /**< Call state characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_CALL_CONTROL_POINT 11         /**< Call control point characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_CALL_CONTROL_POINT_OPTIONAL_OPCODE                                                   \
    12                                                       /**< Call control point optional opcode characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_CALL_TERMINATION_REASON 13 /**< Call termination reason characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_INCOMING_CALL 14           /**< Incoming call characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_CALL_FRIENDLY_NAME 15      /**< Call friendly name characteristic */
#define GA_LIB_TBS_CHARACTERISTIC_MAX 16                     /**< Maximum characteristic index */
    typedef uint8_t ga_lib_tbs_characteristics_t;                   /**< Type for TBS characteristics */

/**
 * @anchor TBS_FEATURE_BIT
 * @name TBS feature bit values
 * @{ */
#define GA_LIB_TBS_FEATURE_BIT_INBAND_RINGTONE 1 /**< inband ringtone: 0 = disabled, 1 = enabled */
#define GA_LIB_TBS_FEATURE_BIT_SILENT_MODE 2     /**< silent_mode:     0 = not in silent mode, 1 = in silent mode */
/** @} TBS_FEATURE_BIT */

/**
 * @anchor TBS_CONTROL_POINT_OPTIONAL_OPCODES
 * @name TBS controlpoint optional opcodes
 * @{ */
#define GA_LIB_TBS_FEATURE_BIT_JOIN 2       /**< Join Call Control Point Opcode:  0 = not supported, 1 = supported*/
#define GA_LIB_TBS_FEATURE_BIT_LOCAL_HOLD 1 /**< Local Hold and Local Retrieve: 0 = not supported 1 = supported*/
/** @} TBS_CONTROL_POINT_OPTIONAL_OPCODES */

/**
  * @anchor TBS_PARAM_LEN
  * @name TBS param length
  * @{ */
#define GA_LIB_TBS_BEARER_TECHNOLOGY_LEN 1                  /**< Bearer technology length*/
#define GA_LIB_TBS_SIGNAL_STRENGTH_LEN 1                    /**< Signal strength length*/
#define GA_LIB_TBS_SIGNAL_STRENGTH_REPORTING_INTERVAL_LEN 1 /**< Signal strength reporting interval length*/
#define GA_LIB_TBS_CONTENT_CONTROL_ID_LEN 1                 /**< Content control ID length */
#define GA_LIB_TBS_CCP_SUPPORTED_OPCODE 2                   /**< CCP Supported opcode length */
#define GA_LIB_TBS_BEARER_STATUS_FLAG_LEN 2                 /**< Status flag length */
#define GA_LIB_TBS_OPTIONAL_OPCODE_LEN 2                    /**< TBS optional opcode length */
#define GA_LIB_TBS_CALL_STATE_LEN 2                         /**< Call state length */
#define GA_LIB_TBS_CALL_ACTION_RESP_LEN 3                   /**< Call action response length */
                                                            /** @} TBS_PARAM_LEN */

/**
  * @anchor CALL_FLAG
  * @name Call flag values
  * @{ */
#define GA_LIB_TBS_INCOMING_CALL 0            /**< Call flag value for incoming call */
#define GA_LIB_TBS_OUTGOING_CALL 1            /**< Call flag value for outgoing call */
#define GA_LIB_TBS_INFO_PROVIDED_BY_SERVER 0  /**< Call flag value information provided by server */
#define GA_LIB_TBS_INFO_WITHHELD_BY_SERVER 2  /**< Call flag value information withheld by server */
#define GA_LIB_TBS_INFO_PROVIDED_BY_NETWORK 0 /**< Call flag value information provided by network */
#define GA_LIB_TBS_INFO_WITHHELD_BY_NETWORK 4 /**< Call flag value information withheld by network */
    /** @} CALL_FLAG */

    /** All possible termination reason */
    /** remote Caller ID value used to place a call was formed improperly. */
#define GA_LIB_TBS_IMPROPER_REMOTE_CALLER_ID 0x00
    /** unable to make the call */
#define GA_LIB_TBS_CALL_FAIL 0x01
    /** remote party ended the call */
#define GA_LIB_TBS_REMOTE_CALL_END 0x02
    /**< call ended from the server */
#define GA_LIB_TBS_SERVER_CALL_END 0x03
    /**< line busy */
#define GA_LIB_TBS_LINE_BUSY 0x04
    /**< network congestion */
#define GA_LIB_TBS_NETWORK_CONGESTION 0x05
    /**< client terminated the call */
#define GA_LIB_TBS_CLIENT_TERMINATED 0x06
    /**< No service */
#define GA_LIB_TBS_NO_SERVICE 0x07
    /**< No answer */
#define GA_LIB_TBS_NO_ANSWER 0x08
    /**< Reason is not specified */
#define GA_LIB_TBS_UNSPECIFIED 0x09
    typedef uint8_t ga_lib_tbs_call_termination_reason_t; /**< Termination reason */

    /** Bearer technology values */
#define GA_LIB_TBS_3G_TECHNOLOGY 0x01               /**< bearer supports 3G technology .*/
#define GA_LIB_TBS_4G_TECHNOLOGY 0x02               /**< bearer supports 4G technology. */
#define GA_LIB_TBS_LTE_TECHNOLOGY 0x03              /**< bearer supports LTE technology. */
#define GA_LIB_TBS_WIFI_TECHNOLOGY 0x04             /**< bearer supports WIFI technology. */
#define GA_LIB_TBS_5G_TECHNOLOGY 0x05               /**< bearer supports 5G technology. */
#define GA_LIB_TBS_GSM_TECHNOLOGY 0x06              /**< bearer supports GSM technology. */
#define GA_LIB_TBS_CDMA_TECHNOLOGY 0x07             /**< bearer supports CDMA technology. */
#define GA_LIB_TBS_2G_TECHNOLOGY 0x08               /**< bearer supports 2G technology. */
#define GA_LIB_TBS_WCDMA_TECHNOLOGY 0x09            /**< bearer supports WCDMA technology. */
#define GA_LIB_TBS_IP_TECHNOLOGY 0x0A               /**< bearer supports IP technology. */
    typedef uint8_t ga_lib_tbs_bearer_technology_t; /**< TBS bearer technology */

    /** All possible call state */
#define GA_LIB_TBS_CALL_STATE_INCOMING 0x00 /**< A remote party is calling (incoming call). */
#define GA_LIB_TBS_CALL_STATE_DIALING                                                                                  \
    0x01                                    /**< The process to call the remote party has started but the remote party
                                             *  is not being alerted (outgoing call).
                                             */
#define GA_LIB_TBS_CALL_STATE_ALERTING 0X02 /**< A remote party is being alerted (outgoing call) */
#define GA_LIB_TBS_CALL_STATE_ACTIVE 0x03   /**< The call is in an active conversation */
#define GA_LIB_TBS_CALL_STATE_LOCALLY_HELD                                                                             \
    0x04 /**< The call is connected but held locally with no audio communicated
                                                  * in either direction. Either server/client can control the state
                                                  */
#define GA_LIB_TBS_CALL_STATE_REMOTELY_HELD                                                                            \
    0x05 /**< The call is connected but held remotely with no audio
                                                   * communicated in either direction. Call state can be controlled
                                                   * by the remote party of the call
                                                   */
#define GA_LIB_TBS_CALL_STATE_LOCALLY_AND_REMOTELY_HELD                                                                \
    0x06 /**< The call is connected but held both locally and
                                                               * remotely with no audio communicated in either
                                                               * direction */

    typedef uint8_t ga_lib_tbs_call_state_t; /**< TBS call state */

    /** call action notification event after a call action is performed */
#define GA_LIB_CALL_SUCCESS 0x00                /**< Opcode write was successful.*/
#define GA_LIB_CALL_OPCODE_NOT_SUPPORTED 0x01   /**< An invalid opcode was used for the Call Control Point write.*/
#define GA_LIB_CALL_OPERATION_NOT_POSSIBLE 0x02 /**< The tbs server does not currently support joining of calls.*/
#define GA_LIB_CALL_INVALID_CALL_ID 0x03        /**< The Call Control Point write is invalid.*/
#define GA_LIB_CALL_STATE_MISMATCH 0x04         /**< Opcode written in an unexpected call state */
#define GA_LIB_CALL_LACK_OF_RESOURCES 0x05      /**< resources to complete the requested action */
#define GA_LIB_CALL_INVALID_URI 0x06            /**< outgoing caller id is incorrect */
    typedef uint8_t ga_lib_tbs_call_operation_result_t; /**< TBS call operation result */

    /** Call action values */
#define GA_LIB_CCP_ACTION_ACCEPT_CALL 0x00              /**< Answer the incoming call. */
#define GA_LIB_CCP_ACTION_TERMINATE_CALL 0x01           /**< End the currently active/outgoing/held call. */
#define GA_LIB_CCP_ACTION_HOLD_CALL 0x02 /**< Place the currently active or alerting call on local hold. */
#define GA_LIB_CCP_ACTION_RETRIEVE_CALL                                                                                \
    0x03                                 /**< Move a locally held call to an active call. Move a locally and
                                               * remotely held call to a remotely held call
                                               */
#define GA_LIB_CCP_ACTION_ORIGINATE 0x04 /**< Place a call */
#define GA_LIB_CCP_ACTION_JOIN_CALL                                                                                    \
    0x05 /**< Put a call to the active state and join all calls that are in the
                                           *  active state. */

    typedef uint8_t ga_lib_tbs_call_action_t; /**< TBS call action  */

    typedef struct
    {
        uint8_t curr_multi_read_char_index;
        uint16_t ccp_multi_read_handles[8];
        uint16_t read_offset;
        uint16_t remaining_data_len;
        uint8_t waiting_for_multi_read_rsp;
        wiced_bool_t waiting_for_call_list;
    } ccp_multi_read_info_t;

#define CCP_MULTI_READ_BEARER_TECHNOLOGY_CHARACTERISTIC 0
#define CCP_MULTI_READ_BEARER_SIGNAL_STRENGTH_CHARACTERISTIC 1
#define CCP_MULTI_READ_BEARER_SIG_STR_REPORTING_INTERVAL_CHARACTERISTIC 2
#define CCP_MULTI_READ_BEARER_SUPPORTED_FEATURES_CHARACTERISTIC 3
#define CCP_MULTI_READ_BEARER_UCI_CHARACTERISTIC 4
#define CCP_MULTI_READ_BEARER_URI_CHARACTERISTIC 5
#define CCP_MULTI_READ_BEARER_PROVIDER_NAME_CHARACTERISTIC 6
#define CCP_MULTI_READ_MEDIA_MAX_CHARACTERISTIC 7

    /** call state data */
    typedef struct
    {
        uint8_t call_id;                    /**< call id of the call */
        ga_lib_tbs_call_state_t call_state; /**< call state of the call */
        uint8_t call_flags;                 /**< call flag bit field of the call */
    } ga_lib_tbs_call_state_data_t;

    /** current call data */
    typedef struct
    {
        uint8_t call_id;                    /**< call id of the call */
        ga_lib_tbs_call_state_t call_state; /**< call state of the call */
        uint8_t call_flags;                 /**< call flags */
        char *p_remote_caller_id;           /**< caller id of the remote user */
        uint8_t remote_caller_id_len;       /**< length of the remote caller id */
    } ga_lib_tbs_current_call_t;

    /** list of bearer properties that is read from the app */
    typedef enum
    {
        GA_LIB_TBS_BEARER_PROVIDER_NAME,                      /**< telephone bearer provider name value */
        GA_LIB_TBS_BEARER_UCI,                                /**< telephone bearer supported uci */
        GA_LIB_TBS_BEARER_TECHNOLOGY,                         /**< telephone bearer supported technology */
        GA_LIB_TBS_BEARER_URI_PREFIX_SUPPORTED_LIST,          /**< telephone bearer supported uri*/
        GA_LIB_TBS_BEARER_SIGNAL_STRENGTH,                    /**< telephone bearer signal strength*/
        GA_LIB_TBS_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL, /**< telephone bearer signal strength reporting interval*/
    } ga_lib_tbs_bearer_property_t;

    /**
 * @addtogroup Call_Control_Profile_APIs
 * @{
 * @brief Telephone Bearer Service (TBS) and Generic Telephone Bearer Service (GTBS)
 *        enable remote control of calls and exposure of bearer information for telephony
 *        applications. TBS can be used for individual telephone bearers (e.g., cellular,
 *        VoIP), while GTBS provides a generic interface aggregating all bearers.
 */

    /**
 * @brief Discover Generic Telephone Bearer Service (GTBS) on remote GATT server
 *
 * This function initiates GATT service discovery to find the GTBS singleton service.
 * GTBS provides a generic telephony interface that aggregates information from all
 * telephone bearers on the device.
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] pfn_on_complete   Callback function invoked when discovery completes
 *
 * @return Pointer to the service discovery context, or NULL on failure
 */
    gatt_intf_service_discovery_ctx_t *ga_lib_gtbs_discover_service(uint16_t conn_id,
                                                                    pfn_on_discovery_complete_t pfn_on_complete);

    /**
 * @brief Discover Telephone Bearer Service (TBS) instance on remote GATT server
 *
 * This function initiates GATT service discovery to find a TBS instance starting at
 * a specific attribute handle. Multiple TBS instances may exist on a device, one per
 * telephone bearer.
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] start_handle      Starting attribute handle for the discovery range
 * @param[in] pfn_on_complete   Callback function invoked when discovery completes
 *
 * @return Pointer to the service discovery context, or NULL on failure
 */
    gatt_intf_service_discovery_ctx_t *ga_lib_tbs_discover_service(uint16_t conn_id,
                                                                   uint16_t start_handle,
                                                                   pfn_on_discovery_complete_t pfn_on_complete);

    /**
 * @brief Write Originate Call action to Call Control Point
 *
 * Initiates an outgoing call to the specified URI by writing the Originate Call opcode
 * along with the URI string to the Call Control Point characteristic.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] p_handle  Pointer to characteristic handles for the Call Control Point
 * @param[in] p_uri     Null-terminated URI string (e.g., "tel:1234567890")
 *
 * @return WICED_BT_GATT_SUCCESS on success,
 *         WICED_BT_GATT_INVALID_ATTR_LEN if URI length exceeds 255 bytes,
 *         otherwise a GATT error code
 *
 * @note Uses GATT write without response
 */
    wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_originate_call(uint16_t conn_id,
                                                                     gatt_intf_characteristic_handles_t *p_handle,
                                                                     const char *p_uri);

    /**
 * @brief Write Join Call action to Call Control Point
 *
 * Joins multiple active calls into a conference by writing the Join Call opcode and
 * the list of call IDs to the Call Control Point characteristic.
 *
 * @param[in] conn_id       Connection identifier for the GATT connection
 * @param[in] p_handle      Pointer to characteristic handles for the Call Control Point
 * @param[in] num_call_ids  Number of call IDs in the array
 * @param[in] p_call_ids    Pointer to array of call IDs to join
 *
 * @return WICED_BT_GATT_SUCCESS on success,
 *         WICED_BT_GATT_INVALID_ATTR_LEN if num_call_ids exceeds 255,
 *         otherwise a GATT error code
 *
 * @note Uses GATT write without response
 */
    wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_join_call(uint16_t conn_id,
                                                                gatt_intf_characteristic_handles_t *p_handle,
                                                                uint8_t num_call_ids,
                                                                const uint8_t *p_call_ids);

    /**
 * @brief Write Accept Call action to Call Control Point
 *
 * Accepts an incoming call by writing the Accept Call opcode with the call ID.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] p_handle  Pointer to characteristic handles for the Call Control Point
 * @param[in] call_id   Identifier of the incoming call to accept
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_accept_call(uint16_t conn_id,
                                                                  gatt_intf_characteristic_handles_t *p_handle,
                                                                  uint8_t call_id);

    /**
 * @brief Write Terminate Call action to Call Control Point
 *
 * Ends an active, outgoing, or held call by writing the Terminate Call opcode with the call ID.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] p_handle  Pointer to characteristic handles for the Call Control Point
 * @param[in] call_id   Identifier of the call to terminate
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_terminate_call(uint16_t conn_id,
                                                                     gatt_intf_characteristic_handles_t *p_handle,
                                                                     uint8_t call_id);

    /**
 * @brief Write Hold Call action to Call Control Point
 *
 * Places the active or alerting call on local hold by writing the Hold Call opcode
 * with the call ID.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] p_handle  Pointer to characteristic handles for the Call Control Point
 * @param[in] call_id   Identifier of the call to hold
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_hold_call(uint16_t conn_id,
                                                                gatt_intf_characteristic_handles_t *p_handle,
                                                                uint8_t call_id);

    /**
 * @brief Write Retrieve Call action to Call Control Point
 *
 * Moves a locally held call to the active state by writing the Retrieve Call opcode
 * with the call ID. For a call that is both locally and remotely held, transitions
 * to remotely held only.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] p_handle  Pointer to characteristic handles for the Call Control Point
 * @param[in] call_id   Identifier of the held call to retrieve
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_write_cp_action_retrieve_call(uint16_t conn_id,
                                                                    gatt_intf_characteristic_handles_t *p_handle,
                                                                    uint8_t call_id);

    /**
 * @brief Write Signal Strength Reporting Interval (SSRI) to remote bearer
 *
 * Configures the reporting interval for signal strength notifications on the remote
 * telephone bearer.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] p_handle  Pointer to characteristic handles for the SSRI characteristic
 * @param[in] ssri      Reporting interval in seconds (0 = report on change only)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 *
 * @note Uses GATT write command (no response)
 */
    wiced_bt_gatt_status_t ga_lib_tbs_write_bearer_ssri(uint16_t conn_id,
                                                        gatt_intf_characteristic_handles_t *p_handle,
                                                        uint8_t ssri);

    /**
 * @brief Send notification for Bearer Provider Name characteristic
 *
 * @param[in] conn_id       Connection identifier for the GATT connection
 * @param[in] handle        Attribute handle of the Bearer Provider Name characteristic
 * @param[in] provider_name Null-terminated UTF-8 string of the provider name
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 *
 * @note Notification length is strlen(provider_name), excluding null terminator
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_provider_name(uint16_t conn_id, uint16_t handle, const char *provider_name);

    /**
 * @brief Send notification for Bearer URI Schemes Supported List characteristic
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the URI Schemes Supported List characteristic
 * @param[in] schemes   Null-terminated comma-separated list of supported URI schemes
 *                      (e.g., "tel,sip")
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_uri_supported_schemes(uint16_t conn_id, uint16_t handle, const char *schemes);

    /**
 * @brief Send notification for Bearer List Current Calls characteristic
 *
 * Notifies the client of the list of currently active calls. Each call entry contains
 * a length byte, call ID, call state, call flags, and the remote caller URI.
 *
 * Per-call format:
 * - 1 byte: list item length (3 + URI length)
 * - 1 byte: call ID
 * - 1 byte: call state
 * - 1 byte: call flags
 * - N bytes: remote caller URI
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Bearer List Current Calls characteristic
 * @param[in] p_call    Pointer to array of current call structures
 * @param[in] num_calls Number of calls in the array
 *
 * @return WICED_BT_GATT_SUCCESS on success,
 *         WICED_BT_GATT_NO_RESOURCES if buffer allocation failed,
 *         otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_current_calls(uint16_t conn_id,
                                                           uint16_t handle,
                                                           const ga_lib_tbs_current_call_t *p_call,
                                                           int num_calls);

    /**
 * @brief Send notification for Call State characteristic
 *
 * Notifies the client of the current state of each call. Each entry contains the call
 * ID, state, and flags (without URI).
 *
 * Per-call format:
 * - 1 byte: call ID
 * - 1 byte: call state (see #ga_lib_tbs_call_state_t)
 * - 1 byte: call flags
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Call State characteristic
 * @param[in] p_call    Pointer to array of current call structures
 * @param[in] num_calls Number of calls in the array
 *
 * @return WICED_BT_GATT_SUCCESS on success,
 *         WICED_BT_GATT_NO_RESOURCES if buffer allocation failed,
 *         otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_call_state(uint16_t conn_id,
                                                        uint16_t handle,
                                                        ga_lib_tbs_current_call_t *p_call,
                                                        int num_calls);

    /**
 * @brief Send notification for Bearer Technology characteristic
 *
 * @param[in] conn_id       Connection identifier for the GATT connection
 * @param[in] handle        Attribute handle of the Bearer Technology characteristic
 * @param[in] technology    Bearer technology value (see #ga_lib_tbs_bearer_technology_t)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_technology(uint16_t conn_id, uint16_t handle, uint8_t technology);

    /**
 * @brief Send notification for Bearer Signal Strength characteristic
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] handle            Attribute handle of the Signal Strength characteristic
 * @param[in] signal_strength   Signal strength value (0 = no signal, 100 = maximum,
 *                              255 = unavailable)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_signal_strength(uint16_t conn_id,
                                                             uint16_t handle,
                                                             uint8_t signal_strength);

    /**
 * @brief Send notification for Signal Strength Reporting Interval characteristic
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the SSRI characteristic
 * @param[in] interval  Reporting interval in seconds (0 = report on change only)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_ssri(uint16_t conn_id, uint16_t handle, uint8_t interval);

    /**
 * @brief Send notification for Content Control ID characteristic
 *
 * @param[in] conn_id               Connection identifier for the GATT connection
 * @param[in] handle                Attribute handle of the Content Control ID characteristic
 * @param[in] content_control_id    Content Control ID (CCID) used to associate this bearer
 *                                  with audio streams
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_content_control_id(uint16_t conn_id,
                                                                uint16_t handle,
                                                                uint8_t content_control_id);

    /**
 * @brief Send notification for Incoming Call Target Bearer URI characteristic
 *
 * Notifies the client of the bearer URI that received an incoming call (the local
 * URI being called).
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Incoming Call Target Bearer URI characteristic
 * @param[in] call_id   Identifier of the incoming call
 * @param[in] p_uri     Null-terminated URI string of the target bearer
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_incoming_bearer(uint16_t conn_id,
                                                             uint16_t handle,
                                                             uint8_t call_id,
                                                             const char *p_uri);

    /**
 * @brief Send notification for Status Flags characteristic
 *
 * Notifies the client of bearer status flags (e.g., inband ringtone enabled,
 * silent mode active).
 *
 * @param[in] conn_id       Connection identifier for the GATT connection
 * @param[in] handle        Attribute handle of the Status Flags characteristic
 * @param[in] status_flag   16-bit status flag bitmask (see TBS_FEATURE_BIT defines)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_status_flag(uint16_t conn_id, uint16_t handle, uint16_t status_flag);

    /**
 * @brief Send notification for Call Termination Reason characteristic
 *
 * Notifies the client that a call has been terminated, identifying the call and reason.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Call Termination Reason characteristic
 * @param[in] call_id   Identifier of the terminated call
 * @param[in] reason    Termination reason (see #ga_lib_tbs_call_termination_reason_e)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_call_termination_reason(uint16_t conn_id,
                                                                     uint16_t handle,
                                                                     uint8_t call_id,
                                                                     uint8_t reason);

    /**
 * @brief Send notification for Call Control Point response
 *
 * Notifies the client of the result of a previously written Call Control Point opcode
 * for a specific call.
 *
 * Notification format: [opcode][call_id][response]
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Call Control Point characteristic
 * @param[in] opcode    Opcode being acknowledged (see #ga_lib_tbs_call_action_e)
 * @param[in] call_id   Call ID associated with the opcode
 * @param[in] response  Result code (see #ga_lib_tbs_call_operation_result_e)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_control_point_response(
        uint16_t conn_id, uint16_t handle, uint8_t opcode, uint8_t call_id, uint8_t response);

    /**
 * @brief Send notification for Incoming Call characteristic
 *
 * Notifies the client of an incoming call, providing the call ID and remote caller URI.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Incoming Call characteristic
 * @param[in] call_id   Identifier of the incoming call
 * @param[in] p_uri     Null-terminated URI string of the remote caller
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_incoming_call(uint16_t conn_id,
                                                           uint16_t handle,
                                                           uint8_t call_id,
                                                           const char *p_uri);

    /**
 * @brief Send notification for Call Friendly Name characteristic
 *
 * Notifies the client of a human-readable friendly name associated with a call
 * (e.g., contact name resolved from the caller URI).
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] handle            Attribute handle of the Call Friendly Name characteristic
 * @param[in] call_id           Identifier of the call
 * @param[in] p_friendly_name   Null-terminated UTF-8 friendly name string
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
    wiced_bt_gatt_status_t ga_lib_tbs_notify_call_friendly_name(uint16_t conn_id,
                                                                uint16_t handle,
                                                                uint8_t call_id,
                                                                const char *p_friendly_name);

    /**@} Call_Control_Profile_APIs */

#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_TBS_H */
