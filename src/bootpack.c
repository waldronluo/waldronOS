
#include "bootpack.h"
#include <stdio.h>

extern struct MOUSE_DEC mdec;
extern struct TIMERCTL timerctl;

/*keyboard table*/
static char keytable[0x54] = {0, 0, '1','2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0, 0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0, ']', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'};
/*queue.c*/
struct Queue8 inputData;
unsigned int inputBuf[128];
 
void task_b_main(struct SHEET* sht_back);

struct MEMMAN* memman = (struct MEMMAN *) MEMMAN_ADDR;
void HariMain(void)
{
		struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
		char s[40];	
		unsigned int i;
		int mx, my;
		unsigned int memtotal;

		/*sheet.c*/
		struct SHTCTL *shtctl;
		struct SHEET *sht_back, *sht_mouse, *sht_win, *sht_win_b[3];
		unsigned char *buf_back, buf_mouse[256], *buf_win, *buf_win_b;
		/* end of sheet.c*/

		/*timer.c*/
		struct TIMER *timer;
		/*end of timer.c*/
		
		/*keyboard.c*/
		int cursor_x, cursor_c;
		cursor_x = 8;
		cursor_c =find_palette(0xffffff);

		/*TSS*/
		struct TASK *task_a, *task_b[3];

	
		init_gdtidt();
		init_pic();
		io_sti();

		/*queue.c*/
		queue8_init( &inputData , 128, inputBuf, 0 );

		/*init timer*/
		init_pit();
		timer = timer_alloc();
		timer_init ( timer, &inputData, 1 );
		timer_settimer ( timer, 50 );

		io_out8(PIC0_IMR, 0xf8);
		io_out8(PIC1_IMR, 0xef);

		/* init keyboard */
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
		for (i = 0;i < 3; i ++) {
			sht_win_b[i] = sheet_alloc (shtctl);
			buf_win_b = (unsigned char *) memman_alloc_4k(memman, 144 * 52);
			sheet_setbuf(sht_win_b[i], buf_win_b, 144, 52, -1);
			sprintf(s, "task_b:%d", i );
			make_window8(buf_win_b, 144, 52, s, 0);
		}
		buf_back = (unsigned char*) memman_alloc_4k ( memman, binfo->scrnx * binfo->scrny );
		buf_win  = (unsigned char*) memman_alloc_4k ( memman, 160 * 52 );
		sheet_setbuf ( sht_back, buf_back, binfo->scrnx, binfo->scrny, -1 );
		sheet_setbuf ( sht_mouse,buf_mouse,16, 16, 99 );
		sheet_setbuf ( sht_win, buf_win, 160, 52 , -1 );
		init_screen ( buf_back , binfo->scrnx, binfo->scrny );
		init_mouse_curosr8( buf_mouse, 99 );
		make_window8 ( buf_win, 160, 52, "Type me", 1 );
		make_textbox8 ( sht_win, 8, 28, 144, 16 , find_palette(0xffffff) );
		sheet_slide ( sht_back, 0, 0 );
		sheet_slide ( sht_win_b[0], 168,  56 );
		sheet_slide ( sht_win_b[1],   8, 116 );
		sheet_slide ( sht_win_b[2], 168, 116 );
		sheet_slide ( sht_win, 8, 56 );
		sheet_slide (  sht_mouse, mx, my );
		sheet_updown (  sht_back, 0 );
		sheet_updown (  sht_win_b[0], 1 );
		sheet_updown (  sht_win_b[1], 2 );
		sheet_updown (  sht_win_b[2], 3 );
		sheet_updown (  sht_win, 4 );
		sheet_updown (  sht_mouse, 5 );
		sheet_refresh (  sht_back, 0, 0, binfo->scrnx, 48);

		/*task status segment:TSS*/
		task_a = task_init (memman);
		inputData.task = task_a;
		for (i = 0;i < 3; i++ )
		{
			task_b[i] = task_alloc();
			task_b[i]->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
			task_b[i]->tss.eip = (int) &task_b_main;
			task_b[i]->tss.es = 1 * 8;	
			task_b[i]->tss.cs = 2 * 8;	
			task_b[i]->tss.ss = 1 * 8;	
			task_b[i]->tss.ds = 1 * 8;	
			task_b[i]->tss.fs = 1 * 8;	
			task_b[i]->tss.gs = 1 * 8;	
			*((int*) (task_b[i]->tss.esp + 4)) = (int) sht_win_b[i];
			task_run(task_b[i]);
		}
		/* The OS */
		for ( ;; )
		{	
				io_cli();
				if (queue8_status(&inputData) == 0 ) {
						task_sleep(task_a);
						io_sti();
				}	
				else {
						i = queue8_get (&inputData) ;
						io_sti();
						if ( i == 0 || i == 1 ) {
								if ( i == 1 ) {
										timer_init ( timer, &inputData, 0 );
										cursor_c = find_palette (0);
								} else {
										timer_init ( timer, &inputData, 1 );
										cursor_c = find_palette ( 0xffffff );
								}
								timer_settimer ( timer, 50 );
								boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43 );
								sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44 );
						} 
						if ( i >= KEYDATA0 && i < MOUSEDATA0 ) {
								i -= KEYDATA0;
								sprintf(s , "%02x" , i );
								putfonts8_asc_sht ( sht_back, 0, 16, find_palette(0xffffff), find_palette(0x8484), s, 2 );
								if (i < 0x54 && keytable[i] != 0 && cursor_x < 144) {
									s[0] = keytable[i];
									s[1] = 0;
									putfonts8_asc_sht(sht_win, cursor_x, 28, find_palette(0), find_palette(0xffffff), s, 1 );
									cursor_x += 8;
								}
								if ( i == 0x0e && cursor_x > 8 ) {
									/*backspace*/
									putfonts8_asc_sht(sht_win, cursor_x, 28, find_palette(0), find_palette(0xffffff), " ", 1 );
									cursor_x -= 8;
								}
								boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43 );
								sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44 );
						}
						if ( i >= MOUSEDATA0 ) {
								i -= MOUSEDATA0  ;
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
										if ( (mdec.btn & 0x01) != 0 ) {
											sheet_slide ( sht_win, mx-80, my-8 );
										}
								}
						}
				}
		}
}




void task_b_main(struct SHEET* sht_win_b)
{
	struct Queue8 queue;
	struct TIMER *timer;
	int i;
	unsigned int count = 0 , count0 = 0;
	int queue_buf [128];
	char s[12];
		
	queue8_init (&queue, 128, queue_buf, 0);
	timer = timer_alloc();
	timer_init(timer, &queue, 100 );
	timer_settimer(timer, 10);
	while ( 1 ) {
		count ++;
		io_cli();
		if ( queue8_status(&queue) == 0 ){
			io_sti();
		} else {
			i = queue8_get (&queue);
			io_sti();
			if ( i == 100 ) {
				sprintf(s, "%11d", count - count0 );
				putfonts8_asc_sht (sht_win_b, 24, 28, find_palette(0), find_palette(0xc6c6c6), s, 11);
				count0 = count;
				timer_settimer(timer, 10);
			}
		}
	}
}
