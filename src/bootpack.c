
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

void HariMain(void)
{
	struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
	char mcursor[17*17];	
	init_palette();
	init_screen( binfo->vram , binfo->scrnx, binfo->scrny );
	char* s;
//	putfont8( binfo->vram , binfo->scrnx , 50 , 50 , 4 , hankaku+'A'*16 );
//	putfont8( binfo->vram , binfo->scrnx , 40 , 40 , 12 ,A );
	putfonts8_asc( binfo->vram , binfo->scrnx , 8 , 8 , find_palette( 0x00ffffff ) , "ABC 123" );	
	sprintf(s , "scrnx = %d" , binfo->scrnx );
	putfonts8_asc ( binfo->vram , binfo->scrnx , 16 , 64 , find_palette( 0x00ffffff) , s );	
	init_mouse_curosr8 ( mcursor , find_palette(0x00008484) );
	putblock8_8 ( binfo->vram , binfo->scrnx , 16 , 16 , 160 , 100 , mcursor , 16 );
	for ( ;; )
		io_hlt();
}

void init_palette(void)
{
	set_palette( 0 , 15 , table_rgb );
	return ;
}

void set_palette( int start , int end , unsigned char* rgb )
{
	int i, eflags;
	eflags = io_load_eflags();
	io_cli();
	io_out8(0x03c8,start);
	for ( i = start ; i<= end ; i++ )
	{
		io_out8(0x03c9, rgb[0]/4);
		io_out8(0x03c9, rgb[1]/4);
		io_out8(0x03c9, rgb[2]/4);
		rgb += 3;
	}	
	io_store_eflags(eflags);
	return ;
}

void init_screen ( char* vram , int xsize , int ysize )
{

	boxfill8 ( vram, 320 , find_palette( 0x00008484)  , 0		,0			,xsize-1	,ysize-29 );	
	boxfill8 ( vram, 320 , find_palette( 0x00c6c6c6)  , 0		,ysize-28	,xsize-1	,ysize-28 );	
	boxfill8 ( vram, 320 , find_palette( 0x00ffffff)  , 0		,ysize-27	,xsize-1	,ysize-27 );	
	boxfill8 ( vram, 320 , find_palette( 0x00c6c6c6)  , 0		,ysize-26	,xsize-1	,ysize-1  );	
	
	boxfill8 ( vram, 320 , find_palette( 0x00ffffff)  , 3		,ysize-24	,59			,ysize-24 );	
	boxfill8 ( vram, 320 , find_palette( 0x00ffffff)  , 2		,ysize-24	,2			,ysize-4  );	
	boxfill8 ( vram, 320 , find_palette( 0x00848484)  , 3		,ysize-4	,59			,ysize-4  );	
	boxfill8 ( vram, 320 , find_palette( 0x00848484)  , 59		,ysize-23	,59			,ysize-5  );	
	boxfill8 ( vram, 320 , find_palette( 0x00000000)  , 2		,ysize-3	,59			,ysize-3  );	
	boxfill8 ( vram, 320 , find_palette( 0x00000000)  , 60		,ysize-24	,60			,ysize-3  );	
	
	boxfill8 ( vram, 320 , find_palette( 0x00848484)  , xsize-47,ysize-24	,xsize-4	,ysize-24 );	
	boxfill8 ( vram, 320 , find_palette( 0x00848484)  , xsize-47,ysize-23	,xsize-47	,ysize-4  );	
	boxfill8 ( vram, 320 , find_palette( 0x00ffffff)  , xsize-47,ysize-3	,xsize-4	,ysize-3  );	
	boxfill8 ( vram, 320 , find_palette( 0x00ffffff)  , xsize-3	,ysize-24	,xsize-3	,ysize-3  );	
}

void init_mouse_curosr8( char* mouse , char bc )
{
	int x,y;
	for ( y=0 ; y<16 ; y++ )
	{
		for ( x=0 ; x<16 ; x++ )
		{
			if ( cursor[y][x] == '*' ) 
				mouse[y*16 + x] = find_palette(0x00000000);
			if ( cursor[y][x] == 'O' )
				mouse[y*16 + x] = find_palette(0x00ffffff);
			if ( cursor[y][x] == '.' )
				mouse[y*16 + x] = bc;	
		}
	}
}

int find_palette( int color )
{
	unsigned char R,G,B;
	unsigned char i=0;
	R = (unsigned char)((color & 0x00ff0000)>>16);
	G = (unsigned char)((color & 0x0000ff00)>>8 );
	B = (unsigned char)((color & 0x000000ff)>>0 );
	for ( ; i<16 ; i++ )
	{
		if ( table_rgb[3*i] == R && table_rgb[3*i+1]==G && table_rgb[3*i+2]==B ) return i;
	}
	return i;
}


void boxfill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1 , int y1 )
{
	int x,y;
	for ( y = y0 ; y<=y1 ; y++ )
		for ( x = x0 ; x<=x1 ; x++ )
		{
			vram[y * xsize + x ] = c ;
		}
}
void putfont8 ( char *vram , int xsize, int x , int y , char c , char *font )
{
	int i;
	char *p , d;
	for ( i=0 ; i<16 ; i++ ) {
 		p = vram + (y+i) * xsize + x ;
		d = font[i];
		if ((d&0x80) != 0 ) { p[0] = c ; }
		if ((d&0x40) != 0 ) { p[1] = c ; }
		if ((d&0x20) != 0 ) { p[2] = c ; }
		if ((d&0x10) != 0 ) { p[3] = c ; }
		if ((d&0x08) != 0 ) { p[4] = c ; }
		if ((d&0x04) != 0 ) { p[5] = c ; }
		if ((d&0x02) != 0 ) { p[6] = c ; }
		if ((d&0x01) != 0 ) { p[7] = c ; }
	}
	return ;
}


void putfonts8_asc ( char *vram , int xsize , int x , int y , char c , unsigned char *s )
{
	for ( ; *s != 0x00 ; s++, x+= 8 )
		putfont8( vram , xsize , x, y, c,  hankaku + *s * 16 );	
}

void putblock8_8( char* vram , int vxsize , int pxsize , int pysize , int px0 , int py0 , char* buf , int bxsize )
{
	int x,y;
	for ( y=0 ; y<pysize ; y++ )
		for ( x=0 ; x<pxsize ; x++ )
			vram [ (py0 + y ) * vxsize + ( px0 + x ) ] = buf[y * bxsize + x ];
}
