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

typedef struct _PROCESS{
	char name;
	int color;
	int arrival;
	int service;
} PROCESS, * P_PROCESS;

typedef struct _PROCESS_LIST_NODE{
	PROCESS process;
	int start;
	int running;
	struct _PROCESS_LIST_NODE* next;
	struct _PROCESS_LIST_NODE* before;
} PROCESS_LIST_NODE, * P_PROCESS_LIST_NODE;

void gotoxy(int x, int y);
void SetConsoleOutColor(int color);
void SetCursorVisibility(int visible);

int getch();

void PrintBoard();
void PrintProcessMenu();
void PrintWorkloadTable();
void PrintSchedMenu();
void PrintSchedTable();
void PrintSelectionBox(int index);
void EraseSelectionBox(int index);
void FindSelectionBoxPosition(int index);

void QueueInsert();
void QueueDelete();
void QueueSearch();

void PrintResult(P_PROCESS result);

void Init();
void InitSchedMenu();

void CreateProcess();
void RunScheduler(int num);
void SortByArrivalTime(P_PROCESS arr);

void FCFS(P_PROCESS arr, P_PROCESS result);

#endif /* LAB1_HEADER_H*/



