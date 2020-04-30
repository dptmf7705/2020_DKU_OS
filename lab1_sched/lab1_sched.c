/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 
*	    Student name : 
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm function'definition.
*
*/

#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab1_sched_types.h"

#define BOARD_HEIGHT 44
#define TEXT_LEFT_ALIGN 15
#define TEXT_TOP_ALIGN 9
#define LINE_SPACE 3
#define SELECTION_BOX_LEFT_ALIGN 12
#define SELECTION_BOX_WIDTH 26
#define TABLE_LEFT_ALIGN 50
#define TABLE_TOP_ALIGN 9
#define TABLE_WIDTH 13
#define TABLE_HEIGHT 2
#define TABLE_MARGIN 6
#define METRICS_LEFT_ALIGN 105

#define EXIT 6
#define BACK_TO_MAIN 9

#define ENTER 10
#define ARROW 27
#define ARROW2 91
#define UP 65
#define DOWN 66

#define MAX_PROCESS 6
#define MAX_TIME 20

#define INITIAL_VALUE -1

const char SCHEDULING_NAME[8][20] = {
	"FCFS(FIFO)",
	"RR(Round-Robin)",
	"SPN(SJF)",
	"HRRN",
	"MLFQ",
	"MLFQ (tquantum=2^i)",
	"RM(Rate Monotonic)",
	"STRIDE"
};

int num_of_proc;
int ready_q_cnt;

workload_type w_type = WORKLOAD_DEFAULT;

process *proc_arr;

queue *ready_q;
queue *result_q;

process *run_proc;

void printBoard(){
	gotoxy(0, 0);
	setColor(RESET);
	puts("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	for(int i = 0 ; i < BOARD_HEIGHT ; i++){
		puts("┃                                                                                                                                                                                           ┃");
	}
	puts("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
	
	gotoxy(TEXT_LEFT_ALIGN + 30, LINE_SPACE);
        printf("SCHEDULING SIMULATOR by YESEUL LEE");
      	printf("  (2020-1 OPERATING SYSTEM LAB1)");	
}

void printProcMenu(){
	gotoxy(TEXT_LEFT_ALIGN - 2, TEXT_TOP_ALIGN);
	setColor(BLU);
	printf("Choose the number of process");
	setColor(RESET);

	for(int i = 2 ; i <= MAX_PROCESS ; i++){
		gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * (i - 1));
		printf("%d", i);
	}

	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 6);
	setColor(RED);
	printf("EXIT");
	setColor(RESET);
}

void printWorkloadTable(){
	int posX = TABLE_LEFT_ALIGN;
	int posY = getTablePosY() - 1;

	gotoxy(posX, posY);
	puts(" ◆  Workload  ");

	switch(w_type){
		case WORKLOAD_DEFAULT:
			gotoxy(posX, ++posY);
			puts("┌────────────┬────────────┬────────────┐            ");
			gotoxy(posX, ++posY);
			puts("│  Process   │  Arrival   │  Service   │                ");
			break;
		case WORKLOAD_PERIOD:
			gotoxy(posX, ++posY);
			puts("┌────────────┬────────────┬────────────┬───────────┐");
			gotoxy(posX, ++posY);
			puts("│  Process   │  Arrival   │  Service   │  Period   │");
			break;
		case WORKLOAD_TICKET:
			gotoxy(posX, ++posY);
			puts("┌────────────┬────────────┬────────────┬───────────┐");
			gotoxy(posX, ++posY);
			puts("│  Process   │  Arrival   │  Service   │  Tickets  │");
			break;
	}

	for(int i = 0 ; i < num_of_proc ; i++){
		if(w_type == WORKLOAD_DEFAULT){
			gotoxy(posX, ++posY);
			puts("├────────────┼────────────┼────────────┤            ");
			gotoxy(posX, ++posY);
			puts("│            │            │            │            ");
		} else {
			gotoxy(posX, ++posY);
			puts("├────────────┼────────────┼────────────┼───────────┤");
			gotoxy(posX, ++posY);
			puts("│            │            │            │           │");
		}

		posX += TABLE_MARGIN;

		gotoxy(posX, posY);
		printf("%c", proc_arr[i].name);

		if(proc_arr[i].arrival != INITIAL_VALUE && proc_arr[i].service != INITIAL_VALUE){
	 		gotoxy(posX + TABLE_WIDTH, posY);
			printf("%d", proc_arr[i].arrival);
	 		gotoxy(posX + TABLE_WIDTH * 2, posY);
			printf("%d", proc_arr[i].service);
		}

		gotoxy(posX + TABLE_WIDTH * 3, posY);
		if(w_type == WORKLOAD_PERIOD && proc_arr[i].period != INITIAL_VALUE)
			printf("%d", proc_arr[i].period);
		else if(w_type == WORKLOAD_TICKET && proc_arr[i].tickets != INITIAL_VALUE)
			printf("%d", proc_arr[i].tickets);

		posX -=TABLE_MARGIN;
	}

	gotoxy(posX, ++posY);
	if(w_type == WORKLOAD_DEFAULT)
		puts("└────────────┴────────────┴────────────┘            ");
	else 
		puts("└────────────┴────────────┴────────────┴───────────┘");

}

