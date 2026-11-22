/* proc_file.c   
 * fork 시 파일이 어떻게 취급되는지 보인다.
 * 수정: fatal() 정의, switch 괄호 오류 수정, 헤더 추가 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
 
void fatal(const char *msg)
{
    perror(msg);
    exit(1);
}
 
/* 파일 내에서 위치 출력 */
void printpos(const char *string, int filedes)
{
    off_t pos;
 
    if ((pos = lseek(filedes, 0, SEEK_CUR)) == -1)
        fatal("lseek failed");
    printf("%s:%ld\n", string, (long)pos);
}
 
int main()
{
    int fd;
    pid_t pid;           /* 프로세스 식별번호 */
    char buf[10];        /* 파일 자료를 저장할 버퍼 */
 
    if ((fd = open("data", O_RDONLY)) == -1)
        fatal("open failed");
    read(fd, buf, 10);   /* 파일 포인터를 전진시킨다 */
    printpos("Before fork", fd);
    fflush(stdout);
 
    /* 이제 두 개의 프로세스를 생성한다 */
    switch (pid = fork()) {
    case -1:             /* 오류 */
        fatal("fork failed");
        break;
    case 0:              /* 자식 */
        printpos("Child before read", fd);
        read(fd, buf, 10);
        printpos("Child after read", fd);
        break;
    default:
        wait((int *) 0);
        printpos("Parent after wait", fd);
    }
    return 0;
}
