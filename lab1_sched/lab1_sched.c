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
#define METRICS_LEFT_ALIGN 110

#define EXIT 6
#define BACK_TO_MAIN 9

#define ENTER 10
#define ARROW 27
#define ARROW2 91
#define UP 65
#define DOWN 66

#define MAX_PROCESS 6
#define MAX_TIME 20
#define MAX_SCHED_TIME 100

#define INITIAL_VALUE -1

const char SCHEDULING[8][25] = {
	"FCFS(FIFO)",
	"RR(Round-Robin)",
	"SPN(SJF)",
	"HRRN",
	"MLFQ",
	"MLFQ (tquantum=2^i)",
	"RM(Rate Monotonic)",
	"STRIDE"
};


int num_of_process = 0;
int ready_queue_cnt;

WORKLOAD_VIEW_TYPE view_type = VIEW_TYPE_DEFAULT;

process *process_arr;

queue *ready_queue;
queue *result_queue;

process *running_proc;


void PrintBoard(){
	gotoxy(0, 0);
	SetConsoleOutColor(RESET);
	puts("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	for(int i = 0 ; i < BOARD_HEIGHT ; i++){
		puts("┃                                                                                                                                                                                           ┃");
	}
	puts("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
	gotoxy(TEXT_LEFT_ALIGN + 30, LINE_SPACE);
        printf("SCHEDULING SIMULATOR by YESEUL LEE");
      	printf("  (2020-1 OPERATING SYSTEM LAB1)");	
}

void PrintProcessMenu(){
	gotoxy(TEXT_LEFT_ALIGN - 2, TEXT_TOP_ALIGN);
	SetConsoleOutColor(BLU);
	printf("Choose the number of process");
	SetConsoleOutColor(RESET);

	for(int i = 2 ; i <= MAX_PROCESS ; i++){
		gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * (i - 1));
		printf("%d", i);
	}

	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 6);
	SetConsoleOutColor(RED);
	printf("EXIT");
	SetConsoleOutColor(RESET);
}

void PrintWorkloadTable(){
	int posY = GetWorkloadPosY() - 1;

	gotoxy(TABLE_LEFT_ALIGN, posY);
	puts(" ◆  Workload  ");

	switch(view_type){
		case VIEW_TYPE_DEFAULT:
			gotoxy(TABLE_LEFT_ALIGN, ++posY);
			puts("┌────────────┬────────────┬────────────┐            ");
			gotoxy(TABLE_LEFT_ALIGN, ++posY);
			puts("│  Process   │  Arrival   │  Service   │                ");
			break;
		case VIEW_TYPE_PERIOD:
			gotoxy(TABLE_LEFT_ALIGN, ++posY);
			puts("┌────────────┬────────────┬────────────┬───────────┐");
			gotoxy(TABLE_LEFT_ALIGN, ++posY);
			puts("│  Process   │  Arrival   │  Service   │  Period   │");
			break;
		case VIEW_TYPE_TICKET:
			gotoxy(TABLE_LEFT_ALIGN, ++posY);
			puts("┌────────────┬────────────┬────────────┬───────────┐");
			gotoxy(TABLE_LEFT_ALIGN, ++posY);
			puts("│  Process   │  Arrival   │  Service   │  Tickets  │");
			break;
	}

	for(int i = 0 ; i < num_of_process ; i++){
		if(view_type == VIEW_TYPE_DEFAULT){
			gotoxy(TABLE_LEFT_ALIGN, ++posY);
			puts("├────────────┼────────────┼────────────┤            ");
			gotoxy(TABLE_LEFT_ALIGN, ++posY);
			puts("│            │            │            │            ");
		} else {
			gotoxy(TABLE_LEFT_ALIGN, ++posY);
			puts("├────────────┼────────────┼────────────┼───────────┤");
			gotoxy(TABLE_LEFT_ALIGN, ++posY);
			puts("│            │            │            │           │");
		}

		gotoxy(TABLE_LEFT_ALIGN + TABLE_MARGIN, posY);
		printf("%c", process_arr[i].name);

		if(process_arr[i].arrival != INITIAL_VALUE && process_arr[i].service != INITIAL_VALUE){
	 		gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH + TABLE_MARGIN, posY);
			printf("%d", process_arr[i].arrival);
	 		gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH * 2 + TABLE_MARGIN, posY);
			printf("%d", process_arr[i].service);
		}

		gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH * 3 + TABLE_MARGIN, posY);
		if(view_type == VIEW_TYPE_PERIOD && process_arr[i].period != INITIAL_VALUE)
			printf("%d", process_arr[i].period);
		else if(view_type == VIEW_TYPE_TICKET && process_arr[i].tickets != INITIAL_VALUE)
			printf("%d", process_arr[i].tickets);
	}

	gotoxy(TABLE_LEFT_ALIGN, ++posY);
	if(view_type == VIEW_TYPE_DEFAULT)
		puts("└────────────┴────────────┴────────────┘            ");
	else 
		puts("└────────────┴────────────┴────────────┴───────────┘");

}

