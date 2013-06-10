void api_putstr0(char *s);
void api_putchar(int s);
void api_end(void);

void HariMain (void)
{
    volatile char *c = "helloworld\n";
//    char c[3];
    char a = 'a';
    c[0] = 'a';
//    c[0] = 'h';
//    c[1] = '\n';
//    c[2] = 0;
    api_putchar(a);
//    api_putstr0(c);
    api_end();
}
