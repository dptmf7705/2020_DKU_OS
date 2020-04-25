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
#define TABLE_WIDTH 17
#define TABLE_HEIGHT 2

#define EXIT 6
#define BACK_TO_MAIN 9

#define ENTER 10
#define ARROW 27
#define ARROW2 91
#define UP 65
#define DOWN 66

#define NUM_OF_COLORS 6
#define RED 31
#define GRN 32
#define YEL 33
#define BLU 34
#define MAG 35
#define CYN 36
#define RESET 0

#define MAX_PROCESS 6
#define MAX_TIME 20
#define NOT_DEFINE -1

int num_of_process = 0;

process *process_arr;
process *process_arr_sorted;

queue *ready_queue;
queue *result_queue;

void gotoxy(int x, int y){
	printf("\033[%d;%dH", y + 1, x + 1);
}

void SetConsoleOutColor(int color){
	printf("\033[%d;1m", color);
}

void SetCursorVisibility(bool visible){
	printf("\e[?25%c", visible ==  true ? 'h' : 'l');
}


int Pow(int a, int b){
	int result = 1;

	for(int i = 0 ; i < b ; i++){
		result *= a;
	}

	return result;
}

/*
 * get console input char 
 */
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
	printf("How many process?");
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
	int posY = TABLE_TOP_ALIGN - 1;

	gotoxy(TABLE_LEFT_ALIGN, posY);
	puts("  Workload  ");

	gotoxy(TABLE_LEFT_ALIGN, ++posY);
	puts("┌────────────────┬────────────────┬────────────────┐");
	gotoxy(TABLE_LEFT_ALIGN, ++posY);
	puts("│  Process Name  │  Arrival Time  │  Service Time  │");

	for(int i = 0 ; i < num_of_process ; i++){
		gotoxy(TABLE_LEFT_ALIGN, ++posY);
		puts("├────────────────┼────────────────┼────────────────┤");
		gotoxy(TABLE_LEFT_ALIGN, ++posY);
		puts("│                │                │                │");
		gotoxy(TABLE_LEFT_ALIGN + 8, posY);
		printf("%c", process_arr[i].name);
		if(process_arr[i].arrival != NOT_DEFINE && process_arr[i].service != NOT_DEFINE){
	 		gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH + 8, posY);
			printf("%d", process_arr[i].arrival);
	 		gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH * 2 + 8, posY);
			printf("%d", process_arr[i].service);
		}
	}

	gotoxy(TABLE_LEFT_ALIGN, ++posY);
	puts("└────────────────┴────────────────┴────────────────┘");
}

void PrintSchedMenu(){
	system("clear");
	PrintBoard();
	PrintWorkloadTable();

	gotoxy(TEXT_LEFT_ALIGN -2, TEXT_TOP_ALIGN);
	SetConsoleOutColor(BLU);
	printf("Choose Scheduling Algorithm");
	SetConsoleOutColor(RESET);
	
	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE);
	printf("FCFS(FIFO)");
	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 2);
	printf("RR");
	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 3);
	printf("SPN(SJF)");
	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 4);
	printf("HRRN");
	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 5);
	printf("MLFQ");
	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 6);
	printf("MLFQ (t_quantum=2^i)");
	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 7);
	printf("RM");
	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 8);
	printf("STRIDE");

	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 9);
	SetConsoleOutColor(RED);
	printf("BACK TO MAIN");
	SetConsoleOutColor(RESET);
}

int GetSchedTableTopAlign(){
	return TABLE_TOP_ALIGN + ((num_of_process + 2) * TABLE_HEIGHT) + LINE_SPACE;
}

