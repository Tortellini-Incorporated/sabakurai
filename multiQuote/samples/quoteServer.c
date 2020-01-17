//
//  main.c
//  QuoteServer
//
//  Created by Steven Bagley on 15/11/2015.
//  Copyright © 2015 Brokentooth. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <signal.h>

#define kQUOTEPORT      1717
#define kMAXQUOTES  128

char *quotes[kMAXQUOTES];
int numQuotes;

void ReadQuotes(const char *path);
void QuoteServerConnection(int client);
void QuoteServer(void);

int main(int argc, const char * argv[])
{
    ReadQuotes(argv[1]);
    QuoteServer();

	return 0;
}

void QuoteServer()
{
    int serverSocket, clientConnection;
    struct sockaddr_in server;
    struct sockaddr_in client;
    unsigned int alen;
        
    printf("Listening for connections on port %d\n", kQUOTEPORT);
    
    serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(kQUOTEPORT);
    server.sin_addr.s_addr = INADDR_ANY;
    
    /* bind socket */
    if((bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) < 0))
    {
        printf("bind() failed -- %d\n", errno);
        return; /* Error */
    }
    
    /* Mark the connection as listening */
    if(listen(serverSocket, 15) < 0)
    {
        fprintf(stderr, "Failed to listen()\n");
        return;
    }
    
    while(1)
    {
        alen = sizeof(client);
        clientConnection = accept(serverSocket, (struct sockaddr*)&client, &alen);
        if(clientConnection < 0)
        {
            fprintf(stderr, "Accept failed -- %d %d\n", clientConnection, errno);
            return;
        }
        
        printf("Conenction from %x port %d...\n", ntohl(client.sin_addr.s_addr), ntohs(client.sin_port));
        
        /* Handle connection */
        QuoteServerConnection(clientConnection);
        close(clientConnection);
    }
}

void ReadQuotes(const char *path)
{
    FILE *fp;
    char buf[512];
    char *p;
    
    numQuotes = 0;
    fp = fopen(path, "r");
    
    while(!feof(fp) && numQuotes < kMAXQUOTES)
    {
        /* Read in a quote */
        fgets(buf, 512, fp);
        buf[strlen(buf)-1] = '\0';
        
        if(strlen(buf)) /* Ignore blank lines */
        {
            p = malloc(strlen(buf) + 1);

            strcpy(p, buf);
            quotes[numQuotes] = p;
            numQuotes++;
        }
    }
    
    fclose(fp);
    return;
}

void QuoteServerConnection(int clientFd)
{
    char buf[550];
    
    strcpy(buf, quotes[0]);
    strcat(buf, "\r\n");
    write(clientFd, buf, strlen(buf)); /* Need to replace this withour dodgy write routine */
}



