/*
 * $ Copyright Cypress Semiconductor $
 */


#ifndef COMPONENT_MW_AIROC_HCI_TRANSPORT
#include "app_rpc.h"

#include "wiced_bt_trace.h"
#include "wiced_transport.h"

#define TRANS_UART_BUFFER_SIZE 1024
#define TRANSPORT_BUFFER_SIZE 1500
#define TRANSPORT_BUFFER_COUNT 2
#define TRANSPORT_UART_BAUD_RATE 3000000

static void app_rpc_transport_status_handler(wiced_transport_type_t type);
static uint32_t app_rpc_rx_callback(uint8_t *p_buffer, uint32_t length);

const wiced_transport_cfg_t transport_cfg = {
    WICED_TRANSPORT_UART,
    {
        {WICED_TRANSPORT_UART_HCI_MODE, TRANSPORT_UART_BAUD_RATE},
    },

    .heap_config = {.data_heap_size =
                        ((TRANS_UART_BUFFER_SIZE * 4) + (TRANSPORT_BUFFER_SIZE * TRANSPORT_BUFFER_COUNT)), // Tx, Rx
                    .hci_trace_heap_size = TRANS_UART_BUFFER_SIZE * 2,
                    .debug_trace_heap_size = 1024},
    app_rpc_transport_status_handler,
    app_rpc_rx_callback,
    NULL};

app_rpc_cback_t g_app_rpc_cback;

static void app_rpc_transport_status_handler(wiced_transport_type_t type)
{
    wiced_transport_send_data(HCI_CONTROL_EVENT_DEVICE_STARTED, NULL, 0);
}

static uint32_t app_rpc_rx_callback(uint8_t *p_buffer, uint32_t length)
{
    uint16_t opcode;
    int payload_len = 0;
    uint8_t *p_data = p_buffer;

    if (!p_buffer)
    {
        return HCI_CONTROL_STATUS_INVALID_ARGS;
    }

    // Expected minimum 4 byte as the wiced header
    if ((length < 4) || (p_data == NULL))
    {
        WICED_BT_TRACE("invalid params\n");
        wiced_transport_free_buffer(p_buffer);
        return HCI_CONTROL_STATUS_INVALID_ARGS;
    }

    STREAM_TO_UINT16(opcode, p_data);      // Get OpCode
    STREAM_TO_UINT16(payload_len, p_data); // Gen Payload Length

    if (g_app_rpc_cback)
    {
        g_app_rpc_cback(opcode, p_data, payload_len);
    }

    return HCI_CONTROL_STATUS_SUCCESS;
}

static void rpc_hci_trace_cback(wiced_bt_hci_trace_type_t type, uint16_t length, uint8_t *p_data)
{
    wiced_transport_send_hci_trace(type, p_data, (length > TRANS_UART_BUFFER_SIZE) ? TRANS_UART_BUFFER_SIZE : length);
}

void app_rpc_init(int host_instance, app_rpc_cback_t app_rpc_cback, wiced_bool_t b_route_traces_to_CC)
{

    g_app_rpc_cback = app_rpc_cback;

    /* Initialize WICED HCI transport to communicate with Client Control */
    wiced_transport_init(&transport_cfg);
    wiced_set_debug_uart(WICED_ROUTE_DEBUG_TO_WICED_UART);

    /* Send HCI traces to CC to get them in BT Spy */
    if (b_route_traces_to_CC) wiced_bt_dev_register_hci_trace(rpc_hci_trace_cback);
}

void app_rpc_send_data(int type, uint8_t *p_data, uint16_t data_size)
{
    wiced_transport_send_data(type, p_data, data_size);
}

#endif /* COMPONENT_MW_AIROC_HCI_TRANSPORT */