void PrintSchedTable(){
	int posY = GetSchedTableTopAlign();

	gotoxy(TABLE_LEFT_ALIGN, posY);
	puts("0                             5                             10                            15                            20");

	for(int i = 0 ; i < num_of_process ; i++){
		gotoxy(TABLE_LEFT_ALIGN, ++posY);
		puts("├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤");
		gotoxy(TABLE_LEFT_ALIGN, ++posY);
		puts("│                             │                             │                             │                             │");	
		gotoxy(TABLE_LEFT_ALIGN - 2, posY);
		SetConsoleOutColor(process_arr[i].color);
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

	free(q);
}


const int colors[NUM_OF_COLORS] = {
	RED, GRN, YEL, BLU, MAG, CYN
};

void CreateProcessArr(){	
	process_arr = malloc(sizeof(process) * num_of_process);	
	process_arr_sorted = malloc(sizeof(process) * num_of_process);	
	
	for(int i = 0 ; i < num_of_process ; i++){
		process_arr[i].name = 'A' + i;
		process_arr[i].color = colors[i % NUM_OF_COLORS];
		process_arr[i].arrival = NOT_DEFINE;
		process_arr[i].service = NOT_DEFINE;
		process_arr[i].start = NOT_DEFINE;
		process_arr[i].remain = NOT_DEFINE;
		process_arr[i].finish = NOT_DEFINE;
	}

	SetCursorVisibility(true);
	PrintWorkloadTable();

	for(int i = 0 ; i < num_of_process ; i++){
	 	gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH + 8, TABLE_TOP_ALIGN + (i * TABLE_HEIGHT) + 3);
		scanf("%d", &process_arr[i].arrival);
	 	gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH * 2 + 8, TABLE_TOP_ALIGN + (i * TABLE_HEIGHT) + 3);
		scanf("%d", &process_arr[i].service);
		process_arr[i].remain = process_arr[i].service;
	}
	getchar();
	
	InitSchedMenu();

	free(process_arr);
	free(process_arr_sorted);
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
	SetCursorVisibility(false);
	PrintBoard();
	PrintWorkloadTable();
	PrintSchedMenu();

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

	Init();
}

/*
 * sort by 
 * 1_ Arrival Time
 * 2_ Name (already sorted when processes created)
 */
void SortProcessArrByArrivalTime(){
	// copy array (original array is needed to print workload table)
	for(int i = 0 ; i < num_of_process ; i++){
		memcpy(process_arr_sorted + i, process_arr + i, sizeof(process));
	}

	// sort by arrival time
	for(int i = 0 ; i < num_of_process - 1 ; i++){
		for(int j = i + 1 ; j < num_of_process ; j++){
			if(process_arr_sorted[j].arrival < process_arr_sorted[i].arrival){
				process temp = process_arr_sorted[j];
				process_arr_sorted[j] = process_arr_sorted[i];
				process_arr_sorted[i] = temp;
			}
		}
	}
}

void UpdateReadyQueue(int now){
	// insert new process first
	for(int i = 0 ; i < num_of_process ; i++){
		if(process_arr_sorted[i].arrival == now)
			InsertQueue(ready_queue, &process_arr_sorted[i]);
	}
}


void PrintResultQueue(){
	const int TOP_ALIGN = GetSchedTableTopAlign() + 2;
	const int LEFT_SPACE = 6;

	PrintSchedTable();

	sched_process *proc = malloc(sizeof(sched_process));

	while(!IsEmptyQueue(result_queue)){
		DeleteQueue(result_queue, (void**) &proc); 
		
		int now = proc->start;
		int index = proc->name - (int) 'A';
		
		while(now < (proc->start + proc->running)){
			gotoxy(TABLE_LEFT_ALIGN + (now * LEFT_SPACE), TOP_ALIGN + (TABLE_HEIGHT * index));
			printf("│");
			SetConsoleOutColor(proc->color);
			printf("  %c  ", proc->name);
			SetConsoleOutColor(RESET);
			printf("│");
			
			now++;
		}
	}
}

void RunScheduling(int index){
	// init ready queue
	ready_queue = NewQueue();	
	// init result queue
	result_queue = NewQueue();	
	
	SortProcessArrByArrivalTime();
	
	switch(index){
		case 1:
			FCFS();
			break;
		case 2:
			RR(1);
			break;
		case 3:
			SJF();
			break;
		case 4:
			HRRN();
			break;
		case 5:
			MLFQ(1);
			break;
		case 6:
			MLFQ_2i();
			break;
	}

	PrintResultQueue();
	
	FreeQueue(ready_queue);
	FreeQueue(result_queue);
}

