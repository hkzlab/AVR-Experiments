#ifndef _CAN_PKT_HEADER_
#define _CAN_PKT_HEADER_
#include <avr/io.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
	uint32_t address;
	uint8_t data[8];
	uint8_t ide, rtr, srr, len;
} can_packet;

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _CAN_PKT_HEADER_ */
