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

struct basic
{
    short int pkt_type;
    short int seq_num;
    long port_no;
    char name[MAX_SIZE];
    char data[MAX_SIZE];
};
struct client_list
{
	unsigned long ip_address;
	short int port_num;
};
void printPacket(struct basic *pkt,char *status,int no_of_bytes)
{
    printf("no.of bytes %s is:%d\r\n",status,no_of_bytes);
    printf("%s %-25s:%d\r\n",status,"packet type",pkt->pkt_type);
    printf("%s %-25s:%d\r\n",status,"sequence no",pkt->seq_num);
    printf("%s %-25s:%ld\r\n",status,"port number",pkt->port_no);
    printf("%s %-25s:%s\r\n",status,"client name",pkt->name);
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
  struct client_list **ptr;
	unsigned long ip;

      int n,j,k;
    char buf[50];
	char temp[20];

    int size;

    /*Reading the password database and forming a linkedlist*/
    FILE *fd = fopen("neighbour.txt","r");
    if(fd == NULL)
    {
        perror("fopen");
        return 0;
    }
    fgets(buf,sizeof(buf),fd);
    size = atoi(buf);
    j = 0;
    k = 0;
    ptr = (struct client_list **)malloc(size * sizeof(struct client_list *));

    for(i = 0;i<size;i++)
    {
        ptr[i] = (struct client_list *)malloc(sizeof(struct client_list));
        if(ptr[i] == NULL)
        {
            perror("malloc");
            return 0;
        }
    }

    while(fgets(buf,sizeof(buf),fd))
    {
	 memset(temp,'\0',sizeof(temp));
     for(i=0;buf[i] != ' ';i++);
     strncpy(temp,buf,i);
	 printf("%s ",temp);
	 ptr[j]->ip_address = inet_addr(temp);
     i++;
	 memset(temp,'\0',sizeof(temp));
     for(k=0;buf[i+k] != '\n';k++);
     strncpy(temp,&buf[i],k);
	 printf("%s\n",temp);
	 ptr[j++]->port_num = atoi(temp);
    }
    fclose(fd);

	ip = inet_addr("127.0.0.7");
	for(i=0;i<size;i++)
	if(ip == ptr[i]->ip_address)
		printf("Resolved\n");
  /*Create UDP socket*/
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(3001);
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
	/*Configure settings in address struct*/
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(3002);
    clientAddr.sin_addr.s_addr = inet_addr(b.data);
    memset(clientAddr.sin_zero, '\0', sizeof clientAddr.sin_zero);

  	addr_size = sizeof clientAddr;
    /*Send uppercase message back to client, using serverStorage as the address*/
    sendto(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&clientAddr,addr_size);
  	
    nBytes = recvfrom(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&clientAddr,&addr_size);
	printPacket(&b,"received",nBytes);
	
	addr_size = sizeof serverStorage;
    sendto(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&serverStorage,addr_size);
	}

  return 0;
}
