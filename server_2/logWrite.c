#include "logWrite.h"

void logWrite_handle(device_t * devices, powsys_t *powsys, int shmid_d, int shmid_s, int msqid, FILE *log_server)
{
	// mtype == 1
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

	///////////////////////////
	// Create sever log file //
	///////////////////////////
	char file_name[255];
	time_t t = time(NULL);
	struct tm *now = localtime(&t);
	strftime(file_name, sizeof(file_name), "log/server_%Y-%m-%d_%H:%M:%S.txt", now);
	log_server = fopen(file_name, "w");
	tprintf("Log server started, file is %s\n", file_name);

	///////////////////////////////
	// Listen to other processes //
	///////////////////////////////
	while (1)
	{
		// got mail!
		if (msgrcv(msqid, &got_msg, MAX_MESSAGE_LENGTH, 1, 0) == -1)
		{
			tprintf("msgrcv() error");
			exit(1);
		}

		// header = 's' => Write log to server
		if (got_msg.mtext[0] == 's')
		{
			char buff[MAX_MESSAGE_LENGTH];
			//extract from message
			sscanf(got_msg.mtext, "%*2c%[^|]|", buff);
			// get time now
			char log_time[20];
			strftime(log_time, sizeof(log_time), "%Y/%m/%d_%H:%M:%S", now);
			// write log
			fprintf(log_server, "%s | %s\n", log_time, buff);
		}
	}
} //end function logWrite_handle