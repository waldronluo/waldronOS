

extern char hankaku[4096];
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

void io_hlt(void);
void io_cli(void);
void io_out8 ( int port , int data );
int io_load_eflags(void);
void io_store_eflags( int eflags );
void boxfill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1 , int y1 );
void putfont8 ( char *vram , int xsize, int x , int y , char c , char *font );
void init_palette(void);
void set_palette(int start, int end, unsigned char* rgb);
int  find_palette( int color );
void write_mem8(int ,int );
void init_screen( char* , int, int );
//static unsigned char A[16] = {
//0x00,0x18,0x18,0x18,0x18,0x24,0x24,0x24,0x24,0x24,0x7e,0x42,0x42,0xe7,0x00,0x00};
struct BOOTINFO {
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
 	char* vram;
};

void HariMain(void)
{
	struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
	
	init_palette();
	init_screen( binfo->vram , binfo->scrnx, binfo->scrny );
	putfont8( binfo->vram , binfo->scrnx , 40 , 40 , 12 , hankaku );
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
