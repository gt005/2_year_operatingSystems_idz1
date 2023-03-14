#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include "../taskSolver.c"

const int buf_size = 10000;

void fileHandler(int fd, char* input_file_name, char* output_file_name) {
    char str_buf[buf_size];
    size_t size_of_file;

    printf("reader started\n");

    int input_file = open(input_file_name, O_RDONLY);
    if (input_file <= 0) {
        printf("\nCan't open file to read\n");
        close(input_file);
        return;
    }
    size_of_file = read(input_file, str_buf, buf_size);

    close(input_file);

    int size = write(fd, str_buf, size_of_file);

    if (size != size_of_file) {
        printf("Can\'t write all string to pipe\n");
        return;
    }
    printf("Reader exit\n");
    fflush(NULL);
    usleep(5000);    // sleep на 0.005 секунды чтобы второй файл точно успел начать чтение

    //  ----------------------------- writer to file -----------------------------------

    printf("writer started\n");
    int result;

    size = read(fd, &result, sizeof(int));
    if (size < 0) {
        printf("Can\'t read string from pipe\n");
        return;
    }

    int output_file = open(output_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (output_file <= 0) {
        printf("\nCan't open file to write\n");
        close(output_file);
        return;
    }

    char str[10];
    int size_ = sprintf(str, "%d", result);

    write(output_file, str, size_);
    close(output_file);

    if (close(fd) < 0) {
        printf("child: Can\'t close reading side of pipe\n");
        return;
    }
    printf("Writer exit\n");
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Not enough arguments\n");
        return 0;
    }

    int fd;

    mknod(argv[3], S_IFIFO | 0666, 0);
    if ((fd = open(argv[3], O_RDWR)) < 0) {
        printf("Can\'t open fifo\n");
        exit(-1);
    }

    printf("\n5 секунд для запуска файла dataProcess\n");
    sleep(5);

    fileHandler(fd, argv[1], argv[2]);
    return 0;
}