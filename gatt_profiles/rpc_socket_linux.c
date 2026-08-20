/*
 * $ Copyright Cypress Semiconductor $
 */

#ifdef __linux__
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>

#include "app_rpc.h"

/* BT Stack includes */
#include "wiced_bt_trace.h"
#include "wiced_bt_types.h"
#include <arpa/inet.h>

#define INVALID_SOCKET -1
#define INVALID_SOCKET_FD -1
#define TRACE_TYPE_GENERIC 0
#define TRACE_TYPE_HCI_EVT_RECV 4
#define TRACE_TYPE_HCI_CMD_XMIT 3
#define TRACE_TYPE_HCI_ACL_RECV 6
#define TRACE_TYPE_HCI_ACL_XMIT 7
#define TYPE_LEN 1
#define HEADER_LEN 4
#define HCI_HEADER_TOTAL_LEN (TYPE_LEN + HEADER_LEN)
#define MAX_CLIENTS 10
#define MAX_AF_INET_BACKLOG 128

static int mServerSocket = WICED_INVALID_SOCKET;
static int mClientSocket = WICED_INVALID_SOCKET;
pthread_t read_thread;
static void *receive(void *p);

static int g_host_instance;
app_rpc_cback_t g_app_rpc_cback;

typedef void (*app_rpc_handle_socket_data_t)(uint16_t opcode, uint8_t *p_data, uint32_t data_len);
static app_rpc_handle_socket_data_t g_app_rpc_handle_socket_data;

#define MAX_PKT_LEN 1500
#define SOCK_PORT_NUM_START 12012

extern void priv_app_rpc_handle_wiced_hci_data(uint16_t opcode, uint8_t *p_data, uint32_t data_len);
extern void route_hci_data_to_CC_init(route_data_to_client_control_t send_data_to_client_control);

void send_hci_traces_to_client_control(uint8_t type, uint8_t *buffer, uint16_t length, uint8_t spy_instance)
{
    uint8_t buf[1024];
    uint8_t *p = buf;

    if (mClientSocket == INVALID_SOCKET)
    {
        WICED_BT_TRACE("Invalid TCP socket - dropping data");
        return;
    }

    //Update trace Type to array mapping in ClientControl [index-1]
    if (type == TRACE_TYPE_GENERIC)
        type = -1;
    else if (type == TRACE_TYPE_HCI_EVT_RECV)
        type = 0;
    else if (type == TRACE_TYPE_HCI_CMD_XMIT)
        type = 1;
    else if (type == TRACE_TYPE_HCI_ACL_RECV)
        type = 2;
    else if (type == TRACE_TYPE_HCI_ACL_XMIT)
        type = 3;

    //Fill header
    p[0] = HCI_WICED_PKT;
    p[1] = HCI_CONTROL_EVENT_HCI_TRACE;
    p[2] = HCI_CONTROL_GROUP_DEVICE << 8;
    p[3] = length & 0xff;
    p[4] = (uint8_t)(length >> 8);
    p[5] = type;

    //WICED_BT_TRACE("length %d type %d \n ", length, type);
    memcpy(&p[6], buffer, length);

    if (-1 == send(mClientSocket, buf, length + 6, MSG_DONTWAIT))
    {
        // WICED_BT_TRACE("send failed \n");
        return;
    }

    return;
}

void app_rpc_init(int host_instance, app_rpc_cback_t app_rpc_cback, wiced_bool_t b_route_traces_to_CC)
{
    pthread_attr_t thread_attr;
    int status = 0;

    g_host_instance = host_instance;
    g_app_rpc_cback = app_rpc_cback;
    g_app_rpc_handle_socket_data = priv_app_rpc_handle_wiced_hci_data;

    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

    // FIXME: Enable after fixing the issue with TCP socket when sharing it with CC
    if (0) // b_route_traces_to_CC
    {
        route_hci_data_to_CC_init(send_hci_traces_to_client_control);
    }

    if (pthread_create(&(read_thread), &thread_attr, receive, NULL) < 0)
    {
        WICED_BT_TRACE("pthread_create failed");
        status = -1;
        return;
    }
}

static void ClosePort(void)
{
    int fd = mServerSocket;

    WICED_BT_TRACE("ClosePort fd %d", fd);

    /* if not already closed */
    if (fd != INVALID_SOCKET_FD)
    {
        mServerSocket = INVALID_SOCKET_FD;
        close(fd);
    }
}

