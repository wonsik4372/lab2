/* POSIX 공유 메모리/mmap 을 이용한 파일 복사
 * 파일 이름: shm_copy.c
 * 빌드: gcc -o shm_copy shm_copy.c -lrt -lpthread
 */
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
 
#define SHMNAME "/my_copy_shm"
#define SEMNAME "/my_copy_sem"
#define MAX_WRITE 4096
 
void child_reader(const char *dst)
{
    int shmd, dst_fd;
    void *shmaddr;
    size_t size, off;
    ssize_t n;
    struct stat buf;
    sem_t *semd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;   /* == 0644 */
 
    /* testpsem.c: 부모가 공유 메모리를 다 채울 때까지 대기 */
    if ((semd = sem_open(SEMNAME, 0)) == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }
    sem_wait(semd);
 
    /* readpshm.c: 공유 메모리를 읽기 전용으로 매핑 */
    if ((shmd = shm_open(SHMNAME, O_RDONLY, 0666)) == -1) {
        perror("shm_open failed");
        exit(1);
    }
    fstat(shmd, &buf);
    if ((shmaddr = mmap(0, buf.st_size, PROT_READ, MAP_SHARED, shmd, 0)) == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    size = *(size_t *)shmaddr;
 
    /* file_copy.c: creat 후 write 로 저장 */
    if ((dst_fd = creat(dst, mode)) == -1) {
        perror("dst open");
        exit(1);
    }
    for (off = 0; off < size; off += n) {
        n = write(dst_fd, (char *)shmaddr + sizeof(size_t) + off,
                  size - off > MAX_WRITE ? MAX_WRITE : size - off);
        if (n <= 0) {
            perror("write");
            exit(1);
        }
    }
    printf("[child ] wrote %zu bytes from shared memory to %s\n", size, dst);
    close(dst_fd);
    munmap(shmaddr, buf.st_size);
    close(shmd);
    sem_close(semd);
    exit(0);
}
 
int main(int argc, char *argv[])
{
    int src_fd, shmd;
    void *shmaddr, *srcaddr;
    size_t size, shmsize;
    struct stat buf;
    sem_t *semd;
    pid_t pid;
 
    if (argc < 3) {
        fprintf(stderr, "Usage: shm_copy src_file dest_file\n");
        exit(1);
    }
    if ((src_fd = open(argv[1], O_RDONLY)) == -1) {
        perror("src open");
        exit(1);
    }
    fstat(src_fd, &buf);
    size = buf.st_size;
 
    /* testpsem.c: 초기값 0 인 세마포어 (자식은 부모가 post 할 때까지 대기) */
    sem_unlink(SEMNAME);
    if ((semd = sem_open(SEMNAME, O_CREAT, 0600, 0)) == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }
 
    fflush(stdout);
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    if (pid == 0)
        child_reader(argv[2]);
 
    /* writepshm.c: 공유 메모리 생성, 크기 지정, 매핑 */
    shmsize = sizeof(size_t) + size;
    if ((shmd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0666)) == -1) {
        perror("shm_open failed");
        exit(1);
    }
    if (ftruncate(shmd, shmsize) != 0) {
        perror("ftruncate failed");
        exit(1);
    }
    if ((shmaddr = mmap(0, shmsize, PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0)) == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    if (mlock(shmaddr, shmsize) != 0)        /* 실패해도 복사는 가능하므로 경고만 */
        perror("mlock");
 
    /* mmaptest.c: 원본 파일을 메모리에 매핑하여 memcpy 로 공유 메모리에 복사 */
    *(size_t *)shmaddr = size;
    if (size > 0) {
        if ((srcaddr = mmap(0, size, PROT_READ, MAP_PRIVATE, src_fd, 0)) == MAP_FAILED) {
            perror("mmap src failed");
            exit(1);
        }
        memcpy((char *)shmaddr + sizeof(size_t), srcaddr, size);
        munmap(srcaddr, size);
    }
    printf("[parent] copied %zu bytes of %s into shared memory %s\n", size, argv[1], SHMNAME);
    fflush(stdout);
    sem_post(semd);                          /* 자식에게 준비 완료 알림 */
 
    waitpid(pid, NULL, 0);
    if (munmap(shmaddr, shmsize) == -1)
        perror("munmap failed");
    close(shmd);
    close(src_fd);
    if (shm_unlink(SHMNAME) == -1)           /* readpshm.c: 공유 메모리 삭제 */
        perror("shm_unlink failed");
    sem_close(semd);
    sem_unlink(SEMNAME);
    printf("[parent] done\n");
    return 0;
}
