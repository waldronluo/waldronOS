#include "bootpack.h"

struct TIMERCTL timerctl;

void init_pit ( void )
{
	int i;
	struct TIMER *t;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	timerctl.next = 0xffffffff;

	for ( i=0 ; i<MAX_TIMER ; i++ )	
		timerctl.timers0[i].flags = 0;
		
	t = timer_alloc();
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0;
	timerctl.t0 = t;
	timerctl.next = t->timeout;
//	timerctl.using = 1;
	return ;
}
struct TIMER *timer_alloc (void)
{
	int i;
	for ( i=0 ; i< MAX_TIMER ; i++ ) {
		if ( timerctl.timers0[i].flags == 0 ) {
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
            timerctl.timers0[i].flags2 = 0;
			return &timerctl.timers0[i];
		}
	}
	return 0;
}

void timer_free ( struct TIMER *timer )
{
	timer->flags = 0;
	return ;
}
void timer_init ( struct TIMER *timer, struct Queue8 *queue, unsigned int data )
{
	timer->queue = queue;
	timer->data = data;
	return; 
}

void timer_settimer ( struct TIMER *timer, unsigned int timeout )
{
	int e;
	struct TIMER *t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
//	timerctl.using ++;

	t = timerctl.t0;
	if ( timer->timeout <= t->timeout ) {
	/* Inserting to the fisrt place*/
		timerctl.t0 = timer;
		timer->next = t;
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return ;
	}
	while (1) {
		s = t;
		t = t->next;
		if ( timer->timeout <= t->timeout ) {
		/*between t and s */
			s->next = timer;
			timer->next = t;
			io_store_eflags(e);
			return ;
		}
	}
	return ;	
}

void inthandler20(int *esp)
{
	char ts = 0;
	struct TIMER *timer;
	io_out8(PIC0_OCW2, 0x60);
	timerctl.count ++ ;
	if ( timerctl.next > timerctl.count ) 
		return;
	
	timer = timerctl.t0;
	while (1) {
		if ( timer->timeout > timerctl.count )
			break;
		timer->flags = TIMER_FLAGS_ALLOC;
		if ( timer != task_timer ) {
			queue8_put ( timer->queue, timer->data );
		} else {
			ts = 1;
		}		
		timer = timer->next;
	}
	
	timerctl.t0 = timer;
	timerctl.next = timerctl.t0->timeout;
	if ( ts != 0 )
		task_switch();
	return ;
}

int timer_cancel (struct TIMER *timer)
{
    int e;
    struct TIMER *t;
    e = io_load_eflags();
    io_cli();
    if (timer->flags == TIMER_FLAGS_USING) {
        if (timer == timerctl.t0) {
            t = timer->next;

            timerctl.t0 = t;
            timerctl.next = t->timeout;
        } else {
            t = timerctl.t0;
            for (;;) {
                if (t->next == timer) break;
                t = t->next;
            }
            t->next = timer->next;
        }
        timer->flags = TIMER_FLAGS_ALLOC;
        io_store_eflags(e);
        return 1;
    }
    io_store_eflags(e);
    return 0;
}

void timer_cancelall (struct Queue8* queue) 
{
    int e,i;
    struct TIMER *t;
    e = io_load_eflags();
    io_cli();
    for (i = 0; i < MAX_TIMER ; i ++ ) {
        t = &timerctl.timers0[i];
        if (t->flags != 0 && t->flags2 != 0 && t->queue == queue) {
            timer_cancel(t);
            timer_free(t);
        }
    }
    io_store_eflags (e);
    return;
}
