#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct aProcess{
	int pid;
	int arrival_time;
	int cpu_time;
	int running;
	int waiting;
	int run_time;
	int complete;
	int completion_time;
	int start_time;
	int rr_run_time;
	int priority;
	int ppa_wait_time;
};

int main(int argc, char *argv[]){

	if((argc != 3) && (argc != 5) && (argc != 7)){
		printf("USAGE: scheduler -a [FCFS | RR | SRTF | PPA] -q [optional quantum] -i [optional increaser]\n");
		exit(1);
	}

	char alg[20];
	char fcfs[10] = "FCFS";
	char rr[10] = "RR";
	char srtf[10] = "SRTF";
	char ppa[10] = "PPA";
	int quantum = -1;
	int increaser = -1;
	int target = 0;

	//Main Array to Hold Process Structures Created from user Input
	struct aProcess processes[25] = { -1 };

	//Getopt to Parse Input Using Option Flags
	int c;
	while((c = getopt(argc, argv, "a:q:i:")) != -1){
		switch(c){
			case 'a':
				strncpy(alg, optarg, strlen(optarg));
				break;
			case 'q':
				quantum = atoi(optarg);
				break;
			case 'i':
				increaser = atoi(optarg);
				break;
			case '?':
				printf("here");
				exit(1);
				break;
			default:
				break;
		}
	}

	//Parse User Input and Select Target Algorithm (Print Appropriate Output)
	if(strcmp(fcfs,alg) == 0){
		if((quantum != -1) && (increaser != -1)){
			printf("(Ignoring Quantum and Increaser)\n");
		}
		else if(quantum != -1){
			printf("(Ignoring Quantum)\n");
		}
		else if(increaser != -1){
			printf("(Ignoring Increaser)\n");
		}
		target = 1;
	}
	else if(strcmp(rr,alg) == 0){
		if(quantum < 1){
			printf("ERROR: Please provide a quantum value greater than or equal to 1 for RR scheduling..\n");
			exit(1);
		}
		if(increaser != -1){
			printf("(Ignoring Increaser)\n");
		}
		target = 2;
	}
	else if(strcmp(srtf,alg) == 0){
		if((quantum != -1) && (increaser != -1)){
			printf("(Ignoring Quantum and Increaser)\n");
		}
		else if(quantum != -1){
			printf("(Ignoring Quantum)\n");
		}
		else if(increaser != -1){
			printf("(Ignoring Increaser)\n");
		}
		target = 3;
	}
	else if(strcmp(ppa,alg) == 0){
		if(increaser < 1){
			printf("ERROR: Please provide an increaser value greater than or equal to 1 for PPA scheduling..\n");
			exit(1);
		}
		if(quantum != -1){
			printf("(Ignoring Quantum)\n");
		}
		target = 4;
	}
	else{
		printf("Selected Scheudling Algorithm Not Valid...\n");
		exit(1);
	}

	int inputLoop = 0;
	int processCount = 0;

	//Main Input Loop for Process Creation
	while(inputLoop != -1){
		char input[20];
		fgets(input,20,stdin);
		char * split = strtok(input," ");

		int inputCount = 0;
		int inputArray[3];
		while (split != NULL){
			inputArray[inputCount] = atoi(split);
			split = strtok (NULL, " ");
			inputCount += 1;
		}

		if((inputArray[0] == 0) && (inputArray[1] == 0) && (inputArray[2] == 0)){
			processCount = inputLoop;
			inputLoop = -1;
		}
		else{
			//Initializing All Values of Process Structure -> Represents a PCB (Holds Necessary Info. for All Scheduling Algorithms)
			struct aProcess newProcess;
			newProcess.pid = inputArray[0];
			newProcess.arrival_time = inputArray[1];
			newProcess.cpu_time = inputArray[2];
			newProcess.running = 0;
			newProcess.waiting = 0;
			newProcess.run_time = 0;
			newProcess.complete = 0;
			newProcess.start_time = 0;
			newProcess.completion_time = 0;
			newProcess.rr_run_time = 0;
			newProcess.priority = 0;
			newProcess.ppa_wait_time = 0;
			processes[inputLoop] = newProcess;
			inputLoop += 1;
		}
	}

	int tick = 0;

	//FCFS
	if(target == 1){

		int total_time = 0;
		int idle_time = 0;

		//Main Clock Loop
		while(tick != -1){

			int process_is_running = 0;

			int i;

			for(i = 0; i < processCount; i++){
				//Handle All New Process Arrivals
				if(processes[i].arrival_time == tick){
					processes[i].waiting = 1;
					printf("%i %i arriving\n", tick, processes[i].pid);
				}
				//Check for Running Processes
				if(processes[i].running == 1){
					printf("%i %i running\n", tick, processes[i].pid);

					process_is_running = 1;

					//Check for Process Completion
					if(processes[i].run_time < processes[i].cpu_time - 1){
						processes[i].run_time += 1;
					}
					else{
						processes[i].running = 0;
						processes[i].complete = 1;
						processes[i].completion_time = tick;
						printf("%i %i completing\n", tick, processes[i].pid);
					}
				}
			}
			//No Processes Currently Running
			if(process_is_running == 0){
				int found_waiting = -1;
				//Look For Waiting Process To Update to Be Running
				for(i = 0; i < processCount; i++){
					if(processes[i].waiting == 1){
						found_waiting = i;
						processes[i].waiting = 0;
						processes[i].running = 1;
						processes[i].run_time += 1;
						processes[i].start_time = tick;
						printf("%i %i running\n", tick, processes[i].pid);
						break;
					}
				}
				//CPU Idle Block
				if(found_waiting == -1){
					printf("%i CPU idle\n", tick);
					idle_time += 1;
				}
				//Signal Completion of Process
				else{
					if(processes[found_waiting].run_time >= processes[found_waiting].cpu_time){
						processes[found_waiting].running = 0;
						processes[found_waiting].complete = 1;
						processes[found_waiting].completion_time = tick;
						printf("%i %i completing\n", tick, processes[found_waiting].pid);
					}
				}
			}

			//Clock Updates
			int continue_clock = 0;
			for(i = 0; i < processCount; i++){
				if(processes[i].complete == 0){
					continue_clock = 1;
					break;
				}
			}
			if(continue_clock != 0){
				tick += 1;
			}
			//Break Loop (tick = -1)
			else{
				total_time = tick;
				tick = -1;
			}
		}

		//Calculate Information for Output
		int total_waiting = 0;
		int total_response = 0;
		int total_turnaround = 0;
		int i;
		for(i = 0; i < processCount; i++){
			int turnaround = processes[i].completion_time - processes[i].arrival_time;
			int waiting = turnaround - processes[i].cpu_time;
			int response = processes[i].start_time - processes[i].arrival_time;
			total_turnaround += turnaround;
			total_response += response;
			total_waiting += waiting;
		}
		printf("Average Waiting Time: %f\n", (float) total_waiting/processCount);
		printf("Average Response Time: %f\n", (float) total_response/processCount);
		printf("Average Turnaround Time: %f\n", (float) total_turnaround/processCount);
		printf("Average CPU Usage (Percentage): %f\n", ((float)(total_time - idle_time)/total_time)*100);
	}

	//RR
	if(target == 2){

		//Initialize Request Queue to Hold Indexes of Processes in Main Processes Array
		int requestQueue[25];

		//Initialize All Values in Request Queue to Be -1
		int j;
		for(j = 0; j < 25; j++){
			requestQueue[j] = -1;
		}

		int queueSize = 0;

		int total_time = 0;
		int idle_time = 0;

		//Main Clock Loop
		while(tick != -1){

			//Handle Process Arrival
			int i;
			for(i = 0; i < processCount; i++){
				if(processes[i].arrival_time == tick){
					//Add New Process to Request Queue
					printf("%i %i arriving\n", tick, processes[i].pid);
					requestQueue[queueSize] = i;
					processes[i].waiting = 1;
					processes[i].running = 0;
					queueSize += 1;
				}
			}

			//Check for a Running Process
			int process_is_running = 0;
			for(i = 0; i < processCount; i++){
				if(processes[i].running == 1){
					process_is_running = 1;
				}
			}

			//If There Is No Process Running Currently
			if(process_is_running == 0){
				//If Request Queue is Not Empty
				if(requestQueue[0] != -1){
					processes[requestQueue[0]].running = 1;
					processes[requestQueue[0]].waiting = 0;

					//Update Start Time of New Running Process if Necessary
					if(processes[requestQueue[0]].run_time == 0){
						processes[requestQueue[0]].start_time = tick;
					}

					//Shift Every Element of Queue to the Left (Pop)
					int i;
					for(i = 0; i < 24; i++){
						requestQueue[i] = requestQueue[i+1];
					}
					queueSize -= 1;
				}
				//IDLE
				else{
					idle_time += 1;
					printf("%i idling\n", tick);
				}
			}

			//Loop Through to All Processes to Handle the Running Process
			for(i = 0; i < processCount; i++){
				if(processes[i].running == 1){
					//If Running Process Has Not Yet Met Quantum Time and Isn't Finished
					if((processes[i].rr_run_time < quantum) && (processes[i].run_time < processes[i].cpu_time)){
						printf("%i %i running\n", tick, processes[i].pid);
						processes[i].run_time += 1;
						processes[i].rr_run_time += 1;
					}
					//Stop Process from Running
					else{
						processes[i].rr_run_time = 0;
						processes[i].running = 0;
						//If The Runnning Process Still Has Time Left
						if(processes[i].run_time < processes[i].cpu_time){
							requestQueue[queueSize] = i;
							processes[i].waiting = 1;
							queueSize += 1;

							processes[requestQueue[0]].running = 1;
							processes[requestQueue[0]].waiting = 0;
							printf("%i %i running\n", tick, processes[requestQueue[0]].pid);

							if(processes[requestQueue[0]].run_time == 0){
								processes[requestQueue[0]].start_time = tick;
							}

							processes[requestQueue[0]].run_time += 1;
							processes[requestQueue[0]].rr_run_time += 1;

							//Shift All Elements of Queue Over
							int i;
							for(i = 0; i < 24; i++){
								requestQueue[i] = requestQueue[i+1];
							}
							queueSize -= 1;
							break;

						}
						//If the Process that Was Running is Complete
						else{
							processes[i].complete = 1;
							processes[i].completion_time = tick;
							printf("%i %i completing\n", tick, processes[i].pid);

							processes[requestQueue[0]].running = 1;
							processes[requestQueue[0]].waiting = 0;
							int i;
							for(i = 0; i < 24; i++){
								requestQueue[i] = requestQueue[i+1];
							}
							queueSize -= 1;
						}
					}
				}
			}

			//Timing Section
			int continue_clock = 0;
			for(i = 0; i < processCount; i++){
				if(processes[i].complete == 0){
					continue_clock = 1;
					break;
				}
			}
			if(continue_clock != 0){
				tick += 1;
			}
			//Exit Condition
			else{
				total_time = tick;
				tick = -1;
			}
		}

		//Calculate Information for Output
		int total_waiting = 0;
		int total_response = 0;
		int total_turnaround = 0;
		int i;
		for(i = 0; i < processCount; i++){
			int turnaround = processes[i].completion_time - processes[i].arrival_time;
			int waiting = turnaround - processes[i].cpu_time;
			int response = processes[i].start_time - processes[i].arrival_time;
			total_turnaround += turnaround;
			total_response += response;
			total_waiting += waiting;
		}
		printf("Average Waiting Time: %f\n", (float) total_waiting/processCount);
		printf("Average Response Time: %f\n", (float) total_response/processCount);
		printf("Average Turnaround Time: %f\n", (float) total_turnaround/processCount);
		printf("Average CPU Usage (Percentage): %f\n", ((float)(total_time - idle_time)/total_time)*100);
	}

	//SRTF
	if(target == 3){

		int total_time = 0;
		int idle_time = 0;

		//Main Clock Loop
		while(tick != -1){

			int min_remaining_time = 10000;
			int target_min = -1;

			int i;
			//Handle Process Arrival and Adjust Minimum Remaing Time When Necessary
			for(i = 0; i < processCount; i++){
				if(processes[i].arrival_time == tick){
					processes[i].waiting = 1;
					printf("%i %i arriving\n", tick, processes[i].pid);
				}
				//Catch-All for Running/Waiting Processes
				if(processes[i].waiting == 1 || processes[i].running == 1){
					if(processes[i].cpu_time - processes[i].run_time < min_remaining_time){
						min_remaining_time = processes[i].cpu_time - processes[i].run_time;
						target_min = i;
					}
				}
			}

			//Handle All Currently Running Processes
			for(i = 0; i < processCount; i++){
				if(i == target_min){
					printf("%i %i running\n", tick, processes[i].pid);
					processes[i].running = 1;
					processes[i].waiting = 0;
					processes[i].start_time = tick;
					//Completion Check
					if(processes[i].run_time < processes[i].cpu_time - 1){
						processes[i].run_time += 1;
					}
					else{
						processes[i].running = 0;
						processes[i].complete = 1;
						processes[i].completion_time = tick;
						printf("%i %i completing\n", tick, processes[i].pid);
					}
				}
				else if(processes[i].running == 1){
					processes[i].running = 0;
					processes[i].waiting = 1;
				}
			}

			//Check for Running Processes and Update
			int process_is_running = -1;
			for(i = 0; i < processCount; i++){
				if(processes[i].running == 1){
					process_is_running = i;
				}
			}
			if(process_is_running == -1){
				idle_time += 1;
			}

			//Main Timing Check
			int continue_clock = 0;
			for(i = 0; i < processCount; i++){
				if(processes[i].complete == 0){
					continue_clock = 1;
					break;
				}
			}
			if(continue_clock != 0){
				tick += 1;
			}
			//Exit Condition
			else{
				total_time = tick;
				tick = -1;
			}
		}

		//Calculate Data for Output Values
		int total_waiting = 0;
		int total_response = 0;
		int total_turnaround = 0;
		int i;
		for(i = 0; i < processCount; i++){
			int turnaround = processes[i].completion_time - processes[i].arrival_time;
			int waiting = turnaround - processes[i].cpu_time;
			int response = processes[i].start_time - processes[i].arrival_time;
			total_turnaround += turnaround;
			total_response += response;
			total_waiting += waiting;
		}
		printf("Average Waiting Time: %f\n", (float) total_waiting/processCount);
		printf("Average Response Time: %f\n", (float) total_response/processCount);
		printf("Average Turnaround Time: %f\n", (float) total_turnaround/processCount);
		printf("Average CPU Usage (Percentage): %f\n", ((float)(total_time - idle_time)/total_time)*100);
	}

	//PPA
	if(target == 4){

		int total_time = 0;
		int idle_time = 0;

		//Main Clock Loop
		while(tick != -1){

			int i;
			//Handle All Arriving Processes
			for(i = 0; i < processCount; i++){
				if(processes[i].arrival_time == tick){
					printf("%i %i arriving\n", tick, processes[i].pid);
					processes[i].waiting = 1;
				}
			}

			//Update the ppa_wait_time for each waiting process (if reaches 5 then increase priority)
			for(i = 0; i < processCount; i++){
				if(processes[i].waiting == 1 && processes[i].running == 0){
					if(processes[i].ppa_wait_time == 5){
						processes[i].priority += increaser;
						processes[i].ppa_wait_time = 0;
					}
					else{
						processes[i].ppa_wait_time += 1;
					}
				}
			}
			
			//Check for a running process
			int process_is_running = 0;
			for(i = 0; i < processCount; i++){
				if(processes[i].running == 1){
					process_is_running = 1;
				}
			}

			//If No Processes Are Currently Running
			if(process_is_running == 0){
				int maxPriority = -1;
				for(i = 0; i < processCount; i++){
					if(processes[i].waiting == 1){
						if(processes[i].priority > maxPriority){
							maxPriority = i;
						}
					}
				}
				//Update Idle Time if maxPriority is still -1
				if(maxPriority == -1){
					idle_time += 1;
					printf("%i idling\n", tick);
				}
				else{
					processes[maxPriority].running = 1;
					processes[maxPriority].waiting = 0;
				}
			}

			//Handle Running Process
			for(i = 0; i < processCount; i++){
				if(processes[i].running == 1){
					//If Running Process Has Yet to Complete
					if(processes[i].run_time < processes[i].cpu_time){
						int maxPriority = processes[i].priority;
						int maxPriorityIndex = i;
						int j;
						for(j = 0; j < processCount; j++){
							if(processes[j].waiting == 1){
								if(processes[j].priority > maxPriority){
									maxPriority = processes[j].priority;
									maxPriorityIndex = j;
								}
							}
						}
						//Preemption of Running Process
						if(processes[i].pid != processes[maxPriorityIndex].pid){
							processes[i].running = 0;
							processes[i].waiting = 1;
							processes[maxPriorityIndex].running = 1;
							processes[maxPriorityIndex].waiting = 0;
							printf("%i %i running\n", tick, processes[maxPriorityIndex].pid);
							if(processes[maxPriorityIndex].run_time == 0){
								processes[maxPriorityIndex].start_time = tick;
							}
							processes[maxPriorityIndex].run_time += 1;
							break;
						}
						//No Preemption Is Required
						else{
							printf("%i %i running\n", tick, processes[i].pid);
							if(processes[i].run_time == 0){
								processes[i].start_time = tick;
							}
							processes[i].run_time += 1;
							break;
						}
					}
					//Completed Process
					else{
						processes[i].running = 0;
						processes[i].waiting = 0;
						processes[i].complete = 1;
						processes[i].completion_time = tick;
						printf("%i %i completing\n", tick, processes[i].pid);

						//Check for Max Priority of Leftover Waiting Processes
						int maxPriority = -1;
						int maxPriorityIndex = -1;
						int j;
						for(j = 0; j < processCount; j++){
							if(processes[j].waiting == 1){
								if(processes[j].priority > maxPriority){
									maxPriority = processes[j].priority;
									maxPriorityIndex = j;
								}
							}
						}
						//Found One
						if(maxPriorityIndex != -1){
							processes[maxPriorityIndex].running = 1;
							processes[maxPriorityIndex].waiting = 0;
							printf("%i %i running\n", tick, processes[maxPriorityIndex].pid);
							if(processes[maxPriorityIndex].run_time == 0){
								processes[maxPriorityIndex].start_time = tick;
							}
							processes[maxPriorityIndex].run_time += 1;
							break;
						}
						//Didn't Find One in Waiting "Queue"
						else{
							idle_time += 1;
							printf("%i idling\n", tick);
							break;
						}
					}
				}
			}

			//Timing Information
			int continue_clock = 0;
			for(i = 0; i < processCount; i++){
				if(processes[i].complete == 0){
					continue_clock = 1;
					break;
				}
			}
			if(continue_clock != 0){
				tick += 1;
			}
			//Exit Condition
			else{
				total_time = tick;
				tick = -1;
			}
		}

		//Calculate and Output Results
		int total_waiting = 0;
		int total_response = 0;
		int total_turnaround = 0;
		int i;
		for(i = 0; i < processCount; i++){
			int turnaround = processes[i].completion_time - processes[i].arrival_time;
			int waiting = turnaround - processes[i].cpu_time;
			int response = processes[i].start_time - processes[i].arrival_time;
			total_turnaround += turnaround;
			total_response += response;
			total_waiting += waiting;
		}
		printf("Average Waiting Time: %f\n", (float) total_waiting/processCount);
		printf("Average Response Time: %f\n", (float) total_response/processCount);
		printf("Average Turnaround Time: %f\n", (float) total_turnaround/processCount);
		printf("Average CPU Usage (Percentage): %f\n", ((float)(total_time - idle_time)/total_time)*100);

	}

	return 0;
}