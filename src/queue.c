#include "bootpack.h"
/*
	Define FLAGS_OVERRUN = 0x0001
	Front  
	|_|_|_|_|_|_|_|
	  Rear
	( 1 element contained Now )
*/
void queue8_init ( struct Queue8* queue , int size , unsigned int* data, struct TASK *task )
{
	queue->size = size;
	queue->data = data;	 
	queue->flags = 0;
	queue->front = 0;
	queue->rear = 0;
	queue->task = task;
	return ;
}

int queue8_put ( struct Queue8* queue , unsigned int data )
{
	if ( isOverRun (queue) && (queue->front==queue->rear) ) return -1;
	
	queue->data[queue->rear] = data;
	queue->rear++;
	if ( queue->rear == queue->size ) queue->rear = 0;
	if ( queue->rear == queue->front ) queue->flags |= FLAGS_OVERRUN;

	/*task ... */
	if ( queue->task != 0)
		if ( queue->task->flags != 2)
			task_run(queue->task);	

	return 0;
}

int queue8_get ( struct Queue8* queue )
{
	int data;
	if ( !isOverRun (queue) && (queue->front==queue->rear) ) return -1;
	data = queue->data[queue->front];
	queue->front++;
	if ( queue->front == queue->size ) queue->front = 0;
	queue->flags &= !FLAGS_OVERRUN;
	return data;
}

int queue8_status ( struct Queue8* queue )
{
	int status;
	if ( isOverRun(queue) ) return queue->size;
	status = queue->rear - queue->front;
	if ( status < 0 ) 
		status += queue->size;
	return status;		
}

int queue8_free ( struct Queue8* queue )
{	
	return queue->size - queue8_status( queue );
}

int isOverRun ( struct Queue8* queue )
{
	return queue->flags & FLAGS_OVERRUN;
}
