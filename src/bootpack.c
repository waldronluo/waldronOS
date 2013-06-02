
#include "bootpack.h"
#include <stdio.h>
#include <string.h>
extern struct MOUSE_DEC mdec;
extern struct TIMERCTL timerctl;

/*keyboard table*/
static char keytable0[0x80] = {
    0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
};

static char keytable1[0x81] = {
    0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0,   0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0,   0,   'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
        //charaters are not right
};
/*queue.c*/
struct Queue8 inputData;
unsigned int inputBuf[128];

void console_task(struct SHEET* sheet, unsigned int memtotal);

struct MEMMAN* memman = (struct MEMMAN *) MEMMAN_ADDR;
void HariMain(void)
{
    struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
    char s[40];	
    unsigned int i;
    int  key_to = 0, key_shift = 0, key_leds = (binfo->leds >> 4) & 7;
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
    *((int *) (task_cons->tss.esp + 8) ) = (int) memtotal;
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
            if ( (i == 0 || i == 1) ) {
                if ( i == 1 ) {
                    timer_init ( timer, &inputData, 0 );
                    if ( cursor_c != -1 )
                        cursor_c = find_palette (0xffffff);
                } else {
                    timer_init ( timer, &inputData, 1 );
                    if ( cursor_c != -1 )
                        cursor_c = find_palette ( 0 );
                }
                timer_settimer ( timer, 50 );
                if ( cursor_c != -1 ) {
                    boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43 );
                    sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44 );
                }
            } 
            if ( i >= KEYDATA0 && i < MOUSEDATA0 ) {
                /*keypat*/
                i -= KEYDATA0;
                //sprintf(s , "%02x" , i );
                //putfonts8_asc_sht ( sht_back, 0, 16, find_palette(0xffffff), find_palette(0x8484), s, 2 );

                if ( i < 0x80 ) {
                    if ( key_shift == 0 ) s[0] = keytable0[i];
                    if ( key_shift == 1 ) s[0] = keytable1[i];
                } else { s[0] = 0; }
                if ( 'A' <= s[0] && s[0] <= 'Z' ) {
                    if (((key_leds & 4) == 0 && key_shift == 0 ) || ((key_leds & 4) != 0 && key_shift != 0)) s[0] += 0x20;
                }
                if ( s[0] != 0 ) {
                    if ( key_to == 0 ) {  
                        if (cursor_x < 128 ) {
                            s[1] = 0;
                            putfonts8_asc_sht(sht_win, cursor_x, 28, find_palette(0), find_palette(0xffffff), s, 1 );
                            cursor_x += 8;	
                        }
                    }
                    if ( key_to == 1 ) {
                        queue8_put(&task_cons->queue, s[0] + 256 );
                    }
                }
                if ( i == 0x3a ) {
                    if ((key_leds & 4) != 0) key_leds &= (~4);
                    else key_leds |= (4);
                }
                if ( i == 0x2a ) key_shift |= 1;
                if ( i == 0x36 ) key_shift |= 2;
                if ( i == 0xaa ) key_shift &= ~1;
                if ( i == 0xb6 ) key_shift &= ~2;
                if ( i == 0x0e && cursor_x > 8 ) {
                    /*backspace*/
                    if ( key_to == 0 ) {
                        putfonts8_asc_sht(sht_win, cursor_x, 28, find_palette(0), find_palette(0xffffff), " ", 1 );
                        cursor_x -= 8;
                    } else {
                        queue8_put(&task_cons->queue, 8 + 256 );
                    }
                }
                if ( i == 0x0f ) {
                    /* Tab */
                    if (key_to == 0){
                        key_to = 1;
                        make_wtitle8(buf_win, sht_win->bxsize, "task_a", 0);
                        make_wtitle8(buf_cons, sht_cons->bxsize, "console", 1);
                        cursor_c = -1;
                        queue8_put (&task_cons->queue, 2);
                    } else 	{
                        key_to = 0;
                        make_wtitle8(buf_win, sht_win->bxsize, "task_a", 1);
                        make_wtitle8(buf_cons, sht_cons->bxsize, "console", 0);
                        cursor_c = find_palette(0);
                        queue8_put (&task_cons->queue, 3);
                    }
                    sheet_refresh(sht_win, 0, 0, sht_win->bxsize, 21 );
                    sheet_refresh(sht_cons, 0, 0, sht_cons->bxsize, 21 );
                }
                if ( i == 0x1c)
                    if (key_to != 0)
                        queue8_put(&task_cons->queue, 10 + 256 );
                if ( cursor_c != -1 ) {
                    boxfill8(sht_win->buf, sht_win->bxsize, find_palette(0), cursor_x, 28, cursor_x + 7, 43 );
                    sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44 );
                }
            }
            if ( i >= MOUSEDATA0 ) {
                i -= MOUSEDATA0  ;
                if ( mouse_decode (&mdec,i) == 1 ){
                //    sprintf ( s , "[lcr %d %d]", mdec.x , mdec.y );
                //    if ( (mdec.btn & 0x01 ) != 0 ) s[1] = 'L';
                //    if ( (mdec.btn & 0x02 ) != 0 ) s[3] = 'R';
                //    if ( (mdec.btn & 0x04 ) != 0 ) s[2] = 'C';
                //    putfonts8_asc_sht ( sht_back, 32, 16, find_palette(0xffffff), find_palette(0x8484), s  , 15	);
                    mx += mdec.x;
                    my += mdec.y;
                    if ( mx < 0 ) mx = 0;
                    if ( my < 0 ) my = 0;
                    if ( mx > binfo->scrnx - 1 ) mx = binfo->scrnx - 1;
                    if ( my > binfo->scrny - 1 ) my = binfo->scrny - 1;
                //    sprintf ( s, "(%3d, %3d)", mx,my );
                //    boxfill8 ( buf_back, binfo->scrnx, find_palette(0x00008484), 0, 0, 79, 15 );
                //    putfonts8_asc( buf_back, binfo->scrnx, 0 , 0 , find_palette(0x00ffffff), s );
                //    sheet_refresh( sht_back, 0, 0, 80, 16 );
                //    putfonts8_asc_sht ( sht_back, 0, 0, find_palette(0xffffff), find_palette(0x8484) , s , 10 );
                    sheet_slide (  sht_mouse, mx, my );
                    if ( (mdec.btn & 0x01) != 0 ) {
                        sheet_slide ( sht_win, mx-80, my-8 );
                    }
                }
            }
        }
    }
}

