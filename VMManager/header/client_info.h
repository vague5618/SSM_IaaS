#ifndef _CLIENT_INFO_H_
#define _CLIENT_INFO_H_

#include <stdint.h>
#include <stdio.h>

typedef struct _client_info
{
	uint8_t command;	// start 0 / create 1 / delete 2
	uint8_t os_type;	// Android 0 / tizen 1 / ubuntu 2 / window 3
	uint8_t index;		// image index
	char* id;	// Client ID
	char* ip;	// Client IP
	uint8_t connect_type;
	uint16_t port_i;	// image port
	uint16_t port_a;	// audio port
	uint16_t port_e;	// event port
} client_info;

#endif
