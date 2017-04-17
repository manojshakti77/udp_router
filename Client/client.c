/************* UDP CLIENT CODE *******************/
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
#include<signal.h>

#define REG		0x01
#define CONF	0x03
#define REQ		0x02
#define REPLY	0x04
#define ERROR	0x05
#define ACK		0X06

#define MAX_SEC		3
#define MAX_SIZE	200


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

/*Global varaiable declaration*/
struct basic b;
int flag;
struct sockaddr_in server_addr;
socklen_t addr_size;
int clientsfd;

void sig_handler(int num)
{
    printf("Alarm handler\n");
    if(flag == 0)
    {
        sendto(clientsfd,&b,sizeof(b),0,(struct sockaddr *)&server_addr,addr_size);
        perror("sendto");
        printPacket(&b,"sent",sizeof(b));
    }
    flag = 0;
    return;
}


int main(int argc,char **argv)
{
	int portNum, read_count;
  	struct timeval tv;
  	FILE *fd;
  	int seq_num = 0;
	int retval;
	
	if(argc < 3)
	{
		printf("Usage : ./cli port_number ip_address username password\n");
		return 0;
	}
  	/*Create UDP socket*/
  	clientsfd = socket(PF_INET, SOCK_DGRAM, 0);
	if(clientsfd < 0)
	{
		perror("clientsfd");
		return 0;
	}

	printf("%d=REG\t%d=REQ\t%d=CONF\t%d=REPLY\t%d=ERROR\t%d=ACK\r\n\n",REG,REQ,CONF,REPLY,ERROR,ACK);
	
	
	/*Configure settings in address struct*/
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = inet_addr(argv[2]);
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);  

	/*Initialize size variable to be used later on*/
	addr_size = sizeof server_addr;


	signal(SIGALRM,&sig_handler);
	b.pkt_type = REG;
	b.seq_num = seq_num;
	b.port_no = 3000;
	strcpy(b.name,argv[3]);
	strcpy(b.data,argv[4]);

	if((sendto(clientsfd,&b,sizeof(b),0,(struct sockaddr *)&server_addr,addr_size)) < 0)
	{
		perror("sendto");
		return 0;
	}
	printf("REG packet sent\r\n");
	printPacket(&b,"sent",sizeof(b));


	/*Opening the default file*/
	fd = fopen("input.txt","r");
	
	if(fd == NULL)
	{
		perror("fopen");
		return 0;
	}
	
	/*Client side vunarability*/
	int j = 1;
	
	while(1)
	{
	printf("Value of j is %d\r\n",j);
	/*Generating vunarability*/
	
	alarm(MAX_SEC);
    flag = 0;
	/*Receive message from server*/
    read_count = recvfrom(clientsfd,&b,sizeof(b),0,NULL, NULL);
	flag = 1;
    alarm(0);
    /*Checking the status of recvfrom*/
	if(read_count < 0)
    {
		printf("ACK not received\r\n");
	    perror("recvfrom");
	    printf("Resending the packet\r\n");
  		sendto(clientsfd,&b,sizeof(b),0,(struct sockaddr *)&server_addr,addr_size);
  		printf("Frame sent\r\n");
		printPacket(&b,"sent",sizeof(b));
		continue;	
    }
  	switch(b.pkt_type)
  	{
    case CONF : printf("Frame received\r\n");
				printf("CONNECTION CONFIRMED\r\n");
				printPacket(&b,"received",read_count);
		
				fgets(b.data,sizeof(b.data),fd);
		  		seq_num = ((seq_num)^(0x01));
        			b.pkt_type = REQ;
		  		b.seq_num = seq_num;
    			if((sendto(clientsfd,&b,sizeof(b),0,(struct sockaddr *)&server_addr,addr_size)) < 0)
				{
					perror("sendto::CONF");
					return 0;
				}
				printf("packet with file name info sent\r\n");
				printPacket(&b,"sent",sizeof(b));
				j = 1;
        		break;
        		
    case ERROR: printf("Frame received\r\n");
				printf("CONNECTION REJECTED\r\n");
				printPacket(&b,"received",read_count);
				printf("Exiting\r\n");
				return 0;
				
	case REPLY: printf("Frame Received\r\n");
				printf("REPLY FROM SERVER\r\n");
				printpacket(&b,"received",read_count);
				printf("Exiting\r\n");
				break;
				
    case ACK: 	printf("Frame received\r\n");
				printf("ACK FROM SERVER\n");
				printPacket(&b,"received",read_count);
				
				if(j == 3)
				{
					printf("Repeating the same packet for vunarability test\r\n");
					sendto(clientsfd,&b,sizeof(b),0,(struct sockaddr *)&server_addr,addr_size);
					printf("Frame sent\r\n");
					printPacket(&b,"sent",sizeof(b));
					j = 1;
					continue;	
				}
				
				
		  		if(fgets(b.data,sizeof(b.data),fd))
				{
		  		seq_num = ((seq_num)^(0x01));
				b.pkt_type = REQ;
		  		b.seq_num = seq_num;
    		 	if((sendto(clientsfd,&b,sizeof(b),0,(struct sockaddr *)&server_addr,addr_size)) < 0)
				{
					perror("sento::ACK");
					return 0;
				}
				printf("Frame sent\r\n");
				printPacket(&b,"sent",sizeof(b));
				}
				else
					return 0;
        	 	break;
        	 	
    default : printf("Frame received\r\n");
				printf("Invalid packet\n");
				printPacket(&b,"received",read_count);
				break;
		}
	j++;
	}
  return 0;
}
