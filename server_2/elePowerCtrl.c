#include "elePowerCtrl.h"

void elePowerCtrl_handle(
	int msqid,
	int shmid_s,
	int shmid_d,
	device_t *devices,
	powsys_t *powsys
	)
{
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

	int i;
	int check_warn_threshold = 0;

	while (1)
	{
		// get total power using
		int sum_temp = 0;
		for (i = 0; i < MAX_DEVICE; i++)
			sum_temp += devices[i].use_power[devices[i].mode];
		powsys->current_power = sum_temp;

		// check threshold
		if (powsys->current_power >= POWER_THRESHOLD)
		{
			powsys->supply_over = 1;
			powsys->threshold_over = 1;
		}
		else if (powsys->current_power >= WARNING_THRESHOLD)
		{
			powsys->supply_over = 0;
			powsys->threshold_over = 1;
			powsys->reset = 0;
		}
		else
		{
			check_warn_threshold = 0;
			powsys->supply_over = 0;
			powsys->threshold_over = 0;
			powsys->reset = 0;
		}

		// WARN over threshold
		if (powsys->threshold_over && !check_warn_threshold)
		{
			check_warn_threshold = 1;

			// send message to logWrite
			msg_t new_msg;
			new_msg.mtype = 1;
			char temp[MAX_MESSAGE_LENGTH_TEMP];
			sprintf(temp, "WARNING!!! Over threshold, power comsuming: %dW", powsys->current_power);
			tprintf("%s\n", temp);
			sprintf(new_msg.mtext, "s|%s", temp);
			msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);
		}

		// overload
		if (powsys->supply_over)
		{
			// send message to logWrite
			msg_t new_msg;
			new_msg.mtype = 1;
			char temp[MAX_MESSAGE_LENGTH_TEMP];

			sprintf(temp, "DANGER!!! System overload, power comsuming: %dW", powsys->current_power);
			tprintf("%s\n", temp);
			sprintf(new_msg.mtext, "s|%s", temp);
			msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);

			tprintf("Server reset in 10 seconds\n");

			int no;
			for (no = 0; no < MAX_DEVICE; no++)
			{
				if (devices[no].mode == 1)
				{
					new_msg.mtype = 2;
					sprintf(new_msg.mtext, "m|%d|2|", devices[no].pid);
					msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);
				}
			}

			pid_t my_child;
			if ((my_child = fork()) == 0)
			{
				// in child
				sleep(1);

				int no;
				for (no = 0; no < MAX_DEVICE; no++)
				{
					if (devices[no].mode != 0)
					{
						new_msg.mtype = 2;
						sprintf(new_msg.mtext, "m|%d|0|", devices[no].pid);
						msgsnd(msqid, &new_msg, MAX_MESSAGE_LENGTH, 0);
					}
				}
				kill(getpid(), SIGKILL);
			}
			else
			{
				//in parent
				while (1)
				{
					sum_temp = 0;
					for (i = 0; i < MAX_DEVICE; i++)
						sum_temp += devices[i].use_power[devices[i].mode];
					powsys->current_power = sum_temp;

					if (powsys->current_power < POWER_THRESHOLD)
					{
						powsys->supply_over = 0;
						tprintf("OK, power now is %d", powsys->current_power);
						kill(my_child, SIGKILL);
						break;
					}
				}
			}
		}
	} // endwhile
} //end function elePowerCtrl_handle