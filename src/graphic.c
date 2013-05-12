
#include "bootpack.h"
#include "hankaku.h"

static unsigned char closebtn[14][16] = {
	"OOOOOOOOOOOOOOO@",
	"OQQQQQQQQQQQQQ$@",
	"OQQQQQQQQQQQQQ$@",
	"OQQQ@@QQQQ@@QQ$@",
	"OQQQQ@@QQ@@QQQ$@",
	"OQQQQQ@@@@QQQQ$@",
	"OQQQQQQ@@QQQQQ$@",
	"OQQQQQ@@@@QQQQ$@",
	"OQQQQ@@QQ@@QQQ$@",
	"OQQQ@@QQQQ@@QQ$@",
	"OQQQQQQQQQQQQQ$@",
	"OQQQQQQQQQQQQQ$@",
	"O$$$$$$$$$$$$$$@",
	"@@@@@@@@@@@@@@@@",
};

static unsigned char table_rgb[17 * 3] = {
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
	"..........*OOOO*",
	"...........*****",
	"****************"
};
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

	boxfill8 ( vram, xsize , find_palette( 0x00008484)  , 0		,0			,xsize-1	,ysize-29 );	
	boxfill8 ( vram, xsize , find_palette( 0x00c6c6c6)  , 0		,ysize-28	,xsize-1	,ysize-28 );	
	boxfill8 ( vram, xsize , find_palette( 0x00ffffff)  , 0		,ysize-27	,xsize-1	,ysize-27 );	
	boxfill8 ( vram, xsize , find_palette( 0x00c6c6c6)  , 0		,ysize-26	,xsize-1	,ysize-1  );	
	
	boxfill8 ( vram, xsize , find_palette( 0x00ffffff)  , 3		,ysize-24	,59			,ysize-24 );	
	boxfill8 ( vram, xsize , find_palette( 0x00ffffff)  , 2		,ysize-24	,2			,ysize-4  );	
	boxfill8 ( vram, xsize , find_palette( 0x00848484)  , 3		,ysize-4	,59			,ysize-4  );	
	boxfill8 ( vram, xsize , find_palette( 0x00848484)  , 59		,ysize-23	,59			,ysize-5  );	
	boxfill8 ( vram, xsize , find_palette( 0x00000000)  , 2		,ysize-3	,59			,ysize-3  );	
	boxfill8 ( vram, xsize , find_palette( 0x00000000)  , 60		,ysize-24	,60			,ysize-3  );	
	
	boxfill8 ( vram, xsize , find_palette( 0x00848484)  , xsize-47,ysize-24	,xsize-4	,ysize-24 );	
	boxfill8 ( vram, xsize , find_palette( 0x00848484)  , xsize-47,ysize-23	,xsize-47	,ysize-4  );	
	boxfill8 ( vram, xsize , find_palette( 0x00ffffff)  , xsize-47,ysize-3	,xsize-4	,ysize-3  );	
	boxfill8 ( vram, xsize , find_palette( 0x00ffffff)  , xsize-3	,ysize-24	,xsize-3	,ysize-3  );	
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
void putfonts8_asc_sht ( struct SHEET *sht, int x, int y, int c, int b, char *s, int l )
{
	boxfill8 ( sht->buf, sht->bxsize, b, x, y, x + l * 8 - 1, y + 15 );
	putfonts8_asc ( sht->buf, sht->bxsize, x, y, c, s );
	sheet_refresh ( sht, x, y, x + l * 8, y + 16 );
}

void make_window8 ( unsigned char* buf, int xsize, int ysize, char *title, char act )
{
	boxfill8 (	buf,	xsize,	find_palette(0x00c6c6c6),			0,			0,	xsize - 1,	0);	
	boxfill8 (	buf,	xsize,	find_palette(0x00ffffff),			1,			1,	xsize - 2,	1);	
	boxfill8 (	buf,	xsize,	find_palette(0x00c6c6c6),			0,			0,			0,	ysize - 1);	
	boxfill8 (	buf,	xsize,	find_palette(0x00ffffff),			1,			1,			1,	ysize - 2);	
	boxfill8 (	buf,	xsize,	find_palette(0x00848484),	xsize - 2,			1,	xsize - 2,	ysize - 2);	
	boxfill8 (	buf,	xsize,	find_palette(0x00000000),	xsize - 1,			0,	xsize - 1,	ysize - 1);	
	boxfill8 (	buf,	xsize,	find_palette(0x00c6c6c6),			2,			2,	xsize - 3,	ysize - 3);	
	boxfill8 (	buf,	xsize,	find_palette(0x00000084),			3,			3,	xsize - 4,	20);	
	boxfill8 (	buf,	xsize,	find_palette(0x00848484),			1,	ysize - 2,	xsize - 2,	ysize - 2);	
	boxfill8 (	buf,	xsize,	find_palette(0x00000000),			0,	ysize - 1,	xsize - 1,	ysize - 1);	
	make_wtitle8(buf, xsize, title, act);
	return;
}
void make_wtitle8(unsigned char *buf, int xsize, char *title, char act)
{
	int x, y;
	char c, tc, tbc;
	if (act != 0 ) {
		tc = find_palette(0xffffff);
		tbc = find_palette(0x84);
	} else {
		tc = find_palette(0xc6c6c6);
		tbc = find_palette(0x848484);
	}
	boxfill8(buf,xsize, tbc, 3, 3, xsize - 4, 20);
	putfonts8_asc ( buf, xsize, 24, 4, tc , title );
	for (y = 0; y < 14; y++ ){
		for ( x = 0; x < 16; x++ ){
			c = closebtn[y][x];
			switch ( c ) {
				case '@': c = find_palette(0x00000000);break;
				case '$': c = find_palette(0x00848484);break;
				case 'Q': c = find_palette(0x00c6c6c6);break;
				default: c = find_palette(0x00ffffff);break;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)]	= c;
		}
	}
	return ;
}

void make_textbox8 ( struct SHEET *sht, int x0, int y0, int sx, int sy, int c )
{
	int x1 = x0 + sx, y1 = y0 + sy;
	boxfill8(sht->buf, sht->bxsize, find_palette(0x848484), x0 - 2 , y0 - 3 , x1 + 1 , y0 - 3 );
	boxfill8(sht->buf, sht->bxsize, find_palette(0x848484), x0 - 3 , y0 - 3 , x0 - 3 , y1 + 1 );
	boxfill8(sht->buf, sht->bxsize, find_palette(0xffffff), x0 - 3 , y1 + 2 , x1 + 1 , y1 + 2 );
	boxfill8(sht->buf, sht->bxsize, find_palette(0xffffff), x1 + 2 , y0 - 3 , x1 + 2 , y1 + 2 );
	boxfill8(sht->buf, sht->bxsize, find_palette(0x0), 		x0 - 1 , y0 - 2 , x1 + 0 , y0 - 2 );
	boxfill8(sht->buf, sht->bxsize, find_palette(0x0), 		x0 - 2 , y0 - 2 , x0 - 2 , y1 + 0 );
	boxfill8(sht->buf, sht->bxsize, find_palette(0xc6c6c6), x0 - 2 , y1 + 1 , x1 + 0 , y1 + 1 );
	boxfill8(sht->buf, sht->bxsize, find_palette(0xc6c6c6),	x1 + 1 , y0 - 2 , x1 + 1 , y1 + 1 );
	boxfill8(sht->buf, sht->bxsize, c, 						x0 - 1 , y0 - 1 , x1 + 0 , y1 + 0 );
	return ;
}
