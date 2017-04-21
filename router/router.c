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

struct client_list
{
  unsigned long ip_address;
  short int port_num;
};

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
    printf("%s %-25s:%d\r\n",status,"Souce port number",pkt->sourcePort);
    printf("%s %-25s:%d\r\n",status,"Destination port number",pkt->destPort);
    printf("%s %-25s:%s\r\n",status,"client name",pkt->clientname);
    printf("%s %-25s:%s\r\n",status,"data",pkt->data);
    printf("\r\n");
}
int main(int argc,char **argv){
  int udpSocket, nBytes;
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
  int flag = 0;
    int size;

  if(argc < 4)
  {
    printf("Usage : ./a.out router_port other_router_ip other_router_port\n");
    return 0;
  }
  printf("Rouer IP....127.0.0.7\n");
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

  /*Create UDP socket*/
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(atoi(argv[1]));
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.7");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*Bind socket with address struct*/
  bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*Initialize size variable to be used later on*/
  addr_size = sizeof serverStorage;
  /*Configure settings in address struct*/
  clientAddr.sin_family = AF_INET;
  memset(clientAddr.sin_zero,'\0', sizeof clientAddr.sin_zero);

  while(1){
    /* Try to receive any incoming UDP datagram. Address and port of 
      requesting client will be stored on serverStorage variable */
    nBytes = recvfrom(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&serverStorage, &addr_size);
    printPacket(&b,"received",nBytes);
  
  
    if(flag == 0)
    {
       for(i=0;i<size;i++)
       {
         if(b.destIP == ptr[i]->ip_address)
         {
            printf("Resolved\n");
            break;
         }
    }
    if(i == size)
    {
     printf("Not found passing to router2...\n");
     clientAddr.sin_port = htons(atoi(argv[3]));
     clientAddr.sin_addr.s_addr = inet_addr(argv[2]);
    }
    else
    {
     printf("%d====\n",i);
     clientAddr.sin_port = htons(b.destPort);
     clientAddr.sin_addr.s_addr = b.destIP;
    }
   flag = 1;
   }
    addr_size = sizeof clientAddr;
  
    printPacket(&b,"sent",nBytes);
    sendto(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&clientAddr,addr_size);
   
#if 1
    nBytes = recvfrom(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&clientAddr,&addr_size);
    printPacket(&b,"received",nBytes);
  
    addr_size = sizeof serverStorage;
    sendto(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&serverStorage,addr_size);
    printPacket(&b,"sent-",nBytes);
#endif
  }

    return 0;
}
