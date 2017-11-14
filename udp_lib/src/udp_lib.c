/*
 * icmp_lib.c
 *
 *  Created on: Oct 23, 2017
 *      Author: konrad
 */

#include <netinet/ip.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/udp.h>

struct pseudo_header
{
	u_int32_t source_address;
	u_int32_t dest_address;
	u_int8_t placeholder;
	u_int8_t protocol;
	u_int16_t udp_length;
};

void PrintHeaderHex ( unsigned short *datagram ){

	//struct iphdr *iphd = (struct iphdr *) dtgr;
	//struct in_addr *addr;
	printf ("\n---------- HEX START ----------\n");

	for ( int i = 0; i < 3; i++ )
		printf ("\n%d: %X", i, ntohs (datagram[i]));

	printf ("\n---------- HEX END ----------\n");
}

unsigned short csum(unsigned short *ptr,int nbytes)
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((u_char*)&oddbyte)=*(u_char*)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;

	return(answer);
}
void CreateUdpPacket ( unsigned char * datagram ) {

	struct pseudo_header *pshd = malloc (sizeof (struct pseudo_header));
	char * pseudogram;

	struct udphdr *udph = ( struct udphdr *) (datagram + sizeof( struct iphdr));
	struct iphdr *iph = (struct iphdr *) datagram;
	char * udpdata = datagram + sizeof ( struct iphdr ) + sizeof ( struct udphdr );
	char input [32];

	iph -> protocol = IPPROTO_UDP;
	memset (udph, 0, iph -> tot_len);
	strcpy (udpdata, "ABCDEFGHIJKLMOPRSTUWXYZ");
	iph -> tot_len = sizeof (struct iphdr) + sizeof ( struct udphdr) + strlen (udpdata);

	printf ("\nUDP source_port: (8745) ");
	fgets (input, 32, stdin);
		if (atoi(input)== 0)
			udph->source = htons (8745);	//ICMP type
		else
			udph->source = htons (atoi (input));

	printf ("\nUDP dest_port: (7554) ");
	fgets (input, 32, stdin);
		if (atoi(input)== 0)
			udph->dest = htons (7554);	//ICMP code
		else
			udph->dest = htons (atoi (input));

	udph -> len = htons (sizeof ( struct udphdr ) + strlen(udpdata));
	udph -> check = 0;


	pshd->source_address = iph->saddr;
	pshd->dest_address = iph->daddr;
	pshd->placeholder = 0;
	pshd->protocol = IPPROTO_UDP;
	pshd->udp_length = udph->len;

	int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(udpdata);
	pseudogram = malloc(psize);

	memcpy(pseudogram , (char*) pshd , sizeof (struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + strlen(udpdata));

	udph->check = csum( (unsigned short*) pseudogram , psize);
	PrintHeaderHex ((unsigned short *)udph);
	printf ("\nCheck: %X", udph->check);
}
