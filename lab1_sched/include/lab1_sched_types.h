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
 * queue node
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
 * I/O
 */
void gotoxy(int x, int y);
void setColor(color c);
void setCursorVisibility(bool visible);
void printBoard();
void printProcMenu();
int getWorkloadPosY();
void printWorkload();
void printSchedMenu();
void printSchedTable();
void findSelectionBoxPosition(int index);
void printSelectionBox(int index);
void eraseSelectionBox(int index);
void printSchedName();
void printResultQueue();
void printResultMetrics();
void inputMenu(int *index, const int bound);
void inputWorkload();
int inputTimeQuantum();
void init();
void initSchedMenu();
int getch();

/*
 * queue
 */
queue* newQueue();
bool isEmptyQueue(queue *q);
void insertQueue(queue *q, void *data);
void deleteQueue(queue *q, void **data);
void deleteQueueNode(queue *q, node *node, void **data);
void freeQueue(queue *q);

/* 
 * process 
 */
void initProc(process *proc);
void createProc(process *proc, char n, int c);
void createProcArr();

/*
 * scheduling
 */
void initScheduling(int index);
void freeResources();
void runScheduling(int index);
void updateReadyQueue(int now);
void updatePeriodReadyQueue(int now);
bool isAllProcFinish();
int waitForProcArrival(int *now);
int waitForProcPeriod(int *now);
sched_process* newSchedProcess(process *src, int s, int r);
void schedule(process *proc, int *now, const int t_while);

/*
 * scheduling algorithms
 */
// FCFS
void FCFS();
// RR
void RR(const int t_quantum);
// SJF
node* getShortestJobNode();
void SJF();
// HRRN
float getResponseRatio(int now, process *proc);
node* getHighestRRNode(int now);
void HRRN();
// MLFQ
int findNotEmptyQueueLevel(int *now);
void increaseReadyQueue();
int POW(int a, int b);
void MLFQ(const feedback_type type, const int t_quantum);
// RM
node* getLeastPeriodNode();
void RM();
// STRIDE
int GCD(int a, int b);
int LCM(int a, int b);
int getLcmFromReadyQueue();
int getStrideSum();
void STRIDE();


#endif /* LAB1_HEADER_H*/