sched_process* NewSchedProcess(process *source, int start, int running){
	sched_process *proc = malloc(sizeof(sched_process));
	
	proc->name = source->name;
	proc->color = source->color;
	proc->start = start;
	proc->running = running;

	return proc;
}

/*
 * find the shortest process node in ready queue
 */
node* GetShortestProcessNode(){
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
	// process to run
	process *runProc = malloc(sizeof(process));
	
	int now = 0;
	
	UpdateReadyQueue(now);

	while(now < MAX_TIME){
		// wait for new process 
		while(now < MAX_TIME && IsEmptyQueue(ready_queue)){
			UpdateReadyQueue(++now);
		}

		// finish scheduling
		if(now == MAX_TIME)
			break;

		// get first process from ready queue 
		DeleteQueue(ready_queue, (void **)&runProc);

		// run until the process finish
		runProc->start = now;
		for(int i = 0 ; i < runProc->service ; i++){
			InsertQueue(result_queue, NewSchedProcess(runProc, now, 1));
			UpdateReadyQueue(++now);
		}
		runProc->remain = 0;
		runProc->finish = now;
	}
}

void RR(const int t_quantum){
	// process to run
	process *runProc = malloc(sizeof(process));

	int now = 0;

	UpdateReadyQueue(now);

	while(now < MAX_TIME){
		// wait for new process
		while(IsEmptyQueue(ready_queue) && now < MAX_TIME){
			UpdateReadyQueue(++now);
		}

		// finish scheduling
		if(now == MAX_TIME)
			break;

		// get first process from ready queue 
		DeleteQueue(ready_queue, (void **) &runProc);

		/* process first run */
		if(runProc->start == NOT_DEFINE)
			runProc->start = now;

		do {
			// run process during time quantum
			for(int i = 0 ; i < t_quantum ; i++){
				runProc->remain -= 1;	
				InsertQueue(result_queue, NewSchedProcess(runProc, now, 1));
				UpdateReadyQueue(++now);

				/* process finish */
				if(runProc->remain == 0)
					break;
			}
		} while(runProc->remain != 0 && 
			IsEmptyQueue(ready_queue)); // repeat if queue empty

		/* service time remains */
		if(runProc->remain != 0)
			InsertQueue(ready_queue, runProc);
		else 
			runProc->finish = now;
	}
}

void SJF(){
	// process to run
	process *runProc = malloc(sizeof(process));
	
	int now = 0;

	UpdateReadyQueue(now);

	while(now < MAX_TIME){
		// wait for new process 
		while(IsEmptyQueue(ready_queue) && now < MAX_TIME){
			UpdateReadyQueue(++now);
		}

		// finish scheduling
		if(now == MAX_TIME)
			break;

		// find the shortest process and delete it from queue
		node *shortest = GetShortestProcessNode();
		DeleteQueueNode(ready_queue, shortest, (void **) &runProc);

		// run until the process finish
		runProc->start = now;
		for(int i = 0 ; i < runProc->service ; i++){
			InsertQueue(result_queue, NewSchedProcess(runProc, now, 1));
			UpdateReadyQueue(++now);
		}
		runProc->remain = 0;
		runProc->finish = now;
	}
}

void HRRN(){
	// process to run
	process *runProc = malloc(sizeof(process));
	
	int now = 0;

	UpdateReadyQueue(now);

	while(now < MAX_TIME){
		// wait for new process 
		while(IsEmptyQueue(ready_queue) && now < MAX_TIME){
			UpdateReadyQueue(++now);
		}

		// finish scheduling
		if(now == MAX_TIME)
			break;

		// find the shortest process and delete it from queue
		node *highest = GetHighestResponseRatioNode(now);
		DeleteQueueNode(ready_queue, highest, (void **) &runProc);

		// run until the process finish
		runProc->start = now;
		for(int i = 0 ; i < runProc->service ; i++){
			InsertQueue(result_queue, NewSchedProcess(runProc, now, 1));
			UpdateReadyQueue(++now);
		}
		runProc->remain = 0;
		runProc->finish = now;
	}
}

