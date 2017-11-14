/*
 * programLib.c
 *
 *  Created on: Oct 29, 2017
 *      Author: konrad
 */
#include <dlfcn.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <net/if.h>
#include "programLib.h"
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

struct Node *head = NULL;	//head of the list

//function to load IPv4 library
void * LoadIpv4 (){

	void *IpLib = malloc ( sizeof (void ) );	//handle to IP lib

	IpLib = dlopen("./ipv4_lib.so", RTLD_LAZY);
		if (!IpLib)
			return 0;
		else
			return IpLib;

}

//function to load udp library
void * LoadUdp (){

	void *UdpLib = malloc ( sizeof (void ) );	//handle to udp lib

	UdpLib = dlopen("./udp_lib.so", RTLD_LAZY);
		if (!UdpLib)
			return 0;
		else
			return UdpLib;

}

//function to send packets from linked list
int SendPacket ( char *interface ){

	char *datagram = head -> datagram;
	struct iphdr *iph = (struct iphdr *) datagram;

	//open socket
	int s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
		if (s == -1){
			printf ("\nSocket not opened\n");
			return EXIT_FAILURE;
		}


	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons (80);
	sin.sin_addr.s_addr = iph->daddr;

	struct ifreq ifr;	//struct to take name of interface

	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", interface);
	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {		//find typed interface
			perror("ioctl() failed to find interface ");
			return (EXIT_FAILURE);
		}

	if (setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {	//bind socket to interface
		printf ("\nNot binded\n");
		return EXIT_FAILURE;
	}


	while ( head != NULL ) {	//go to end of list

		if (sendto (s, datagram, iph->tot_len, 0, (struct sockaddr *) &sin, sizeof (sin)) < 0){	//send packet
			perror("Sendto failed: ");
			return EXIT_FAILURE;
		}
		else
			printf ("Packet send. Length: %d \n" , iph->tot_len);

		head = head -> next;	//go to next node
	}
	close (s);	//close socket
	//free ( &(head -> datagram));
	DeleteList ();	//delete list
	PrintList ();	//print list
	//free (interface);
	return EXIT_SUCCESS;
}

//function to print menu
int Menu ( int *count, char *interface ) {

	char input [32];	//temporary input data
	int result;	//return value
	char cmd [30];	//temporary sys command

	printf ("\033[H\033[J");	//cleans console
	printf ("Welcome to sender. What do you want to do: \n");
	printf ("[1] Send IPv4 packet \n");
	printf ("[2] Send UDP packet \n");

	fgets ( input, 32, stdin );	//get type of operation
	result = atoi (input);

	printf ("\033[H\033[J");	//cleans console
	printf ("Which interface do you want to use? Type name of interface.\n");


	//usu sys command
	sprintf(cmd, "/bin/ip link");
	system(cmd);

	printf ("Name:");
	fgets ( input, 32, stdin );	//get name of interface
	memcpy ( interface, input, strlen (input)-1 );


	printf ("How many packets do you want to send? ");
	fgets ( input, 32, stdin );	//get number of packets to send
	*count = atoi (input);

	return result;
}



void LoadToList ( int *count, char *dtgr ){

	for ( int i = 0; i != *count; i++ )
		InsertTail ( dtgr );
}



//function to reserve memory for new list element
struct Node *ReserveMem ( char *datagram ){

	//create new node
	struct Node *new_node;
	static int id = 1;

	//reserve memory for new node
	new_node = malloc(sizeof (struct Node));


	if (new_node == NULL) {
		printf("Cannot create new node");
		return NULL;
	}

	//add data to new node
	new_node->id = id;
	id++;
	new_node-> datagram = datagram;
	new_node->next = NULL;
	new_node->prev = NULL;

	//return address
	return new_node;
}

//function to put new element at the end of the list
void InsertTail ( char *datagram ){

	struct Node *temp = head;
	struct Node *new_node = ReserveMem ( datagram );


	if (head == NULL){
		head = new_node;
		return;
	}

	while (temp->next != NULL) //goto end of list
		temp = temp->next;

	//add new node to the end
	temp->next = new_node;
	new_node->prev = temp;
	new_node->next = NULL;
}

//function to print linked list
void PrintList () {

	struct Node *temp = head;

	if ( temp == NULL)
		printf ("\nList empty\n");

	while (temp != NULL){
		printf ("id: %d, dtgr: %p\n", temp->id, (void *)(temp-> datagram) );
		temp = temp->next;
	}
}

//function to return head of the list
struct Node * ReturnHead () {
	return head;
}

//function to delete linked list
void DeleteList (){

	struct Node *temp = head;
	struct Node *del = NULL;

	while ( temp != NULL ){
		del = temp;
		temp = temp -> next;
		free (del);
	}
	head = NULL;

}





