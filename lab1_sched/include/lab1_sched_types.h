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
	true 	= 1,
	false 	= 0,
	TRUE 	= 1,
	FALSE 	= 0
} bool;

/*
 * process info
 */
typedef struct _process {
	char	name;
	int	color;
	int	arrival;
	int	service;
} process;

/*
 * process scheduling info
 */
typedef struct _sched_process {
	char	name;
	int	color;
	int	start;
	int	running;
	int	finish;
} sched_process;

typedef	struct _node {
	void		*data;
	struct	_node	*next;
	struct	_node	*before;
} node;

typedef struct _queue {
	node	*head;
	node	*tail;
	int	count;
} queue;

/*
 * move cursor position to (x, y)
 */
void gotoxy(int x, int y);

/*
 * change console output text color
 */
void SetConsoleOutColor(int color);

void SetCursorVisibility(bool visible);

/*
 * get console input charactor
 */
int getch();


/*
 * formatting output
 */
void PrintBoard();
void PrintProcessMenu();
void PrintWorkloadTable();
void PrintSchedMenu();
void PrintSchedTable();
void PrintSelectionBox(int index);
void EraseSelectionBox(int index);
void FindSelectionBoxPosition(int index);
int GetSchedTableTopAlign();


/*
 * init menu
 */
void Init();
void InitSchedMenu();
void CreateProcessArr();	


/*
 * managing queue
 */
queue* NewQueue();
bool IsEmptyQueue(queue *q);
void InsertQueue(queue *q, void *data);
void DeleteQueue(queue *q, void **out);
void DeleteQueuePosition(queue *q, int pos, void **out);
void FreeQueue(queue *q);


/* 
 * init scheduling 
 */
sched_process* NewSchedProcess(process *source, int start, int running);

void SortReadyQueueByArrivalTime();
void RunScheduling(int num);
void PrintResultQueue();


/*
 * scheduling algorithms 
 */
void FCFS();

#endif /* LAB1_HEADER_H*/



