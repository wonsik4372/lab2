/*
 * ls -R 과 같은 결과를 출력하는 프로그램
 * 파일 이름: myls_r.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_PATH 1024

int first = 1;                    /* 첫 디렉토리인지 (블록 사이 빈 줄 출력용) */

/* qsort 비교 함수: 대소문자 무시, 같으면 strcmp */
int compare(const void *a, const void *b)
{
    int r = strcasecmp(*(char * const *)a, *(char * const *)b);
    if (r != 0)
        return r;
    return strcmp(*(char * const *)a, *(char * const *)b);
}

void list(const char *dirname)
{
    DIR *pdir;
    struct dirent *pde;
    struct stat buf;
    char **names = NULL;          /* 항목 수를 모르므로 malloc/realloc 으로 늘림 */
    int n = 0, cap = 0, i;
    char path[MAX_PATH];

    /* 1) 이름 읽기 */
    if ((pdir = opendir(dirname)) == NULL) {
        fprintf(stderr, "myls_r2: cannot open directory '%s': ", dirname);
        perror(NULL);
        return;
    }
    while ((pde = readdir(pdir)) != NULL) {
        if (pde->d_name[0] == '.')            /* ., .., 숨김 파일 제외 */
            continue;
        if (n == cap) {                       /* 가득 차면 두 배로 늘림 */
            cap = (cap == 0) ? 16 : cap * 2;
            names = (char **)realloc(names, cap * sizeof(char *));
            if (names == NULL) {
                perror("realloc");
                exit(1);
            }
        }
        names[n] = (char *)malloc(strlen(pde->d_name) + 1);
        if (names[n] == NULL) {
            perror("malloc");
            exit(1);
        }
        strcpy(names[n], pde->d_name);
        n++;
    }
    closedir(pdir);

    /* 정렬 후 출력 */
    qsort(names, n, sizeof(char *), compare);
    if (!first)
        printf("\n");
    first = 0;
    printf("%s:\n", dirname);
    for (i = 0; i < n; i++)
        printf("%s\n", names[i]);

    /* 디렉토리면 같은 함수를 다시 호출 */
    for (i = 0; i < n; i++) {
        sprintf(path, "%s/%s", dirname, names[i]);   /* 상위 경로를 붙여야 lstat 이 찾음 */
        if (lstat(path, &buf) == 0 && S_ISDIR(buf.st_mode))
            list(path);
    }

    /* 이 호출에서 할당한 메모리 반환 */
    for (i = 0; i < n; i++)
        free(names[i]);
    free(names);
}

int main(int argc, char *argv[])
{
    list(argc > 1 ? argv[1] : ".");
    return 0;
}