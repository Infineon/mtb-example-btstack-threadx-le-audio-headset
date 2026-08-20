/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
  *
  * Media Control Service common
  */

#ifndef GA_LIB_MCS_H
#define GA_LIB_MCS_H

#include "gatt_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYER_NAME 0             /**< Media Player Name characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_CHANGED 1           /**< Media Track Changed characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_TITLE 2             /**< Media Track Title characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_DURATION 3          /**< Media Track Duration characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_TRACK_POSITION 4          /**< Media Track Position characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYBACK_SPEED 5          /**< Media Playback Speed characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_SEEKING_SPEED 6           /**< Media Seeking Speed characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYING_ORDER 7           /**< Media Playing Order characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_PLAYING_ORDER_SUPPORTED 8 /**< Media Playing Order Supported characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_STATE 9                   /**< Media State characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_CONTROL_POINT 10          /**< Media Control Point characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MEDIA_OPCODE_SUPPORTED 11       /**< Media Opcode Supported characteristic */
#define GA_LIB_MCS_CHARACTERISTIC_MAX 12 /**< Number of MCS characteristics without content id enabled */
    typedef uint8_t ga_lib_mcs_characteristics_t; /**< Type for MCS characteristics */

/**
 * @brief MCS error code values
 */
#define GA_LIB_MCS_ERROR_VALUE_CHANGED_READ_LONG 0x80 /**< Value Changed during read long */

//#define DEFAULT_AUTH GATT_AUTH_REQ_NO_MITM
#define DEFAULT_AUTH GATT_AUTH_REQ_NONE

#define MEDIA_TRACK_POSITION_LENGTH sizeof(int32_t)
#define MEDIA_TRACK_DURATION_LENGTH sizeof(int32_t)
#define MEDIA_PLAYBACK_SPEED_LENGTH sizeof(int8_t)
#define MEDIA_SEEKING_SPEED_LENGTH sizeof(int8_t)
#define MEDIA_PLAYING_ORDER_LENGTH 1
#define MEDIA_PLAYING_ORDER_SUPPORTED_LENGTH sizeof(uint16_t)
#define MEDIA_MEDIA_STATE_LENGTH 1
#define MEDIA_CONTROL_OPCODE_SUPPORTED_LENGTH sizeof(uint32_t)
#define MEDIA_CONTENT_CONTROL_ID_LENGTH sizeof(uint8_t)

    /**
  * @addtogroup Media_Control_APIs
  * @{
  * @brief MCS/GMCS exposes characteristics that describe a single media player. A media player in this context is a device, or part of a device, that allows another device to control the media that is played. For example, a media player could be a television, a set-top box, a radio, a phone running a radio or music application, a
  * phone running a podcast application, or a similar type of device or application. MCS does not directly manage the audio transport
  * GMCS provides status and control of media playback for the device as a single unit.
  */

    /**
* @brief Media Playing order (Media Control Service Section 3.15 Playing Order)
*/

#define GA_LIB_MCS_SINGLE_ONCE 0x01     /**< A single track is played once; there is no next track */
#define GA_LIB_MCS_SINGLE_REPEAT 0x02   /**< A single track is played repeatedly; the next track is the current track */
#define GA_LIB_MCS_IN_ORDER_ONCE 0x03   /**< The tracks within a group are played once in track order */
#define GA_LIB_MCS_IN_ORDER_REPEAT 0x04 /**< The tracks within a group are played in track order repeatedly */
#define GA_LIB_MCS_OLDEST_ONCE 0x05     /**< The tracks within a group are played once only from the oldest first */
#define GA_LIB_MCS_OLDEST_REPEAT 0x06   /**< The tracks within a group are played from the oldest first repeatedly. */
#define GA_LIB_MCS_NEWEST_ONCE 0x07     /**< The tracks within a group are played once only from the newest first */
#define GA_LIB_MCS_NEWEST_REPEAT 0x08   /**< The tracks within a group are played from the newest first repeatedly */
#define GA_LIB_MCS_SHUFFLE_ONCE 0x09    /**< The tracks within a group are played in random order once */
#define GA_LIB_MCS_SHUFFLE_REPEAT 0x0A  /**< The tracks within a group are played in random order repeatedly */
    typedef uint8_t ga_lib_mcs_playing_order_t; /**< MCS Playing order values (see #ga_lib_mcs_playing_order_e) */