void PrintSchedMenu(){
	system("clear");
	PrintBoard();
	PrintWorkloadTable(VIEW_TYPE_DEFAULT);

	gotoxy(TEXT_LEFT_ALIGN -2, TEXT_TOP_ALIGN);
	SetConsoleOutColor(BLU);
	printf("Choose Scheduling Algorithm");
	SetConsoleOutColor(RESET);

	int i = 0;
	while(i < BACK_TO_MAIN - 1){
		gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * (i + 1));
		printf("%s", SCHEDULING[i++]);
	}

	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * ++i);
	SetConsoleOutColor(RED);
	printf("BACK TO MAIN");
	SetConsoleOutColor(RESET);
}

int GetWorkloadPosY(){
	return TABLE_TOP_ALIGN + ((num_of_process + 2) * TABLE_HEIGHT);
}

void PrintSchedTable(){
	int posY = TABLE_TOP_ALIGN - 1;

	gotoxy(TABLE_LEFT_ALIGN, posY);
	puts("0                             5                             10                            15                            20");

	for(int i = 0 ; i < num_of_process ; i++){
		gotoxy(TABLE_LEFT_ALIGN, ++posY);
		puts("├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤");
		gotoxy(TABLE_LEFT_ALIGN, ++posY);
		puts("│                             │                             │                             │                             │");	
		gotoxy(TABLE_LEFT_ALIGN - 2, posY);
		SetConsoleOutColor(process_arr[i].textColor);
		printf("%c", process_arr[i].name);
		SetConsoleOutColor(RESET);
	}
	
	gotoxy(TABLE_LEFT_ALIGN, ++posY);
	puts("├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤");
}


/*
 * Selection Box Position
 */
int pos[2] = {0, 0};

void FindSelectionBoxPosition(int index){
	pos[0] = SELECTION_BOX_LEFT_ALIGN;
	pos[1] = TEXT_TOP_ALIGN - 1 + (index * LINE_SPACE);
}

void PrintSelectionBox(int index){
	FindSelectionBoxPosition(index);

	gotoxy(pos[0], pos[1]);
	puts("┌─────────────────────────┐");
	gotoxy(pos[0], pos[1] + 1);
	puts("│");
	gotoxy(pos[0] + SELECTION_BOX_WIDTH, pos[1] + 1);	
	puts("│");
	gotoxy(pos[0], pos[1] + 2);
	puts("└─────────────────────────┘");
}

void EraseSelectionBox(int index){
	FindSelectionBoxPosition(index);

	gotoxy(pos[0], pos[1]);
	puts("                            ");
	gotoxy(pos[0], pos[1] + 1);
	puts(" ");
	gotoxy(pos[0] + SELECTION_BOX_WIDTH, pos[1] + 1);	
	puts(" ");
	gotoxy(pos[0], pos[1] + 2);
	puts("                            ");
}

