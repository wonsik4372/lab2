/*
 * 7장 파일 처리
 * 파일 이름: file_chdir.c  
 * 수정: string.h 추가
 * getcwd 실패 검사를 == NULL 로 수정 (getcwd 는 포인터 반환)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // 추가 
#include <unistd.h>
 
#define MAX_BUF 256
 
int main(int argc, char *argv[])
{
    char buf[MAX_BUF];
 
    if (argc < 2) {
        fprintf(stderr, "Usage: file_chdir dirname\n");
        exit(1);
    }
    memset(buf, 0, MAX_BUF);
    if (getcwd(buf, MAX_BUF) == NULL) {
        perror("getcwd");
        exit(1);
    }
    printf("working directory (before) = %s\n", buf);
    if (chdir(argv[1]) < 0) {
        perror("chdir");
        exit(1);
    }
    memset(buf, 0, MAX_BUF);
    if (getcwd(buf, MAX_BUF) == NULL) {
        perror("getcwd");
        exit(1);
    }
    printf("working directory (after ) = %s\n", buf);
    return 0;
}

