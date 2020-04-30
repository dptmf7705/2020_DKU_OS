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
	FEEDBACK_DEFAULT,
	FEEDBACK_MULTIPLE
} feedback_type;

typedef enum {
	WORKLOAD_DEFAULT,
	WORKLOAD_PERIOD,
	WORKLOAD_TICKET
} workload_type;

/*
 * process info
 */
typedef struct _process {
	char	name;
	color	color;
	int	arrival;
	int	service;
	int	period;
	int	tickets;
	int	start;
	int	remain;
	int	finish;
} process;

/*
 * scheduled process info
 */
typedef struct _sched_process {
	char	name;
	color	color;
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


/*
 * formatting output
 */
void printBoard();
void printProcessMenu();
void printWorkloadTable();
void printSchedMenu();
void printSchedTable();
void printSelectionBox(int index);
void eraseSelectionBox(int index);
void findSelectionBoxPosition(int index);
int getTablePosY();

/*
 * init menu
 */
void init();
void initSchedMenu();
void createProcArr();	
void sortProcessArrByArrivalTime();

/*
 * managing queue
 */
queue* newQueue();
bool isEmptyQueue(queue *q);
void* getFromQueue(queue *q, int pos);
void insertQueue(queue *q, void *data);
void deleteQueue(queue *q, void **data);
void deleteQueuePosition(queue *q, int pos, void **data);
void deleteQueueNode(queue *q, node *node, void **data);
void freeQueue(queue *q);

/* 
 * init scheduling 
 */
void runScheduling(int num);
void updateReadyQueue(int now);
void updatePeriodReadyQueue(int now);
void waitIfReadyQueueEmpty(int *now);
void updateReadyQueueTimeout(process *proc);
void printResultQueue();
sched_process* newSchedProcess(process *source, int start, int running);

bool beginScheduling();
/*
 * scheduling algorithms 
 */
void FCFS();
void RR(const int t_quantum);
void SJF();
void HRRN();
void MLFQ(const feedback_type type, const int t_quantum);
void RM();


int getLcmFromReadyQueue();
int getStrideSum();
void STRIDE();

node* getShortestProcNodeInReadyQueue();

void gotoxy(int x, int y);
void setColor(color c);
void setCursorVisibility(bool visible);

int getch();

int POW(int a, int b);
int GCD(int a, int b);
int LCM(int a, int b);

#endif /* LAB1_HEADER_H*/



