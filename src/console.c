#include "bootpack.h"
#include <string.h>
#include <stdio.h>
void console_task (struct SHEET* sheet, unsigned int memtotal)
{
    /* timer */
    struct TIMER *timer;
    struct TASK *task = task_now();

    /* file */
//    struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
    
    int i, queuebuf[128];
    
    /* memory */
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    
    /* cons */
    struct CONSOLE cons;
    char cmdline[30];

    /*fat*/
    int *fat = (int *)memman_alloc_4k (memman, 4 * 2880 );
    file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
    
    /* init cons */
    cons.sht = sheet;
    cons.cur_x = 8, cons.cur_y = 28, cons.cur_c = -1;


    /* init timer */
    queue8_init(&task->queue, 128, queuebuf, task);
    timer = timer_alloc();
    timer_init (timer, &task->queue, 1);
    timer_settimer ( timer, 50 );

    cons_putchar (&cons, '>', 1);
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
                    if ( cons.cur_c >= 0 )
                        cons.cur_c = find_palette(0xffffff);
                } else {
                    timer_init(timer, &task->queue, 1);
                    if ( cons.cur_c >= 0 )                    
                        cons.cur_c = find_palette(0);
                }
                timer_settimer(timer, 50);
            }
            if ( i == 2 ) {
                cons.cur_c = find_palette(0xffffff);
            }
            if ( i == 3 ) {
                boxfill8(sheet->buf, sheet->bxsize, find_palette(0), cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
                cons.cur_c = -1;
            }
            if (i >= 256 && i <= 511) {
                if (i == 8 + 256) {
                    if (cons.cur_x > 16) {
                        cons_putchar (&cons, ' ', 0);
                        cons.cur_x -= 8;
                    }
                } else if (i == 10 + 256){
                    cons_putchar(&cons, ' ', 0);
                    /*cmdline*/
                    cmdline[cons.cur_x / 8 - 2] = 0;
                    cons_newline (&cons);
                    cons_runcmd (cmdline, &cons, fat, memtotal);
                    cons_putchar(&cons, '>', 1);
                } else {
                    if ( cons.cur_x < 500 - 24 ) {
                        cmdline[cons.cur_x / 8 - 2] = i - 256;
                        cons_putchar (&cons, i - 256, 1);
                    }
                }
            }
            if ( cons.cur_c != -1 ) {
                boxfill8(sheet->buf, sheet->bxsize, cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 16 );
            }
            sheet_refresh(sheet, cons.cur_x, cons.cur_y, cons.cur_x+8, cons.cur_y+16);
        }
    }
}


void cons_putchar (struct CONSOLE *cons, int chr, char move)
{
    char s[2];
    s[0] = chr;
    s[1] = 0;
    if ( s[0] == 0x09 ) {
        for (;;) {
            putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, find_palette(0xffffff), find_palette(0), " ", 1);
            cons->cur_x += 8;
            if (cons->cur_x >= 500 - 24 )
                cons_newline(cons);

            if (((cons->cur_x - 8) & 0x1f) == 0) break;
        }
    } else if (s[0] == 0x0a) {
        cons_newline(cons);
    } else if (s[0] == 0x0d) {
        
    } else {
        putfonts8_asc_sht (cons->sht, cons->cur_x, cons->cur_y, find_palette(0xffffff), find_palette(0), s, 1);
        if (move != 0) {
            cons->cur_x += 8;
            if ( cons->cur_x >= 500 - 24 )
                cons_newline(cons);
        }
    }
    return ;

}
void cons_newline (struct CONSOLE* cons)
{
    int x, y;
    struct SHEET *sheet = cons->sht;
    if (cons->cur_y < 28 + 16 * 28 ) {
        cons->cur_y += 16;
    } else {
        for (y = 28;y <= 28 + 16 * 28; y ++ )
            for (x = 8; x < 500 - 16; x ++ )
                sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];

        for (y = 28 + 16 * 28; y <= 28 + 16 * 29; y ++ )
            for ( x = 8 ; x < 500 - 16 ; x ++ )
                sheet->buf[x + y * sheet->bxsize] = find_palette(0);
        sheet_refresh(sheet, 8, 28, 500 - 8, 28 + 16 * 29 );
        //STOP;
    }
    cons->cur_x = 8;
}
void cons_runcmd (char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal)
{
    if (strcmp(cmdline, "mem") == 0)
        cmd_mem(cons, memtotal);
    else if ((strcmp(cmdline, "cls") == 0) || (strcmp(cmdline, "clear") == 0))
        cmd_cls(cons);
    else if ((strcmp(cmdline, "dir") == 0) || (strcmp(cmdline, "ls") == 0))
        cmd_ls(cons);
    else if ((strncmp(cmdline, "type ", 5) == 0) || (strncmp(cmdline, "cat ", 4) == 0))
        cmd_cat(cons, fat, cmdline);
    else if (strcmp(cmdline, "hlt") == 0)
        cmd_hlt(cons, fat);
    else if (cmdline[0] != 0) {
        putfonts8_asc_sht(cons->sht, 8, cons->cur_y, find_palette(0xffffff), find_palette(0), "BAD COMMAND", 12);
        cons_newline(cons);
    }
    return ;
}

