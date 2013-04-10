#include "bootpack.h"
extern struct Queue8 keyinfo;
extern struct Queue8 mouseinfo;
void init_pic (void)
{
	io_out8(PIC0_IMR , 0xff );
	io_out8(PIC1_IMR , 0xff );

	io_out8(PIC0_ICW1, 0x11 );
	io_out8(PIC0_ICW2, 0x20 );
	io_out8(PIC0_ICW3, 0x08 );
	io_out8(PIC0_ICW4, 0x01 );

	io_out8(PIC1_ICW1, 0x11 );
	io_out8(PIC1_ICW2, 0x28 );
	io_out8(PIC1_ICW3, 0x02 );
	io_out8(PIC1_ICW4, 0x01 );

	io_out8(PIC0_IMR , 0xfb );
	io_out8(PIC1_IMR , 0xff );
	
	return;
}

void inthandler21 ( int* esp )
{
	unsigned char data;
	io_out8( PIC0_OCW2, 0x61 );
	data = io_in8 ( PORT_KEYDAT );
	queue8_put( &keyinfo , data );	
	return ;
}

void inthandler2c ( int* esp )
{
	unsigned char data, s[4];
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	io_out8(PIC1_OCW2, 0x64);
	io_out8(PIC0_OCW2, 0x62);
	data = io_in8(PORT_KEYDAT);
	queue8_put( &mouseinfo , data );
	sprintf( s , "%02X", data );
}

void inthandler27 ( int* esp )
{
	io_out8(PIC0_OCW2, 0x67);
	return;
}