void printSchedMenu(){
	gotoxy(TEXT_LEFT_ALIGN -2, TEXT_TOP_ALIGN);
	setColor(BLU);
	printf("Choose Scheduling Algorithm");
	setColor(RESET);

	int i = 0;
	while(i < BACK_TO_MAIN - 1){
		gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * (i + 1));
		printf("%s", SCHEDULING_NAME[i++]);
	}

	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * ++i);
	setColor(RED);
	printf("BACK TO MAIN");
	setColor(RESET);
}

int getTablePosY(){
	return TABLE_TOP_ALIGN + (num_of_proc + 2) * TABLE_HEIGHT;
}

void printSchedTable(){
	int posX = TABLE_LEFT_ALIGN;
	int posY = TABLE_TOP_ALIGN - 1;

	gotoxy(posX, posY);
	puts("0                             5                             10                            15                            20");

	for(int i = 0 ; i < num_of_proc ; i++){
		gotoxy(posX, ++posY);
		puts("├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤");
		gotoxy(posX, ++posY);
		puts("│                             │                             │                             │                             │");	
		gotoxy(posX - 2, posY);
		setColor(proc_arr[i].color);
		printf("%c", proc_arr[i].name);
		setColor(RESET);
	}
	
	gotoxy(posX, ++posY);
	puts("├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤");
}


/*
 * Selection Box Position
 */
int pos[2] = {0, 0};

void findSelectionBoxPos(int index){
	pos[0] = SELECTION_BOX_LEFT_ALIGN;
	pos[1] = TEXT_TOP_ALIGN - 1 + (index * LINE_SPACE);
}

void printSelectionBox(int index){
	findSelectionBoxPos(index);

	gotoxy(pos[0], pos[1]);
	puts("┌─────────────────────────┐");
	gotoxy(pos[0], pos[1] + 1);
	puts("│");
	gotoxy(pos[0] + SELECTION_BOX_WIDTH, pos[1] + 1);	
	puts("│");
	gotoxy(pos[0], pos[1] + 2);
	puts("└─────────────────────────┘");
}

void eraseSelectionBox(int index){
	findSelectionBoxPos(index);

	gotoxy(pos[0], pos[1]);
	puts("                            ");
	gotoxy(pos[0], pos[1] + 1);
	puts(" ");
	gotoxy(pos[0] + SELECTION_BOX_WIDTH, pos[1] + 1);	
	puts(" ");
	gotoxy(pos[0], pos[1] + 2);
	puts("                            ");
}

queue* newQueue(){
	queue *q = malloc(sizeof(queue));	
	node *head = malloc(sizeof(node));	
	node *tail = malloc(sizeof(node));	

	q->head = head;
	q->tail = tail;
	q->count = 0;

	q->head->next = q->tail;
	q->head->before = NULL;

	q->tail->before = q->head;
	q->tail->next = NULL;

	return q;
}

bool isEmptyQueue(queue *q){
	return !(q && q->count > 0);
}

void insertQueue(queue *q, void *data){
	if(q == NULL)
		q = newQueue();

	node *newNode = malloc(sizeof(node));

	newNode->data = data;

	newNode->before = q->tail->before;
	q->tail->before = newNode;
	newNode->before->next = newNode;
	newNode->next = q->tail;

	q->count += 1;
}

