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
    *((int *) 0x0fec) = (int) &cons;

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
    else if ((strncmp(cmdline, "type ", 5) == 0) || (strncmp(cmdline, "cat ", 4) == 0)) {
        cmd_cat(cons, fat, cmdline);
        if (cons->cur_x > 8 )
            cons_newline(cons);
    }
    //else if (strcmp(cmdline, "hlt") == 0)
    //    cmd_hlt(cons, fat);
    else if (cmdline[0] != 0) {
        if (cmd_app(cons, fat, cmdline) == 0) {
            putfonts8_asc_sht(cons->sht, 8, cons->cur_y, find_palette(0xffffff), find_palette(0), "BAD COMMAND", 12);
        }
    }
    return ;
}

void cmd_mem (struct CONSOLE *cons, unsigned int memtotal)
{
    struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
    char s[60];
    sprintf (s, "total\t%dMB\nfree\t%dKB\n\n", memtotal/(1<<20), memman_total(memman) / 1024);
    cons_putstr0(cons, s);
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
                sprintf(s, "filename.ext    %7d\n", finfo[x].size);
                for (y = 0;y < 8; y ++ )
                    s[y] = finfo[x].name[y];
                s[ 9] = finfo[x].ext[0];
                s[10] = finfo[x].ext[1];
                s[11] = finfo[x].ext[2];
                cons_putstr0 (cons, s);
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
    cmd_flag = (strncmp(cmdline, "cat ", 4) == 0) ? 4 : 5;
    finfo = file_search (cmdline + cmd_flag, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
    if ( finfo != 0 ) {
        p = (char *)memman_alloc_4k(memman, finfo->size);
        file_loadfile (finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x3e00));
        cons_putstr1 (cons, p, finfo->size);
        memman_free_4k(memman, (int) p, finfo->size);
    } else {
        cons_putstr0(cons, "File Not Found.\n");
    }
    return;
}

