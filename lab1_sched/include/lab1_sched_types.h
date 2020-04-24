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
 * boolean type 
 */
typedef enum {
	true = 1,
	false = 0,
	TRUE = 1,
	FALSE = 0
} bool;

typedef struct {
	char name;
	int color;
	int arrival;
	int service;
} PROCESS;

typedef struct {
	char name;
	int color;
	int start;
	int running;
	int finish;
} SCHED_PROCESS;

typedef struct _NODE{
	void* data;
	struct _NODE* next;
	struct _NODE* before;
} NODE, * P_NODE;

typedef struct _QUEUE{
	P_NODE head; // insert
	P_NODE tail; // delete
	int count;
} QUEUE, * P_QUEUE;

void gotoxy(int x, int y);
void SetConsoleOutColor(int color);
void SetCursorVisibility(bool visible);

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

P_QUEUE NewQueue();
bool IsEmptyQueue(P_QUEUE queue);
void InsertQueue(P_QUEUE queue, void *data);
void DeleteQueue(P_QUEUE queue, void **out);
void DeleteQueuePosition(P_QUEUE queue, int pos, void **out);
void FreeQueue(P_QUEUE queue);

void CreateProcessArr();	
void SortReadyQueueByArrivalTime();
void RunScheduling(int num);

int GetSchedTableTopAlign();
void PrintResultQueue();

SCHED_PROCESS* NewSchedProcess(PROCESS *source, int start, int running);

void FCFS();

#endif /* LAB1_HEADER_H*/



