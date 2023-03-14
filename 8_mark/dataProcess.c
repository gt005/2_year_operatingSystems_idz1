#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include "../taskSolver.c"

const int buf_size = 10000;

void dataProcess(int fd) {

    printf("process started\n");
    char str_buf[buf_size];

    int size = read(fd, str_buf, buf_size);
    if (size < 0) {
        printf("Can\'t read string from pipe\n");
        return;
    }

    int result = taskSolver(str_buf, size);

    size = write(fd, &result, sizeof(int));

    if (size != sizeof(int)) {
        printf("Can\'t write all string to pipe\n");
        return;
    }

    printf("Process exit\n");
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Not enough arguments\n");
        return 0;
    }

    int fd;

    mknod(argv[1], S_IFIFO | 0666, 0);
    if ((fd = open(argv[1], O_RDWR)) < 0) {
        printf("Can\'t open fifo\n");
        exit(-1);
    }

    dataProcess(fd);
    return 0;
}