void deleteQueue(queue *q, void **data){
	if(isEmptyQueue(q)) 
		return;

	node *target = q->head->next;

	// save data to delete
	*data = target->data;	

	q->head->next = target->next;
	target->next->before = q->head;
	free(target);

	q->count -= 1;
}

void deleteQueueNode(queue *q, node *node, void **data){
	if(isEmptyQueue(q))
		return;

	// save data to delete
	*data = node->data;

	node->before->next = node->next;
	node->next->before = node->before;
	free(node);
	
	q->count -= 1;
}

void freeQueue(queue *q){
	if(q == NULL)
		return;

	node *curr = q->head;
	node *temp;

	while(curr != NULL){
		temp = curr->next;
		free(curr);
		curr = temp;
	}
}

void waitSelectionMenu(int *menuNum, int bound){
        int key;

	while(1){
		key = getch();

		/* Enter key pressed */
		if(key == ENTER){ 
			return;
		}
		
		/* Arrow key pressed */
		if(key == ARROW && (key = getch()) == ARROW2){ 
			key = getch();

			if(key == UP && *menuNum > 1){
				eraseSelectionBox(*menuNum);
				(*menuNum)--;
				printSelectionBox(*menuNum);
			} else if(key == DOWN && *menuNum < bound){
				eraseSelectionBox(*menuNum);
				(*menuNum)++;
				printSelectionBox(*menuNum);
			}
		}
	}
}

void init(){
	w_type = WORKLOAD_DEFAULT;
	
	system("clear");
	setCursorVisibility(false);
	printBoard();
	printProcMenu();

	int menuNum = 1;
	printSelectionBox(menuNum);
	waitSelectionMenu(&menuNum, EXIT);

	if(menuNum == EXIT)
		return;

	num_of_proc = menuNum + 1;
	createProcArr();	
}

void initSchedMenu(){
	system("clear");
	printBoard();
	printSchedMenu();
	printSchedTable();
	printWorkloadTable();

	int menuNum = 1;
	printSelectionBox(menuNum);

	while(1){
		waitSelectionMenu(&menuNum, BACK_TO_MAIN);

		if(menuNum == BACK_TO_MAIN){
			init();
			return;
		}

		runScheduling(menuNum);
	}
}

void resetProc(process *proc){
	proc->start = INITIAL_VALUE;
	proc->remain = proc->service;
	proc->finish = INITIAL_VALUE;
}

void initProcess(process *proc, char n, int c){
	proc->name = n;
	proc->color = c;
	proc->arrival = INITIAL_VALUE;
	proc->service = INITIAL_VALUE;
	proc->period = INITIAL_VALUE;
	proc->tickets = INITIAL_VALUE;
	proc->start = INITIAL_VALUE;
	proc->remain = INITIAL_VALUE;
	proc->finish = INITIAL_VALUE;
}

void createProcArr(){	
	proc_arr = malloc(sizeof(process) * num_of_proc);	

	for(int i = 0 ; i < num_of_proc ; i++){
		initProcess(proc_arr + i, 'A' + i, RED + (i % NUM_OF_COLORS));
	}

	int posX = TABLE_LEFT_ALIGN + TABLE_MARGIN + TABLE_WIDTH;
	int posY = getTablePosY() + TABLE_HEIGHT * 2 - 1;

	printSchedTable();
	printWorkloadTable();

	setCursorVisibility(true);
	for(int i = 0 ; i < num_of_proc ; i++){
	 	gotoxy(posX, posY + (i * TABLE_HEIGHT));
		scanf("%d", &proc_arr[i].arrival);
	 	gotoxy(posX + TABLE_WIDTH, posY + (i * TABLE_HEIGHT));
		scanf("%d", &proc_arr[i].service);
		proc_arr[i].remain = proc_arr[i].service;
	}
	getchar();
	setCursorVisibility(false);
	
	initSchedMenu();

	free(proc_arr);
}

