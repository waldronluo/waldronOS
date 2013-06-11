int api_openwin (char *buf, int xsiz, int ysiz, int col_inv, char *title);
void api_putstrwin (int win, int x, int y, int col, int len, char *str);
void api_boxfilwin (int win, int x0, int y0, int x1, int y1, int col );
void api_initmalloc (void);
void api_point (int win, int x, int y, int col);
char* api_malloc (int size);

void api_end();
char buf[150 * 50];
void HariMain (void)
{
//    char *buf;
    int win;
    api_initmalloc();
    //buf = api_malloc (150 * 100);
    win = api_openwin (buf, 150, 100, -1, "star");
//    while (1);
    api_boxfilwin(win, 6, 26, 143, 93, 0);
    api_putstrwin(win, 28,28,0,12,"hello world");
    api_point(win, 75, 59, 3);
    api_end();
}
