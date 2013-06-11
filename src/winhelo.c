int rand(void);


int api_openwin (char *buf, int xsiz, int ysiz, int col_inv, char *title);
void api_putstrwin (int win, int x, int y, int col, int len, char *str);
void api_boxfilwin (int win, int x0, int y0, int x1, int y1, int col );
void api_initmalloc (void);
void api_point (int win, int x, int y, int col);
char* api_malloc (int size);
void api_refreshwin (int win, int x0, int y0, int x1, int y1);
void api_end();
void api_linewin (int win, int x0, int y0, int x1, int y1, int col);
void api_closewin (int win);

//char buf[150 * 50];
void HariMain (void)
{
    char *buf;
    int win;
    int i;
    api_initmalloc();
    buf = api_malloc (150 * 100);
    //while (1);
    win = api_openwin (buf, 150, 100, -1, "star");
    api_boxfilwin(win, 6, 26, 143, 93, 0);
    for (i = 0;i < 50; i ++ ) {
        api_linewin (win, (rand() % 137) + 6, 88, 26 ,(rand() % 67) + 26, (i%8)+1);
    //    api_linewin (win, 6, 88, 26 ,7, 3);
    }
    api_refreshwin (win, 6, 26, 144, 94);
    api_closewin(win);
    api_end();
}
