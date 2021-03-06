
#define STOP while(1)io_hlt();


/*precompile*/
struct TASK;


/* asmhead.nas*/
struct BOOTINFO {
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
 	char* vram;
};
#define ADR_BOOTINFO 0x00000ff0
#define ADR_DISKIMG 0x00100000


/*dsctbl.c*/
#define ADR_IDT 0x0026f800
#define LIMIT_IDT 0x000007ff
#define ADR_GDT 0x00270000
#define LIMIT_GDT 0x0000ffff
#define ADR_BOTPAK 0x00280000
#define LIMIT_BOTPAK 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_CODE32_ER 0x409a
#define AR_INTGATE32 0x008e
#define AR_TSS32 0x0089

struct SEGMENT_DESCRIPTOR {
	short limit_low , base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
	short offset_low , selector;
	char dw_count, access_right;
	char offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR* gd , int offset, int selector, int ar);


/*naskfunc.nas*/
void load_gdtr (int limit , int addr);
void load_idtr (int limit , int addr);
void io_hlt (void);
void io_cli (void);
void io_sti (void);
void io_stihlt (void);
int io_in8 (int port);
void load_tr(int tr);
void farjmp (int eip, int cs);
void farcall (int eip, int cs);
void io_out8 ( int port , int data );
int io_load_eflags (void);
void io_store_eflags ( int eflags );
void asm_inthandler0c (void);
void asm_inthandler0d (void);
void asm_inthandler20 (void);
void asm_inthandler21 (void);
void asm_inthandler27 (void);
void asm_inthandler2c (void);
void asm_cons_putchar (void);
void asm_wal_api (void);
void asm_end_app (void);
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
int memtest_sub (int start , int end);
int load_cr0(void);
int store_cr0(int cr0);

/*int.c*/
#define PIC0_ICW1 0x0020
#define PIC0_OCW2 0x0020
#define PIC0_IMR 0x0021
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC1_ICW1 0x00a0
#define PIC1_OCW2 0x00a0
#define PIC1_IMR 0x00a1
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1
#define PORT_KEYDAT 0x0060
#define KEYDATA0 256
#define MOUSEDATA0 768

void init_pic(void);
int inthandler0c ( int* esp );
int inthandler0d ( int* esp );
void inthandler20 ( int* esp );
void inthandler21 ( int* esp );
void inthandler27 ( int* esp );
void inthandler2c ( int* esp );

/*queue.c*/
#define FLAGS_OVERRUN 0x0001
struct Queue8 {
	unsigned int* data;
	int front, rear, size, flags;
	struct TASK *task;
	/*
	front: 	front of the queue | rear: rear of the queue |
	size: size of the queue
	flag: 	flag of overflow 
	In my structure, the there's always a free position that can't be used.
	It's unacceptable in early Computer world but not now so...
	Just do  it.
	*/
};
void queue8_init ( struct Queue8* queue , int size , unsigned int* data, struct TASK *task );
int queue8_put ( struct Queue8* queue, unsigned int data );
int queue8_get ( struct Queue8* queue );
int queue8_status ( struct Queue8* queue );
int queue8_free ( struct Queue8* queue );
int isOverRun ( struct Queue8* queue );
/*
	status: how much data? 
	free: 	size - statu
*/

/*keyboard.c*/
#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA 0x0064
#define PORT_KEYCMD 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47
void init_keyboard(void);
void wait_KBC_sendready(void);

/*mouse.c*/
#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4
struct MOUSE_DEC{
	unsigned char buf[3] , phase;
	int x,y,btn;
};
void enable_mouse(void);
void init_mouse ( struct MOUSE_DEC* mdec );
int mouse_decode ( struct MOUSE_DEC* mdec, unsigned char data );

/*memory.h*/
#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLE 0x60000000
#define MEMMAN_FREES 4090
#define MEMMAN_ADDR 0x003c0000
struct FREEINFO {
	unsigned int addr , size ;
};
struct MEMMAN {
	int frees, maxfrees , lostsize , losts;
	struct FREEINFO free[MEMMAN_FREES];
};
unsigned int memtest( unsigned int start , unsigned int end );
void memman_init ( struct MEMMAN *man );
unsigned int memman_total ( struct MEMMAN* man );
unsigned int memman_alloc ( struct MEMMAN* man , unsigned int size );
int memman_free ( struct MEMMAN* man , unsigned int addr , unsigned int size );
unsigned int memman_alloc_4k (struct MEMMAN* man , unsigned int size );
int memman_free_4k ( struct MEMMAN* man , unsigned int addr , unsigned int size );

/* sheet.c */
#define MAX_SHEETS 256
#define SHEET_USE 1
struct SHEET {
    struct TASK *task;
	struct SHTCTL* ctl;
	unsigned char* buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags ;
};

struct SHTCTL {
	unsigned char *vram, *map;
	int xsize, ysize, top;
	struct SHEET* sheets[MAX_SHEETS];//-----|pointing
	struct SHEET sheets0[MAX_SHEETS];// <---|back
};

