
#include "bootpack.h"

void HariMain(void)
{
	struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
	char mcursor[17*17];	
	init_gdtidt();
	init_palette();
	init_screen( binfo->vram , binfo->scrnx, binfo->scrny );
	char* s;
//	putfont8( binfo->vram , binfo->scrnx , 50 , 50 , 4 , hankaku+'A'*16 );
//	putfont8( binfo->vram , binfo->scrnx , 40 , 40 , 12 ,A );
	putfonts8_asc( binfo->vram , binfo->scrnx , 8 , 8 , find_palette( 0x00ffffff ) , "ABC 123" );	
//	sprintf(s , "scrnx = %d" , binfo->scrnx );
//	sprintf(s , "fxxk = %d" , fuck() );
	putfonts8_asc ( binfo->vram , binfo->scrnx , 16 , 64 , find_palette( 0x00ffffff) , s );	
	init_mouse_curosr8 ( mcursor , find_palette(0x00008484) );
	putblock8_8 ( binfo->vram , binfo->scrnx , 16 , 16 , 160 , 100 , mcursor , 16 );
	for ( ;; )
		io_hlt();
}
