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

#define TEXT_LEFT_ALIGN 15
#define TEXT_TOP_ALIGN 9
#define LINE_SPACE 3
#define SELECTION_BOX_LEFT_ALIGN 12
#define SELECTION_BOX_WIDTH 26
#define TABLE_LEFT_ALIGN 50
#define TABLE_TOP_ALIGN 9
#define TABLE_WIDTH 17
#define TABLE_HIGHT 2

#define ENTER 10
#define ARROW 27
#define ARROW_SND 91
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


int num_of_process = 0;
int total_service_time = 0;

P_PROCESS p_process;

/*
 * move cursor position to (x, y)
 */
void gotoxy(int x, int y){
	printf("\033[%d;%dH", y + 1, x + 1);
}

void SetConsoleOutColor(int color){
	printf("\033[%d;1m", color);
}

void SetCursorVisibility(int visible){
	printf("\e[?25%c", visible ==  TRUE ? 'h' : 'l');
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


void PrintBoard(){
	gotoxy(0, 0);
	SetConsoleOutColor(RESET);
	puts("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	for(int i = 0 ; i < 44 ; i++){
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
		printf("%c", p_process[i].name);
		if(p_process[i].arrival > -1 && p_process[i].service > -1){
	 		gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH + 8, posY);
			printf("%d", p_process[i].arrival);
	 		gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH * 2 + 8, posY);
			printf("%d", p_process[i].service);
		}
	}

	gotoxy(TABLE_LEFT_ALIGN, ++posY);
	puts("└────────────────┴────────────────┴────────────────┘");
}

void PrintSchedMenu(){
	system("clear");
	PrintBoard();
	PrintWorkloadTable();

	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN);
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
	return TABLE_TOP_ALIGN + ((num_of_process + 2) * TABLE_HIGHT) + LINE_SPACE;
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
		SetConsoleOutColor(p_process[i].color);
		printf("%c", p_process[i].name);
		SetConsoleOutColor(RESET);
	}
	
	gotoxy(TABLE_LEFT_ALIGN, ++posY);
	puts("├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤");
}


int pos[2] = {0, 0};

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

void FindSelectionBoxPosition(int index){
	pos[0] = SELECTION_BOX_LEFT_ALIGN;
	pos[1] = TEXT_TOP_ALIGN - 1 + (index * LINE_SPACE);
}


void Init(){
	system("clear");
	SetCursorVisibility(FALSE);
	PrintBoard();
	PrintProcessMenu();

	int menuIndex = 1;
        int key;

	PrintSelectionBox(menuIndex);

	while(1){
		key = getch();

		if(key == ENTER){		// ENTER Key Pressed
			if(menuIndex == 6){	// EXIT
				return;
			} else if(menuIndex >= 1 && menuIndex <= 5){
				break;
			}
		}
		
		if(key == ARROW && (key = getch()) == ARROW_SND){
			key = getch();

			if(key == UP && menuIndex > 1){
				EraseSelectionBox(menuIndex);
				menuIndex--;
				PrintSelectionBox(menuIndex);
			} else if(key == DOWN && menuIndex < 6){
				EraseSelectionBox(menuIndex);
				menuIndex++;
				PrintSelectionBox(menuIndex);
			}
		}
	}
	
	num_of_process = menuIndex + 1;

	CreateProcess();	
}

void InitSchedMenu(){
	system("clear");
	SetCursorVisibility(FALSE);
	PrintBoard();
	PrintWorkloadTable();
	PrintSchedMenu();

	int menuIndex = 1;
        int key;

	PrintSelectionBox(menuIndex);
	
	while(1){
		key = getch();
		
		if(key == ENTER){		// ENTER Key Pressed
			if(menuIndex == 8){	// BACK TO MAIN
				Init();
				return;
			} else if(menuIndex >= 1 && menuIndex <= 7){	
				RunScheduler(menuIndex);	
			}
		}
		
		if(key == ARROW && (key = getch()) == ARROW_SND){
			key = getch();

			if(key == UP && menuIndex > 1){
				EraseSelectionBox(menuIndex);
				menuIndex--;
				PrintSelectionBox(menuIndex);
			} else if(key == DOWN && menuIndex < 8){
				EraseSelectionBox(menuIndex);
				menuIndex++;
				PrintSelectionBox(menuIndex);
			}
		}
	}
}

void NewQueue(P_PROCESS_LIST_NODE head, P_PROCESS_LIST_NODE tail){
	head->next = tail;
	head->before = NULL;
	tail->before = head;
	tail->next = NULL;
}