struct SHTCTL* shtctl_init ( struct MEMMAN* memman, unsigned char* vram, int xsize, int ysize );
void sheet_setbuf ( struct SHEET* sht, unsigned char* buf, int xsize, int ysize, int col_inv );
struct SHEET* sheet_alloc ( struct SHTCTL* ctl );
void sheet_updown (  struct SHEET* sht, int height );
void sheet_refresh (  struct SHEET *sht, int bx0, int by0, int bx1, int by1 );
void sheet_slide (  struct SHEET *sht, int vx0, int vy0 );
void sheet_free (  struct SHEET *sht );
void sheet_refreshsub ( struct SHTCTL* ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1 );
void sheet_refreshmap ( struct SHTCTL* ctl, int vx0, int vy0, int vx1, int vy1, int h0 );



/* timer.h */
#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040
#define MAX_TIMER   500
#define TIMER_FLAGS_ALLOC 1
#define TIMER_FLAGS_USING 2

struct TIMER {
	struct TIMER *next;
	unsigned int timeout;
    char flags,flags2;
	struct Queue8 *queue;
	unsigned int data;
};
struct TIMERCTL {
	unsigned int count, next;
	struct TIMER *t0;
	struct TIMER timers0[MAX_TIMER];
};

void init_pit ( void ) ;
void inthandler20(int *esp );
struct TIMER *timer_alloc ( void );
void timer_free (struct TIMER *timer);
void timer_init ( struct TIMER *timer, struct Queue8 *queue, unsigned int data );
void timer_settimer ( struct TIMER *timer, unsigned int timeout );
int timer_cancel (struct TIMER *timer);
void timer_cancelall (struct Queue8 *queue);

/*graphic.h*/
void boxfill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1 , int y1 );
void putfont8 ( char *vram , int xsize, int x , int y , char c , char *font );
void putfonts8_asc ( char *vram , int xsize , int x , int y , char c , unsigned char *s );
void putfonts8_asc_sht ( struct SHEET *sht, int x, int y, int c, int b, char *s, int l );
void putblock8_8( char* vram , int vxsize , int pxsize , int pysize , int px0 , int py0 , char* buf , int bxsize );
void init_palette(void);
void set_palette(int start, int end, unsigned char* rgb);
int  find_palette( int color );
void write_mem8(int ,int );
void init_screen( char* , int, int );
void init_mouse_curosr8( char* mouse , char bc );
void make_window8 ( unsigned char* buf, int xsize, int ysize, char *title, char act );
void make_textbox8 ( struct SHEET* sht, int x0, int y0, int sx, int sy, int c );
void make_wtitle8(unsigned char *buf, int xsize, char *title, char act);
void change_wtitle (struct SHEET *sht, char act);

/*mtask.c*/
#define MAX_TASKS 1000
#define MAX_TASKS_LV 100
#define MAX_TASKLEVELS 10
#define TASK_GDT0 3
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

struct TASK {
	int sel, flags;
	int level, priority;
	struct Queue8 queue;
	struct TSS32 tss;
};

struct TASKLEVEL {
	int running;
	int now;
	struct TASK *tasks[MAX_TASKS_LV];
};

struct TASKCTL {
	int now_lv;
	char lv_change; // which one is running now
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};

extern struct TIMER *task_timer;

struct TASK *task_now (void);
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc (void);
void task_add (struct TASK *task);
void task_remove(struct TASK *task);
void task_run (struct TASK *task, int level, int priority);
void task_switch (void);
void task_sleep (struct TASK *task);


/* file.c */
struct FILEINFO {
    unsigned char name[8], ext[3], type;
    char reserve[10];
    unsigned short time, date, clustno;
    unsigned int size;
};
void file_readfat (int *fat, unsigned char *img);
void file_loadfile (int clustno, int size, char *buf, int *fat, char *img);
struct FILEINFO *file_search (char *name, struct FILEINFO *finfo, int max);

/* console.c */
struct CONSOLE {
    struct SHEET *sht;
    int cur_x, cur_y, cur_c;
    struct TIMER *timer;
};
void console_task (struct SHEET* sheet, unsigned int memtotal);
void cons_newline (struct CONSOLE* cons);
void cons_putchar (struct CONSOLE* cons, int chr, char move);
void cons_runcmd (char* cmdline, struct CONSOLE* cons, int *fat, unsigned int memtotal);
void cmd_mem (struct CONSOLE *cons, unsigned int memtotal);
void cmd_cls (struct CONSOLE *cons);
void cmd_ls (struct CONSOLE *cons);
void cmd_cat (struct CONSOLE *cons, int *fat, char *cmdline);
int  cmd_app (struct CONSOLE *cons, int *fat, char *cmdline);
void cons_putstr0 (struct CONSOLE *cons, char *s);
void cons_putstr1 (struct CONSOLE *cons, char *s, int length);
int wal_api (int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);
void wal_api_linewin (struct SHEET* sht, int x0, int y0, int x1, int y1, int col);



/* bootpack.c */
int keywin_off (struct SHEET *key_win, struct SHEET *sht_win, int cur_c, int cur_x);
int keywin_on (struct SHEET *key_win, struct SHEET *sht_win, int cur_c);
