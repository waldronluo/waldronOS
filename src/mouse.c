#include "bootpack.h"

struct MOUSE_DEC mdec;


void enable_mouse(void)
{
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	return;
}

void init_mouse (struct MOUSE_DEC* mdec )
{
	mdec->phase = 0;
}

int mouse_decode ( struct MOUSE_DEC* mdec, unsigned char data )
{
	switch ( mdec->phase )
	{
		case 0: if ( data == 0xfa ) 
				mdec->phase = 1;break;
		case 1: if ( (data&0xc8) == 0x08 )
			{//Check if it's not error
				mdec->phase = 2; 
				mdec->buf[0] = data; 
				break;
			}
		case 2: 	mdec->phase = 3; mdec->buf[1] = data; break;
		case 3:		mdec->phase = 1; mdec->buf[2] = data;
				mdec->btn = mdec->buf[0] & 0x07;
				mdec->x = mdec->buf[1];
				mdec->y = mdec->buf[2];
				
				if ((mdec->buf[0] & 0x10) != 0 )
					mdec->x |= 0xffffff00;
				if ((mdec->buf[0] & 0x20) != 0 )
					mdec->y |= 0xffffff00;
				mdec->y = - mdec->y ;
				return 1;
		default: return -1;				
	}
	return 0;
}