void cmd_mem (struct CONSOLE *cons, unsigned int memtotal)
{
    struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
    char s[30];
    sprintf(s, "total\t%dMB", memtotal / (1<<20));
    putfonts8_asc_sht(cons->sht, 8, cons->cur_y, find_palette(0xffffff), find_palette(0), s, 30);
    cons_newline(cons);
    sprintf(s, "free\t%dKB", memman_total(memman) / (1<<10));
    putfonts8_asc_sht(cons->sht, 8, cons->cur_y, find_palette(0xffffff), find_palette(0), s, 30);
    cons_newline(cons);
    return;
}
void cmd_cls (struct CONSOLE *cons)
{
    int x,y;
    struct SHEET *sheet = cons->sht;
    for (y = 28;y < 28 + 16 * 29; y ++ )
        for(x = 8;x < 500 - 8; x ++ )
            sheet->buf[x + y * sheet->bxsize] = find_palette(0);
    sheet_refresh(sheet, 8, 28, 500 - 8, 28 + 16 * 29 );
    cons->cur_y = 28;
}

void cmd_ls (struct CONSOLE *cons)
{
    struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
    int x,y;
    char s[30];
    for (x = 0; x < 224; x ++ ){
        if ( finfo[x].name[0] == 0x00 )
            break;
        if ( finfo[x].name[0] != 0xe5 ) {
            if ((finfo[x].type & 0x18) == 0) {
                sprintf(s, "filename.ext    %7d", finfo[x].size);
                for (y = 0;y < 8; y ++ )
                    s[y] = finfo[x].name[y];
                s[ 9] = finfo[x].ext[0];
                s[10] = finfo[x].ext[1];
                s[11] = finfo[x].ext[2];
                putfonts8_asc_sht(cons->sht, 8, cons->cur_y, find_palette(0xffffff), find_palette(0), s, 30);
                cons_newline(cons);
            }
        }
    }
    return;
}

void cmd_cat (struct CONSOLE *cons, int *fat, char *cmdline)
{
    int cmd_flag = 0;/*4 for cat and 5 for type*/
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    struct FILEINFO *finfo;
    char *p;
    int i;
    cmd_flag = (strncmp(cmdline, "cat ", 4) == 0) ? 4 : 5;
    finfo = file_search (cmdline + cmd_flag, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
    if ( finfo != 0 ) {
        p = (char *)memman_alloc_4k(memman, finfo->size);
        file_loadfile (finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x3e00));
        for ( i = 0 ; i < finfo->size; i ++ )
            cons_putchar (cons, p[i], 1);
        memman_free_4k(memman, (int)p, finfo->size);
    } else {
        putfonts8_asc_sht (cons->sht, 8, cons->cur_y, find_palette(0xffffff), find_palette(0), "File Not Found.", 15);
        cons_newline(cons);
    }
    return;

}
void cmd_hlt (struct CONSOLE *cons, int *fat)
{
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    struct FILEINFO *finfo;
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    char *p;
    finfo = file_search ("hlt.wal", (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
    if ( finfo != 0 ) {
        p = (char *)memman_alloc_4k(memman, finfo->size);
        file_loadfile (finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x3e00));
        set_segmdesc (gdt + 1003, finfo->size - 1, (int) p, AR_CODE32_ER);
        farjmp(0, 1003 * 8);
        memman_free_4k(memman, (int)p, finfo->size);
    } else {
        putfonts8_asc_sht (cons->sht, 8, cons->cur_y, find_palette(0xffffff), find_palette(0), "File Not Found.", 15);
        cons_newline(cons);
    }
    return;
    
}