void inputWorkload(){
	if(w_type == WORKLOAD_PERIOD && proc_arr && proc_arr[0].period != INITIAL_VALUE)
		return;

	if(w_type == WORKLOAD_TICKET && proc_arr && proc_arr[0].tickets != INITIAL_VALUE)
		return;

	int posX = TABLE_LEFT_ALIGN + TABLE_WIDTH * 3 + TABLE_MARGIN;
	int posY = getTablePosY() + 3;

	setCursorVisibility(true);
	for(int i = 0 ; i < num_of_proc ; i++){
	 	gotoxy(posX, posY + i * TABLE_HEIGHT);
		scanf("%d", w_type == WORKLOAD_PERIOD ? &proc_arr[i].period : &proc_arr[i].tickets);
	}
	getch();
	setCursorVisibility(false);
}

void printResultQueue(){
	int posX = TABLE_LEFT_ALIGN;
	int posY = TABLE_TOP_ALIGN + 1;
	const int LEFT_SPACE = 6;

	printSchedTable();

	sched_process *proc = malloc(sizeof(sched_process));

	int now = 0;
	while(now < MAX_TIME && !isEmptyQueue(result_q)){
		deleteQueue(result_q, (void**) &proc); 
		
		now = proc->start;
		int index = proc->name - (int) 'A';

		while(now < MAX_TIME && now < proc->start + proc->running){		
			gotoxy(posX + (now * LEFT_SPACE), posY + (TABLE_HEIGHT * index));
			printf("│");
			setColor(proc->color);
			printf("  %c  ", proc->name);
			setColor(RESET);
			printf("│");
		
			now++;
		}
	}

	free(proc);
}

void printResultMetrics(){
	int posX = METRICS_LEFT_ALIGN;
	int posY = getTablePosY() - 1;

	gotoxy(posX, posY);
	puts(" ◆  Result Metrics  ");

	gotoxy(posX, ++posY);
	puts("┌────────────┬────────────┬────────────┬────────────┬────────────┐");
	gotoxy(posX, ++posY);
	puts("│  Process   │  Firstrun  │ Completion │ Turnaround │  Response  │");

	int totalT = 0, totalR = 0;

	for(int i = 0 ; i < num_of_proc ; i++){
		gotoxy(posX, ++posY);
		puts("├────────────┼────────────┼────────────┼────────────┼────────────┤");
		gotoxy(posX, ++posY);
		puts("│            │            │            │            │            │");

		int arrival = proc_arr[i].arrival;
		int start = proc_arr[i].start;
		int finish = proc_arr[i].finish;

		if(start == INITIAL_VALUE || finish == INITIAL_VALUE)
			continue;

		int turnaround = finish - arrival;
		int response = start - arrival;
		totalT += turnaround;
		totalR += response;

		posX += TABLE_MARGIN;

		setColor(RED + (i % NUM_OF_COLORS));
		gotoxy(posX, posY);
		printf("%c", proc_arr[i].name);
		gotoxy(posX + TABLE_WIDTH, posY);
		printf("%d", start);
	 	gotoxy(posX + TABLE_WIDTH * 2, posY);
		printf("%d", finish);
	 	gotoxy(posX + TABLE_WIDTH * 3, posY);
		printf("%d", turnaround);
	 	gotoxy(posX + TABLE_WIDTH * 4, posY);
		printf("%d", response);
		setColor(RESET);

		posX -= TABLE_MARGIN;
	}

	gotoxy(posX, ++posY); posY++;
	puts("└────────────┴────────────┴────────────┴────────────┴────────────┘");
	
	gotoxy(posX, ++posY); posY++;
	printf(" ◆  Average Turnaround Time = %0.3f\n", totalT / (float) num_of_proc);
	gotoxy(posX, ++posY);
	printf(" ◆  Average Response Time   = %0.3f\n", totalR / (float) num_of_proc);
}

void printSchedulingTitle(int index){
	int posX = TABLE_LEFT_ALIGN;
	int posY = TABLE_TOP_ALIGN - 3;
	
	gotoxy(posX, posY);
	printf(" ◆  %-40s", SCHEDULING_NAME[index - 1]);
}

