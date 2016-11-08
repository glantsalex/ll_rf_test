// messages.h

#ifndef _MESSAGES_h
#define _MESSAGES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

typedef enum operation_code
{
	KEEP_ALIVE_MESSAGE = 1
}operation_code_t;


typedef struct netafim_message
{
	operation_code_t opCode;
	uint8_t			 payloadLength;
	uint8_t			 payload[]; 
}netafim_message_t;

#endif

