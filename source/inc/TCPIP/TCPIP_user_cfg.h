#ifndef _TCPIP_USR_CFG_H
#define _TCPIP_USR_CFG_H

/* Public types */
typedef enum 
{
    E_OK = 0U,
    E_CONNECTION,
    E_SOCKET,
    E_BIND,
    E_LISTEN,
    E_CLOSE,
    E_ACCEPT,
    E_SEND,
    E_RCV,
    E_NOT_OK
} tcpError;


#endif 
