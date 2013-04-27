#include "bootpack.h"

struct TIMERCTL timerctl;

void init_pit ( void )
{
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	timerctl.timeout = 0;
	return ;
}

void inthandler20(int *esp)
{
	io_out8(PIC0_OCW2, 0x60);
	timerctl.count ++ ;
	if (timerctl.timeout > 0) {
		timerctl.timeout -- ;
		if (timerctl.timeout == 0) {
			queue8_put (timerctl.queue, timerctl.data);
		}
	}
	return ;
}

void settimer (unsigned int timeout, struct Queue8 *queue, unsigned char data)
{
	int eflags;
	eflags = io_load_eflags();
	io_cli();
	timerctl.timeout = timeout;
	timerctl.data = data;
	io_store_eflags (eflags);
	return ;
}
