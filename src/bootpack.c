
#include "bootpack.h"
#include <stdio.h>

extern struct MOUSE_DEC mdec;
extern struct TIMERCTL timerctl;

/*queue.c*/
struct Queue8 inputData;
unsigned int inputBuf[128]; 

struct MEMMAN* memman = (struct MEMMAN *) MEMMAN_ADDR;
void HariMain(void)
{
		struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
		char s[40];	
		unsigned int i;
		int mx, my;
		unsigned int memtotal;
		unsigned int count = 0;

		/*sheet.c*/
		struct SHTCTL *shtctl;
		struct SHEET *sht_back, *sht_mouse, *sht_win;
		unsigned char *buf_back, buf_mouse[256], *buf_win;
		/* end of sheet.c*/

		/*timer.c*/
		struct TIMER *timer, *timer2, *timer3;
		/*end of timer.c*/


		init_gdtidt();
		init_pic();
		io_sti();

		/*queue.c*/
		queue8_init( &inputData , 128, inputBuf );

		/*init timer*/
		init_pit();
		timer = timer_alloc();
		timer2 = timer_alloc();
		timer3 = timer_alloc();
		timer_init ( timer, &inputData, 10 );
		timer_init ( timer2, &inputData, 3 );
		timer_init ( timer3, &inputData, 1 );
		timer_settimer ( timer, 1000 );
		timer_settimer ( timer2, 300 );
		timer_settimer ( timer3, 50 );

		io_out8(PIC0_IMR, 0xf8);
		io_out8(PIC1_IMR, 0xef);

		init_keyboard();
		/* Init mouse  */	
		enable_mouse();
		init_mouse(&mdec);	
		mx = ( binfo->scrnx - 16 ) / 2;
		my = ( binfo->scrny - 28 - 16 ) / 2;

		/* memory info */
		memtotal = memtest ( 0x00400000 , 0xbfffffff );
		memman_init(memman);
		memman_free(memman , 0x00001000 , 0x0009e000 );
		memman_free(memman , 0x00400000 , memtotal - 0x10000000 );	

		/* sheet.c*/
		init_palette();
		shtctl = shtctl_init ( memman, binfo->vram, binfo->scrnx, binfo->scrny );
		sht_back = sheet_alloc ( shtctl );
		sht_mouse= sheet_alloc ( shtctl );
		sht_win  = sheet_alloc ( shtctl );
		buf_back = (unsigned char*) memman_alloc_4k ( memman, binfo->scrnx * binfo->scrny );
		buf_win  = (unsigned char*) memman_alloc_4k ( memman, 160 * 52 );
		sheet_setbuf ( sht_back, buf_back, binfo->scrnx, binfo->scrny, -1 );
		sheet_setbuf ( sht_mouse,buf_mouse,16, 16, 99 );
		sheet_setbuf ( sht_win, buf_win, 160, 52 , -1 );
		init_screen ( buf_back , binfo->scrnx, binfo->scrny );
		init_mouse_curosr8( buf_mouse, 99 );
		make_window8 ( buf_win, 160, 68, "count" );
		sheet_slide (  sht_mouse, mx, my );
		sheet_slide ( sht_win, 80, 72 );
		sheet_updown (  sht_back, 0 );
		sheet_updown (  sht_win, 1 );
		sheet_updown (  sht_mouse, 2 );
		sheet_refresh (  sht_back, 0, 0, binfo->scrnx, 48);

		/* The OS */
		for ( ;; )
		{	
				count ++;
				sprintf(s, "%010d", timerctl.count );
				boxfill8(buf_win, 160, find_palette(0x00c6c6c6), 40, 28, 119, 43 );
				putfonts8_asc(buf_win, 160, 40, 28, find_palette(0), s );
				sheet_refresh(sht_win, 40,28,120,44);
				io_cli();
				if (queue8_status(&inputData) == 0 ) {
						io_sti();
				}	
				else {
						i = queue8_get (&inputData) ;
						io_sti();
						if ( i == 10 ) {
								putfonts8_asc (buf_back, binfo->scrnx, 0, 64, find_palette(0xffffff), "10[sec]");
								sheet_refresh(sht_back, 0, 64, 56, 80);
						}
						if ( i == 3 ) {
								putfonts8_asc (buf_back, binfo->scrnx, 0, 80, find_palette(0xffffff), "3[sec]");
								sheet_refresh(sht_back, 0, 80, 48, 96);
						} 
						if ( i == 0 || i == 1 ) {
								if ( i == 1 ) {
										timer_init ( timer3, &inputData, 0 );
										boxfill8 ( buf_back, binfo->scrnx, find_palette(0xffffff), 8, 96, 15, 111 );
								} else {
										timer_init ( timer3, &inputData, 1 );
										boxfill8 ( buf_back, binfo->scrnx, find_palette(0x008484), 8, 96, 15, 111 );
								}
								timer_settimer ( timer3, 50 );
								sheet_refresh(sht_back, 8, 96, 16, 112 );
						} 
						if ( i >= KEYDATA0 && i < MOUSEDATA0 ) {
								i -= KEYDATA0;
								io_sti();
								sprintf(s , "%02x" , i );
								putfonts8_asc_sht ( sht_back, 0, 16, find_palette(0xffffff), find_palette(0x8484), s, 2 );
						}
						if ( i >= MOUSEDATA0 ) {
								i -= MOUSEDATA0  ;
								io_sti();
								if ( mouse_decode (&mdec,i) == 1 ){
										sprintf ( s , "[lcr %d %d]", mdec.x , mdec.y );
										if ( (mdec.btn & 0x01 ) != 0 ) s[1] = 'L';
										if ( (mdec.btn & 0x02 ) != 0 ) s[3] = 'R';
										if ( (mdec.btn & 0x04 ) != 0 ) s[2] = 'C';
										putfonts8_asc_sht ( sht_back, 32, 16, find_palette(0xffffff), find_palette(0x8484), s  , 15	);
										mx += mdec.x;
										my += mdec.y;
										if ( mx < 0 ) mx = 0;
										if ( my < 0 ) my = 0;
										if ( mx > binfo->scrnx - 1 ) mx = binfo->scrnx - 1;
										if ( my > binfo->scrny - 1 ) my = binfo->scrny - 1;
										sprintf ( s, "(%3d, %3d)", mx,my );
										boxfill8 ( buf_back, binfo->scrnx, find_palette(0x00008484), 0, 0, 79, 15 );
										putfonts8_asc( buf_back, binfo->scrnx, 0 , 0 , find_palette(0x00ffffff), s );
										sheet_refresh( sht_back, 0, 0, 80, 16 );
										putfonts8_asc_sht ( sht_back, 0, 0, find_palette(0xffffff), find_palette(0x8484) , s , 10 );
										sheet_slide (  sht_mouse, mx, my );

								}
						}
				}
		}
}