static int priv_app_rpc_read_TCP_pkt(unsigned char *pPkt, int32_t max_data)
{
    int readLen = 0, hdrLen = 0, dataLen = 0, payloadLen = 0;

    if ((readLen = recv(mClientSocket, (char *)pPkt, TYPE_LEN, 0)) != TYPE_LEN)
    {
        WICED_BT_TRACE("[%s] readHostTCPpkt() Expected 1, got: %d, errno: %d(%s)", __FUNCTION__, readLen, errno, strerror(errno));
        return (-1);
    }

    // ACL and WICED-HCI share the same basic format
    if (pPkt[0] == HCI_WICED_PKT)
    {
        if ((hdrLen = recv(mClientSocket, (char *)&pPkt[1], HEADER_LEN, 0)) != HEADER_LEN)
        {
            WICED_BT_TRACE("[%s] readHostTCPpkt() Expected 4, got: %d", __FUNCTION__, hdrLen);
            return (-1);
        }
        dataLen = pPkt[3] | (pPkt[4] << 8);
    }
    else
    {
        struct sockaddr_in peer;
        socklen_t peersz = sizeof(peer);
        if (getpeername(mClientSocket, (struct sockaddr*)&peer, &peersz) == 0)
        {
            WICED_BT_TRACE("[%s] !!!!Unknown Type: 0x%02X from %s:%d (fd=%d)", __FUNCTION__, pPkt[0], inet_ntoa(peer.sin_addr), ntohs(peer.sin_port), mClientSocket);
        }
        else
        {
            WICED_BT_TRACE("[%s] !!!!Unknown Type: 0x%02X (fd=%d)", __FUNCTION__, pPkt[0], mClientSocket);
        }
        return 0; // Skip without closing
    }

    if (dataLen != 0)
    {
        int totalLen = readLen + hdrLen + dataLen;
	if (totalLen > max_data)
        {
            uint32_t available = max_data - (readLen + hdrLen);
            WICED_BT_TRACE("[%s] Data overflow: only %u bytes available in buffer, payload length is %d bytes", __FUNCTION__, available, dataLen);
            return (-1);
        }
        if ((payloadLen = recv(mClientSocket, (char *)&pPkt[1 + hdrLen], dataLen, 0)) != dataLen)
        {
            WICED_BT_TRACE("[%s] Expected to read datalen of %u, actually got: %d", __FUNCTION__, dataLen, payloadLen);
            return (-1);
        }
    }

    return (readLen + hdrLen + payloadLen);
}