void QueueInsert(P_PROCESS_LIST_NODE head, PROCESS process, int start, int running){
	P_PROCESS_LIST_NODE newNode = malloc(sizeof(PROCESS_LIST_NODE));
	
	newNode->process = process;
	newNode->start = start;
	newNode->running = running;

	newNode->next = head->next;
	head->next = newNode;
	newNode->next->before = newNode;
	newNode->before = head;
}

void QueueDelete(P_PROCESS_LIST_NODE tail){
	P_PROCESS_LIST_NODE delNode = tail->before;

	tail->before = delNode->before;
	delNode->before->next = tail;
	free(delNode);
}

void FreeQueue(P_PROCESS_LIST_NODE head){
	P_PROCESS_LIST_NODE curr = head;
	P_PROCESS_LIST_NODE temp;

	while(curr != NULL){
		temp = curr->next;
		free(curr);
		curr = temp;
	}
}


const int colors[NUM_OF_COLORS] = {
	RED, GRN, YEL, BLU, MAG, CYN
};

void CreateProcess(){	
	p_process = malloc(sizeof(PROCESS) * num_of_process);	
	
	for(int i = 0 ; i < num_of_process ; i++){
		p_process[i].name = 'A' + i;
		p_process[i].color = colors[i % NUM_OF_COLORS];
		p_process[i].arrival = -1;
		p_process[i].service = -1;
	}

	SetCursorVisibility(TRUE);
	PrintWorkloadTable();

	for(int i = 0 ; i < num_of_process ; i++){
	 	gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH + 8, TABLE_TOP_ALIGN + (i * TABLE_HIGHT) + 3);
		scanf("%d", &p_process[i].arrival);
	 	gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH * 2 + 8, TABLE_TOP_ALIGN + (i * TABLE_HIGHT) + 3);
		scanf("%d", &p_process[i].service);
		total_service_time += p_process[i].service;
	}

	getchar();
	InitSchedMenu();

	free(p_process);
}

void RunScheduler(int num){	
	P_PROCESS sortedArr = malloc(sizeof(PROCESS) * num_of_process);
	SortByArrivalTime(sortedArr);

	switch(num){
		case 1:
			FCFS(sortedArr);
			break;
	}

	free(sortedArr);	
}

void PrintResultQueue(P_PROCESS_LIST_NODE head, P_PROCESS_LIST_NODE tail){
	PrintSchedTable();	
	const int TOP_ALIGN = GetSchedTableTopAlign() + 2;
	const int LEFT_MARGIN = 6;

	P_PROCESS_LIST_NODE curr = head->next;
	
	while(curr != NULL && curr != tail){
		int current = curr->start;
		int index = curr->process.name - (int) 'A';
		
		while(current < curr->start + curr->running){
			gotoxy(TABLE_LEFT_ALIGN + (current * LEFT_MARGIN), TOP_ALIGN + (TABLE_HIGHT * index));
			printf("│");
			SetConsoleOutColor(curr->process.color);
			printf("  %c  ", curr->process.name);
			SetConsoleOutColor(RESET);
			printf("│");

			current++;
		}

		curr = curr->next;
	}
}

void SortByArrivalTime(P_PROCESS result){
	for(int i = 0 ; i < num_of_process ; i++){
		result[i] = p_process[i];
	}

	for(int i = 0 ; i < num_of_process - 1 ; i++){
		for(int j = i + 1 ; j < num_of_process ; j++){
			if(result[j].arrival < result[i].arrival){
				PROCESS temp = result[j];
				result[j] = result[i];
				result[i] = temp;
			}
		}
	}
}

void FCFS(P_PROCESS pros){
	P_PROCESS_LIST_NODE head = malloc(sizeof(PROCESS_LIST_NODE));
	P_PROCESS_LIST_NODE tail = malloc(sizeof(PROCESS_LIST_NODE));
	
	NewQueue(head, tail);

	for(int now = 0, i = 0 ; i < num_of_process ; i++){
		while(now < pros[i].arrival){
			now++;
		}
		QueueInsert(head, pros[i], now, pros[i].service);
		now += pros[i].service;
	}

	PrintResultQueue(head, tail);
	FreeQueue(head);
}

void SPN(P_PROCESS pros, P_PROCESS result){
	int current = 0;
	int i = 0;

	while(i < num_of_process){
		if(pros[i].arrival > current){
			current++;
			continue;
		}
		
		// check if short process exists 
		for(int j = i + 1 ; j < num_of_process ; j++){
			if(pros[j].arrival <= current){

			}	
		}


		for(int j = 0 ; j < pros[i].service ; j++){

		}		
	}
}


