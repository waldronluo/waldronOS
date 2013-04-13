/*graphic.h*/
void boxfill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1 , int y1 );
void putfont8 ( char *vram , int xsize, int x , int y , char c , char *font );
void putfonts8_asc ( char *vram , int xsize , int x , int y , char c , unsigned char *s );
void putblock8_8( char* vram , int vxsize , int pxsize , int pysize , int px0 , int py0 , char* buf , int bxsize );
void init_palette(void);
void set_palette(int start, int end, unsigned char* rgb);
int  find_palette( int color );
void write_mem8(int ,int );
void init_screen( char* , int, int );
void init_mouse_curosr8( char* mouse , char bc );

/* asmhead.nas*/
struct BOOTINFO {
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
 	char* vram;
};
#define ADR_BOOTINFO 0x00000ff0

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
void io_out8 ( int port , int data );
int io_load_eflags (void);
void io_store_eflags ( int eflags );
void asm_inthandler21 (void);
void asm_inthandler27 (void);
void asm_inthandler2c (void);
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

void init_pic(void);
void inthandler21 ( int* esp );
void inthandler27 ( int* esp );
void inthandler2c ( int* esp );

/*queue.c*/
#define FLAGS_OVERRUN 0x0001
struct Queue8 {
	unsigned char* data;
	int front, rear, size, flags;
	/*
	front: 	front of the queue | rear: rear of the queue |
	size: size of the queue
	flag: 	flag of overflow 
	In my structure, the there's always a free position that can't be used.
	It's unacceptable in early Computer world but not now so...
	Just do  it.
	*/
};
void queue8_init ( struct Queue8* queue , int size , unsigned char* data );
int queue8_put ( struct Queue8* queue, unsigned char data );
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

unsigned int memtest( unsigned int start , unsigned int end );
void memman_init ( struct MEMMAM *man );
unsigned int memman_total ( struct MEMMAN* man );
unsigned int memman_alloc ( struct MEMMAN* man , unsigned int size );
int memman_free ( struct MEMMAN* man , unsigned int addr , unsigned int size );
unsigned int memman_alloc_4k (struct MEMMAN* man , unsigned int size );
int memman_free_4k ( struct MEMMAN* man , unsigned int addr , unsigned int size );
