#include "bootpack.h"
#include <string.h>
#include <stdio.h>
void console_task (struct SHEET* sheet, unsigned int memtotal)
{
    //	struct Queue8 queue;
    struct TIMER *timer;
    struct TASK *task = task_now();
    struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
    //	int array[10000];	
    char s[30], cmdline[30], *p;
    int i, queuebuf[128], cursor_x = 16, cursor_y = 28 , cursor_c = -1;
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    /*fat*/
    int *fat = (int *)memman_alloc_4k (memman, 4 * 2880 );
    file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
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
            if ( i == 2 ) {
                cursor_c = find_palette(0xffffff);
            }
            if ( i == 3 ) {
                boxfill8(sheet->buf, sheet->bxsize, find_palette(0), cursor_x, 28, cursor_x + 7, 43);
                cursor_c = -1;
            }
            if (i >= 256 && i <= 511 ) {
                if (i == 8 + 256) {
                    if (cursor_x > 16) {
                        putfonts8_asc_sht (sheet, cursor_x, cursor_y, find_palette(0xffffff), find_palette(0), " ", 1 );
                        cursor_x -= 8;
                        putfonts8_asc_sht (sheet, cursor_x, cursor_y, find_palette(0xffffff), find_palette(0), " ", 1 );
                    }
                } else if (i == 10 + 256){
                    putfonts8_asc_sht(sheet, cursor_x, cursor_y, find_palette(0xffffff), find_palette(0), " ", 1);
                    /*cmdline*/
                    cmdline[cursor_x / 8 - 2] = 0;
                    cursor_y = cons_newline(cursor_y, sheet);
                    if ( (strcmp (cmdline, "dir") == 0) || (strcmp (cmdline, "ls") == 0) ) {
                        int x,y;
                        for ( x = 0 ; x < 224 ; x ++ ) {
                            if (finfo[x].name[0] == 0x00 )
                                break;
                            if (finfo[x].name[0] != 0xe5 ) {
                                if ((finfo[x].type & 0x18) == 0) {
                                    sprintf(s, "filename.ext    %7d", finfo[x].size);
                                    for (y = 0;y < 8;y ++) {
                                        s[y] = finfo[x].name[y];
                                    }
                                    s[9]  = finfo[x].ext[0];
                                    s[10] = finfo[x].ext[1];
                                    s[11] = finfo[x].ext[2];
                                    putfonts8_asc_sht (sheet, 8, cursor_y, find_palette(0xffffff), find_palette(0), s, 30);
                                    cursor_y = cons_newline(cursor_y, sheet);
                                }
                            }
                        }
                        cursor_y = cons_newline (cursor_y, sheet);
                    } else if ( ( strcmp (cmdline, "cls") == 0 ) || ( strcmp (cmdline, "clear") == 0 ) ) {
                        int x, y;
                        for ( y = 28 ; y < 28 + 16 * 29 ; y ++ )
                            for ( x = 8 ; x < 8 + 500 - 16 ; x ++ )
                                sheet->buf[x + y * sheet->bxsize] = find_palette(0);
                        sheet_refresh(sheet, 8 , 28 , 8 + 500 - 16 , 28 + 500 - 53 );
                        cursor_y = 28;
                    } else if (strcmp (cmdline, "mem") == 0 ) {
                        sprintf(s, "total %dMB", memtotal / (1024 * 1024) );
                        putfonts8_asc_sht (sheet, 8, cursor_y, find_palette(0xffffff), find_palette(0), s, 30);
                        cursor_y = cons_newline(cursor_y, sheet);
                        sprintf(s, "free %dKB", memman_total (memman) / 1024 );
                        putfonts8_asc_sht (sheet, 8, cursor_y, find_palette(0xffffff), find_palette(0), s, 30);
                        cursor_y = cons_newline(cursor_y, sheet);
                        cursor_y = cons_newline(cursor_y, sheet);
                    } else if ( (strncmp (cmdline, "cat ", 4) == 0)) {
                        int x,y;
                        for (y = 0;y < 11; y ++ )
                            s[y] = ' ';

                        y = 0;
                        for (x = 4; y < 11 && cmdline[x] != 0; x ++ ) {
                            if (cmdline[x] == '.' && y <= 8 ) {
                                y = 8;
                            } else {
                                s[y] = cmdline[x];
                                if ( 'a' <= s[y] && s[y] <= 'z' )
                                    s[y] -= 0x20;
                                
                                y++;
                            }
                        }

                        for (x = 0; x < 224 ; ) {
                            if (finfo[x].name[0] == 0x00)
                                break;
                            if ((finfo[x].type & 0x18) == 0) {
                                for (y = 0;y < 11; y ++ ) {
                                    if (finfo[x].name[y] != s[y] )
                                        goto type_next_file;
                                }
                                break;
                            }
type_next_file:
                            x++;
                        }

                        if ( x < 224 && finfo[x].name[0] != 0x00 ) {
                            p = (char *) memman_alloc_4k(memman, finfo[x].size);
                            file_loadfile (finfo[x].clustno, finfo[x].size, p, fat, (char*) ADR_DISKIMG + 0x003e00);
                    //        y = finfo[x].size;
                    //        p = (char *) (finfo[x].clustno * 512 + 0x003e00 + ADR_DISKIMG);
                            cursor_x = 8;
                            for (y = 0;y < finfo[x].size; y ++ ) {
                                s[0] = p[y];
                                s[1] = 0;
                                if ( s[0] == 0x09 ) {
                                    while (1) {
                                        putfonts8_asc_sht (sheet, cursor_x, cursor_y, find_palette(0xffffff), find_palette(0), " ", 1);
                                        cursor_x += 8;
                                        if ( cursor_x >= 500 - 8 ) {
                                            cursor_x = 8;
                                            cursor_y = cons_newline(cursor_y, sheet);
                                        }
                                        if (((cursor_x - 8) & 0x1f) == 0) {
                                            break;
                                        }
                                    }
                                } else if (s[0] == 0x0a) {
                                    cursor_x = 8;
                                    cursor_y = cons_newline (cursor_y, sheet);
                                } else if (s[0] == 0x0d) {
                                } else {
                                    putfonts8_asc_sht(sheet, cursor_x, cursor_y, find_palette(0xffffff), find_palette(0), s, 1);
                                    cursor_x += 8;
                                    if ( cursor_x >= 500 - 24 ) {
                                        cursor_x = 8;
                                        cursor_y = cons_newline(cursor_y, sheet);
                                    }       
                                }
                            }
                            memman_free_4k (memman, (int) p, finfo[x].size);
                        } else {
                            putfonts8_asc_sht (sheet, 8, cursor_y, find_palette(0xffffff), find_palette(0), "File Not Found", 15 );
                            cursor_y = cons_newline(cursor_y, sheet);
                        }
                        cursor_y = cons_newline(cursor_y, sheet);


                    } else if ( cmdline[0] != 0 ){
                        putfonts8_asc_sht (sheet, 8, cursor_y, find_palette(0xffffff), find_palette(0), "Bad command", 12 );
                        cursor_y = cons_newline(cursor_y, sheet);
                        cursor_y = cons_newline(cursor_y, sheet);
                    }
                    putfonts8_asc_sht(sheet, 8, cursor_y, find_palette(0xffffff), find_palette(0), ">", 1);
                    cursor_x = 16;
                } else {
                    if ( cursor_x < 500 - 24 ) {
                        s[0] = i - 256;
                        s[1] = 0;
                        cmdline[cursor_x / 8 - 2] = i - 256;
                        putfonts8_asc_sht(sheet, cursor_x, cursor_y, find_palette(0xffffff), find_palette(0), s, 1);	
                        cursor_x += 8;

                    }
                }
            }
            if ( cursor_c != -1 ) {
                boxfill8(sheet->buf, sheet->bxsize, cursor_c, cursor_x, cursor_y, cursor_x + 7, cursor_y + 16 );
                sheet_refresh(sheet, cursor_x, cursor_y, cursor_x + 8, cursor_y + 16 );
            }
            sheet_refresh(sheet, cursor_x, cursor_y, cursor_x+8, cursor_y+16);
        }
    }
}


int cons_newline (int cursor_y, struct SHEET* sheet)
{
    if ( cursor_y < 28 + 16 * 28 ) {
        cursor_y += 16;
    } else {
        int x,y;
        for ( y = 28; y <= 28 + 16 * 28 ; y ++ ) 
            for ( x = 8 ; x < 500 - 16; x ++ ) 
                sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize]; 

        for ( y = 28 + 16 * 28; y <= 28 + 16 * 29; y ++ )
            for ( x = 8 ; x < 500 - 16; x ++ ) 
                sheet->buf[x + y * sheet->bxsize] = find_palette(0);
        sheet_refresh(sheet, 8 , 28, 500 - 8, 28 + 16 * 29 );
    }
    return cursor_y;
}
