#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>


#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include <string.h>

#include "server.h"

#define ATTACH_POINT "my_name"

//resources:

//http://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/msgsend.html
//http://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/n/name_open.html
//http://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/n/name_close.html


int main(int argc, char **argv)
{
	//Declare variables needed
	char pwd [MAX_STRING_LEN] = "";

	cksum_header_t header;

	//establish a connection to the server's channel
	int server_coid = name_open(ATTACH_POINT, 0);

	printf("You can enter exit at anytime to leave the program.\n");

	while (strcmp(pwd, "exit") != 0)
	{
		// overwrite the message everytime the client sends a message
		if (strcmp(pwd, "") == 0)
		{
			printf("Initial Setup: Please set a PIN or Password.\n");
			scanf("%s", pwd);

			if (strcmp (pwd, "exit") == 0)
			{
				printf("Exiting. \n");
				exit(EXIT_SUCCESS);
			}

			//build the header
			header.msg_type = SET_PWD;
			header.data_size = strlen(pwd);
		}
		else
		{
			// get user input
			printf("Please enter your PIN/Password: \n");
			scanf("%s", pwd);

			if (strcmp (pwd, "exit") == 0)
			{
				printf("Exiting. \n");
				break;
			}

			header.msg_type = IOV_MSG_TYPE;
			header.data_size = strlen(pwd);
		}


		iov_t siov[2];

		// set and create iov_t
		SETIOV(&siov[0], &header, sizeof(header)); // set the header message type
		SETIOV(&siov[1], &pwd, sizeof(pwd));// set the message string in siov

		//send the message to the server and get a reply
		MsgSendvs(server_coid, siov,  2, 0, 0);
	}

    //send a pulse
	MsgSendPulse(server_coid , -1, 2, 100);

    //Close a server connection that was opened by name_open()
	name_close(server_coid);
	return 0;
}
