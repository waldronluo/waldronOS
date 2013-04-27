#include "bootpack.h"

struct Queue8 keyinfo;
void wait_KBC_sendready(void)
{
	while ( (io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) != 0 );
}

void init_keyboard(void)
{
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}