int ready_queue_cnt;

void IncreaseReadyQueue(){
	ready_queue = realloc(ready_queue, sizeof(queue) * ++ready_queue_cnt);
	ready_queue[ready_queue_cnt - 1] = *NewQueue();
}


void MLFQ(const int t_quantum){
	// process to run
	process *runProc = malloc(sizeof(process));

	ready_queue_cnt = 1;
	int now = 0;

	UpdateReadyQueue(now);

	while(now < MAX_TIME){
		// find the process to run
		int queLevel = 0;
		for( ; queLevel < ready_queue_cnt ; queLevel++){
			if(!IsEmptyQueue(&ready_queue[queLevel]))
				break;
		}

		/* all queue empty */
		if(queLevel == ready_queue_cnt){
			UpdateReadyQueue(++now);
			continue;
		}

		DeleteQueue(&ready_queue[queLevel], (void **) &runProc);

		/* process first run */
		if(runProc->start == NOT_DEFINE)
			runProc->start = now;

		bool repeat;
		do{
			repeat = true;
			// run process during time quantum 
			for(int i = 0 ; i < t_quantum ; i++){
				runProc->remain -= 1;
				InsertQueue(result_queue, NewSchedProcess(runProc, now, 1));
				UpdateReadyQueue(++now);

				/* process finish */
				if(runProc->remain == 0)
					break;
			}

			// find the process to run next 
			for(int i = 0 ; i < ready_queue_cnt ; i++){
				/* process exist */
				if(!IsEmptyQueue(&ready_queue[i])){
					repeat = false;
					break;
				}
			}
		} while(runProc->remain != 0 && repeat);

		/* service time remains */
		if(runProc->remain != 0){
			/* need to increase the num of queue */
			if(queLevel == ready_queue_cnt - 1){
				IncreaseReadyQueue();
			}
			InsertQueue(&ready_queue[queLevel + 1], runProc);
		} else 
			runProc->finish = now;
	}
}

void MLFQ_2i(){
	// process to run
	process *runProc = malloc(sizeof(process));

	ready_queue_cnt = 1;
	int now = 0;

	UpdateReadyQueue(now);

	while(now < MAX_TIME){
		// find the process to run
		int queLevel = 0;
		for( ; queLevel < ready_queue_cnt ; queLevel++){
			if(!IsEmptyQueue(&ready_queue[queLevel]))
				break;
		}

		/* all queue empty */
		if(queLevel == ready_queue_cnt){
			UpdateReadyQueue(++now);
			continue;
		}

		DeleteQueue(&ready_queue[queLevel], (void **) &runProc);

		/* process first run */
		if(runProc->start == NOT_DEFINE)
			runProc->start = now;

		bool repeat;
		do{
			repeat = true;
			// run process during time quantum 
			for(int i = 0 ; i < Pow(2, queLevel) ; i++){
				runProc->remain -= 1;
				InsertQueue(result_queue, NewSchedProcess(runProc, now, 1));
				UpdateReadyQueue(++now);

				/* process finish */
				if(runProc->remain == 0)
					break;
			}

			// find the process to run next 
			for(int i = 0 ; i < ready_queue_cnt ; i++){
				/* process exist */
				if(!IsEmptyQueue(&ready_queue[i])){
					repeat = false;
					break;
				}
			}
		} while(runProc->remain != 0 && repeat);

		/* service time remains */
		if(runProc->remain != 0){
			/* need to increase the num of queue */
			if(queLevel == ready_queue_cnt - 1){
				IncreaseReadyQueue();
			}
			InsertQueue(&ready_queue[queLevel + 1], runProc);
		} else 
			runProc->finish = now;
	}
}