/*
* @brief Media Playing order Supported (Media Control Service Section 3.16 Playing Order Supported)
*/
#define MCS_SINGLE_ONCE_PLAYING_ORDER_MASK (1 << 0) /**< A single track is played once; there is no next track */
#define MCS_SINGLE_REPEAT_PLAYING_ORDER_MASK                                                                           \
    (1 << 1) /**< A single track is played repeatedly; the next track is the current track */
#define MCS_IN_ORDER_ONCE_PLAYING_ORDER_MASK (1 << 2) /**< The tracks within a group are played once in track order */
#define MCS_IN_ORDER_REPEAT_PLAYING_ORDER_MASK                                                                         \
    (1 << 3) /**< The tracks within a group are played in track order repeatedly */
#define MCS_OLDEST_ONCE_PLAYING_ORDER_MASK                                                                             \
    (1 << 4) /**< The tracks within a group are played once only from the oldest first */
#define MCS_OLDEST_REPEAT_PLAYING_ORDER_MASK                                                                           \
    (1 << 5) /**< The tracks within a group are played from the oldest first repeatedly */
#define MCS_NEWEST_ONCE_PLAYING_ORDER_MASK                                                                             \
    (1 << 6) /**< The tracks within a group are played once only from the newest first */
#define MCS_NEWEST_REPEAT_PLAYING_ORDER_MASK                                                                           \
    (1 << 7) /**< The tracks within a group are played from the newest first repeatedly */
#define MCS_SHUFFLE_ONCE_PLAYING_ORDER_MASK (1 << 8) /**< The tracks within a group are played in random order once */
#define MCS_SHUFFLE_REPEAT_PLAYING_ORDER_MASK                                                                          \
    (1 << 9) /**< The tracks within a group are played in random order repeatedly */

    /**
* @brief Media State (Media Control Service Section 3.17 Media State)
*/
#define GA_LIB_MCS_MEDIA_INACTIVE 0x00  /**< The current track is inactive */
#define GA_LIB_MCS_MEDIA_PLAYING 0x01   /**< The current track is playing */
#define GA_LIB_MCS_MEDIA_PAUSED 0x02    /**< The current track is paused */
#define GA_LIB_MCS_MEDIA_SEEKING 0x03   /**< The current track is fast forwarding or fast rewinding */
    typedef uint8_t ga_lib_mcs_state_t; /**< Media state*/

    /**
* @brief Media Control Opcode (Media Control Service Section 3.18 Media Control Point)
*/
#define GA_LIB_MCS_INVALID 0x00          /**< Invalid Operation ID */
#define GA_LIB_MCS_PLAY 0x01             /**< Start playing the current track */
#define GA_LIB_MCS_PAUSE 0x02            /**< Pause playing the current track */
#define GA_LIB_MCS_FAST_REWIND 0x03      /**< Fast rewind the current track */
#define GA_LIB_MCS_FAST_FORWARD 0x04     /**< Fast forward the current track */
#define GA_LIB_MCS_STOP 0x05             /**< Stop current activity */
#define GA_LIB_MCS_MOVE_RELATIVE 0x10    /**< Set the current position relative to the current position */
#define GA_LIB_MCS_PREVIOUS_SEGMENT 0x20 /**< Set the current position to the previous segment of the current track */
#define GA_LIB_MCS_NEXT_SEGMENT 0x21     /**< Set the current position to the next segment of the current track */
#define GA_LIB_MCS_FIRST_SEGMENT 0x22    /**< Set the current position to the first segment of the current track */
#define GA_LIB_MCS_LAST_SEGMENT 0x23     /**< Set the current position to the last segment of the current track */
#define GA_LIB_MCS_GOTO_SEGMENT 0x24     /**< Set the current position to the nth segment of the current track */
#define GA_LIB_MCS_PREVIOUS_TRACK                                                                                      \
    0x30 /**< Set the current track to the previous track in the current group playing order */
