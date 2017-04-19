/************* UDP SERVER CODE *******************/

#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_SIZE    200
struct basic{
long sourceIP;
long destIP;
int sourcePort;
int destPort;
short pkt_type;     //REG, REQ, DATA, CONF, REPLY, ERROR, ACK
char clientname[MAX_SIZE]; //client’s username
char data[MAX_SIZE];   // use more fields only if required
};



void printPacket(struct basic *pkt,char *status,int no_of_bytes)
{
    printf("no.of bytes %s is:%d\r\n",status,no_of_bytes);
    printf("%s %-25s:%d\r\n",status,"packet type",pkt->pkt_type);
//  printf("%s %-25s:%d\r\n",status,"sequence no",pkt->seq_num);
    printf("%s %-25s:%d\r\n",status,"Souce port number",pkt->sourcePort);
    printf("%s %-25s:%d\r\n",status,"Destination port number",pkt->destPort);
    printf("%s %-25s:%s\r\n",status,"client name",pkt->clientname);
    printf("%s %-25s:%s\r\n",status,"data",pkt->data);
    printf("\r\n");
}

int main(){
  int udpSocket, nBytes;
  char buffer[1024];
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size, client_addr_size;
  int i;
  struct basic b;

  /*Create UDP socket*/
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(36);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.6");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*Bind socket with address struct*/
  bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*Initialize size variable to be used later on*/
  addr_size = sizeof serverStorage;

  while(1){
    /* Try to receive any incoming UDP datagram. Address and port of 
      requesting client will be stored on serverStorage variable */
    nBytes = recvfrom(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&serverStorage, &addr_size);
	printPacket(&b,"received",nBytes);
#if 0
    /*Convert message received to uppercase*/
    for(i=0;i<nBytes-1;i++)
      buffer[i] = toupper(buffer[i]);

#endif
	b.pkt_type = 0x06;
	strcpy(b.data,"MANOJPRASANTHI\n");
    /*Send uppercase message back to client, using serverStorage as the address*/
    sendto(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&serverStorage,addr_size);
	//printPacket(&b,"sent",nBytes);
	printf("Sent Reply\n");
	}

  return 0;
}
