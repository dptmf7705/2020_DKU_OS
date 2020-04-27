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

typedef enum {
	RED	= 31,
	GRN	= 32,
	YEL	= 33,
	BLU	= 34,
	MAG	= 35,
	CYN	= 36,
	RESET	= 0,
	NUM_OF_COLORS	= 6
} color;

typedef enum {
	TYPE_DEFAULT,
	TYPE_MULTIPLE
} MLFQ_TYPE;

/*
 * process info
 */
typedef struct _process {
	char	name;
	color	textColor;
	int	arrival;
	int	service;
	int	start;
	int	remain;
	int	finish;
} process;

/*
 * scheduled process info
 */
typedef struct _sched_process {
	char	name;
	color	textColor;
	int	start;
	int	running;
} sched_process;

/*
 * process queue node
 */
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


void gotoxy(int x, int y);
void SetConsoleOutColor(color color);
void SetCursorVisibility(bool visible);

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
void SortProcessArrByArrivalTime();

int getch();

/*
 * managing queue
 */
queue* NewQueue();
bool IsEmptyQueue(queue *q);
void* getFromQueue(queue *q, int pos);
void InsertQueue(queue *q, void *data);
void DeleteQueue(queue *q, void **data);
void DeleteQueuePosition(queue *q, int pos, void **data);
void DeleteQueueNode(queue *q, node *node, void **data);
void FreeQueue(queue *q);

/* 
 * init scheduling 
 */
void RunScheduling(int num);
void UpdateReadyQueue(int now);
void UpdateReadyQueueTimeout(process *proc);
void PrintResultQueue();
sched_process* NewSchedProcess(process *source, int start, int running);

/*
 * scheduling algorithms 
 */
void FCFS();
void RR(const int t_quantum);
void SJF();
void HRRN();
void MLFQ(const MLFQ_TYPE type, const int t_quantum);

node* GetShortestProcNodeInReadyQueue();

int Pow(int a, int b);

#endif /* LAB1_HEADER_H*/



