/*
 * $ Copyright Cypress Semiconductor $
 */

#if (COMPONENT_MW_AIROC_HCI_TRANSPORT == 1)
#include "app_rpc.h"
#include "wiced_bt_trace.h"
#include "cybt_debug_uart.h"

#define TRANS_UART_BUFFER_SIZE 1024
#define TRANSPORT_BUFFER_SIZE 1500
#define TRANSPORT_BUFFER_COUNT 2
#define TRANSPORT_UART_BAUD_RATE 3000000

static uint32_t app_rpc_rx_callback(uint8_t *p_buffer, uint32_t length);

app_rpc_cback_t g_app_rpc_cback;
static void rpc_hci_trace_cback(wiced_bt_hci_trace_type_t type, uint16_t length, uint8_t *p_data)
{
    cybt_debug_uart_send_hci_trace(type, (length > TRANS_UART_BUFFER_SIZE) ? TRANS_UART_BUFFER_SIZE : length, p_data);
}

void app_rpc_init(int host_instance, app_rpc_cback_t app_rpc_cback, wiced_bool_t b_route_traces_to_CC)
{

    g_app_rpc_cback = app_rpc_cback;

    /* Initialize cybt debug uart to communicate with Client Control */
   // cybt_debug_uart_init(&debug_uart_cfg, app_rpc_rx_callback);

    /* Send HCI traces to CC to get them in BT Spy */
    if (b_route_traces_to_CC) wiced_bt_dev_register_hci_trace(rpc_hci_trace_cback);
}

void app_rpc_send_data(int type, uint8_t *p_data, uint16_t data_size)
{
    cybt_debug_uart_send_data(type, data_size, p_data);
}

#endif /* COMPONENT_MW_AIROC_HCI_TRANSPORT */