#define GA_LIB_MCS_NEXT_TRACK 0x31  /**< Set the current track to the next track in the current group playing order */
#define GA_LIB_MCS_FIRST_TRACK 0x32 /**< Set the current track to the first track in the current group playing order */
#define GA_LIB_MCS_LAST_TRACK 0x33  /**< Set the current track to the last track in the current group playing order */
#define GA_LIB_MCS_GOTO_TRACK 0x34  /**< Set the current track to the nth track in the current group playing order */
#define GA_LIB_MCS_PREVIOUS_GROUP 0x40 /**< Set the current group to the previous group in the sequence of groups */
#define GA_LIB_MCS_NEXT_GROUP 0x41     /**< Set the current group to the next group in the sequence of groups */
#define GA_LIB_MCS_FIRST_GROUP 0x42    /**< Set the current group to the first group in the sequence of groups */
#define GA_LIB_MCS_LAST_GROUP 0x43     /**< Set the current group to the last group in the sequence of groups */
#define GA_LIB_MCS_GOTO_GROUP 0x44     /**< Set the current group to the nth group in the sequence of groups */
typedef uint8_t ga_lib_mcs_media_control_operation_t; /**< MCS Control point operations */

/**
* @brief Media Control Opcode (Media Control Service Section 3.18 Media Control Point, Table 3.9: Media Control Point Notification Result Codes)
*/
#define GA_LIB_MCS_SUCCESS 1                      /**< Action requested by the opcode write was completed successfully */
#define GA_LIB_MCS_OPCODE_NOT_SUPPORTED 2     /**< An invalid opcode was used for the Media Control Point write. */
#define GA_LIB_MCS_MEDIA_PLAYER_INACTIVE 3            /**< The Media Player State characteristic value is Inactive */
#define GA_LIB_MCS_COMMAND_CANNOT_BE_COMPLETED 4 /**< The requested action of any Media Control Point write cannot be completed successfully due to a condition within the player */
typedef uint8_t ga_lib_mcs_result_t; /**< MCP result list */

/**
* @brief Media Control Opcode Supported (Media Control Service Section 3.19 Media Control Point Opcodes Supported)
*/
#define MCS_PLAY_OPCODE_SUPPORTED_MASK (1 << 0)         /**< Start playing the current track */
#define MCS_PAUSE_OPCODE_SUPPORTED_MASK (1 << 1)        /**< Pause playing the current track */
#define MCS_FAST_REWIND_OPCODE_SUPPORTED_MASK (1 << 2)  /**< Fast forward the current track */
#define MCS_FAST_FORWARD_OPCODE_SUPPORTED_MASK (1 << 3) /**< Fast rewind the current track */
#define MCS_STOP_OPCODE_SUPPORTED_MASK (1 << 4)         /**< Stop current activity and return to stopped state */
#define MCS_MOVE_RELATIVE_OPCODE_SUPPORTED_MASK                                                                        \
    (1 << 5) /**< Set the current position relative to the current position */
#define MCS_PREVIOUS_SEGMENT_OPCODE_SUPPORTED_MASK                                                                     \
    (1 << 6) /**< Set the current position to the previous segment of the current track */
#define MCS_NEXT_SEGMENT_OPCODE_SUPPORTED_MASK                                                                         \
    (1 << 7) /**< Set the current position to the next segment of the current track */
#define MCS_FIRST_SEGMENT_OPCODE_SUPPORTED_MASK                                                                        \
    (1 << 8) /**< Set the current position to the first segment of the current track */
#define MCS_LAST_SEGMENT_OPCODE_SUPPORTED_MASK                                                                         \
    (1 << 9) /**< Set the current position to the last segment of the current track */
