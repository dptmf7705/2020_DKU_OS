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

#define MAX_process 6
#define NOT_DEFINE -1

int num_of_process = 0;

process *process_arr;

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
        printf("schedULING SIMULATOR by YESEUL LEE");
      	printf("  (2020-1 OPERATING SYSTEM LAB1)");	
}

void PrintProcessMenu(){
	gotoxy(TEXT_LEFT_ALIGN - 2, TEXT_TOP_ALIGN);
	SetConsoleOutColor(BLU);
	printf("How many process?");
	SetConsoleOutColor(RESET);

	for(int i = 2 ; i <= MAX_process ; i++){
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

void DeleteQueue(queue *q, void **out){
	if(IsEmptyQueue(q)) 
		return;

	node *delNode = q->head->next;

	// save data to delete
	*out = delNode->data;	

	q->head->next = delNode->next;
	delNode->next->before = q->head;
	free(delNode);

	q->count -= 1;
}

void DeleteQueuePosition(queue *q, int pos, void **out){
	if(IsEmptyQueue(q))
		return;

	node *delNode = q->head->next;
	node *prev = q->head;

	// find target node
	for(int i = 0 ; i < pos ; i++){
		prev = delNode;
		delNode = delNode->next;
	}
	
	// save data to delete
	*out = delNode->data;

	prev->next = delNode->next;
	delNode->next->before = prev;
	free(delNode);

	q->count -= 1;
}

void FreeQueue(queue *q){
	if(q == NULL)
		return;

	node *curr = q->head;
	node *temp;

	while(curr != NULL){
		temp = curr->next;
		free(curr->data);
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
 * create ready queue, sorting by arrival time
 * already sorted by name when the processes created
 */
void SortReadyQueueByArrivalTime(){
	process *sortedArr = malloc(sizeof(process) * num_of_process);	

	// copy array (original array is needed to print workload table)
	for(int i = 0 ; i < num_of_process ; i++){
		sortedArr[i] = process_arr[i];
	}

	// sort by arrival time
	for(int i = 0 ; i < num_of_process - 1 ; i++){
		for(int j = i + 1 ; j < num_of_process ; j++){
			if(sortedArr[j].arrival < sortedArr[i].arrival){
				process temp = sortedArr[j];
				sortedArr[j] = sortedArr[i];
				sortedArr[i] = temp;
			}
		}
	}

	// insert array into ready queue 
	for(int i = 0 ; i < num_of_process ; i++){
		InsertQueue(ready_queue, &sortedArr[i]);
	}
}

void PrintResultQueue(){
	const int TOP_ALIGN = GetSchedTableTopAlign() + 2;
	const int LEFT_SPACE = 6;

	PrintSchedTable();

	sched_process *proc = malloc(sizeof(sched_process));

	while(!IsEmptyQueue(result_queue)){
		DeleteQueue(result_queue, (void**)&proc); 
		
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
	ready_queue = NewQueue();	
	result_queue = NewQueue();	
	
	SortReadyQueueByArrivalTime();	

	switch(index){
		case 1:
			FCFS();
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

void FCFS(){
	// process to run
	process *proc = malloc(sizeof(process));

	int now = 0;

	while(!IsEmptyQueue(ready_queue)){
		// get next process from ready queue
		DeleteQueue(ready_queue, (void **)&proc);

		// wait until the process arrive
		while(now < proc->arrival){
			now++;
		}

		// insert process into result queue
		InsertQueue(result_queue, NewSchedProcess(proc, now, proc->service));

		// run until the process finished
		now += proc->service;
	}
}


