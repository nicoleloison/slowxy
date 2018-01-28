/*
 * tcpclient.c
 *
 *  Created on: Jan 18, 2018
 *      Author: nicoleloison
 */

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
#include <signal.h>

void get_filetype(char *filename, char *filetype);
int parse_uri(char *uri, char *filename, char *cgiargs);

void get_filetype(char *filename, char *filetype)
{
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else
        strcpy(filetype, "text/plain");
}

int parse_uri(char *uri, char *filename, char *cgiargs)
{
    char *ptr;
    
    if (!strstr(uri, "cgi-bin")) {  /* Static content */
        strcpy(cgiargs, "");
        strcpy(filename, ".");
        strcat(filename, uri);
        if (uri[strlen(uri)-1] == '/')
            strcat(filename, "home.html");
        return 1;
    }
    else {  /* Dynamic content */
        ptr = index(uri, '?');
        if (ptr) {
            strcpy(cgiargs, ptr+1);
            *ptr = '\0';
        }
        else
            strcpy(cgiargs, "");
        strcpy(filename, ".");
        strcat(filename, uri);
        return 0;
    }
}


int main() {
    
    //addresses
    struct sockaddr_in server;
    
    //port can be changed
    int PORT = 8001;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //listening
    /* Create the listening socket */
    int lstn_sock;
    lstn_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (lstn_sock < 0) {
        printf("Error in socket() while creating lstn_sock\n");
        exit(-1);
    }
    
    //binding
    int status;
    status = bind(lstn_sock, (struct sockaddr *) &server,
                  sizeof(struct sockaddr_in));
    if (status < 0) {
        printf("Error in bind()\n");
        exit(-1);
    }
    
    //connect to tcp
       status = listen(lstn_sock, 5);
    if (status < 0) {
        printf("Error in listen()\n");
        exit(-1);
    }
    
    //loop for listening

    while (1) {
        
        //acceptance
        int connected_sock;
        connected_sock = accept(lstn_sock, NULL,
                                NULL);
        if (connected_sock < 0) {
            printf("Error in accept()\n");
            exit(-1);
        }
        
        //connected to web browser
        
        //TODO
        /**************
        int is_static;
        struct stat sbuf;
        char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
        char filename[MAXLINE], cgiargs[MAXLINE];
        rio_t rio;
        
        /* Read request line and headers */
        Rio_readinitb(&rio, fd);
        Rio_readlineb(&rio, buf, MAXLINE);
        sscanf(buf, "%s %s %s", method, uri, version);
        if (strcasecmp(method, "GET")) {
            clienterror(fd, method, "501", "Not Implemented",
                        "Proxy only handles GET./n");
            return;
        }
        read_requesthdrs(&rio);
        
        /* Parse URI from GET request */
        is_static = parse_uri(uri, filename, cgiargs);
        if (stat(filename, &sbuf) < 0) {
            clienterror(fd, filename, "404", "Not found",
                        "couldn't find this file");
            return;
        }
       / **************/
        
        //RETURN TO OG server CODE
        //send data
        int count;
        char message[1024] = { "For termination send \"Bye\"\n" };
        count = send(connected_sock, message, sizeof(message), 0);
        if (count < 0) {
            printf("Error in send()\n");
        }
        
        
        //receive data from web browser here
        
        char rcv_message[1024];
        count = recv(connected_sock, rcv_message, sizeof(rcv_message), 0);
        if (count < 0) {
            printf("Error in recv()\n");
        } else {
            printf("Web browser said: %s\n", rcv_message);
        }
        while (1) {
            count = recv(connected_sock, rcv_message, sizeof(rcv_message), 0);
            if (count < 0) {
                printf("Error in recv()\n");
            } else {
                printf("Browser said: %s\n", rcv_message);
            }
            if (strstr(rcv_message, "Bye") != NULL) {
                exit(0);
            }
        }
        
    
        close(connected_sock);
        exit(0);

        
    }
    
    //close listening socket

    close(lstn_sock);
    return 0;
}
