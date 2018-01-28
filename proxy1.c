#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>

void error(char* msg)
{
    perror(msg);
    exit(0);
}

int main(int argc,char* argv[])
{
    pid_t pid;
    struct sockaddr_in addr_in,cli_addr,server;
    struct hostent* host;
    int sockfd,newsockfd;
    
    if(argc<2)
        error("./proxy <port_no>");
    
    printf("\nProxy launching.\n");
    
    memset((char*)&server, '\0', sizeof(server));
    memset((char*)&cli_addr, '\0', sizeof(cli_addr));
    
    server.sin_family=AF_INET;
    server.sin_port=htons(atoi(argv[1]));
    server.sin_addr.s_addr=INADDR_ANY;
    
    
    sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sockfd<0)
        error("Problem in initializing socket");
    
    //bind
    if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed");
        exit(1);
    }
    
    //listen
    listen(sockfd,50);
    int client_length=sizeof(cli_addr);
    printf("\nReady for request.\n");
    
    //acceptance
accepting:
    
    newsockfd=accept(sockfd,(struct sockaddr*)&cli_addr,&client_length);
    
    if(newsockfd<0)
        error("Problem in accepting connection");
    
    pid=fork();
    if(pid==0)
    {
        struct sockaddr_in host_addr;
        int flag=0,newsockfd1=0,n,port=0,i=0,sockfd1=0;
        char buffer[510],t1[300],t2[300],t3[10];
        char* temp=NULL;
        memset(buffer, '\0', 500);
        recv(newsockfd,buffer,500,0);
        
        sscanf(buffer,"%s %s %s",t1,t2,t3);
        
        //structure of GET request
        if(((strncmp(t1,"GET",3)==0))&&((strncmp(t3,"HTTP/1.1",8)==0)||(strncmp(t3,"HTTP/1.0",8)==0))&&(strncmp(t2,"http://",7)==0))
        {
            //message writing that the GET request has been accepted and get request starting
            printf("\nGET request valid and starting.\n");
            
            strcpy(t1,t2);//t1=GET , t2=url, put url in t1
            
            flag=0;
            
            for(i=7;i<strlen(t2);i++)
            {
                if(t2[i]==':')
                {
                    flag=1;
                    break;
                }
            }
            
            temp=strtok(t2,"//");
            if(flag==0)
            {
                port=80;
                temp=strtok(NULL,"/");
            }
            else
            {
                temp=strtok(NULL,":");
            }
            
            sprintf(t2,"%s",temp);
            printf("host = %s",t2);
            host=gethostbyname(t2);
            
            if(flag==1)
            {
                temp=strtok(NULL,"/");
                port=atoi(temp);
            }
            
            
            strcat(t1,"^]");
            temp=strtok(t1,"//");
            temp=strtok(NULL,"/");
            if(temp!=NULL)
                temp=strtok(NULL,"^]");
            printf("\npath = %s\nPort = %d\n",temp,port);
            
            
            memset((char*)&host_addr, '\0', sizeof(host_addr));
            host_addr.sin_port=htons(port);
            host_addr.sin_family=AF_INET;
            //try and replace bccopy(depreciated in most OS)
            bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
            
            sockfd1=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
            newsockfd1=connect(sockfd1,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
            sprintf(buffer,"\nConnected to %s  IP - %s\n",t2,inet_ntoa(host_addr.sin_addr));
            if(newsockfd1<0)
                error("Error in connecting to remote server");
            
            printf("\n%s\n",buffer);
            //send(newsockfd,buffer,strlen(buffer),0);
            
            memset((char*)buffer, '\0', sizeof(buffer));
            if(temp!=NULL)
                sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,t3,t2);
            else
                sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",t3,t2);
            
            
            n=send(sockfd1,buffer,strlen(buffer),0);
            printf("\n%s\n",buffer);
            if(n<0)
                error("Error writing to socket");
            else{
                do
                {
                    memset((char*)buffer, '\0', 500);
                    n=recv(sockfd1,buffer,500,0);
                    if(!(n<=0))
                        send(newsockfd,buffer,n,0);
                }while(n>0);
            }
        }
        
        //kill proxy
        else if( ((strncmp(t1,"kill",3)==0))||
                ((strncmp(t1,"exit",3)==0))||
                 ((strncmp(t1,"q",3)==0)) )
        {
            close(sockfd1);
            close(newsockfd);
            close(sockfd);
            _exit(0);
        }
        
        else
        {
            send(newsockfd,"400 : BAD REQUEST\nONLY HTTP REQUESTS ALLOWED",18,0);
        }
        close(sockfd1);
        close(newsockfd);
        close(sockfd);
        _exit(0);
    }
    else
    {
        close(newsockfd);
        goto accepting;
    }
    return 0;
}