#define MCS_GOTO_SEGMENT_OPCODE_SUPPORTED_MASK                                                                         \
    (1 << 10) /**< Set the current position to the nth segment of the current track */
#define MCS_PREVIOUS_TRACK_OPCODE_SUPPORTED_MASK                                                                       \
    (1 << 11) /**< Set the current track to the previous track in the current group playing order */
#define MCS_NEXT_TRACK_OPCODE_SUPPORTED_MASK                                                                           \
    (1 << 12) /**< Set the current track to the next track in the current group playing order */
#define MCS_FIRST_TRACK_OPCODE_SUPPORTED_MASK                                                                          \
    (1 << 13) /**< Set the current track to the first track in the current group playing order */
#define MCS_LAST_TRACK_OPCODE_SUPPORTED_MASK                                                                           \
    (1 << 14) /**< Set the current track to the last track in the current group playing order */
#define MCS_GOTO_TRACK_OPCODE_SUPPORTED_MASK                                                                           \
    (1 << 15) /**< Set the current track to the nth track in the current group playing order */
#define MCS_PREVIOUS_GROUP_OPCODE_SUPPORTED_MASK                                                                       \
    (1 << 16) /**< Set the current group to the previous group in the sequence of groups */
#define MCS_NEXT_GROUP_OPCODE_SUPPORTED_MASK                                                                           \
    (1 << 17) /**< Set the current group to the next group in the sequence of groups */
#define MCS_FIRST_GROUP_OPCODE_SUPPORTED_MASK                                                                          \
    (1 << 18) /**< Set the current group to the first group in the sequence of groups */
#define MCS_LAST_GROUP_OPCODE_SUPPORTED_MASK                                                                           \
    (1 << 19) /**< Set the current group to the last group in the sequence of groups */
#define MCS_GOTO_GROUP_OPCODE_SUPPORTED_MASK                                                                           \
    (1 << 20) /**< Set the current group to the nth group in the sequence of groups */

/**
* @brief Media Control Basic Opcodes supported mask
*/
#define MCS_BASIC_OPCODES_SUPPORTED                                                                                    \
    (MCS_PLAY_OPCODE_SUPPORTED_MASK | MCS_PAUSE_OPCODE_SUPPORTED_MASK | MCS_FAST_REWIND_OPCODE_SUPPORTED_MASK |        \
     MCS_FAST_FORWARD_OPCODE_SUPPORTED_MASK | MCS_STOP_OPCODE_SUPPORTED_MASK |                                         \
     MCS_MOVE_RELATIVE_OPCODE_SUPPORTED_MASK)

/**
* @brief Media Control Segment Opcodes supported mask
*/
#define MCS_SEGMENT_OPCODES_SUPPORTED                                                                                  \
    (MCS_PREVIOUS_SEGMENT_OPCODE_SUPPORTED_MASK | MCS_NEXT_SEGMENT_OPCODE_SUPPORTED_MASK |                             \
     MCS_FIRST_SEGMENT_OPCODE_SUPPORTED_MASK | MCS_LAST_SEGMENT_OPCODE_SUPPORTED_MASK |                                \
     MCS_GOTO_SEGMENT_OPCODE_SUPPORTED_MASK)

/**
* @brief Media Control Track Opcodes supported mask
*/
#define MCS_TRACK_OPCODES_SUPPORTED                                                                                    \
    (MCS_PREVIOUS_TRACK_OPCODE_SUPPORTED_MASK | MCS_NEXT_TRACK_OPCODE_SUPPORTED_MASK |                                 \
     MCS_FIRST_TRACK_OPCODE_SUPPORTED_MASK | MCS_LAST_TRACK_OPCODE_SUPPORTED_MASK |                                    \
     MCS_GOTO_TRACK_OPCODE_SUPPORTED_MASK)

