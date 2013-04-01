void io_hlt(void);
void write_mem8(int ,int );

void HariMain(void)
{
	int i;
	char *vram=0xa0000;
	for ( i=0 ; i<=0xffff ; i++ )
		vram[i]=15;


	;
	for ( ;; )
		io_hlt();
}
