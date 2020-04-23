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

/*
 * Array of processes
 */
P_PROCESS p_process;

/*
 * Selection box position
 */
int p_pos[2] = {0, 0};

void clear(){
	system("clear");
}

/*
 * move cursor position to (x, y)
 */
void gotoxy(int x, int y){
	printf("\033[%d;%dH", y + 1, x + 1);
}

void setColor(int color){
	printf("\033[%d;1m", color);
}

void setCursorVisibility(int visible){
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
	setColor(RESET);
	puts("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	for(int i = 0 ; i < 44 ; i++){
		puts("┃                                                                                                                                                                                           ┃");
	}
	puts("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
	
	gotoxy(TEXT_LEFT_ALIGN + 30, LINE_SPACE);
        printf("SCHEDULING SIMULATOR by YESEUL LEE  (2020-1 OPERATING SYSTEM LAB1)");	
}

void PrintNumProcessMenu(){
	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN);
	setColor(BLU);
	printf("How many process?");
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

void PrintWorkloadTable(){
	gotoxy(TABLE_LEFT_ALIGN, TABLE_TOP_ALIGN - 1);
	puts("  Workload  ");

	gotoxy(TABLE_LEFT_ALIGN, TABLE_TOP_ALIGN);
	puts("┌────────────────┬────────────────┬────────────────┐");
	gotoxy(TABLE_LEFT_ALIGN, TABLE_TOP_ALIGN + 1);
	puts("│  Process Name  │  Arrival Time  │  Service Time  │");

	for(int i = 0 ; i < num_of_process ; i++){
		gotoxy(TABLE_LEFT_ALIGN, TABLE_TOP_ALIGN + (i * TABLE_HIGHT) + 2);
		puts("├────────────────┼────────────────┼────────────────┤");
		gotoxy(TABLE_LEFT_ALIGN, TABLE_TOP_ALIGN + (i * TABLE_HIGHT) + 3);
		puts("│                │                │                │");
		gotoxy(TABLE_LEFT_ALIGN + 8, TABLE_TOP_ALIGN + (i * TABLE_HIGHT) + 3);
		printf("%c", p_process[i].name);
		if(p_process[i].arrival > -1 && p_process[i].service > -1){
	 		gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH + 8, TABLE_TOP_ALIGN + (i * TABLE_HIGHT) + 3);
			printf("%d", p_process[i].arrival);
	 		gotoxy(TABLE_LEFT_ALIGN + TABLE_WIDTH * 2 + 8, TABLE_TOP_ALIGN + (i * TABLE_HIGHT) + 3);
			printf("%d", p_process[i].service);
		}
	}

	gotoxy(TABLE_LEFT_ALIGN, TABLE_TOP_ALIGN + ((num_of_process + 1) * TABLE_HIGHT));
	puts("└────────────────┴────────────────┴────────────────┘");
}

void PrintSchedMenu(){
	clear();
	PrintBoard();
	PrintWorkloadTable();

	gotoxy(TEXT_LEFT_ALIGN, TEXT_TOP_ALIGN);
	setColor(BLU);
	printf("Choose Scheduling Algorithm");
	setColor(RESET);
	
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
	setColor(RED);
	printf("BACK TO MAIN");
	setColor(RESET);
}

void PrintSelectionBox(int x, int y){
	gotoxy(x, y);
	puts("┌─────────────────────────┐");
	gotoxy(x, y + 1);
	puts("│");
	gotoxy(x + SELECTION_BOX_WIDTH, y + 1);	
	puts("│");
	gotoxy(x, y + 2);
	puts("└─────────────────────────┘");
}

void EraseSelectionBox(int x, int y){
	gotoxy(x, y);
	puts("                            ");
	gotoxy(x, y + 1);
	puts(" ");
	gotoxy(x + SELECTION_BOX_WIDTH, y + 1);	
	puts(" ");
	gotoxy(x, y + 2);
	puts("                            ");
}

void FindSelectionBoxPosition(int num){
	p_pos[0] = SELECTION_BOX_LEFT_ALIGN;
	p_pos[1] = TEXT_TOP_ALIGN - 1 + (num * LINE_SPACE);
}

void Init(){
	clear();
	setCursorVisibility(FALSE);
	PrintBoard();
	PrintNumProcessMenu();

	int menuNum = 1;
        int key;

	FindSelectionBoxPosition(menuNum);
	PrintSelectionBox(p_pos[0], p_pos[1]);

	while(1){
		key = getch();

		if(key == ENTER){		// ENTER Key Pressed
			if(menuNum == 6){	// EXIT
				return;
			} else if(menuNum >= 1 && menuNum <= 5){
				break;
			}
		}
		
		if(key == ARROW && (key = getch()) == ARROW_SND){
			key = getch();

			if(key == UP && menuNum > 1){
				FindSelectionBoxPosition(menuNum);
				EraseSelectionBox(p_pos[0], p_pos[1]);
				menuNum--;
				FindSelectionBoxPosition(menuNum);
				PrintSelectionBox(p_pos[0], p_pos[1]);
			} else if(key == DOWN && menuNum < 6){
				FindSelectionBoxPosition(menuNum);
				EraseSelectionBox(p_pos[0], p_pos[1]);
				menuNum++;
				FindSelectionBoxPosition(menuNum);
				PrintSelectionBox(p_pos[0], p_pos[1]);	
			}
		}
	}
	
	num_of_process = menuNum + 1;

	CreateProcess();	
}