/**
* @brief Media Control Group Opcodes supported mask
*/
#define MCS_GROUP_OPCODES_SUPPORTED                                                                                    \
    (MCS_PREVIOUS_GROUP_OPCODE_SUPPORTED_MASK | MCS_NEXT_GROUP_OPCODE_SUPPORTED_MASK |                                 \
     MCS_FIRST_GROUP_OPCODE_SUPPORTED_MASK | MCS_LAST_GROUP_OPCODE_SUPPORTED_MASK |                                    \
     MCS_GOTO_GROUP_OPCODE_SUPPORTED_MASK)

/** @brief Media Control Opcode Data */
typedef union
{
    int32_t move_relative_offset; /**< Move Relative Offset in seconds */
    int32_t track_number;         /**< Track number */
    int32_t segment_number;       /**< segment number */
    int32_t group_number;         /**< group number */
} ga_lib_mcs_operation_data_t;

/** @brief MCS operation */
typedef struct
{
    ga_lib_mcs_media_control_operation_t opcode; /**< Media Opcode */
    ga_lib_mcs_operation_data_t data;            /**< Media Operation data */
} ga_lib_mcs_operation_t;

/**
 * @brief Discover Generic Media Control Service (GMCS) on remote GATT server
 *
 * This function initiates GATT service discovery to find the Generic Media Control
 * Service on a connected remote device. GMCS is the singleton variant of MCS used
 * for the device's primary media player.
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] pfn_on_complete   Callback function invoked when discovery completes
 *
 * @return Pointer to the service discovery context, or NULL on failure
 */
gatt_intf_service_discovery_ctx_t *ga_lib_gmcs_discover_service(uint16_t conn_id,
                                                                pfn_on_discovery_complete_t pfn_on_complete);

/**
 * @brief Discover Media Control Service (MCS) on remote GATT server
 *
 * This function initiates GATT service discovery to find an instance of the Media
 * Control Service starting at a specific attribute handle. Multiple MCS instances
 * may exist on a device, one per media player.
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] start_handle      Starting attribute handle for the discovery range
 * @param[in] pfn_on_complete   Callback function invoked when discovery completes
 *
 * @return Pointer to the service discovery context, or NULL on failure
 */
gatt_intf_service_discovery_ctx_t *ga_lib_mcs_discover_service(uint16_t conn_id,
                                                               uint16_t start_handle,
                                                               pfn_on_discovery_complete_t pfn_on_complete);

/**
 * @brief Write to Media Control Point characteristic
 *
 * This function writes a media control command to the remote server's Media Control
 * Point characteristic to control playback (play, pause, stop, next/previous track,
 * fast forward/rewind, seek, etc.). Some opcodes require an additional 4-byte parameter:
 * - GA_LIB_MCS_MOVE_RELATIVE: 32-bit move offset
 * - GA_LIB_MCS_GOTO_TRACK: 32-bit track number
 * - GA_LIB_MCS_GOTO_SEGMENT: 32-bit segment number
 * - GA_LIB_MCS_GOTO_GROUP: 32-bit group number
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] p_handle  Pointer to characteristic handles structure for the Media Control Point
 * @param[in] op_data   Pointer to operation structure containing opcode and optional parameter
 *
 * @return WICED_BT_GATT_SUCCESS if write command was sent successfully,
 *         WICED_BT_GATT_NO_RESOURCES if buffer allocation failed,
 *         otherwise a GATT error code
 *
 * @note Uses GATT_CMD_WRITE (write command, no response)
 */
wiced_bt_gatt_status_t ga_lib_mcs_write_control_media(uint16_t conn_id,
                                                      gatt_intf_characteristic_handles_t *p_handle,
                                                      ga_lib_mcs_operation_t *op_data);

/**
 * @brief Send notification for Media Player Name characteristic
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Media Player Name characteristic
 * @param[in] name      Null-terminated UTF-8 string containing the player name
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 *
 * @note The notification length is determined by strlen(name) (excludes null terminator)
 */
wiced_bt_gatt_status_t ga_lib_mcs_notify_player_name(uint16_t conn_id, uint16_t handle, const char *name);

