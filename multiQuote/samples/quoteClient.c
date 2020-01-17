//
//  main.c
//  QuoteClient
//
//  Created by Steven Bagley on 11/11/2015.
//  Copyright © 2015 Brokentooth. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

#define kQUOTEPORT      1717
#define kBufSize 512

int main(int argc, const char * argv[])
{
    char buf[kBufSize+1];
    char line[128];
    ssize_t n;
    int i;
    int more;

    int sockFD;

    sockFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockFD > -1)
    {
        struct sockaddr_in sad;
        struct hostent *ptrh;
        memset(&sad, 0, sizeof(sad));
        sad.sin_family = AF_INET;
        sad.sin_port = htons(kQUOTEPORT);
        ptrh = gethostbyname(argv[1]);
        memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

        if(0 == connect(sockFD, (struct sockaddr *)&sad, sizeof(sad)))
        {
            /* Read input line */
            do
            {
                more = 1;
                n = read(sockFD, buf, kBufSize);
                buf[n] = '\0';
                if(n <= 0)
                    break;

                for(i = 0; i < n; i++)
                {
                    if(buf[i] == 10)
                    {
                        more = 0;
                        break;
                    }
                }
                
                printf("%s", buf);
            } while(more);
        }
            
        close(sockFD);
    }
   
    return 0;
}