queue* NewQueue(){
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

bool IsEmptyQueue(queue *q){
	return q == NULL || q->count == 0;
}

void* getFromQueue(queue *q, int pos){
	if(IsEmptyQueue(q) || pos >= q->count)
		return NULL;

	node *target = q->head->next;

	for(int i = 0 ; i < pos ; i++){
		target = target->next;
	}

	return target->data;
}

void InsertQueue(queue *q, void *data){
	if(q == NULL)
		q = NewQueue();

	node *newNode = malloc(sizeof(node));

	newNode->data = data;

	newNode->before = q->tail->before;
	q->tail->before = newNode;
	newNode->before->next = newNode;
	newNode->next = q->tail;

	q->count += 1;
}

void DeleteQueue(queue *q, void **data){
	if(IsEmptyQueue(q)) 
		return;

	node *target = q->head->next;

	// save data to delete
	*data = target->data;	

	q->head->next = target->next;
	target->next->before = q->head;
	free(target);

	q->count -= 1;
}

void DeleteQueuePosition(queue *q, int pos, void **data){
	if(IsEmptyQueue(q) || pos >= q->count)
		return;

	node *target = q->head->next;

	// find target node
	for(int i = 0 ; i < pos ; i++){
		target = target->next;
	}

	DeleteQueueNode(q, target, data);	
}

void DeleteQueueNode(queue *q, node *node, void **data){
	if(IsEmptyQueue(q))
		return;

	// save data to delete
	*data = node->data;

	node->before->next = node->next;
	node->next->before = node->before;
	free(node);
	
	q->count -= 1;
}

void FreeQueue(queue *q){
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

void Init(){
	system("clear");
	SetCursorVisibility(false);
	PrintBoard();
	PrintProcessMenu();

	int menuIndex = 1;
        int key;

	PrintSelectionBox(menuIndex);

	while(1){
		key = getch();

		/* Enter key pressed */
		if(key == ENTER){ 
			if(menuIndex == EXIT)
				return;
			else 
				break;
		}
		
		/* Arrow key pressed */
		if(key == ARROW && (key = getch()) == ARROW2){ 
			key = getch();

			if(key == UP && menuIndex > 1){
				EraseSelectionBox(menuIndex);
				menuIndex--;
				PrintSelectionBox(menuIndex);
			} else if(key == DOWN && menuIndex < EXIT){
				EraseSelectionBox(menuIndex);
				menuIndex++;
				PrintSelectionBox(menuIndex);
			}
		}
	}
	
	num_of_process = menuIndex + 1;
	CreateProcessArr();	
}

void InitSchedMenu(){
	system("clear");
	PrintSchedMenu();
	PrintSchedTable();

	int menuIndex = 1;
        int key;

	PrintSelectionBox(menuIndex);	

	while(1){
		key = getch();
	
		/* Enter key pressed */
		if(key == ENTER){  
			if(menuIndex == BACK_TO_MAIN)
				break;
			else 
				RunScheduling(menuIndex);	
		}
	
		/* Arrow key pressed */
		if(key == ARROW && (key = getch()) == ARROW2){  
			key = getch();

			if(key == UP && menuIndex > 1){
				EraseSelectionBox(menuIndex);
				menuIndex--;
				PrintSelectionBox(menuIndex);
			} else if(key == DOWN && menuIndex < BACK_TO_MAIN){
				EraseSelectionBox(menuIndex);
				menuIndex++;
				PrintSelectionBox(menuIndex);
			}
		}
	}

	view_type = VIEW_TYPE_DEFAULT;
	Init();
}

void ResetProcess(process *proc){
	proc->start = INITIAL_VALUE;
	proc->remain = proc->service;
	proc->finish = INITIAL_VALUE;
}

void InitProcess(process *proc, char n, int c){
	proc->name = n;
	proc->textColor = c;
	proc->arrival = INITIAL_VALUE;
	proc->service = INITIAL_VALUE;
	proc->period = INITIAL_VALUE;
	proc->tickets = INITIAL_VALUE;
	proc->start = INITIAL_VALUE;
	proc->remain = INITIAL_VALUE;
	proc->finish = INITIAL_VALUE;
}

void CreateProcessArr(){	
	process_arr = malloc(sizeof(process) * num_of_process);	

	for(int i = 0 ; i < num_of_process ; i++){
		InitProcess(process_arr + i, 'A' + i, RED + (i % NUM_OF_COLORS));
	}

	PrintWorkloadTable();
	PrintSchedTable();

	int posX = TABLE_LEFT_ALIGN + TABLE_MARGIN;
	int posY = GetWorkloadPosY() + TABLE_HEIGHT * 2 - 1;

	SetCursorVisibility(true);
	for(int i = 0 ; i < num_of_process ; i++){
	 	gotoxy(posX + TABLE_WIDTH, posY + (i * TABLE_HEIGHT));
		scanf("%d", &process_arr[i].arrival);
	 	gotoxy(posX + TABLE_WIDTH * 2, posY + (i * TABLE_HEIGHT));
		scanf("%d", &process_arr[i].service);
	}
	getchar();
	SetCursorVisibility(false);
	
	InitSchedMenu();

	free(process_arr);
}

void InputWorkload(){
	int posX = TABLE_LEFT_ALIGN + TABLE_WIDTH * 3 + TABLE_MARGIN;
	int posY = GetWorkloadPosY() + 3;

	SetCursorVisibility(true);
	for(int i = 0 ; i < num_of_process ; i++){
	 	gotoxy(posX, posY + i * TABLE_HEIGHT);
		scanf("%d", view_type == VIEW_TYPE_PERIOD ? &process_arr[i].period : &process_arr[i].tickets);
	}
	getch();
	SetCursorVisibility(false);
}

void PrintResultQueue(){
	int posY = TABLE_TOP_ALIGN + 1;
	const int LEFT_SPACE = 6;

	sched_process *proc = malloc(sizeof(sched_process));

	int now = 0;
	while(now < MAX_TIME && !IsEmptyQueue(result_queue)){
		DeleteQueue(result_queue, (void**) &proc); 
		
		now = proc->start;
		int index = proc->name - (int) 'A';

		while(now < MAX_TIME && now < proc->start + proc->running){		
			gotoxy(TABLE_LEFT_ALIGN + (now * LEFT_SPACE), posY + (TABLE_HEIGHT * index));
			printf("│");
			SetConsoleOutColor(proc->textColor);
			printf("  %c  ", proc->name);
			SetConsoleOutColor(RESET);
			printf("│");
		
			now++;
		}
	}
}

void PrintResultMetrics(){
	int posX = METRICS_LEFT_ALIGN;
	int posY = GetWorkloadPosY() - 1;

	gotoxy(posX, posY);
	puts(" ◆  Result Metrics  ");

	gotoxy(posX, ++posY);
	puts("┌────────────┬────────────┬────────────┬────────────┬────────────┐");
	gotoxy(posX, ++posY);
	puts("│  Process   │  FirstRun  │ Completion │ Turnaround │  Response  │");

	int totalT = 0, totalR = 0;

	for(int i = 0 ; i < num_of_process ; i++){
		gotoxy(posX, ++posY);
		puts("├────────────┼────────────┼────────────┼────────────┼────────────┤");
		gotoxy(posX, ++posY);
		puts("│            │            │            │            │            │");

		int arrival = process_arr[i].arrival;
		int start = process_arr[i].start;
		int finish = process_arr[i].finish;

		if(start == INITIAL_VALUE || finish == INITIAL_VALUE)
			continue;

		int turnaround = finish - arrival;
		int response = start - arrival;
		totalT += turnaround;
		totalR += response;

		SetConsoleOutColor(RED + (i % NUM_OF_COLORS));
		gotoxy(posX + TABLE_MARGIN, posY);
		printf("%c", process_arr[i].name);
		gotoxy(posX + TABLE_MARGIN + TABLE_WIDTH, posY);
		printf("%d", start);
	 	gotoxy(posX + TABLE_MARGIN + TABLE_WIDTH * 2, posY);
		printf("%d", finish);
	 	gotoxy(posX + TABLE_MARGIN + TABLE_WIDTH * 3, posY);
		printf("%d", turnaround);
	 	gotoxy(posX + TABLE_MARGIN + TABLE_WIDTH * 4, posY);
		printf("%d", response);
		SetConsoleOutColor(RESET);
	}

	gotoxy(posX, ++posY); posY++;
	puts("└────────────┴────────────┴────────────┴────────────┴────────────┘");
	gotoxy(posX, ++posY); posY++;
	printf(" ◆  Average Turnaround Time = %0.3f\n", totalT / (float) num_of_process);
	gotoxy(posX, ++posY);
	printf(" ◆  Average Response Time   = %0.3f\n", totalR / (float) num_of_process);
}

void PrintSchedulingTitle(int index){
	int posX = TABLE_LEFT_ALIGN;
	int posY = TABLE_TOP_ALIGN - 3;
	
	gotoxy(posX, posY);
	printf("◆  %-40s", SCHEDULING[index - 1]);
}

int InputTimeQuantum(){
	int posX = TABLE_LEFT_ALIGN + 20;
	int posY = TABLE_TOP_ALIGN - 3;
	
	int tquantum;

	gotoxy(posX, posY);
	SetCursorVisibility(true);
	SetConsoleOutColor(BLU);
	printf("time quantum = ");
	scanf("%d", &tquantum);
	getch();
	SetConsoleOutColor(RESET);
	SetCursorVisibility(false);

	return tquantum;
}

void RunScheduling(int index){
	system("clear");
	PrintBoard();
	PrintSchedMenu();
	PrintSelectionBox(index);
	PrintSchedulingTitle(index);

	// init ready queue
	ready_queue_cnt = 1;
	ready_queue = NewQueue();	
	// init result queue
	result_queue = NewQueue();	
	// init running process
	running_proc = malloc(sizeof(process));

	view_type = VIEW_TYPE_DEFAULT;
	PrintWorkloadTable();
	PrintSchedTable();
	
	switch(index){
		case 1:
			FCFS();
			PrintResultMetrics();
			break;
		case 2:	
			RR(InputTimeQuantum());
			PrintResultMetrics();
			break;
		case 3:
			SJF();
			PrintResultMetrics();
			break;
		case 4:
			HRRN();
			PrintResultMetrics();
			break;
		case 5:
			MLFQ(TYPE_DEFAULT, InputTimeQuantum());
			PrintResultMetrics();
			break;
		case 6:
			MLFQ(TYPE_MULTIPLE, 2);
			PrintResultMetrics();
			break;
		case 7:
			view_type = VIEW_TYPE_PERIOD;
			PrintWorkloadTable();
			RM();
			break;
		case 8:
			view_type = VIEW_TYPE_TICKET;
			PrintWorkloadTable();
			STRIDE();
			PrintResultMetrics();
			break;
	}

	PrintResultQueue();

	// free ready queue
	for(int i = 0 ; i < ready_queue_cnt ; i++){
		FreeQueue(ready_queue + i);
	}	
	free(ready_queue);

	// free result queue
	FreeQueue(result_queue);
	free(result_queue);
}

void UpdateReadyQueue(int now){
	// insert new process first
	for(int i = 0 ; i < num_of_process ; i++){
		if(process_arr[i].arrival == now){
			ResetProcess(process_arr + i);
			InsertQueue(ready_queue, process_arr + i);
		}
	}
}

void WaitIfReadyQueueEmpty(int *now){
	while(*now < MAX_SCHED_TIME && IsEmptyQueue(ready_queue)){
		if(view_type == VIEW_TYPE_PERIOD)
			UpdatePeriodReadyQueue(++(*now));
		else
			UpdateReadyQueue(++(*now));
	}
}

void UpdatePeriodReadyQueue(const int now){
	for(int i = 0 ; i < num_of_process ; i++){
		int parr = 0;
		int cnt = 0;
		int arr = process_arr[i].arrival;
		int p = process_arr[i].period;
		while(parr <= now){
			parr = arr + p * cnt++; 
			if(now == parr){
				ResetProcess(process_arr + i);
				InsertQueue(ready_queue, process_arr + i);
			}
		}
	}
}

sched_process* NewSchedProcess(process *source, int start, int running){
	sched_process *proc = malloc(sizeof(sched_process));
	
	proc->name = source->name;
	proc->textColor = source->textColor;
	proc->start = start;
	proc->running = running;

	return proc;
}

void Schedule(process *proc, int *now, const int t_while){
	const int start = *now;	

	/* process first run */
	if(proc->start == INITIAL_VALUE)
		proc->start = start;
	
	int running = 0;
	while(running < t_while){
		proc->remain -= 1;

		if(view_type == VIEW_TYPE_PERIOD)
			UpdatePeriodReadyQueue(++(*now));
		else
			UpdateReadyQueue(++(*now));

		running++;

		/* process finish */
		if(proc->remain == 0){
			proc->finish = *now;
			break;
		}
	}

	InsertQueue(result_queue, NewSchedProcess(proc, start, running));
}

/*
 * find the shortest process node in ready queue
 */
node* GetShortestJobNode(){
	if(IsEmptyQueue(ready_queue))
		return NULL;

	node *curr = ready_queue->head->next;
	process *currPros = (process *) curr->data;

	node *shortest = curr;
	while(curr != NULL && curr != ready_queue->tail){
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
float GetResponseRatio(int now, process *proc){
	return 1 + (float) (now - proc->arrival) / (float) proc->service;
}

/*
 * find the highest response ratio process node in ready queue
 */
node* GetHighestResponseRatioNode(int now){
	if(IsEmptyQueue(ready_queue))
		return NULL;

	node *curr = ready_queue->head->next;
	process *currPros = (process *) curr->data;

	node *highest = curr;
	float maxRatio = GetResponseRatio(now, currPros);
	while(curr != NULL && curr != ready_queue->tail){
		float currRatio = GetResponseRatio(now, currPros);
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
	int now = 0;
	UpdateReadyQueue(now);

	while(now < MAX_SCHED_TIME){
		// wait for new process 
		WaitIfReadyQueueEmpty(&now);

		// finish scheduling
		if(now == MAX_SCHED_TIME)
			break;

		// get first process from ready queue 
		DeleteQueue(ready_queue, (void **) &running_proc);

		// run until the process finish
		Schedule(running_proc, &now, running_proc->service);
	}
}

void RR(const int t_quantum){
	int now = 0;
	UpdateReadyQueue(now);

	while(now < MAX_SCHED_TIME){
		// wait for new process
		WaitIfReadyQueueEmpty(&now);

		// finish scheduling
		if(now == MAX_SCHED_TIME)
			break;

		// get first process from ready queue 
		DeleteQueue(ready_queue, (void **) &running_proc);

		// run process during time quantum		
		do
			Schedule(running_proc, &now, t_quantum);
		while(running_proc->remain != 0 && 
			IsEmptyQueue(ready_queue)); // repeat if ready queue is empty

		/* insert the process into ready queue if service time remains */
		if(running_proc->remain != 0)
			InsertQueue(ready_queue, running_proc);
	}
}

void SJF(){
	int now = 0;
	UpdateReadyQueue(now);

	while(now < MAX_SCHED_TIME){
		// wait for new process 
		WaitIfReadyQueueEmpty(&now);

		// finish scheduling
		if(now == MAX_SCHED_TIME)
			break;

		// find the shortest process and delete it from queue
		node *target = GetShortestJobNode();
		DeleteQueueNode(ready_queue, target, (void **) &running_proc);

		// run until the process finish
		Schedule(running_proc, &now, running_proc->service);
	}
}

void HRRN(){
	int now = 0;
	UpdateReadyQueue(now);

	while(now < MAX_SCHED_TIME){
		// wait for new process 
		WaitIfReadyQueueEmpty(&now);

		// finish scheduling
		if(now == MAX_SCHED_TIME)
			break;

		// find the shortest process and delete it from queue
		node *target = GetHighestResponseRatioNode(now);
		DeleteQueueNode(ready_queue, target, (void **) &running_proc);

		// run until the process finish
		Schedule(running_proc, &now, running_proc->service);
	}
}

void IncreaseReadyQueue(){
	ready_queue = realloc(ready_queue, sizeof(queue) * ++ready_queue_cnt);
	ready_queue[ready_queue_cnt - 1] = *NewQueue();
}

/*
 * find the level of queue not empty (exist process to run)
 */
int FindNotEmptyQueueLevel(int *now){
	while(*now < MAX_SCHED_TIME){
		int level = 0;
		while(level < ready_queue_cnt){
			if(!IsEmptyQueue(&ready_queue[level]))
				break;
			level++;
		}

		/* queue is not empty */
		if(level < ready_queue_cnt)
			return level;

		UpdateReadyQueue(++(*now));
	}

	return -1;
}

void MLFQ(const MLFQ_TYPE type, const int t_quantum){
	int now = 0;
	UpdateReadyQueue(now);

	while(now < MAX_SCHED_TIME){
		// find the process to run from multi queue
		int qLevel = FindNotEmptyQueueLevel(&now);

		// finish scheduling
		if(qLevel == -1)
			break;

		DeleteQueue(&ready_queue[qLevel], (void **) &running_proc);

		bool repeat;
		do{
			repeat = true;

			int t_while = (type == TYPE_DEFAULT) ? 
					t_quantum : 
					Pow(t_quantum, qLevel);

			// run process during time quantum 
			Schedule(running_proc, &now, t_while);

			// check is there any process to run next 
			for(int i = 0 ; i < ready_queue_cnt ; i++){
				if(!IsEmptyQueue(&ready_queue[i])){
					repeat = false;
					break;
				}
			}
		} while(running_proc->remain != 0 && repeat); // repeat if queue empty

		/* insert the process if service time remains */
		if(running_proc->remain != 0){
			/* check if we need to increase the num of queue */
			if(qLevel == ready_queue_cnt - 1)
				IncreaseReadyQueue();
			// insert process into the next level of queue
			InsertQueue(&ready_queue[qLevel + 1], running_proc);
		}
	}
}

node* GetLeastPeriodNode(){
	if(IsEmptyQueue(ready_queue))
		return NULL;

	node *curr = ready_queue->head->next;
	process *currProc = (process *) curr->data;

	node *target = curr;
	int min = currProc->period;
	while(curr && curr != ready_queue->tail){
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
	if(process_arr && process_arr[0].period == INITIAL_VALUE){
		InputWorkload();
	}

	int now = 0;
	UpdatePeriodReadyQueue(now);

	while(now < MAX_SCHED_TIME){
		// wait for new process 
		WaitIfReadyQueueEmpty(&now);

		// finish scheduling
		if(now == MAX_SCHED_TIME)
			break;

		node *target = GetLeastPeriodNode();
		running_proc = (process *) target->data;

		Schedule(running_proc, &now, 1);

		if(running_proc->remain == 0)
			DeleteQueueNode(ready_queue, target, (void **) &running_proc);
	}
}

node* GetLeastStrideNode(){
	if(IsEmptyQueue(ready_queue))
		return NULL;

	node *curr = ready_queue->head->next;
	process *currProc = (process *) curr->data;

	int lcm = GetLcmFromReadyQueue();
	int minStride = INT_MAX;
	node *target = curr;
	while(curr && curr != ready_queue->tail){
		int sum = GetStrideSum(currProc, lcm);
		if(sum < minStride){
			minStride = sum;
			target = curr;
		}
		curr = curr->next;
		currProc = (process *) curr->data;
	}
	
	return target;
}

int GetStrideSum(process *proc, int lcm){
	return lcm / proc->tickets * (proc->service - proc->remain);
}

int GetLcmFromReadyQueue(){
	node *curr = ready_queue->head->next;
	process *currProc = (process *) curr->data;

	int lcm = 1;
	while(curr && curr != ready_queue->tail){
		lcm = LCM(lcm, currProc->tickets);
		curr = curr->next;
		currProc = (process *) curr->data;
	}

	return lcm;
}

void STRIDE(){
	if(process_arr && process_arr[0].tickets == INITIAL_VALUE){
		InputWorkload();
	}

	int now = 0;
	UpdateReadyQueue(now);

	while(now < MAX_SCHED_TIME){
		// wait for new process 
		WaitIfReadyQueueEmpty(&now);

		// finish scheduling
		if(now == MAX_SCHED_TIME)
			break;

		// find the most stride process and delete it from queue
		node *target = GetLeastStrideNode();
		running_proc = (process *) target->data;

		Schedule(running_proc, &now, 1);

		if(running_proc->remain == 0)
			DeleteQueueNode(ready_queue, target, (void **) &running_proc);
	}
}


void gotoxy(int x, int y){
	printf("\033[%d;%dH", y + 1, x + 1);
}

void SetConsoleOutColor(color c){
	printf("\033[%d;1m", c);
}

void SetCursorVisibility(bool visible){
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

int Pow(int a, int b){
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

