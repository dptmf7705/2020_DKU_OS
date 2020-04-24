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
#define BACK_TO_MAIN 8

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
#define NOT_DEFINE -1

int num_of_process = 0;

PROCESS *process_arr;

P_QUEUE readyQueue;
P_QUEUE resultQueue;

/*
 * move cursor position to (x, y)
 */
void gotoxy(int x, int y){
	printf("\033[%d;%dH", y + 1, x + 1);
}

void SetConsoleOutColor(int color){
	printf("\033[%d;1m", color);
}

void SetCursorVisibility(bool visible){
	printf("\e[?25%c", visible ==  true ? 'h' : 'l');
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
	printf("RM");
	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 7);
	printf("STRIDE");

	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN + LINE_SPACE * 8);
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

P_QUEUE NewQueue(){
	P_QUEUE queue = malloc(sizeof(QUEUE));	
	P_NODE head = malloc(sizeof(NODE));	
	P_NODE tail = malloc(sizeof(NODE));	

	queue->head = head;
	queue->tail = tail;
	queue->count = 0;

	queue->head->next = queue->tail;
	queue->head->before = NULL;

	queue->tail->before = queue->head;
	queue->tail->next = NULL;

	return queue;
}

bool IsEmptyQueue(P_QUEUE queue){
	return queue == NULL || queue->count == 0;
}

void InsertQueue(P_QUEUE queue, void *data){
	if(queue == NULL){
		queue = NewQueue();
	}

	P_NODE newNode = malloc(sizeof(NODE));

	newNode->data = data;

	newNode->next = queue->head->next;
	queue->head->next = newNode;
	newNode->next->before = newNode;
	newNode->before = queue->head;

	queue->count += 1;
}

void DeleteQueue(P_QUEUE queue, void **out){
	if(IsEmptyQueue(queue)){
		return;
	}

	P_NODE delNode = queue->tail->before;

	// save data to delete
	*out = delNode->data;	

	queue->tail->before = delNode->before;
	delNode->before->next = queue->tail;
	free(delNode);

	queue->count -= 1;
}

void DeleteQueuePosition(P_QUEUE queue, int pos, void **out){
	if(IsEmptyQueue(queue)){
		return;
	}

	P_NODE delNode = queue->tail->before;
	P_NODE prev = queue->tail;

	// find target node
	for(int i = 0 ; i < pos ; i++){
		prev = delNode;
		delNode = delNode->before;
	}
	
	// save data to delete
	*out = delNode->data;

	prev->before = delNode->before;
	delNode->before->next = prev;
	free(delNode);

	queue->count -= 1;
}

void FreeQueue(P_QUEUE queue){
	if(queue == NULL){
		return;
	}

	P_NODE curr = queue->head;
	P_NODE temp;

	while(curr != NULL){
		temp = curr->next;
		free(curr);
		curr = temp;
	}

	free(queue);
}


const int colors[NUM_OF_COLORS] = {
	RED, GRN, YEL, BLU, MAG, CYN
};

void CreateProcessArr(){	
	process_arr = malloc(sizeof(PROCESS) * num_of_process);	
	
	for(int i = 0 ; i < num_of_process ; i++){
		process_arr[i].name = 'A' + i;
		process_arr[i].color = colors[i % NUM_OF_COLORS];
		process_arr[i].arrival = NOT_DEFINE;
		process_arr[i].service = NOT_DEFINE;
	}

	SetCursorVisibility(true);
	PrintWorkloadTable();

	for(int i = 0 ; i < num_of_process ; i++){
	 	gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH + 8, TABLE_TOP_ALIGN + (i * TABLE_HEIGHT) + 3);
		scanf("%d", &process_arr[i].arrival);
	 	gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH * 2 + 8, TABLE_TOP_ALIGN + (i * TABLE_HEIGHT) + 3);
		scanf("%d", &process_arr[i].service);
	}

	getchar();	
	InitSchedMenu();

	free(process_arr);
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

		if(key == ENTER){  /* Enter key pressed */
			if(menuIndex == EXIT){
				return;
			} else if(menuIndex > 0 && menuIndex < EXIT){
				break;
			}
		}
		
		if(key == ARROW && (key = getch()) == ARROW2){  /* Arrow key pressed */
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
		
		if(key == ENTER){  /* Enter key pressed */
			if(menuIndex == BACK_TO_MAIN){
				Init();
				return;
			} else if(menuIndex > 0 && menuIndex < BACK_TO_MAIN){	
				RunScheduling(menuIndex);	
			}
		}
		
		if(key == ARROW && (key = getch()) == ARROW2){  /* Arrow key pressed */
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
}


/*
 * create ready queue, sorting by arrival time
 * already sorted by name when the processes created
 */
void SortReadyQueueByArrivalTime(){
	PROCESS *sortedArr = malloc(sizeof(PROCESS) * num_of_process);	

	// copy array because we need original array to print out Workload Table
	for(int i = 0 ; i < num_of_process ; i++){
		sortedArr[i] = process_arr[i];
	}

	// sort by arrival time
	for(int i = 0 ; i < num_of_process - 1 ; i++){
		for(int j = i + 1 ; j < num_of_process ; j++){
			if(sortedArr[j].arrival < sortedArr[i].arrival){
				PROCESS temp = sortedArr[j];
				sortedArr[j] = sortedArr[i];
				sortedArr[i] = temp;
			}
		}
	}

	// insert array into ready queue 
	for(int i = 0 ; i < num_of_process ; i++){
		InsertQueue(readyQueue, &sortedArr[i]);
	}
}

void PrintResultQueue(){
	const int TOP_ALIGN = GetSchedTableTopAlign() + 2;
	const int LEFT_SPACE = 6;

	PrintSchedTable();

	SCHED_PROCESS *process = malloc(sizeof(SCHED_PROCESS));

	while(!IsEmptyQueue(resultQueue)){
		DeleteQueue(resultQueue, (void**)&process); 
		
		int now = process->start;
		int index = process->name - (int) 'A';
		
		while(now < (process->start + process->running)){
			gotoxy(TABLE_LEFT_ALIGN + (now * LEFT_SPACE), TOP_ALIGN + (TABLE_HEIGHT * index));
			printf("│");
			SetConsoleOutColor(process->color);
			printf("  %c  ", process->name);
			SetConsoleOutColor(RESET);
			printf("│");
			
			now++;
		}
	}
}

void RunScheduling(int index){
	readyQueue = NewQueue();	
	resultQueue = NewQueue();	
	
	SortReadyQueueByArrivalTime();	

	switch(index){
		case 1:
			FCFS();
			break;
	}

	PrintResultQueue();
	
	FreeQueue(readyQueue);
	FreeQueue(resultQueue);
}

SCHED_PROCESS* NewSchedProcess(PROCESS *source, int start, int running){
	SCHED_PROCESS *schedProc = malloc(sizeof(SCHED_PROCESS));
	
	schedProc->name = source->name;
	schedProc->color = source->color;
	schedProc->start = start;
	schedProc->running = running;

	return schedProc;
}

void FCFS(){
	// process to run
	PROCESS *process = malloc(sizeof(PROCESS));

	int now = 0;

	while(!IsEmptyQueue(readyQueue)){
		// get next process from ready queue
		DeleteQueue(readyQueue, (void **)&process);

		// wait until the process arrive
		while(now < process->arrival){
			now++;
		}

		// insert process into result queue
		InsertQueue(resultQueue, NewSchedProcess(process, now, process->service));

		// run until the process finished
		now += process->service;
	}
}