void InitSchedMenu(){
	clear();
	setCursorVisibility(FALSE);
	PrintBoard();
	PrintWorkloadTable();
	PrintSchedMenu();

	int menuNum = 1;
        int key;

	FindSelectionBoxPosition(menuNum);
	PrintSelectionBox(p_pos[0], p_pos[1]);
	
	while(1){
		key = getch();
		
		if(key == ENTER){		// ENTER Key Pressed
			if(menuNum == 8){	// BACK TO MAIN
				Init();
				return;
			} else if(menuNum >= 1 && menuNum <= 7){	
				RunScheduler(menuNum);	
			}
		}
		
		if(key == ARROW && (key = getch()) == ARROW_SND){
			key = getch();

			if(key == UP && menuNum > 1){
				FindSelectionBoxPosition(menuNum);
				EraseSelectionBox(p_pos[0], p_pos[1]);
				menuNum--;
				FindSelectionBoxPosition(menuNum);
				PrintSelectionBox(p_pos[0], p_pos[1]);
			} else if(key == DOWN && menuNum < 8){
				FindSelectionBoxPosition(menuNum);
				EraseSelectionBox(p_pos[0], p_pos[1]);
				menuNum++;
				FindSelectionBoxPosition(menuNum);
				PrintSelectionBox(p_pos[0], p_pos[1]);
			}
		}
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

/*
 * create new processes
 */
void CreateProcess(){
	const int colors[NUM_OF_COLORS] = {RED, GRN, YEL, BLU, MAG, CYN};
	
	p_process = malloc(sizeof(PROCESS) * num_of_process);	
	for(int i = 0 ; i < num_of_process ; i++){
		p_process[i].name = 'A' + i;
		p_process[i].color = colors[i % NUM_OF_COLORS];
		p_process[i].arrival = -1;
		p_process[i].service = -1;
	}

	setCursorVisibility(TRUE);
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

void PrintSchedTable(){
	gotoxy(TABLE_LEFT_ALIGN, TABLE_TOP_ALIGN + ((num_of_process + 2) * TABLE_HIGHT) + LINE_SPACE);
	puts("0                             5                             10                            15                            20");

	for(int i = 0 ; i < num_of_process ; i++){
		gotoxy(TABLE_LEFT_ALIGN, TABLE_TOP_ALIGN + ((num_of_process + 2 + i) * TABLE_HIGHT) + LINE_SPACE + 1);
		puts("├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤");
		gotoxy(TABLE_LEFT_ALIGN, TABLE_TOP_ALIGN + ((num_of_process + 2 + i) * TABLE_HIGHT) + LINE_SPACE + 2);
		puts("│                             │                             │                             │                             │");	
		gotoxy(TABLE_LEFT_ALIGN - 2, TABLE_TOP_ALIGN + ((num_of_process + 2 + i) * TABLE_HIGHT) + LINE_SPACE + 2);
		setColor(p_process[i].color);
		printf("%c", p_process[i].name);
		setColor(RESET);
	}
	
	gotoxy(TABLE_LEFT_ALIGN, TABLE_TOP_ALIGN + ((num_of_process * 2 + 2) * TABLE_HIGHT) + LINE_SPACE + 1);
	puts("├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤");
}

void PrintResult(P_PROCESS result){
	for(int i = 0 ; i < total_service_time ; i++){
		for(int j = 0 ; j < num_of_process ; j++){
			gotoxy(TABLE_LEFT_ALIGN + (i * 6), TABLE_TOP_ALIGN + ((num_of_process + 2 + j) * TABLE_HIGHT) + LINE_SPACE + 2);
			printf("│");
			if(result[i].name == p_process[j].name){
				setColor(result[i].color);
				printf("  %c  ", result[i].name);
				setColor(RESET);
			} else {
				printf("     ");
			}
			printf("│");
		}
	}
}

void RunScheduler(int num){	
	PrintSchedTable();
	
	P_PROCESS result = malloc(sizeof(PROCESS) * total_service_time);	
	P_PROCESS sortedArr = malloc(sizeof(PROCESS) * num_of_process);
	
	SortByArrivalTime(sortedArr);

	switch(num){
		case 1:
			FCFS(sortedArr, result);
			break;
	}

	PrintResult(result);

	free(sortedArr);	
	free(result);
}


void FCFS(P_PROCESS pros, P_PROCESS result){
	int current = 0;
	int i = 0;

	while(i < num_of_process){
		// if the process doesn't arrive yet 
		if(pros[i].arrival > current){
			current++;	
			continue;
		}

		// run until the process finish
		for(int j = 0 ; j < pros[i].service ; j++){	
			// put the process into the result arr
			result[current++] = pros[i];
		}

		// move on to the next process
		i++;
	} 
}

