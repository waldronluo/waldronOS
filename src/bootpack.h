
#include <stdio.h>
//extern char hankaku[4096];
#include "hankaku.h"
static unsigned char table_rgb[16 * 3] = {
		0x00,0x00,0x00,
		0xff,0x00,0x00,
		0x00,0xff,0x00,
		0xff,0xff,0x00,
		0x00,0x00,0xff,
		0xff,0x00,0xff,
		0x00,0xff,0xff,
		0xff,0xff,0xff,
		0xc6,0xc6,0xc6,
		0x84,0x00,0x00,
		0x00,0x84,0x00,
		0x84,0x84,0x00,
		0x00,0x00,0x84,
		0x84,0x00,0x84,
		0x00,0x84,0x84,
		0x84,0x84,0x84,
		0x00,0x00,0x00,
};

static char cursor[17][17] = {
	"**************..",
	"*OOOOOOOOOOO*...",
	"*OOOOOOOOOO*....",
	"*OOOOOOOOO*.....",
	"*OOOOOOOO*......",
	"*OOOOOOO*.......",
	"*OOOOOO*........",
	"*OOOOOOO*.......",
	"*OOOOOOOO*......",
	"*OOOO*OOOO*.....",
	"*OOO*.*OOOO*....",
	"*OO*...*OOOO*...",
	"*O*.....*OOOO*..",
	"**.......*OOOO*.",
	"*.........*OOOO*",
	"...........*OOOO",
	"****************"
};

int fuck();
void io_hlt(void);
void io_cli(void);
void io_out8 ( int port , int data );
int io_load_eflags(void);
void io_store_eflags( int eflags );
void boxfill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1 , int y1 );
void putfont8 ( char *vram , int xsize, int x , int y , char c , char *font );
void putfonts8_asc ( char *vram , int xsize , int x , int y , char c , unsigned char *s );
void putblock8_8( char* vram , int vxsize , int pxsize , int pysize , int px0 , int py0 , char* buf , int bxsize );
void init_palette(void);
void set_palette(int start, int end, unsigned char* rgb);
int  find_palette( int color );
void write_mem8(int ,int );
void init_screen( char* , int, int );
void init_mouse_curosr8( char* mouse , char bc );

struct BOOTINFO {
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
 	char* vram;
};

