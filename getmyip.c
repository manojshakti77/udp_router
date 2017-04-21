#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<arpa/inet.h>
#define h_addr h_addr_list[0]

in_addr_t get_my_ip(void)
{
        char name[80];
        struct hostent *ip;
        int ret;
        memset(name,'\0',sizeof(name));
        
        ret = gethostname(name,sizeof(name));
        if(ret == -1)
        {
            printf("Error in getting the hostname\n");
            perror("gethostname");
            return 0;
        }
        printf("Hostname:%s\n",name);

        ip = gethostbyname(name);
        if(ip == NULL)
        {
            printf("Unable to get the IP address\n");
            perror("hostbyname");
            return 0;
        }
        return ((struct in_addr *)ip->h_addr_list[0])->s_addr;
}

int main(void)
{
    in_addr_t ip = get_my_ip();
    struct in_addr temp;
    if(ip == 0)
    {
        return 0;
    }
    temp.s_addr = ip;
    printf("%s\n",inet_ntoa(temp));
}
