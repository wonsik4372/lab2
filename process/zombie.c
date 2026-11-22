/* zombie.c   
 * zombie 프로세스 실행 확인
 * 수정: unistd.h 추가 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    pid_t pid;
 
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)   /* child */
        exit(0);

    /* parent: wait 하지 않고 4초 대기 -> 자식은 좀비 상태로 남음 */
    sleep(4);
    system("ps -o pid,ppid,state,tty,command");
    exit(0);
}
