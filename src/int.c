#include "bootpack.h"

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
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	unsigned char data, s[4];
	io_out8( PIC0_OCW2, 0x61 );
	data = io_in8 ( PORT_KEYDAT );
	sprintf( s , "%02X", data );
	boxfill8(binfo->vram , binfo->scrnx , find_palette( 0x00008484 ) , 0 , 0 , 15 , 15 );
	putfonts8_asc ( binfo->vram, binfo->scrnx, 0, 0, find_palette(0x00ffffff), s);
	return ;
}

void inthandler2c ( int* esp )
{
	struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
	init_screen( binfo->vram , binfo->scrnx , binfo->scrny);
	boxfill8(binfo->vram , binfo->scrnx , find_palette( 0x00000000 ) , 0 , 0 , 32*8-1 , 15 );
	putfonts8_asc ( binfo->vram, binfo->scrnx, 0, 0, find_palette(0x00ffffff), "INT 2C (IRQ-12):PS/2 keyboard");
	return;
}

void inthandler27 ( int* esp )
{
	io_out8(PIC0_OCW2, 0x67);
	return;
}


