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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define REG		0x01
#define CONF	0x03
#define REQ		0x02
#define REPLY	0x04
#define ERROR	0x05
#define ACK		0X06

#define MAX_SIZE	200

struct database
{
    char name[10];
    char password[10];
};

struct client_info
{
    int port_no;
    char name[10];
};

struct clientdatabase
{
	char name[10];
	char ip_address[15];
	char port_num[10];
};

struct basic
{
	short int pkt_type;
	short int seq_num;
	long port_no;
	char name[MAX_SIZE];
	char data[MAX_SIZE];
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

struct database **ptr;
struct database base;
int size;

struct clientdatabase **cli_ptr;
int cli_size;

int database_create(void)
{
	int i,n,j,k;
	char buf[50];

	int size;

	/*Reading the password database and forming a linkedlist*/
    FILE *fd = fopen("database.txt","r");
	if(fd == NULL)
	{
		perror("fopen");
		return 0;
	}
	fgets(buf,sizeof(buf),fd);
    size = atoi(buf);
	j = 0;
	k = 0;
    ptr = (struct database **)malloc(size * sizeof(struct database *));

    for(i = 0;i<size;i++)
	{
        ptr[i] = (struct database *)malloc(sizeof(base));
		if(ptr[i] == NULL)
		{
    		perror("malloc");
			return 0;
		}
	}

	while(fgets(buf,sizeof(buf),fd))
    {
     for(i=0;buf[i] != ' ';i++);
     strncpy(ptr[j]->name,buf,i);
     i++;
     for(k=0;buf[i+k] != '\n';k++);
     strncpy(ptr[j++]->password,&buf[i],k);
    }
    fclose(fd);
	return size;
}
int database_create_clientlist(void)
{
    int i,n,j,k;
    char buf[50];
	char ip_address[15];
	char port_num[5];

    int size;

    /*Reading the password database and forming a linkedlist*/
    FILE *fd = fopen("clientlist.txt","r");
    if(fd == NULL)
    {
        perror("fopen");
        return 0;
    }
    fgets(buf,sizeof(buf),fd);
    cli_size = atoi(buf);
    j = 0;
    k = 0;
    cli_ptr = (struct clientdatabase **)malloc(cli_size * sizeof(struct clientdatabase *));

    for(i = 0;i<cli_size;i++)
    {
        cli_ptr[i] = (struct clientdatabase *)malloc(sizeof(struct clientdatabase));
        if(cli_ptr[i] == NULL)
        {
            perror("malloc");
            return 0;
        }
//		printf("%d....%d\n",cli_size,i);
    }

    while(fgets(buf,sizeof(buf),fd))
    {
	puts(buf);
     for(i=0;buf[i] != ' ';i++);
     strncpy(cli_ptr[j]->name,buf,i);
//	 cli_ptr[j]->name[i+1] = '\0';
     i++;
     for(k=0;buf[i+k] != ' ';k++);
     strncpy(cli_ptr[j]->ip_address,&buf[i],k);
//	 ip_address[k] = '\0';
	 i = i+k+1;
     for(k=0;buf[i+k] != '\n';k++);
     strncpy(cli_ptr[j++]->port_num,&buf[i],k);
//	 port_num[k] = '\0';
//	 cli_ptr[j]->ip_address = inet_addr(ip_address);
//	 cli_ptr[j++]->port_num = atoi(port_num); 
//	 printf("%s \n",ip_address);
    }
    fclose(fd);
	for(i=0;i<cli_size;i++)
		printf("%s %s %s\n",cli_ptr[i]->ip_address,cli_ptr[i]->name,cli_ptr[i]->port_num);
    return size;
}
int main(int argc,char **argv)
{
	int udpSocket, nBytes;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size, client_addr_size;
	int i,j;
	struct client_info client_info;//To store the client information once connection confirmed 
	int seq_num = 1;
	int index = 0;
	struct basic b;
	int retval;
	int size;
	int fd,flag =0;

/*Check for no.of arguments received*/
#if 1
	if(argc < 2)
	{
		printf("Usage : ./ser port_number ip_address\n");
		return 0;
	}
#endif	
	size = database_create();
	if(size == 0)
		return 0;
	
	retval = database_create_clientlist();
	if(size == 0)
		return 0;
		
	
	printf("%d=REG\t%d=REQ\t%d=CONF\t%d=REPLY\t%d=ERROR\t%d=ACK\r\n\n",REG,REQ,CONF,REPLY,ERROR,ACK);
	
		

	/*Create UDP socket*/
	udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
	if(udpSocket < 0)
	{
		perror("socket");
		return 0;
	}


	/*Configure settings in address struct*/
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[1]));
	serverAddr.sin_addr.s_addr = inet_addr(argv[2]);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	/*Bind socket with address struct*/
	if(bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("bind");
		return 0;
	}
	else
	{
		printf("Listening for connections\r\n\n\n");
	}
	

	/*Initialize size variable to be used later on*/
	addr_size = sizeof serverStorage;

