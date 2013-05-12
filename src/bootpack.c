
#include "bootpack.h"
#include <stdio.h>

extern struct MOUSE_DEC mdec;
extern struct TIMERCTL timerctl;

/*keyboard table*/
static char keytable[0x54] = {0, 0, '1','2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0, 0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0, ']', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'};
/*queue.c*/
struct Queue8 inputData;
unsigned int inputBuf[128];
 
void console_task(struct SHEET* sheet);

struct MEMMAN* memman = (struct MEMMAN *) MEMMAN_ADDR;
void HariMain(void)
{
		struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
		char s[40];	
		unsigned int i;
		int  key_to = 0;
		int mx, my;
		unsigned int memtotal;

		/*sheet.c*/
		struct SHTCTL *shtctl;
		struct SHEET *sht_back, *sht_mouse, *sht_win, *sht_cons;//, *sht_win_b[3];
		unsigned char *buf_back, buf_mouse[256], *buf_win, *buf_cons;//, *buf_win_b;
		/* end of sheet.c*/

		/*timer.c*/
		struct TIMER *timer;
		/*end of timer.c*/
		
		/*keyboard.c*/
		int cursor_x, cursor_c;
		cursor_x = 8;
		cursor_c =find_palette(0xffffff);

		/*TSS*/
		struct TASK *task_a, *task_cons;// *task_b[3];

	
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
		buf_back = (unsigned char*) memman_alloc_4k ( memman, binfo->scrnx * binfo->scrny );
		buf_win  = (unsigned char*) memman_alloc_4k ( memman, 160 * 52 );
		sheet_setbuf ( sht_back, buf_back, binfo->scrnx, binfo->scrny, -1 );
		sheet_setbuf ( sht_mouse,buf_mouse,16, 16, 99 );
		sheet_setbuf ( sht_win, buf_win, 160, 52 , -1 );
		init_screen ( buf_back , binfo->scrnx, binfo->scrny );
		init_mouse_curosr8( buf_mouse, 99 );
		make_window8 ( buf_win, 160, 52, "task_a", 1 );
		make_textbox8 ( sht_win, 8, 28, 144, 16 , find_palette(0xffffff) );
		sheet_slide ( sht_back, 0, 0 );
		sheet_slide ( sht_win, 8, 56 );
		sheet_slide (  sht_mouse, mx, my );
		sheet_updown (  sht_back, 0 );
		sheet_updown (  sht_win, 4 );
		sheet_updown (  sht_mouse, 5 );
		sheet_refresh (  sht_back, 0, 0, binfo->scrnx, binfo->scrny);

		/*task status segment:TSS*/
		task_a = task_init (memman);
		inputData.task = task_a;
		task_run(task_a, 1, 0);


		/* console */
		sht_cons = sheet_alloc(shtctl);
		buf_cons = (unsigned char *) memman_alloc_4k(memman, 256 * 165);
		sheet_setbuf (sht_cons, buf_cons, 256, 165, -1);
		make_window8 (buf_cons, 256, 165, "console", 0);
		make_textbox8(sht_cons, 8, 28, 240, 128, find_palette(0));
		task_cons = task_alloc();
		task_cons->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
		task_cons->tss.eip = (int) &console_task;
		task_cons->tss.es = 1 * 8;
		task_cons->tss.cs = 2 * 8;
		task_cons->tss.ss = 1 * 8;
		task_cons->tss.ds = 1 * 8;
		task_cons->tss.fs = 1 * 8;
		task_cons->tss.gs = 1 * 8;
		*((int *) (task_cons->tss.esp + 4) ) = (int) sht_cons;
		task_run (task_cons, 2, 2);
		
		sheet_slide(sht_cons, 32, 4);
		sheet_updown(sht_cons, 1);
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
									if ( key_to == 0 ) {
										s[0] = keytable[i];
										s[1] = 0;
										putfonts8_asc_sht(sht_win, cursor_x, 28, find_palette(0), find_palette(0xffffff), s, 1 );
										cursor_x += 8;
									} else {
										queue8_put(&task_cons->queue, keytable[i] + 256 );
									}
								}
								if ( i == 0x0e && cursor_x > 8 ) {
									/*backspace*/
									if ( key_to == 0 ) {
										putfonts8_asc_sht(sht_win, cursor_x, 28, find_palette(0), find_palette(0xffffff), " ", 1 );
										cursor_x -= 8;
									} else {
										queue8_put(&task_cons->queue, 8 + 256 );
									}
								}
								boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43 );
								sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44 );
								if ( i == 0x0f ) {
									if (key_to == 0){
										key_to = 1;
										make_wtitle8(buf_win, sht_win->bxsize, "task_a", 0);
										make_wtitle8(buf_cons, sht_cons->bxsize, "console", 1);
									} else 	{
										key_to = 0;
										make_wtitle8(buf_win, sht_win->bxsize, "task_a", 1);
										make_wtitle8(buf_cons, sht_cons->bxsize, "console", 0);
									}
									sheet_refresh(sht_win, 0, 0, sht_win->bxsize, 21 );
									sheet_refresh(sht_cons, 0, 0, sht_cons->bxsize, 21 );
								}
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

void console_task (struct SHEET* sheet)
{
//	struct Queue8 queue;
	struct TIMER *timer;
	struct TASK *task = task_now();
	
	char s[2];
	int i, queuebuf[128], cursor_x = 16 , cursor_c = find_palette(0);
	queue8_init(&task->queue, 128, queuebuf, task);
	timer = timer_alloc();
	timer_init (timer, &task->queue, 1);
	timer_settimer ( timer, 50 );

	putfonts8_asc_sht(sheet, 8, 28, find_palette(0xffffff), find_palette(0), ">", 1);
	while (1) {
		io_cli();
		if ( queue8_status(&task->queue) == 0 ) {
			task_sleep(task);
			io_sti();
		} else {
			i = queue8_get (&task->queue);
			io_sti();	
			if (i <= 1) {
				if ( i != 0 ) {
					timer_init(timer, &task->queue, 0);
					cursor_c = find_palette(0xffffff);
				} else {
					timer_init(timer, &task->queue, 1);
					cursor_c = find_palette(0);
				}
				timer_settimer(timer, 50);
			}
			if (i >= 256 && i <= 511 ){
				if (i == 8 + 256) {
					if (cursor_x > 16) {
						putfonts8_asc_sht (sheet, cursor_x, 28, find_palette(0xffffff), find_palette(0), " ", 1 );
						cursor_x -= 8;
					}
				} else {
					if ( cursor_x < 240 ) {
						s[0] = i - 256;
						s[1] = 0;
						putfonts8_asc_sht(sheet, cursor_x, 28, find_palette(0xffffff), find_palette(0), s, 1);	
						cursor_x += 8;
					}
				}
			}
			boxfill8(sheet->buf, sheet->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43 );
			sheet_refresh(sheet, cursor_x, 28, cursor_x + 8, 44);
			
		}
	}
}
