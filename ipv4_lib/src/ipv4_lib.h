/*
 * ipv4_lib.h
 *
 *  Created on: Oct 23, 2017
 *      Author: konrad
 */

#ifndef IPV4_LIB_H_
#define IPV4_LIB_H_

#include <netinet/ip.h>


unsigned short * CreateIpv4Packet ();
//unsigned short Checksum ( unsigned short *datagram, int nbytes );
//unsigned short Ipv4_checksum (unsigned short *dtg, struct iphdr *iph);


#endif /* IPV4_LIB_H_ */
