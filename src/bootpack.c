
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

struct MEMMAN* memman = (struct MEMMAN *) MEMMAN_ADDR;
void HariMain(void)
{
    /* what ? */
    struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
    char s[40];	
    unsigned int i;
    int  key_shift = 0, key_ctrl = 0, key_leds = (binfo->leds >> 4) & 7;
    int mx, my;
    unsigned int memtotal;

    /*sheet.c*/
    struct SHTCTL *shtctl;
    struct SHEET *sht_back, *sht_mouse, *sht_win, *sht_cons;
    unsigned char *buf_back, buf_mouse[256], *buf_win, *buf_cons;

    /* sheet control in UI */
    int j, x, y, mmx = -1, mmy = -1;
    struct SHEET *sht = 0, *key_win;

    /*timer.c*/
    struct TIMER *timer;

    /*console.c*/
    struct CONSOLE *cons;

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
    buf_cons = (unsigned char *) memman_alloc_4k(memman, 500 * 500);
    sheet_setbuf (sht_cons, buf_cons, 500, 500, -1);
    make_window8 (buf_cons, 500, 500, "console", 0);
    make_textbox8(sht_cons, 8, 28, 500 - 16, 500 - 37, find_palette(0));
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

    key_win = sht_win;
    sht_cons->task = task_cons;
    sht_cons->flags |= 0x20;

    /* for static variable */
    *((int *) 0xfe4) = (int) shtctl;
    /* The OS */
    for ( ;; )
    {	
        io_cli();
        if (queue8_status(&inputData) == 0 ) {
            task_sleep(task_a);
            io_sti();
        } else {
            i = queue8_get (&inputData) & 0xffff;
            io_sti();
            if (key_win->flags == 0) {
                key_win = shtctl->sheets[shtctl->top - 1];
                cursor_c = keywin_on (key_win, sht_win, cursor_c);
            }
            if (i == 1||i == 0) {
            /* cursor */
                if (i == 1) {
                    timer_init (timer, &inputData, 0); 
                    if (cursor_c >= 0) cursor_c = find_palette(0);
                } else {
                    timer_init (timer, &inputData, 1);
                    if (cursor_c >= 0) cursor_c = find_palette(0xffffff);
                } 
                timer_settimer(timer, 50);
                if (cursor_c >= 0) {
                    boxfill8 (sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
                    sheet_refresh (sht_win, cursor_x, 28, cursor_x + 8, 44);
                }
            }
            if ( i >= KEYDATA0 && i < MOUSEDATA0 ) {
             /* keypat */
                i -= KEYDATA0;
                sprintf (s, "%02x", i);
                putfonts8_asc_sht (sht_back, 0, 16, find_palette(0xffffff), find_palette(0), s, 2);
                /* normal input */
                if ( i < 0x80 && key_ctrl == 0 ) {
                    if ( key_shift == 0 ) s[0] = keytable0[i];
                    if ( key_shift == 1 ) s[0] = keytable1[i];
                } else { s[0] = 0; }
                if ( 'A' <= s[0] && s[0] <= 'Z' ) {
                    if (((key_leds & 4) == 0 && key_shift == 0 ) || ((key_leds & 4) != 0 && key_shift != 0)) s[0] += 0x20;
                }
                if ( s[0] != 0 ) {
                    if (key_win == sht_win) {
                        if (cursor_x < 128) {
                            s[1] = 0;
                            putfonts8_asc_sht (sht_win, cursor_x, 28, find_palette(0),find_palette(0xffffff), s, 1);
                            cursor_x += 8;
                        }  
                    } else {
                        queue8_put (&key_win->task->queue, s[0] + 256);
                    }
                }
                if ( i == 0x2e && key_ctrl == 1 && task_cons->tss.ss0 != 0 ) {
                    putfonts8_asc_sht (sht_back, 0, 16, find_palette(0xffffff), find_palette(0), "in!", 3);
                    cons = (struct CONSOLE *) *((int *) 0xfec);
                    cons_putstr0 (cons, "\nInterrupt!!!\n");
                    io_cli();
                    task_cons->tss.eax = (int) &(task_cons->tss.esp0);
                    task_cons->tss.eip = (int) &asm_end_app;
                    io_sti();
                }
                if ( i == 0x3a ) {
                    if ((key_leds & 4) != 0) key_leds &= (~4);
                    else key_leds |= (4);
                }
                if ( i == 0x57 && shtctl->top > 2 ) sheet_updown (shtctl->sheets[1], shtctl->top - 1);
                if ( i == 0x1d ) key_ctrl |= 1;
                if ( i == 0x9d ) key_ctrl &= ~1;
                if ( i == 0x2a ) key_shift |= 1;
                if ( i == 0x36 ) key_shift |= 2;
                if ( i == 0xaa ) key_shift &= ~1;
                if ( i == 0xb6 ) key_shift &= ~2;
                if ( i == 0x0e ) {
                    /*backspace*/
                    if ( key_win == sht_win) {
                        if (cursor_x > 8) {
                            putfonts8_asc_sht(sht_win, cursor_x, 28, find_palette(0), find_palette(0xffffff), " ", 1 );
                            cursor_x -= 8;
                        }
                    } else {
                        queue8_put(&key_win->task->queue, 8 + 256);
                    }
                }
                if ( i == 0x0f ) {
                    /* Tab */
                    cursor_c = keywin_off (key_win, sht_win, cursor_c, cursor_x);
                    j = key_win->height - 1;
                    if (j == 0)
                        j = shtctl->top - 1;
                    key_win = shtctl->sheets[j];
                    cursor_c = keywin_on (key_win, sht_win, cursor_c);

                }
                if ( i == 0x1c ) {
                    if (key_win != sht_win)
                        queue8_put(&key_win->task->queue, 0x0a + 256 );
                }
                if ( cursor_c != -1 ) {
                    boxfill8(sht_win->buf, sht_win->bxsize, find_palette(0), cursor_x, 28, cursor_x + 7, 43 );
                    sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44 );
                }
            }
            if ( i >= MOUSEDATA0 ) {
                i -= MOUSEDATA0  ;
                if ( mouse_decode (&mdec,i) == 1 ){
                    mx += mdec.x;
                    my += mdec.y;
                    if ( mx < 0 ) mx = 0;
                    if ( my < 0 ) my = 0;
                    if ( mx > binfo->scrnx - 1 ) mx = binfo->scrnx - 1;
                    if ( my > binfo->scrny - 1 ) my = binfo->scrny - 1;
                    sheet_updown (sht_mouse, shtctl->top);
                    sheet_slide (  sht_mouse, mx, my );
                    if ( (mdec.btn & 0x01) != 0 ) {
                        if (mmx < 0) {
                            for (j = shtctl->top - 1;j > 0;j --) {
                                sht = shtctl->sheets[j];
                                x = mx - sht->vx0;
                                y = my - sht->vy0;
                                if (0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {
                                    if (sht->buf[y * sht->bxsize + x] != sht->col_inv) {
                                        sheet_updown (sht, shtctl->top - 1);
                                        if (sht != key_win) {
                                            cursor_c = keywin_off (key_win, sht_win, cursor_c, cursor_x);
                                            key_win = sht;
                                            cursor_c = keywin_on (key_win, sht_win, cursor_c);
                                        }
                                        if (3 <= x && x < sht->bxsize - 3&& 3 <= y && y < 21) {
                                        /*Move a window*/
                                            mmx = mx;
                                            mmy = my;
                                        }
                                        if ( sht->bxsize - 21 <= x && x <= sht->bxsize - 5 && 5 <= y && y <= 19) {
                                            /* Click XXX*/
                                            if ((sht->flags & 0x10) != 0) {
                                                cons = (struct CONSOLE *) *((int *)0xfec);
                                                cons_putstr0(cons, "\nBreak(mouse)\n");
                                                io_cli();
                                                task_cons->tss.eax = (int) &(task_cons->tss.esp0);
                                                task_cons->tss.eip = (int) asm_end_app;
                                                io_sti();
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                        } else {
                            x = mx - mmx;
                            y = my - mmy;
                            sheet_slide (sht, sht->vx0 + x, sht->vy0 + y);
                            mmx = mx;
                            mmy = my;
                        }
                    } else {
                        mmx = -1;
                    }
                }
            }
        }
    }
}

int keywin_off (struct SHEET *key_win, struct SHEET *sht_win, int cur_c, int cur_x) 
{
    change_wtitle (key_win, 0);
    if (key_win == sht_win) {
        cur_c = -1;
        boxfill8 (sht_win->buf, sht_win->bxsize, find_palette(0xffffff), cur_x, 28, cur_x + 7, 43);
    } else {
        if ((key_win->flags & 0x20) != 0) {
            queue8_put (&key_win->task->queue, 3);
        }
    }
    return cur_c;
}

int keywin_on  (struct SHEET *key_win, struct SHEET *sht_win, int cur_c)
{
    change_wtitle (key_win, 1);
    if (key_win == sht_win) {
        cur_c = find_palette(0);
    } else {
        if ((key_win->flags & 0x20) != 0 ) {
            queue8_put (&key_win->task->queue, 2);
        }
    }
    return cur_c;
}
