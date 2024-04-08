#ifndef _TCPIP_USR_H
#define _TCPIP_USR_H

#include "TCPIP_user_cfg.h"

/* Public function prototypes */
extern tcpError initTcpIp();
extern tcpError closeTcpIp();
extern tcpError sendTcpIp(char txBuffer[]);
extern tcpError receiveTcpIp();

#endif 