void console_task (struct SHEET* sheet, unsigned int memtotal)
{
    //	struct Queue8 queue;
    struct TIMER *timer;
    struct TASK *task = task_now();
    //	int array[10000];	
    char s[30], cmdline[30];
    int i, queuebuf[128], cursor_x = 16, cursor_y = 28 , cursor_c = -1;
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    //	int *test;
    //	test = memman_alloc_4k(memman, 10000000 );
    queue8_init(&task->queue, 128, queuebuf, task);
    timer = timer_alloc();
    timer_init (timer, &task->queue, 1);
    timer_settimer ( timer, 50 );

    putfonts8_asc_sht(sheet, 8, cursor_y, find_palette(0xffffff), find_palette(0), ">", 1);
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
                    if ( cursor_c >= 0 )
                        cursor_c = find_palette(0xffffff);
                } else {
                    timer_init(timer, &task->queue, 1);
                    if ( cursor_c >= 0 )                    
                        cursor_c = find_palette(0);
                }
                timer_settimer(timer, 50);
            }
            if ( i == 2 )
                cursor_c = find_palette(0xffffff);
            if ( i == 3 ){
                boxfill8(sheet->buf, sheet->bxsize, find_palette(0), cursor_x, 28, cursor_x + 7, 43);
                cursor_c = -1;
                //    while (1) io_hlt();
            }
            if (i >= 256 && i <= 511 ){
                if (i == 8 + 256) {
                    if (cursor_x > 16) {
                        putfonts8_asc_sht (sheet, cursor_x, cursor_y, find_palette(0xffffff), find_palette(0), " ", 1 );
                        cursor_x -= 8;
                    }
                } else if (i == 10 + 256){
                    putfonts8_asc_sht(sheet, cursor_x, cursor_y, find_palette(0xffffff), find_palette(0), " ", 1);
                    /*cmdline*/
                    cmdline[cursor_x / 8 -2] = 0;
                    cursor_y = cons_newline(cursor_y, sheet);
                    if ( ( strcmp (cmdline, "cls") == 0 ) || ( strcmp (cmdline, "clear") == 0 ) ) {
                        int x, y;
                        for ( y = 28 ; y < 28 + 128 ; y ++ )
                            for ( x = 8 ; x < 8 + 240 ; x ++ )
                                sheet->buf[x + y * sheet->bxsize] = find_palette(0);
                        sheet_refresh(sheet, 8 , 28 , 8 + 240 , 28 + 128 );
                        cursor_y = 28;
                    } else if (strcmp (cmdline, "mem") == 0 ) {
                        sprintf(s, "total %dMB", memtotal / (1024 * 1024) );
                        putfonts8_asc_sht (sheet, 8, cursor_y, find_palette(0xffffff), find_palette(0), s, 30);
                        cursor_y = cons_newline(cursor_y, sheet);
                        sprintf(s, "free %dKB", memman_total (memman) / 1024 );
                        putfonts8_asc_sht (sheet, 8, cursor_y, find_palette(0xffffff), find_palette(0), s, 30);
                        cursor_y = cons_newline(cursor_y, sheet);
                        cursor_y = cons_newline(cursor_y, sheet);
                    } else if ( cmdline[0] != 0 ){
                        putfonts8_asc_sht (sheet, 8, cursor_y, find_palette(0xffffff), find_palette(0), "Bad command", 12 );
                        cursor_y = cons_newline(cursor_y, sheet);
                        cursor_y = cons_newline(cursor_y, sheet);
                    }
                    putfonts8_asc_sht(sheet, 8, cursor_y, find_palette(0xffffff), find_palette(0), ">", 1);
                    cursor_x = 16;

                } else {
                    if ( cursor_x < 240 ) {
                        s[0] = i - 256;
                        s[1] = 0;
                        cmdline[cursor_x / 8 - 2] = i - 256;
                        putfonts8_asc_sht(sheet, cursor_x, cursor_y, find_palette(0xffffff), find_palette(0), s, 1);	
                        cursor_x += 8;
                    }
                }
            }
            if ( cursor_c >= 0 ){
                boxfill8(sheet->buf, sheet->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43 );
                sheet_refresh(sheet, cursor_x, cursor_y, cursor_x + 8, 44);
            }
            sheet_refresh(sheet, cursor_x, cursor_y, cursor_x+8, cursor_y+16);
        }
    }
}

int cons_newline (int cursor_y, struct SHEET* sheet)
{
    if ( cursor_y < 28 + 112 ) {
        cursor_y += 16;
    } else {
        int x,y;
        for ( y = 28; y < 28 + 112 ; y ++ ) 
            for ( x = 8 ; x < 8 + 240 ; x ++ ) 
                sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize]; 

        for ( y = 28 + 112; y < 28 + 128 ; y ++ )
            for ( x = 8 ; x < 8 + 240 ; x ++ ) 
                sheet->buf[x + y * sheet->bxsize] = find_palette(0);
        sheet_refresh(sheet, 8 , 28, 8 + 240, 28 + 128 );
    }
    return cursor_y;
}
