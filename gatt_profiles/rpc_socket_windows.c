/*
 * $ Copyright Cypress Semiconductor $
 */
#ifdef WIN32
#include "windows.h"
#include "winsock.h"
#include <string.h>

#include "app_rpc.h"

/* BT Stack includes */
#include "wiced_bt_trace.h"
#include "wiced_bt_types.h"

static SOCKET m_ListenSocket = INVALID_SOCKET;
static SOCKET m_ClientSocket = INVALID_SOCKET;
static DWORD __stdcall priv_app_rpc_receive_thread(VOID *p);

static int g_host_instance;

typedef void (*app_rpc_handle_socket_data_t)(uint16_t opcode, uint8_t *p_data, uint32_t data_len);
static app_rpc_handle_socket_data_t g_app_rpc_handle_socket_data;

typedef wiced_bool_t (*app_rpc_cback_t)(uint16_t opcode, uint8_t *p_data, uint32_t data_len);

app_rpc_cback_t g_app_rpc_cback;


#define MAX_DATA 1500
#define SOCK_PORT_NUM_START 12012

extern void priv_app_rpc_handle_wiced_hci_data(uint16_t opcode, uint8_t *p_data, uint32_t data_len);

void app_rpc_init(int host_instance, app_rpc_cback_t app_rpc_cback, wiced_bool_t b_route_traces_to_CC)
{
    DWORD thread_address;

    g_host_instance = host_instance;
    g_app_rpc_cback = app_rpc_cback;
    g_app_rpc_handle_socket_data = priv_app_rpc_handle_wiced_hci_data;

    CreateThread(0, 0, priv_app_rpc_receive_thread, 0, 0, &thread_address);
}

static int priv_app_rpc_read_TCP_pkt(unsigned char *pPkt, int32_t max_data)
{
    int readLen, hdrLen, dataLen;

    if ((readLen = recv(m_ClientSocket, (char *)pPkt, 1, 0)) != 1)
    {
        WICED_BT_TRACE("readHostTCPpkt() Expected 1, got: %d", readLen);
        return (-1);
    }

    // ACL and WICED-HCI share the same basic format
    if (pPkt[0] == HCI_WICED_PKT)
    {
        if ((hdrLen = recv(m_ClientSocket, (char *)&pPkt[1], 4, 0)) != 4)
        {
            WICED_BT_TRACE("readHostTCPpkt() Expected 4, got: %d", readLen);
            return (-1);
        }
        dataLen = pPkt[3] | (pPkt[4] << 8);
    }
    else
    {
        WICED_BT_TRACE("!!!!Unknown Type: %u", pPkt[0]);
        return (-1);
    }

    if (dataLen != 0)
    {
        if (dataLen > max_data)
        {
            WICED_BT_TRACE("[%s] max readable data of %u, actually got: %d", __FUNCTION__, max_data, dataLen);
            return (-1);
        }
        readLen = recv(m_ClientSocket, (char *)&pPkt[1 + hdrLen], dataLen, 0);
        if (readLen != dataLen)
        {
            WICED_BT_TRACE("[%s] Expected to read datalen of %u, actually got: %d",
                                __FUNCTION__,
                                dataLen,
                                readLen);
            return (-1);
        }
        else if (readLen > max_data)
        {
            WICED_BT_TRACE("[%s] max readable data of %u, actually got: %d", __FUNCTION__, max_data, readLen);
            return (-1);
        }
    }

    return (1 + hdrLen + dataLen);
}

wiced_bool_t priv_app_rpc_socket_init()
{
    SOCKADDR_IN service;
    int result;

    // Create a local SOCKET for incoming connection
    if (INVALID_SOCKET == (m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
    {
        WICED_BT_TRACE("listen socket failed with error: %ld, socket thread exiting\n", WSAGetLastError());
        return 0;
    }

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(g_host_instance + SOCK_PORT_NUM_START);
    memset(service.sin_zero, 0, sizeof(service.sin_zero));

    result = bind(m_ListenSocket, (SOCKADDR *)&service, sizeof(service));
    if (SOCKET_ERROR == result)
    {
        WICED_BT_TRACE("bind to port: %d  failed with error: %d, socket thread exiting\n",
                            g_host_instance + SOCK_PORT_NUM_START,
                            WSAGetLastError());

        closesocket(m_ListenSocket);
        m_ListenSocket = INVALID_SOCKET;
        return FALSE;
    }

    return TRUE;
}

static DWORD __stdcall priv_app_rpc_receive_thread(VOID *p)
{
    uint8_t pkt[MAX_DATA];
    int bytes_rcvd;
    uint16_t opcode;
    uint16_t paylen;

    if (!priv_app_rpc_socket_init()) return 0;

    for (;;)
    {
        WICED_BT_TRACE("Listening for client to connect TCP socket.... %d", g_host_instance + SOCK_PORT_NUM_START);

        //if (m_ClientSocket != INVALID_SOCKET)
        //{
        //    closesocket(m_ClientSocket);
        //    m_ClientSocket = INVALID_SOCKET;
        //    break;
        //}

        if (SOCKET_ERROR == listen(m_ListenSocket, 1))
        {
            WICED_BT_TRACE("TCP socket listen failed with error: %d\n", WSAGetLastError());
            break;
        }

        // Accept the client TCP socket
        if (INVALID_SOCKET == (m_ClientSocket = accept(m_ListenSocket, NULL, NULL)))
        {
            WICED_BT_TRACE("Client TCP socket accept failed with error: %d", WSAGetLastError());
            break;
        }

        WICED_BT_TRACE("Client TCP socket accepted OK");

        // Receive until the peer shuts down the connection
        for (;;)
        {
            bytes_rcvd = priv_app_rpc_read_TCP_pkt((char *)&pkt, MAX_DATA);

            if (bytes_rcvd <= 4)
            {
                WICED_BT_TRACE("Client TCP socket recv failed    Closing...");
                break;
            }
            else
            {
                opcode = pkt[1] | (pkt[2] << 8);
                paylen = pkt[3] | (pkt[4] << 8);

                g_app_rpc_handle_socket_data(opcode, &pkt[5], paylen);
            }

        }

        // sockets will get closed when program exits
        WICED_BT_TRACE("Socket thread exiting");
    }

    // sockets will get closed when program exits
    WICED_BT_TRACE("[%s] exiting..\n", __FUNCTION__);

    return 0;
}

void app_rpc_send_data(int type, uint8_t *p_data, uint16_t data_size)
{
    uint8_t buf[MAX_DATA];

    buf[0] = HCI_WICED_PKT;
    buf[1] = (uint8_t)type;
    buf[2] = (uint8_t)(type >> 8);
    buf[3] = (uint8_t)data_size;
    buf[4] = (uint8_t)(data_size >> 8);

    if(p_data && data_size){
        memcpy(&buf[5], p_data, data_size);
    }

    if (m_ClientSocket == WICED_INVALID_SOCKET)
    {
        WICED_BT_TRACE("!!!!  hci_control_send_script_event() - no TCP socket - dropping data !!!");
        return;
    }

    if (-1 == (send(m_ClientSocket, buf, data_size + 5, 0)))
    {
        WICED_BT_TRACE("send failed \n");
        return;
    }

    return;
}


#endif
