/* TCPIP source file
 * 
 * For its use with CYW43 on board chip. 
 * Any client IP can bind on the defined port, once connected to a point a
 * task excecutes cyw43_arch_poll which runs necessary processes that otherwise 
 * can end up in errors.
 * 
 * Based on pico-examples/wifi/tcp_server/picow_tcp_server.c
 */

#include "TCPIP/TCPIP_usr.h"
#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>
#include <lwip/tcp.h>

/* Private definitions */
#define TCP_PORT 4242
#define BUF_SIZE 256

/* Private types */
typedef struct
{
  struct tcp_pcb *server_pcb;
  struct tcp_pcb *client_pcb;
  bool complete;
  uint8_t buffer_sent[BUF_SIZE];
  uint8_t buffer_recv[BUF_SIZE];
  int sent_len;
  int recv_len;
  int run_count;
  double environmentVars[4];
} TCP_SERVER_T;

/* Global variables */
TCP_SERVER_T *TcpIpInstance;

/* Private function prototypes */
err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err);
void tcp_server_err(void *arg, err_t err);

/* Name: initTcpIp() 
 * Description: Initializes CYW43 then connects to wifi point. If this connections doesn't times
 * out then a socket is created and put to listen for receiving any IP address request. 
 */
tcpError initTcpIp()
{
  tcpError returnValue = E_OK;

  /* Initialize CYW43 module */
  if (cyw43_arch_init())
  {
    returnValue = E_CONNECTION;
  }

  cyw43_arch_enable_sta_mode();

  /* Connect to given wifi point with 20s timeout */
  if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000U))
  {
    returnValue = E_CONNECTION;
  }

  /* Initialize instance with mem allocation */
  TcpIpInstance = (TCP_SERVER_T *)calloc(1, sizeof(TCP_SERVER_T));

  /* Create socket */
  struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
  if (!pcb)
  {
    returnValue = E_SOCKET;
  }

  /* Bind socket, any client IP can bind */
  err_t err = tcp_bind(pcb, IP_ANY_TYPE, TCP_PORT);
  if (err)
  {
    returnValue = E_BIND;
  }

  /* Listen */
  TcpIpInstance->server_pcb = tcp_listen_with_backlog(pcb, 4);
  if (!TcpIpInstance->server_pcb)
  {
    if (pcb)
    {
      tcp_close(pcb);
      returnValue = E_LISTEN;
    }
  }

  /* Callback to accept any connection request started */
  tcp_arg(TcpIpInstance->server_pcb, TcpIpInstance);
  tcp_accept(TcpIpInstance->server_pcb, tcp_server_accept);

  return returnValue;
}

/* Name: closeTcpIp() 
 * Description: Clears the existent client instances closing any connection with them.
 */
tcpError closeTcpIp()
{
  tcpError returnValue = E_OK;
  err_t err = ERR_OK;

  /* Once running, server is never closed. Only client instances can */
  if (TcpIpInstance->client_pcb != NULL)
  {
    tcp_arg(TcpIpInstance->client_pcb, NULL);
    tcp_poll(TcpIpInstance->client_pcb, NULL, 0);
    tcp_sent(TcpIpInstance->client_pcb, NULL);
    tcp_recv(TcpIpInstance->client_pcb, NULL);
    tcp_err(TcpIpInstance->client_pcb, NULL);
    err = tcp_close(TcpIpInstance->client_pcb);
    if (err != ERR_OK)
    {
      tcp_abort(TcpIpInstance->client_pcb);
      returnValue = E_CLOSE;
    }
    TcpIpInstance->client_pcb = NULL;
  }

  /* Write socket with tx buffer from server */
  /*  if (TcpIpInstance->server_pcb) {
       tcp_arg(TcpIpInstance->server_pcb, NULL);
       tcp_close(TcpIpInstance->server_pcb);
       TcpIpInstance->server_pcb = NULL;
   } */

  return returnValue;
}

/* Name: sendTcpIp(char txBuffer[])
 * Arguments: txBuffer is the char array written to server tx buffer. It size can't be 
 * smaller than BUF_SIZE.
 * Description: Copies char array to tx buffer and writes it to the current socket.
 */
tcpError sendTcpIp(char txBuffer[])
{
  tcpError returnValue = E_OK;
  err_t err = ERR_OK;

  /* Start context */
  cyw43_arch_lwip_begin();

  /* Copy passed char array to the tx buffer of the initialized handler */
  memcpy(TcpIpInstance->buffer_sent, txBuffer, strlen(txBuffer));

  /* Check context */
  cyw43_arch_lwip_check();

  /* Write socket with tx buffer from server */
  err = tcp_write(TcpIpInstance->client_pcb, TcpIpInstance->buffer_sent, strlen(txBuffer), TCP_WRITE_FLAG_COPY);

  if (err != ERR_OK)
  {
    returnValue = E_SEND;
  }

  /* Exit context */
  cyw43_arch_lwip_end();

  return returnValue;
}

/* Name: receiveTcpIp()
 * Description: Retrieves received data from server rx buffer.
 */
tcpError receiveTcpIp()
{
  tcpError returnValue = E_OK;

  /* ToDo: Rx Poll  */

  return returnValue;
}

/* Name: tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err)
 * Arguments: arg is the pointer to current tcpip server instance, client_pcb
 * is the pointer to the tcpip client instance which wants to connect, err is
 * the error message received from the connection accept process.
 * Description: Callback for tcp_accept, the client instance is copied to
 * our tcpip one. 
 */
err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err)
{
  err_t returnValue = ERR_OK;

  if (err != ERR_OK || client_pcb == NULL)
  {
    returnValue = ERR_VAL;
  }
  else
  {
    TcpIpInstance->client_pcb = client_pcb;
    tcp_arg(client_pcb, TcpIpInstance);
    tcp_err(client_pcb, tcp_server_err);
  }

  return returnValue;
}

/* Name: tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err)
 * Arguments: arg is the pointer to current tcpip server instance, err is
 * the error message received from the connection accept process.
 * Description: Callback for tcp_err, if any error occurs the socket connection
 * with client is closed.
 */
void tcp_server_err(void *arg, err_t err)
{
  (void)closeTcpIp();
}
