#ifndef __TUPLE_H__
#define __TUPLE_H__

typedef struct __attribute__ ((__packed__)) FlowKey {
	// 8 (4*2) bytes
    uint32_t src_ip;  // source IP address
    uint32_t dst_ip;
	// 4 (2*2) bytes
    uint16_t src_port;
    uint16_t dst_port;
    // 1 bytes
    uint8_t proto;
} flow_key_t;

#define TUPLE_NORMAL 0
#define TUPLE_PUNC   1
#define TUPLE_TERM   2
#define TUPLE_START  3

typedef struct Tuple {

    /**************************************
     * keys
     *************************************/
    flow_key_t key;
    // 1 bytes
    // only used in multi-thread environment
    uint8_t flag;

    /**************************************
     * values 
     *************************************/

	// 8 bytes
	double pkt_ts;				// timestamp of the packet
    // 8 bytes
    uint64_t seq;
    // 8 bytes
	int64_t size;			// inner IP datagram length (header + data)
} tuple_t;

#endif
