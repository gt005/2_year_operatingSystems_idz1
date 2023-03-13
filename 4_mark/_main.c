// 02-parent-child.c
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

const int buf_size = 5000;
const int mes_size = 5000;
sem_t *sem;

void fileReader(int *fd) {
    char str_buf[buf_size];
    printf("reader started\n");
    if (close(fd[0]) < 0) {
        printf("parent: Can\'t close reading side of pipe\n");
        return;
    }

    FILE *a = fopen("input.txt", "rt");
    if (a <= 0) {
        printf("\nCan't open file to read\n");
        fclose(a);
        return;
    }
    fscanf(a, "%s", str_buf);
    fclose(a);

    int size = write(fd[1], str_buf, mes_size);

    if (size != mes_size) {
        printf("Can\'t write all string to pipe %d %d\n", size, mes_size);
        return;
    }
    if (close(fd[1]) < 0) {
        printf("parent: Can\'t close writing side of pipe\n");
        return;
    }

    printf("Parent exit\n");
}

void fileWriter(int *fd) {
    char str_buf[buf_size];

    if (close(fd[1]) < 0) {
        printf("child: Can\'t close writing side of pipe\n");
        return;
    }
    int size = read(fd[0], str_buf, mes_size);
    if (size < 0) {
        printf("Can\'t read string from pipe\n");
        return;
    }

    FILE *a = fopen("output.txt", "wt");
    if (a <= 0) {
        printf("\nCan't open file to read\n");
        fclose(a);
        return;
    }
    fprintf(a, "%s", str_buf);
    fclose(a);

    if (close(fd[0]) < 0) {
        printf("child: Can\'t close reading side of pipe\n");
        return;
    }
}

int main() {
    int fd[2], result, i;
    pid_t pid;

    unsigned int value = 1;

    if (pipe(fd) < 0) {
        printf("Can\'t open pipe\n");
        exit(-1);
    }

    sem = sem_open("pSem", O_CREAT | O_EXCL, 0644, value);

    for (i = 0; i < 2; ++i) {
        pid = fork();

        if (pid < 0) {
            sem_unlink("pSem");
            sem_close(sem);
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
        sem_close(sem);
        exit(0);
    } else {
        switch (i) {
            case 0:
                sem_wait(sem);

                fileReader(fd);

                sem_post(sem);
                break;
            case 1:
                sem_wait(sem);

                fileWriter(fd);

                sem_post(sem);
                break;
        }
    }


    return 0;
}
