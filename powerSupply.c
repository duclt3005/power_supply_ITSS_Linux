#include "powerSupply.h"


void powerSupply_handle(int conn_sock, powsys_t *powsys, int shmid_s, int msqid)
{
	// check if this is first time client sent
	int is_first_message = 1;
	char recv_data[BUFF_SIZE];
	//////////////////////////////
	// Connect to shared memory //
	//////////////////////////////
	if ((powsys = (powsys_t *)shmat(shmid_s, (void *)0, 0)) == (void *)-1)
	{
		tprintf("shmat() failed\n");
		exit(1);
	}

	while (1)
	{
		///////////////////
		// listen on tcp //
		///////////////////
		int bytes_received = recv(conn_sock, recv_data, BUFF_SIZE - 1, 0);
		if (bytes_received <= 0)
		{
			// if DISCONNECT
			// send message to powSupplyInfoAccess
			msg_t new_msg;
			new_msg.mtype = 2;
			sprintf(new_msg.mtext, "d|%d|", getpid()); // d for DIS
			msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);

			powsys->powerSupply_count--;
			// kill this process
			kill(getpid(), SIGKILL);
			break;
		}
		else
		{
			// if receive message from client
			recv_data[bytes_received] = '\0';
			printf("%s", recv_data);
			if (is_first_message)
			{
				is_first_message = 0;
				// send device info to powSupplyInfoAccess
				msg_t new_msg;
				new_msg.mtype = 2;
				sprintf(new_msg.mtext, "n|%d|%s|", getpid(), recv_data); // n for NEW
				msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);
			}
			else
			{
				// if not first time client send
				// send mode to powSupplyInfoAccess
				msg_t new_msg;
				new_msg.mtype = 2;
				sprintf(new_msg.mtext, "m|%d|%s|", getpid(), recv_data); // m for MODE
				msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);
			}
		}
	} // endwhile
} // end function powerSupply_handle