int inputTimeQuantum(){
	int posX = TABLE_LEFT_ALIGN + 20;
	int posY = TABLE_TOP_ALIGN - 3;
	
	int tquantum;

	gotoxy(posX, posY);
	setCursorVisibility(true);
	setColor(BLU);
	printf("time quantum = ");
	scanf("%d", &tquantum);
	getch();
	setColor(RESET);
	setCursorVisibility(false);

	return tquantum;
}

void runScheduling(int index){
	system("clear");
	printBoard();
	printSchedMenu();
	printSchedTable();
	printSelectionBox(index);
	printSchedulingTitle(index);

	// init ready queue
	ready_q_cnt = 1;
	ready_q = newQueue();	
	// init result queue
	result_q = newQueue();	
	// init running process
	run_proc = malloc(sizeof(process));
	
	w_type = WORKLOAD_DEFAULT;
	printWorkloadTable();

	for(int i = 0 ; i < num_of_proc ; i++){
		resetProc(proc_arr + i);
	}
	
	switch(index){
		case 1:
			FCFS();
			printResultMetrics();
			break;
		case 2:	
			RR(inputTimeQuantum());
			printResultMetrics();
			break;
		case 3:
			SJF();
			printResultMetrics();
			break;
		case 4:
			HRRN();
			printResultMetrics();
			break;
		case 5:
			MLFQ(FEEDBACK_DEFAULT, inputTimeQuantum());
			printResultMetrics();
			break;
		case 6:
			MLFQ(FEEDBACK_MULTIPLE, 2);
			printResultMetrics();
			break;
		case 7:
			w_type = WORKLOAD_PERIOD;
			printWorkloadTable();
			inputWorkload();
			RM();
			break;
		case 8:
			w_type = WORKLOAD_TICKET;
			printWorkloadTable();
			inputWorkload();
			STRIDE();
			printResultMetrics();
			break;
	}

	printResultQueue();

	// free ready queue
	for(int i = 0 ; i < ready_q_cnt ; i++){
		freeQueue(ready_q + i);
	}	
	free(ready_q);

	// free result queue
	freeQueue(result_q);
	free(result_q);
}

void updateReadyQueue(int now){
	// insert new process first
	for(int i = 0 ; i < num_of_proc ; i++){
		if(proc_arr[i].arrival == now)
			insertQueue(ready_q, proc_arr + i);
	}
}

bool isProcAllFinish(){
	for(int i = 0 ; i < num_of_proc ; i++){
		if(proc_arr[i].finish == INITIAL_VALUE)
			return false;
	}

	return true;
}

int waitForProcArrival(int *now){
	// finish scheduling
	if(isProcAllFinish()) 
		return -1;

	while(isEmptyQueue(ready_q)){
		updateReadyQueue(++(*now));
	}

	return 0;
}

int waitForProcPeriod(int *now){
	while(isEmptyQueue(ready_q)){
		// scheduling finished
		if(*now == MAX_TIME) 
			return -1;

		updatePeriodReadyQueue(++(*now));
	}
	return 0;
}

void updatePeriodReadyQueue(const int now){
	for(int i = 0 ; i < num_of_proc ; i++){
		int parr = 0;
		int cnt = 0;
		int arr = proc_arr[i].arrival;
		int p = proc_arr[i].period;
		while(parr <= now){
			parr = arr + p * cnt++; 
			if(now == parr){
				resetProc(proc_arr + i);
				insertQueue(ready_q, proc_arr + i);
			}
		}
	}
}

sched_process* newSchedProc(process *source, int start, int running){
	sched_process *proc = malloc(sizeof(sched_process));
	
	proc->name = source->name;
	proc->color = source->color;
	proc->start = start;
	proc->running = running;

	return proc;
}

void schedule(process *proc, int *now, const int t_while){
	const int start = *now;	

	/* process first run */
	if(proc->start == INITIAL_VALUE)
		proc->start = start;
	
	int running = 0;
	while(running < t_while){
		proc->remain -= 1;

		if(w_type == WORKLOAD_PERIOD)
			updatePeriodReadyQueue(++(*now));
		else
			updateReadyQueue(++(*now));

		running++;

		/* process finish */
		if(proc->remain == 0){
			proc->finish = *now;
			break;
		}
	}

	insertQueue(result_q, newSchedProc(proc, start, running));
}

/*
 * find the shortest process node in ready queue
 */