/**
 * @brief Send notification for Track Title characteristic
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Track Title characteristic
 * @param[in] title      Null-terminated UTF-8 string containing the track title
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_mcs_notify_track_title(uint16_t conn_id, uint16_t handle, const char *title);

/**
 * @brief Send notification indicating the current track has changed
 *
 * Sends an empty notification (zero-length) on the Track Changed characteristic to
 * indicate that a new track has become the current track.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Track Changed characteristic
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_mcs_notify_track_changed(uint16_t conn_id, uint16_t handle);

/**
 * @brief Send notification for Track Duration characteristic
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Track Duration characteristic
 * @param[in] duration  Total duration of the current track in units of 0.01 seconds
 *                      (signed 32-bit; 0xFFFFFFFF indicates unknown duration)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_mcs_notify_track_duration(uint16_t conn_id, uint16_t handle, int32_t duration);

/**
 * @brief Send notification for Track Position characteristic
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Track Position characteristic
 * @param[in] position  Current position within the track in units of 0.01 seconds
 *                      (signed 32-bit; 0xFFFFFFFF indicates unavailable position)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_mcs_notify_track_position(uint16_t conn_id, uint16_t handle, int32_t position);

/**
 * @brief Send notification for Playback Speed characteristic
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Playback Speed characteristic
 * @param[in] speed     Signed 8-bit playback speed value where actual speed = 2^(speed/64);
 *                      0 = normal speed (1x), negative = slower, positive = faster
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_mcs_notify_playback_speed(uint16_t conn_id, uint16_t handle, int8_t speed);

/**
 * @brief Send notification for Seeking Speed characteristic
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Seeking Speed characteristic
 * @param[in] speed     Signed 8-bit seeking speed multiplier (0 = not seeking,
 *                      positive = fast forward, negative = rewind)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_mcs_notify_seeking_speed(uint16_t conn_id, uint16_t handle, int8_t speed);

/**
 * @brief Send notification for Playing Order characteristic
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Playing Order characteristic
 * @param[in] order     Playing order value (e.g., Single Once, Single Repeat,
 *                      In Order Once, In Order Repeat, Oldest Once, Shuffle, etc.)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_mcs_notify_playing_order(uint16_t conn_id, uint16_t handle, uint8_t order);

/**
 * @brief Send notification for Media State characteristic
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Media State characteristic
 * @param[in] state     Media state value (0 = Inactive, 1 = Playing, 2 = Paused, 3 = Seeking)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_mcs_notify_media_state(uint16_t conn_id, uint16_t handle, uint8_t state);

/**
 * @brief Send notification for Media Control Point result
 *
 * Notifies the client of the result of a previously written Media Control Point
 * command, identifying the opcode and the corresponding result code.
 *
 * @param[in] conn_id   Connection identifier for the GATT connection
 * @param[in] handle    Attribute handle of the Media Control Point characteristic
 * @param[in] opcode    Opcode of the operation being acknowledged
 * @param[in] result    Result code (e.g., Success, Opcode Not Supported, Media Player Inactive,
 *                      Command Cannot Be Completed)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_mcs_notify_cp_result(uint16_t conn_id, uint16_t handle, uint8_t opcode, uint8_t result);

/**
 * @brief Send notification for Media Control Point Opcodes Supported characteristic
 *
 * Notifies the client of the bitmask of media control opcodes supported by the
 * server. Each bit corresponds to a specific opcode (Play, Pause, Stop, Next Track, etc.).
 *
 * @param[in] conn_id           Connection identifier for the GATT connection
 * @param[in] handle            Attribute handle of the Opcodes Supported characteristic
 * @param[in] opcodes_supported 32-bit bitmask of supported opcodes (sent little-endian)
 *
 * @return WICED_BT_GATT_SUCCESS on success, otherwise a GATT error code
 */
wiced_bt_gatt_status_t ga_lib_mcs_notify_media_control_opcodes_supported(uint16_t conn_id,
                                                                         uint16_t handle,
                                                                         uint32_t opcodes_supported);

#ifdef __cplusplus
}
#endif

#endif /* GA_LIB_MCS_H */