void *receive(void *ppp)
{
    uint8_t pkt[MAX_PKT_LEN];
    int bytes_rcvd;
    uint16_t opcode, paylen;
    struct sockaddr_in service;
    fd_set read_fds, all_fds;
    int new_socket, client_sockets[MAX_CLIENTS], max_sd, sd, activity;

    struct sockaddr_in cli;
    socklen_t clilen;


    // Initialize client sockets
    for (int i = 0; i < MAX_CLIENTS; i++) client_sockets[i] = INVALID_SOCKET_FD ;

    if ((mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET_FD )
    {
        WICED_BT_TRACE("[%s] socket failed", __FUNCTION__);
        return 0;
    }

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(g_host_instance + SOCK_PORT_NUM_START);
    memset(service.sin_zero, 0, sizeof(service.sin_zero));

    if (bind(mServerSocket, (struct sockaddr *)&service, sizeof(service)) < 0)
    {
        WICED_BT_TRACE("[%s] bind failed", __FUNCTION__);
        ClosePort();
        return 0;
    }

    if (listen(mServerSocket, MAX_AF_INET_BACKLOG) < 0)
    {
        WICED_BT_TRACE("[%s] listen error", __FUNCTION__);
        return 0;
    }

    // Clear and set file descriptors
    FD_ZERO(&all_fds);
    FD_SET(mServerSocket, &all_fds);
    max_sd = mServerSocket;

    while (TRUE)
    {
        // Copy all_fds to read_fds
        read_fds = all_fds;

        // Wait for activity
        activity = select(max_sd + 1, &read_fds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR))
        {
            WICED_BT_TRACE("[%s] Select error", __FUNCTION__);
            break;
        }

        // Check for new connection
        if (FD_ISSET(mServerSocket, &read_fds))
        {
            clilen = sizeof(cli);
            if ((new_socket = accept(mServerSocket, (struct sockaddr *)&cli, &clilen)) < 0)
            {
                WICED_BT_TRACE("[%s] Accept error", __FUNCTION__);
                break;
            }

            // Add new socket to client array
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == INVALID_SOCKET_FD )
                {
                    client_sockets[i] = new_socket;
                    FD_SET(new_socket, &all_fds);
                    if (new_socket > max_sd)
                    {
                        max_sd = new_socket;
                    }
                    WICED_BT_TRACE("[%s] New client connected at index %d, socket fd = %d \n", __FUNCTION__, i, client_sockets[i]);
                    break;
                }
            }
        }

        // Check for I/O on existing sockets
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_sockets[i];
            if ((sd != INVALID_SOCKET_FD ) && FD_ISSET(sd, &read_fds))
            {
                mClientSocket = sd;
                WICED_BT_TRACE("[%s] Received data from client at index %d, socket fd =  %d \n", __FUNCTION__, i, mClientSocket);

                bytes_rcvd = priv_app_rpc_read_TCP_pkt(pkt, MAX_PKT_LEN);

                if (bytes_rcvd == 0)
                {
                    WICED_BT_TRACE("[%s] Client sent packet with unknown type - skipping, connection kept open", __FUNCTION__);
                    continue;
                }

                if (bytes_rcvd < 0)
                {
                    if (sd == mClientSocket)
                    {
                        mClientSocket = INVALID_SOCKET_FD;
                        WICED_BT_TRACE("[%s] Primary client closed, reset mClientSocket", __FUNCTION__);
		            }
                    close (sd);
                    FD_CLR(sd, &all_fds);
                    client_sockets[i] = INVALID_SOCKET_FD ;

                    WICED_BT_TRACE("[%s] Client TCP socket recv failed    Closing...", __FUNCTION__);
                    break;
                }

                if (bytes_rcvd < HCI_HEADER_TOTAL_LEN) {
                    WICED_BT_TRACE("[%s] Short header: got %d bytes", __FUNCTION__, bytes_rcvd);
                    continue;
                }

                opcode = (uint16_t)pkt[1] | ((uint16_t)pkt[2] << 8);
                paylen = (uint16_t)pkt[3] | ((uint16_t)pkt[4] << 8);

                if (paylen > (uint16_t)(MAX_PKT_LEN - HCI_HEADER_TOTAL_LEN)) {
                    WICED_BT_TRACE("[%s] Invalid payload length: %u (max %u)", __FUNCTION__, paylen, (unsigned)(MAX_PKT_LEN - HCI_HEADER_TOTAL_LEN));
                    continue;
                }

                if (bytes_rcvd != (int)(HCI_HEADER_TOTAL_LEN + paylen)) {
                    WICED_BT_TRACE("[%s] Packet length mismatch: expected total %u bytes, but received %d bytes", __FUNCTION__, (unsigned)(HCI_HEADER_TOTAL_LEN + paylen), bytes_rcvd);
                    continue;
                }

                g_app_rpc_handle_socket_data(opcode, &pkt[HCI_HEADER_TOTAL_LEN], paylen);

            }
        }

        // sockets will get closed when program exits
        WICED_BT_TRACE("[%s] Socket thread loop iteration completed", __FUNCTION__);
    }

    ClosePort();
}

void app_rpc_send_data(int type, uint8_t *p_data, uint16_t data_size)
{
    uint8_t buf[MAX_PKT_LEN];

    buf[0] = HCI_WICED_PKT;
    buf[1] = (uint8_t)type;
    buf[2] = (uint8_t)(type >> 8);
    buf[3] = (uint8_t)data_size;
    buf[4] = (uint8_t)(data_size >> 8);

    if(p_data && data_size){
        memcpy(&buf[5], p_data, data_size);
    }

    if (mClientSocket == WICED_INVALID_SOCKET)
    {
        WICED_BT_TRACE("!!!!  hci_control_send_script_event() - no TCP socket - dropping data !!!");
        return;
    }

    if (-1 == (send(mClientSocket, buf, data_size + 5, 0)))
    {
        WICED_BT_TRACE("send failed \n");
        return;
    }

    return;
}

#endif // __linux__
