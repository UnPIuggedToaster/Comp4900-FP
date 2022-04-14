
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include "server.h"

#define MAX_STRING_LEN    256
#define PORT 8080
#define SA struct sockaddr

// Function designed for chat between client and server.
void connectPython(int connfd, char * str)
{
    char buff[MAX_STRING_LEN ];
    memset(buff, 0 , MAX_STRING_LEN );

    // read the message from client and copy it in buffer
    read(connfd, buff, sizeof(buff));

    // print buffer which contains the client contents
    printf("Relaying information to python mail server.\n");
    strcpy (buff, str);

    // and send that buffer to client
    write(connfd, buff, strlen(buff));
}

#define ATTACH_POINT "my_name"

int main(void)
{
    //message struct
	typedef union
	{
		struct _pulse pulse;
		cksum_header_t cksum_hdr;
		uint16_t msg_type;
	    char rmsg [MAX_STRING_LEN +1];
	} myMessage_t;

	// variables
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, clientAddr;
    name_attach_t * attach;
    myMessage_t msg;


    char * host;
    char masterpwd [MAX_STRING_LEN] = "";


    // create a channel
	if ((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL)
	{
		printf("EXIT_FAILURE\n");
		return EXIT_FAILURE;
	}

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else{ printf("Socket successfully created..\n"); }

    // set the server address
    memset(&servaddr, 0,sizeof(servaddr));

    // assign IP, PORT, accept any connection from localhost

	// TO THE TA/PROF, CHANGE THE IP ADDRESS TO THE VM'S IP ADDRESS 
    host = "192.168.150.128";
	// change the above ip address before running 

    servaddr.sin_family = AF_INET;
    //memcpy (&servaddr.sin_addr, gethostbyname(host)->h_addr_list, gethostbyname(host)->h_length);
    servaddr.sin_addr.s_addr = inet_addr(host);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) == -1)
    {
        printf("socket bind failed...\n");
        return EXIT_FAILURE;
    }
    else { printf("Socket successfully binded..\n"); }

    len = sizeof(clientAddr);


	listen(sockfd, 5);


    while (1)
    {

    	// accept the python server requests and listen everytime the pinpad qnx server sends an iov_t message
    	if ((connfd = accept(sockfd, (struct sockaddr*)&clientAddr, &len)) < 0)
    	{
    		printf("Connection with python server failed.\n");
    	}

    	memset (&servaddr, sizeof(servaddr), 0);

    	// receive the message from the qnx keypad box
    	int rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

		if (rcvid == -1)
		{
			perror("MsgReceive");
			return EXIT_FAILURE;
		}

		else if (rcvid > 0)
		{
			// declare variables in the loop so the data is properly flushed
		    char data [8];
		    iov_t riov[1];

			switch (msg.msg_type)
			{
			case IOV_MSG_TYPE:

				// read the data
				SETIOV (&riov[1], data, msg.cksum_hdr.data_size);
				MsgReadv (rcvid, &riov[1], 1, sizeof(msg.cksum_hdr));

				if (strcmp (masterpwd, data) == 0)
				{
					connectPython(connfd, "Your PinPad has been successfully unlocked.");
				}

				else
				{

					connectPython(connfd, "Someone unsuccessfully tried to unlock your PinPad.");
				}

				MsgReply(rcvid, 0, 0, 0);
				break;


			case SET_PWD:


				// read the data
				SETIOV (&riov[1], data, msg.cksum_hdr.data_size);
				MsgReadv (rcvid, &riov[1], 1, sizeof(msg.cksum_hdr));

				strcpy(masterpwd, data);

				connectPython(connfd, "Your PinPad password has successfully been set.");

				//flush the data of any characters
				data[0] = '\0';

				MsgReply(rcvid, 0, 0, 0);
				break;
			}
		}

		else if (rcvid == 0)
		{
			if (msg.pulse.code == _PULSE_CODE_DISCONNECT)
			{
				connectPython(connfd, "Your PinPad has been disconnected from the network.");

				printf("No more QNX pinpad boxes detected. Alerted user, shutting down.\n");
				break;
			}
		}
		// close client sock
		close(connfd);
    }



    // close the socket when the program is done
    printf("Closing the socket and the server!\n");
    close(sockfd);

    // destroy channel and detach the name
	name_detach(attach, 0);
	ChannelDestroy(attach->chid);

	return 0;
}
