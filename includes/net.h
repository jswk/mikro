#ifndef NET_H
#define NET_H

// ******* ETH *******
#define ETH_HEADER_LEN        14
// values of certain bytes:
#define ETHTYPE_ARP_H_V 0x08
#define ETHTYPE_ARP_L_V 0x06
#define ETHTYPE_IP_H_V 0x08
#define ETHTYPE_IP_L_V 0x00
#define ETHTYPE_IPv6_H_V 0x86
#define ETHTYPE_IPv6_L_V 0xdd
// byte positions in the ethernet frame:
//
// Ethernet type field (2bytes):
#define ETH_TYPE_H_P 12
#define ETH_TYPE_L_P 13
//
#define ETH_DST_MAC 0
#define ETH_SRC_MAC 6

// ******* IPv6 ********
#define IPv6_HEADER_LEN 40

#define IPv6_VERSION 0
#define IPv6_VERSION_VAL 0x06

#define IPv6_PAYLOAD 4
#define IPv6_NEXT 6
#define IPv6_HOP_LIMIT 7
#define IPv6_SRC_ADDR 8
#define IPv6_DST_ADDR 24

#endif
