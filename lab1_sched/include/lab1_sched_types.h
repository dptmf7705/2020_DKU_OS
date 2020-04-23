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

void Init();
void InitSchedMenu();

void QueueInsert(P_PROCESS_LIST_NODE head, PROCESS data, int start, int running);
void QueueDelete(P_PROCESS_LIST_NODE tail);
void FreeQueue(P_PROCESS_LIST_NODE head);

void CreateProcess();
void RunScheduler(int num);

void PrintResultQueue(P_PROCESS_LIST_NODE head, P_PROCESS_LIST_NODE tail);
void SortByArrivalTime(P_PROCESS arr);

void FCFS(P_PROCESS pros);

#endif /* LAB1_HEADER_H*/



