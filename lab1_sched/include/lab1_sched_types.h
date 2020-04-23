/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 
*	    Student name : 
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H

/*
 * Boolean 
 */
#define TRUE 1
#define FALSE 0

/* 
 * Process
 */
typedef struct _PROCESS{
	char name;
	int color;
	int arrival;
	int service;
} PROCESS, * P_PROCESS;

void clear();
void gotoxy(int x, int y);
void setColor(int color);
void setCursorVisibility(int visible);
int getch();

void PrintBoard();
void PrintNumProcessMenu();
void PrintWorkloadTable();
void PrintSchedMenu();
void PrintSchedTable();
void PrintSelectionBox(int x, int y);
void EraseSelectionBox(int x, int y);
void FindSelectionBoxPosition(int num);
void PrintResult(P_PROCESS result);

void Init();
void InitSchedMenu();

void CreateProcess();
void RunScheduler(int num);
void SortByArrivalTime(P_PROCESS arr);

void FCFS(P_PROCESS arr, P_PROCESS result);

#endif /* LAB1_HEADER_H*/



