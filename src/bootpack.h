/*graphic.h*/
static unsigned char table_rgb[16 * 3] = {
		0x00,0x00,0x00,
		0xff,0x00,0x00,
		0x00,0xff,0x00,
		0xff,0xff,0x00,
		0x00,0x00,0xff,
		0xff,0x00,0xff,
		0x00,0xff,0xff,
		0xff,0xff,0xff,
		0xc6,0xc6,0xc6,
		0x84,0x00,0x00,
		0x00,0x84,0x00,
		0x84,0x84,0x00,
		0x00,0x00,0x84,
		0x84,0x00,0x84,
		0x00,0x84,0x84,
		0x84,0x84,0x84,
		0x00,0x00,0x00,
};

static char cursor[17][17] = {
	"**************..",
	"*OOOOOOOOOOO*...",
	"*OOOOOOOOOO*....",
	"*OOOOOOOOO*.....",
	"*OOOOOOOO*......",
	"*OOOOOOO*.......",
	"*OOOOOO*........",
	"*OOOOOOO*.......",
	"*OOOOOOOO*......",
	"*OOOO*OOOO*.....",
	"*OOO*.*OOOO*....",
	"*OO*...*OOOO*...",
	"*O*.....*OOOO*..",
	"**.......*OOOO*.",
	"..........*OOOO*",
	"...........*****",
	"****************"
};

void io_hlt(void);
void io_cli(void);
void io_out8 ( int port , int data );
int io_load_eflags(void);
void io_store_eflags( int eflags );
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

struct BOOTINFO {
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
 	char* vram;
};
/*dsctbl.c*/
#define ADR_IDT 0x0026f800
#define LIMIT_IDT 0x000007ff
#define ADR_GDT 0x00270000
#define LIMIT_GDT 0x0000ffff
#define ADR_BOTPAK 0x00280000
#define LIMIT_BOTPAK 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_CODE32_ER

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
//void load_gdtr(int limit , int addr);
//void load_idtr(int limit , int addr);