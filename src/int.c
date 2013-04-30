#include "bootpack.h"
#include <stdio.h>
extern struct Queue8 inputData;
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
	int data;
	io_out8( PIC0_OCW2, 0x61 );
	data = io_in8 ( PORT_KEYDAT );
	queue8_put( &inputData , data + KEYDATA0 );	
	return ;
}

void inthandler2c ( int* esp )
{
	int data;
	//struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	io_out8(PIC1_OCW2, 0x64);
	io_out8(PIC0_OCW2, 0x62);
	data = io_in8(PORT_KEYDAT);
	queue8_put( &inputData , data + MOUSEDATA0 );
}

void inthandler27 ( int* esp )
{
	io_out8(PIC0_OCW2, 0x67);
	return;
}