node* getShortestJobNode(){
	if(isEmptyQueue(ready_q))
		return NULL;

	node *curr = ready_q->head->next;
	process *currPros = (process *) curr->data;

	node *shortest = curr;
	while(curr != NULL && curr != ready_q->tail){
		if(currPros->remain < ((process *) shortest->data)->remain)
			shortest = curr;
		curr = curr->next;
		currPros = (process *) curr->data;
	}

	return shortest;
}

/*
 * calculate response ratio
 * 1 + ( Waiting Time / Estimated Run Time )
 */
float getResponseRatio(int now, process *proc){
	return 1 + (float) (now - proc->arrival) / (float) proc->service;
}

/*
 * find the highest response ratio process node in ready queue
 */
node* getHighestRRNode(int now){
	if(isEmptyQueue(ready_q))
		return NULL;

	node *curr = ready_q->head->next;
	process *currPros = (process *) curr->data;

	node *highest = curr;
	float maxRatio = getResponseRatio(now, currPros);
	while(curr != NULL && curr != ready_q->tail){
		float currRatio = getResponseRatio(now, currPros);
		if(maxRatio < currRatio){
			highest = curr;
			maxRatio = currRatio;
		}
		curr = curr->next;
		currPros = (process *) curr->data;	
	}

	return highest;
}

void FCFS(){
	int now = -1;

	while(1){
		// wait for new process arrival if ready queue empty 
		if(isEmptyQueue(ready_q) && 
		   waitForProcArrival(&now) == -1)
			return;

		// get first process from ready queue 
		deleteQueue(ready_q, (void **) &run_proc);

		// run until the process finish
		schedule(run_proc, &now, run_proc->service);
	}
}

void RR(const int t_quantum){
	int now = -1;

	while(1){
		// wait for new process arrival if ready queue empty 
		if(isEmptyQueue(ready_q) && 
		   waitForProcArrival(&now) == -1)
			return;

		// get first process from ready queue 
		deleteQueue(ready_q, (void **) &run_proc);

		// run process during time quantum		
		do
			schedule(run_proc, &now, t_quantum);
		while(run_proc->remain && 
			isEmptyQueue(ready_q)); // repeat if ready queue is empty

		/* insert the process into ready queue if service time remains */
		if(run_proc->remain)
			insertQueue(ready_q, run_proc);
	}
}

void SJF(){
	int now = -1;

	while(1){
		// wait for new process arrival if ready queue empty 
		if(isEmptyQueue(ready_q) && 
		   waitForProcArrival(&now) == -1)
			return;

		// find the shortest process and delete it from queue
		node *target = getShortestJobNode();
		deleteQueueNode(ready_q, target, (void **) &run_proc);

		// run until the process finish
		schedule(run_proc, &now, run_proc->service);
	}
}

void HRRN(){
	int now = -1;

	while(1){
		// wait for new process arrival if ready queue empty 
		if(isEmptyQueue(ready_q) && 
		   waitForProcArrival(&now) == -1)
			return;

		// find the shortest process and delete it from queue
		node *target = getHighestRRNode(now);
		deleteQueueNode(ready_q, target, (void **) &run_proc);

		// run until the process finish
		schedule(run_proc, &now, run_proc->service);
	}
}

void increaseReadyQueue(){
	ready_q = realloc(ready_q, sizeof(queue) * ++ready_q_cnt);
	ready_q[ready_q_cnt - 1] = *newQueue();
}

/*
 * find the level of queue not empty (exist process to run)
 */
int findNotEmptyQueueLevel(int *now){
	// all process finished
	if(isProcAllFinish())
		return -1;

	while(1){
		int level = 0;
		while(level < ready_q_cnt){
			/* queue is not empty */
			if(!isEmptyQueue(&ready_q[level]))
				return level;
			level++;
		}

		updateReadyQueue(++(*now));
	}
}