#if 0
	if((fd = open("output.txt",O_CREAT|O_WRONLY)) < 0)
	{
		perror("open::file");
		return 0;
	}
#endif

	while(1){
	if(flag)
	{
	printf("value of j : %d\r\n",index);
	printf("Expecting packet sequence : %d\r\n",!seq_num);
	}

    /* Try to receive any incoming UDP datagram. Address and port of 
      requesting client will be stored on serverStorage variable */
    nBytes = recvfrom(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&serverStorage, &addr_size);
    
    /*Checkk for no.of bytes received*/
    if(nBytes < 0)
	{
		perror("recvfrom::server");
		continue;
	}
	
	/*Check for the sequence number*/
	if(seq_num == b.seq_num)
	{
		printf("Frame received\r\n");
		printf("Packet received with incorrect sequence number/ Duplicate packet\r\n");
		printPacket(&b,"received",nBytes);
		b.pkt_type = ACK;
		sendto(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&serverStorage,addr_size);
		printf("Response packet sent\r\n");
		printPacket(&b,"sent",sizeof(b));
		continue;
	}
	seq_num = b.seq_num;

     switch(b.pkt_type)
     {
        case REG:
				printf("Regestration request packet received\r\n");
				printf("----%s\n",b.data);
				printPacket(&b,"received",nBytes);
				/*Logic for the AUTHENTICATION*/
                 for(i=0;i<size;i++)
                 {
                   if(!strcmp(b.name,ptr[i]->name))
                   {
                      if(!strcmp(ptr[i]->password,b.data))
                      {
						client_info.port_no = b.port_no;
						strcpy(client_info.name,b.name);
                        b.pkt_type = CONF;
                        break;
                      }
                   }
                 }
                 if(i == size)
                    b.pkt_type = ERROR;
					sendto(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&serverStorage,addr_size);
					printf("Response packet sent\r\n");
					printPacket(&b,"sent",sizeof(b));

					for(j = 0;j<size;j++)
					{
						free(ptr[j]);
					}
					free(ptr);
					if(i == size)
						return 0;
					break;
					 
		case REQ:if((b.port_no == client_info.port_no) && (strcmp(b.name,client_info.name)))
				 {
					printf("Frame received\r\n");
					printf("Authentication error\r\n");
					printPacket(&b,"received",nBytes);
					b.pkt_type = ERROR;
    		   		sendto(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&serverStorage,addr_size);
    		   		printf("Response packet sent\r\n");
					printPacket(&b,"sent",sizeof(b));
					break;
				 }
	
				printf("Frame received\r\n");
				printf("Data frame\r\n");
				printPacket(&b,"received",nBytes);

				#if 0
				for(i=0;i<cli_size;i++)
				{
						printf("REPLY--------%d\n",i);
					if(strncmp(b.data,cli_ptr[i]->name,10) == 0)
					{
               	 		b.pkt_type = REPLY;
						strcpy(b.data,cli_ptr[i]->ip_address);
						return 0;
					}
				}
				if(i == cli_size)
					b.pkt_type = ERROR;
				#endif
               	 		b.pkt_type = REPLY;
						strcpy(b.data,cli_ptr[0]->ip_address);

				 if((++index) == 5)
				 {
					 index = 1;
					 continue;
				 }
    		   	 if(sendto(udpSocket,&b,sizeof(b),0,(struct sockaddr *)&serverStorage,addr_size) < 0)
				 {
				 	perror("sendto::REQ");
				 	close(fd);
					return 0;
				 }

				printf("ACK packet sent\r\n");
				printPacket(&b,"sent",sizeof(b));
				return 0;	
               	break;
               	
        default:printf("Frame received\r\n");
				printf("Unknown\n");
				printPacket(&b,"received",nBytes);
                break;
     }
  }

  return 0;
}
