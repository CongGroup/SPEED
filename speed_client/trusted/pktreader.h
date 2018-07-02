#pragma once

#include <stdint.h>
#include "../../common/config.h"

#define IPPROTO_TCP 6

const int ether_len = 14;
const int ip_default_len = 20;
const int tcp_default_len = 20;
const int udp_default_len = 8;

#define ntohs(A)  ((((uint16_t)(A) & 0xff00) >> 8) | \
(((uint16_t)(A) & 0x00ff) << 8))

#define ntohl(A)  ((((uint32_t)(A) & 0xff000000) >> 24) | \
(((uint32_t)(A) & 0x00ff0000) >> 8) | \
(((uint32_t)(A) & 0x0000ff00) << 8) | \
(((uint32_t)(A) & 0x000000ff) << 24))


class PktReader
{
public:
	PktReader() {
		p = 0;
		
	}
	PktReader(const char* pkg) {
		attach(pkg);
	}

	inline void attach(const char * pkg) 
	{ 
		p = pkg; 
		network_offset = ether_len;
		transport_offset = network_offset + getIPHeaderLength();
	}
	inline const char* getPacket() { return p; }

#define PACKET_DATA_OFFSET(p,type,offset) ((p)?((type)((const char *)p +(offset))):0)

	inline uint8_t* getIPHeader() { return PACKET_DATA_OFFSET(p, uint8_t*, network_offset); }
	inline uint8_t getIPVersion() { return  (*PACKET_DATA_OFFSET(p, uint8_t*, network_offset)) >> 4; };
	inline uint8_t getIPHeaderLength() { return ((*PACKET_DATA_OFFSET(p, uint8_t*, network_offset)) & 0xf) * 4; };
	inline uint16_t getIPTotalLength() { return ntohs(*PACKET_DATA_OFFSET(p, uint16_t*, network_offset + 2)); };
	inline uint16_t getIPID() { return ntohs(*PACKET_DATA_OFFSET(p, uint16_t*, network_offset + 4)); };
	inline bool getMF() { return *PACKET_DATA_OFFSET(p, uint8_t*, network_offset + 6) >> 5 & 0x1; };
	inline bool getDF() { return *PACKET_DATA_OFFSET(p, uint8_t*, network_offset + 6) >> 6 & 0x1; };
	inline uint16_t getFragmentOffset() { return ntohs(*PACKET_DATA_OFFSET(p, uint16_t*, network_offset + 6)) & 0x1FFF; };
	inline uint32_t getSrcIP() { return *PACKET_DATA_OFFSET(p, uint32_t*, network_offset + 12); };
	inline uint32_t getDstIP() { return *PACKET_DATA_OFFSET(p, uint32_t*, network_offset + 16); };
	inline uint8_t* getIpOption() { return PACKET_DATA_OFFSET(p, uint8_t*, network_offset + ip_default_len); }
	inline uint16_t getSrcPort() { return ntohs(*PACKET_DATA_OFFSET(p, uint16_t*, transport_offset)); };
	inline uint16_t getDstPort() { return ntohs(*PACKET_DATA_OFFSET(p, uint16_t*, transport_offset + sizeof(uint16_t))); };
	
	inline uint8_t getProtocol() { return *PACKET_DATA_OFFSET(p, uint8_t*, network_offset + 9); };
	inline uint8_t* getTransportHeader() { return PACKET_DATA_OFFSET(p, uint8_t*, transport_offset);  };
	inline uint8_t getTransportLength() { return getProtocol() == IPPROTO_TCP ? (((*PACKET_DATA_OFFSET(p, uint8_t*, transport_offset + 12))>>4) * 4) : udp_default_len; };
	inline uint8_t* getPayload() { return getTransportHeader() + getTransportLength(); };
	inline uint16_t getPayloadLength() { return getProtocol() == IPPROTO_TCP ? (getIPTotalLength() - getIPHeaderLength() - getTransportLength()) : (getIPTotalLength() - getIPHeaderLength() - udp_default_len); };

	inline uint8_t getTCPFlag() {return getProtocol() == IPPROTO_TCP ? *PACKET_DATA_OFFSET(p, uint8_t*, transport_offset + 13) : 0;};
	inline bool getFINFlag() { return getTCPFlag() & 0x01; };
	inline bool getSYNFlag() { return getTCPFlag() & 0x02; };
	inline bool getRSTFlag() { return getTCPFlag() & 0x04; };
	inline bool getPSHFlag() { return getTCPFlag() & 0x08; };

private:
	const char * p;

	int network_offset;
	int transport_offset;
};


