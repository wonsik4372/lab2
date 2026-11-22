/* iopipe.c   
 * 로그인 사용자 수를 출력
 * 명령을 인자로 바꿀 수 있게 확장
 */
#include <stdio.h>
 
#define BUFSIZE 256
 
int main(int argc, char *argv[])
{
    FILE *pin, *pout;
    char buf[BUFSIZE];
    const char *cmd1 = argc > 2 ? argv[1] : "who";
    const char *cmd2 = argc > 2 ? argv[2] : "wc -l";
 
    pin = popen(cmd1, "r");
    pout = popen(cmd2, "w");
    while (fgets(buf, BUFSIZE, pin) != NULL)
        fputs(buf, pout);
    pclose(pin);
    pclose(pout);
    return 0;
}
