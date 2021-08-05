#include "powerSupply.h"


void powerSupply_handle(
	int msqid,
	int shmid_s,
	int shmid_d,
	int conn_sock,
	int bytes_received,
	powsys_t *powsys
	)
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
		bytes_received = recv(conn_sock, recv_data, BUFF_SIZE - 1, 0);
		if (bytes_received <= 0)
		{
			// if DISCONNECT
			// send message to powSupplyInfoAccess
			msg_t new_msg;
			new_msg.mtype = 2;
			sprintf(new_msg.mtext, "d|%d|", getpid()); // d for DISS
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

void powSupplyInfoAccess_handle(
	int msqid,
	int shmid_s,
	int shmid_d,
	char use_mode[][10],
	device_t *devices,
	powsys_t *powsys
	)
{
	// mtype = 2
	msg_t got_msg;

	//////////////////////////////
	// Connect to shared memory //
	//////////////////////////////
	if ((devices = (device_t *)shmat(shmid_d, (void *)0, 0)) == (void *)-1)
	{
		tprintf("shmat() failed\n");
		exit(1);
	}

	if ((powsys = (powsys_t *)shmat(shmid_s, (void *)0, 0)) == (void *)-1)
	{
		tprintf("shmat() failed\n");
		exit(1);
	}

	////////////////
	// check mail //
	////////////////
	while (1)
	{
		// got mail!
		if (msgrcv(msqid, &got_msg, MAX_MESSAGE_LENGTH, 2, 0) <= 0)
		{
			tprintf("msgrcv() error");
			exit(1);
		}
		// header = 'n' => Create new device
		if (got_msg.mtext[0] == 'n')
		{
			int no;
			for (no = 0; no < MAX_DEVICE; no++)
			{
				if (devices[no].pid == 0)
					break;
			}
			if (no < MAX_DEVICE) 
			{
				sscanf(got_msg.mtext, "%*c|%d|%[^|]|%d|%d|",
						&devices[no].pid,
						devices[no].name,
						&devices[no].use_power[1],
						&devices[no].use_power[2]);
				devices[no].mode = 0;
				tprintf("--- Connected equipment info ---\n");
				tprintf("       Equipment: %s\n", devices[no].name);
				tprintf("    Normal power: %dW\n", devices[no].use_power[1]);
				tprintf("     Limit power: %dW\n", devices[no].use_power[2]);
				tprintf("    Current mode: %s\n", use_mode[devices[no].mode]);
				tprintf("-----------------------------\n\n");
				tprintf("System power using: %dW\n", powsys->current_power);

				// send message to logWrite
				msg_t new_msg;
				new_msg.mtype = 1;
				sprintf(new_msg.mtext, "s|[%s] connected (Normal use: %dW, Linited use: %dW)|",
						devices[no].name,
						devices[no].use_power[1],
						devices[no].use_power[2]);
				msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);

				sprintf(new_msg.mtext, "s|Device [%s] set mode to [off] ~ using 0W|", devices[no].name);
				msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);
			}
		}

		// header = 'm' => Change the mode!
		else if (got_msg.mtext[0] == 'm')
		{
			int no, temp_pid, temp_mode;

			sscanf(got_msg.mtext, "%*c|%d|%d|", &temp_pid, &temp_mode);

			for (no = 0; no < MAX_DEVICE; no++)
			{
				if (devices[no].pid == temp_pid)
					break;
			}
			devices[no].mode = temp_mode;

			// send message to logWrite
			msg_t new_msg;
			new_msg.mtype = 1;
			char temp[MAX_MESSAGE_LENGTH_TEMP];

			tprintf("--- Working equipment info ---\n");
			tprintf("       Equipment: %s\n", devices[no].name);
			tprintf("    Normal power: %dW\n", devices[no].use_power[1]);
			tprintf("     Limit power: %dW\n", devices[no].use_power[2]);
			tprintf("    Current mode: %s\n", use_mode[devices[no].mode]);
			tprintf("     Using power: %dW\n",devices[no].use_power[devices[no].mode]);
			tprintf("-----------------------------\n\n");

			sleep(1);
			sprintf(temp, "======> System power using: %dW", powsys->current_power);
			tprintf("%s\n\n", temp);
			sprintf(new_msg.mtext, "s|%s", temp);
			msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);
		}

		// header = 'd' => Disconnect
		else if (got_msg.mtext[0] == 'd')
		{
			int no, temp_pid;
			sscanf(got_msg.mtext, "%*c|%d|", &temp_pid);

			// send message to logWrite
			msg_t new_msg;
			new_msg.mtype = 1;
			char temp[MAX_MESSAGE_LENGTH_TEMP];

			// printf("----%d--%d", no, devices[no].pid);
			// printf("Device %d and %d and  %s disconnected\n", temp_pid, no, devices[no].name);
			// sprintf(temp, "Device [%s] disconnected", devices[no].name);

			for (no = 0; no < MAX_DEVICE; no++)
			{
				if (devices[no].pid == temp_pid)
				{
					tprintf("Removing equipment at pid: %d\n\n", temp_pid);
					devices[no].pid = 0;
					strcpy(devices[no].name, "");
					devices[no].use_power[0] = 0;
					devices[no].use_power[1] = 0;
					devices[no].use_power[2] = 0;
					devices[no].mode = 0;
					break;
				}
			}
			if (no >= MAX_DEVICE)
			{
				tprintf("Error! Device not found\n\n");
			}
			sprintf(new_msg.mtext, "s|%s", temp);
			msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);

			sprintf(temp, "System power using: %dW", powsys->current_power);
			tprintf("%s\n", temp);
			sprintf(new_msg.mtext, "s|%s", temp);
			msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);
		}

	} // endwhile
} //end function powSupplyInfoAccess_handle