void MLFQ(const feedback_type type, const int t_quantum){
	int now = -1;
	int qlevel;

	while(1){
		// find the process to run from multi queue
		if((qlevel = findNotEmptyQueueLevel(&now)) == -1)
			break; // finish scheduling

		deleteQueue(&ready_q[qlevel], (void **) &run_proc);

		bool repeat;
		do{
			repeat = true;

			int t_while = (type == FEEDBACK_DEFAULT) ? 
					t_quantum : 
					POW(t_quantum, qlevel);

			// run process during time quantum 
			schedule(run_proc, &now, t_while);

			// check is there any process to run next 
			for(int i = 0 ; i < ready_q_cnt ; i++){
				if(!isEmptyQueue(ready_q + i)){
					repeat = false;
					break;
				}
			}
		} while(run_proc->remain && repeat); // repeat if queue empty

		/* insert the process if service time remains */
		if(run_proc->remain){
			/* check if we need to increase the num of queue */
			if(qlevel == ready_q_cnt - 1)
				increaseReadyQueue();
			// insert process into the next level of queue
			insertQueue(&ready_q[qlevel + 1], run_proc);
		}
	}
}

node* getLeastPeriodNode(){
	if(isEmptyQueue(ready_q))
		return NULL;

	node *curr = ready_q->head->next;
	process *currProc = (process *) curr->data;

	node *target = curr;
	int min = currProc->period;
	while(curr && curr != ready_q->tail){
		if(currProc->period < min){
			min = currProc->period;
			target = curr;
		}
		curr = curr->next;
		currProc = (process *) curr->data;
	}

	return target;
}

void RM(){
	int now = -1;

	while(1){
		// wait for new process arrival if ready queue empty 
		if(isEmptyQueue(ready_q) && 
		   waitForProcPeriod(&now) == -1)
			return;

		// finish scheduling
		if(now == MAX_TIME)
			break;

		node *target = getLeastPeriodNode();
		run_proc = (process *) target->data;

		schedule(run_proc, &now, 1);

		if(run_proc->remain == 0)
			deleteQueueNode(ready_q, target, (void **) &run_proc);
	}
}

node* getLeastStrideNode(){
	if(isEmptyQueue(ready_q))
		return NULL;

	node *curr = ready_q->head->next;
	process *currProc = (process *) curr->data;

	int lcm = getLcmFromReadyQueue();
	int minStride = INT_MAX;
	node *target = curr;
	while(curr && curr != ready_q->tail){
		int sum = getStrideSum(currProc, lcm);
		if(sum < minStride){
			minStride = sum;
			target = curr;
		}
		curr = curr->next;
		currProc = (process *) curr->data;
	}
	
	return target;
}

int getStrideSum(process *proc, int lcm){
	return lcm / proc->tickets * (proc->service - proc->remain);
}

int getLcmFromReadyQueue(){
	node *curr = ready_q->head->next;
	process *currProc = (process *) curr->data;

	int lcm = 1;
	while(curr && curr != ready_q->tail){
		lcm = LCM(lcm, currProc->tickets);
		curr = curr->next;
		currProc = (process *) curr->data;
	}

	return lcm;
}

void STRIDE(){
	int now = -1;

	while(1){
		// wait for new process arrival if ready queue empty 
		if(isEmptyQueue(ready_q) && 
		   waitForProcArrival(&now) == -1)
			return;

		// find the most stride process and delete it from queue
		node *target = getLeastStrideNode();
		run_proc = (process *) target->data;

		schedule(run_proc, &now, 1);

		if(run_proc->remain == 0)
			deleteQueueNode(ready_q, target, (void **) &run_proc);
	}
}


void gotoxy(int x, int y){
	printf("\033[%d;%dH", y + 1, x + 1);
}

void setColor(color c){
	printf("\033[%d;1m", c);
}

void setCursorVisibility(bool visible){
	printf("\e[?25%c", visible ==  true ? 'h' : 'l');
}

int getch(){
	int ch;
	struct termios save;
	struct termios buf;

	tcgetattr(0, &save);
	buf = save;
	buf.c_lflag &= ~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);

	return ch;
}

int POW(int a, int b){
	int result = 1;

	for(int i = 0 ; i < b ; i++){
		result *= a;
	}

	return result;
}

int GCD(int a, int b){
	if(a < b)
		return GCD(b, a);
	if(a % b == 0)
		return b;

	return GCD(b, a % b);
}

int LCM(int a, int b){
	return a * b / GCD(a, b);
}