int cmd_app (struct CONSOLE *cons, int *fat, char *cmdline)
{
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    struct FILEINFO *finfo;
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    struct TASK *task = task_now();
    char name[18], *p, *q;
    int i;
    int segsiz, datsiz, esp, dathrb;

    for (i = 0;i < 13; i ++ ) {
        if (cmdline[i] <= ' ') break;

        name[i] = cmdline[i];
    }
    name[i] = 0;
    finfo = file_search (name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
    if (finfo == 0 && name[i-1] != '.') {
        name[i    ] = '.';
        name[i + 1] = 'w';
        name[i + 2] = 'a';
        name[i + 3] = 'l';
        name[i + 4] =  0 ;
        finfo = file_search (name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
    }
    if (finfo == 0 && name[i-1] != '.') {
        name[i    ] = '.';
        name[i + 1] = 'h';
        name[i + 2] = 'r';
        name[i + 3] = 'b';
        name[i + 4] =  0 ;
        finfo = file_search (name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
    }
    
    if ( finfo != 0 ) {
        p = (char *)memman_alloc_4k(memman, finfo->size);
//        q = (char *)memman_alloc_4k(memman, 64 * 1024);
//        *((int *)0xfe8) = (int) p;
        file_loadfile (finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x3e00));
        if (finfo->size >= 36 && strncmp(p + 4, "Hari", 4 ) == 0 && *p == 0x00) {
           segsiz = *((int *) (p + 0x0000));
           esp    = *((int *) (p + 0x000c));
           datsiz = *((int *) (p + 0x0010));
           dathrb = *((int *) (p + 0x0014));
           q = (char *) memman_alloc_4k (memman, segsiz);
           *((int *) 0xfe8) = (int) q;
           set_segmdesc(gdt + 1003, finfo->size - 1, (int) p, AR_CODE32_ER + 0x60);
           set_segmdesc(gdt + 1004, segsiz - 1, (int) q, AR_DATA32_RW + 0x60);
           for (i = 0;i < datsiz; i ++)
               q[esp+i] = p[dathrb + i];
           start_app(0x1b, 1003 * 8, esp, 1004 * 8, &(task->tss.esp0));
           memman_free_4k(memman, (int) q, segsiz);
        } else {
            cons_putstr0 (cons, "\n.hrb file format error.\n");
        }
        memman_free_4k (memman, (int) p, finfo->size);
        cons_newline(cons);
        return 1;
    }
    return 0;
}

void cons_putstr0 (struct CONSOLE *cons, char *s)
{
    for (; *s != 0 ; s ++ )
        cons_putchar(cons, *s, 1);
    return;
}

void cons_putstr1 (struct CONSOLE *cons, char *s, int l)
{
    int i;
    for (i = 0; i < l ; i ++ )
        cons_putchar(cons, s[i], 1);
    return;
}

int wal_api (int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
    int ds_base = *((int *) 0xfe8);

    struct TASK *task = task_now();
    struct CONSOLE *cons = (struct CONSOLE *) *((int *) 0x0fec);
    
    struct SHTCTL *shtctl = (struct SHTCTL *) *((int *)0x0fe4);
    struct SHEET *sht;
    int *reg = &eax + 1;
    if (edx == 1)
        cons_putchar(cons, eax & 0xff, 1);
    else if (edx == 2)
        cons_putstr0(cons, (char *)ebx + ds_base);
    else if (edx == 3)
        cons_putstr1(cons, (char *)ebx + ds_base, ecx);
    else if (edx == 4)
        return (int)&(task->tss.esp0);
    else if (edx == 5) {
        sht = sheet_alloc (shtctl);
        sheet_setbuf (sht, (char *)ebx + ds_base, esi, edi, eax);
        make_window8 ((char *) ebx + ds_base, esi, edi, (char *)ecx + ds_base, 0);
        sheet_slide (sht, 200, 200);
        sheet_updown (sht, 3);
        reg[7] = (int) sht;
    } else if ( edx == 6 ) { /*api_putstrwin */
        sht = (struct SHEET *) (ebx & 0xfffffffe);
        putfonts8_asc (sht->buf, sht->bxsize, esi, edi, eax, (char *) ebp + ds_base );
        if ((ebx & 1) == 0)
            sheet_refresh (sht, esi, edi, esi + ecx * 8, edi + 16 );
    } else if ( edx == 7 ) { /*api_boxfilwin */
        sht = (struct SHEET *) (ebx & 0xfffffffe);
        boxfill8 (sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
        if ((ebx & 1) == 0)
            sheet_refresh (sht, eax, ecx, esi + 1, edi + 1);
    } else if ( edx == 8 ) { /*init memman*/
        memman_init ((struct MEMMAN *) (ebx + ds_base));
        ecx &= 0xfffffff0;
        memman_free ((struct MEMMAN *) (ebx + ds_base), eax, ecx);
    } else if ( edx == 9 ) {/*malloc*/
        ecx = (ecx + 0x0f) & 0xfffffff0;
        reg[7] = memman_alloc ((struct MEMMAN *) (ebx + ds_base), ecx);
    } else if ( edx == 10) {/*free*/
        ecx = (ecx + 0x0f) & 0xfffffff0 ;
        memman_free ((struct MEMMAN *) (ebx + ds_base), eax, ecx);
    } else if ( edx == 11) {/*point*/
        sht = (struct SHEET *) (ebx & 0xfffffffe);
        sht->buf[sht->bxsize * edi + esi] = eax;
        if ((ebx & 1) == 0)
            sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
    } else if ( edx == 12) {/*refresh*/
        sht = (struct SHEET *)ebx;
        sheet_refresh(sht, eax, ecx, esi, edi);
    } else if ( edx == 13) { /*line*/
        sht = (struct SHEET *) (ebx & 0xfffffffe);
        wal_api_linewin (sht, eax, ecx, esi, edi, ebp);
        if ( (ebx & 1) == 0 )
            sheet_refresh (sht, eax, ecx, esi + 1, edi + 1);
    } else if ( edx == 14) {
        sheet_free ((struct SHEET *) ebx);
    }
    return 0;
}

void wal_api_linewin (struct SHEET* sht, int x0, int y0, int x1, int y1, int col)
{
    int i, x, y, len, dx, dy;

    dx = x1 - x0;
    dy = y1 - y0;
    x = x0 << 10;
    y = y0 << 10;

    dx = (dx < 0) ? -dx : dx;
    dy = (dy < 0) ? -dy : dy;

    if (dx >= dy) {
        len = dx + 1;
        dx = (x0 > x1) ? -1024 : 1024;
        dy = (y0 <= y1) ? ((y1 - y0 + 1) << 10)/len : ((y1 - y0 - 1) << 10)/len ;
    } else {
        len = dy + 1;
        dy = (y0 > y1) ? -1024 : 1024;
        dy = (x0 <= x1) ? ((x1 - x0 + 1) << 10)/len : ((x1 - x0 - 1) << 10)/len ;
    }
    
    for (i = 0;i < len;i ++) {
        sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
        x += dx;
        y += dy;
    }
    return ;
}
