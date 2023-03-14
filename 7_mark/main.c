//
// Created by Карим Хамид on 14.03.2023.
//
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include "../taskSolver.c"

const int buf_size = 10000;
sem_t *sem_to_process_data;

void fileHandler(int fd, char* input_file_name, char* output_file_name) {
    sem_wait(sem_to_process_data);

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
    sem_post(sem_to_process_data);
    printf("Reader exit\n");
    fflush(NULL);
    usleep(5000);    // sleep на 0.005 секунды чтобы семафор точно переключился

    //  ----------------------------- writer to file -----------------------------------

    sem_wait(sem_to_process_data);

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

void dataProcess(int fd) {
    sem_wait(sem_to_process_data);

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

    sem_post(sem_to_process_data);
    printf("Process exit\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Not enough arguments\n");
        return 0;
    }

    int fd, i;
    pid_t pid;

    mknod(argv[3], S_IFIFO | 0666, 0);
    if ((fd = open(argv[3], O_RDWR)) < 0) {
        printf("Can\'t open fifo\n");
        exit(-1);
    }

    sem_to_process_data = sem_open("pSem", O_CREAT | O_EXCL, 0644, 1);

    for (i = 0; i < 2; ++i) {
        pid = fork();

        if (pid < 0) {
            sem_unlink("pSem");
            sem_close(sem_to_process_data);
            printf("\nfork error\n");
        } else if (pid == 0) {
            break;
        }
    }

    if (pid != 0) {  // parent
        while ((pid = waitpid(-1, NULL, 0))) {
            if (errno == ECHILD)
                break;
        }

        printf("\nParent: All children have exited.\n");

        sem_unlink("pSem");
        sem_close(sem_to_process_data);
        exit(0);
    } else {  // child
        switch (i) {
            case 0:
                fileHandler(fd, argv[1], argv[2]);
                break;
            case 1:
                dataProcess(fd);
                break;
        }
    }

    return 0;
}
