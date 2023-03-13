#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

const int buf_size = 25;
const int mes_size = 15;

int main(int argc, char *argv[]) {
    int fd[2];

    pid_t reader_process_id = fork();  // Создается процесс-читатель

    if (reader_process_id == 0) {
        pid_t worker_process_id = fork();  // Создается процесс-обработчик строки

        if (worker_process_id == 0) {
            printf("handler\n");
            fflush(NULL);
        } else {
            size_t size;
            char str_buf[buf_size];
            printf("\nwriter\n");
            fflush(NULL);
            if (pipe(fd) < 0) {
                printf("Can\'t open pipe\n");
                fflush(NULL);
                exit(-1);
            }

            if (close(fd[1]) < 0) {
                printf("child: Can\'t close writing side of pipe\n");
                fflush(NULL);
                exit(-1);
            }
            size = read(fd[0], str_buf, mes_size);
            if (size < 0) {
                printf("Can\'t read string from pipe\n");
                fflush(NULL);
                exit(-1);
            }
            printf("Child exit, str_buf: %s\n", str_buf);
            fflush(NULL);
            if (close(fd[0]) < 0) {
                printf("child: Can\'t close reading side of pipe\n");
                fflush(NULL);
                exit(-1);
            }

            printf("\nwriter ends\n");
            fflush(NULL);
            wait(NULL);
        }
    } else {
        size_t size;

        if(pipe(fd) < 0) {
            printf("Can\'t open pipe\n");
            fflush(NULL);
            exit(-1);
        }

        if (close(fd[0]) < 0) {
            printf("parent: Can\'t close reading side of pipe\n");
            fflush(NULL);
            exit(-1);
        }

        size = write(fd[1], "Hello, world!\0", mes_size);
        if (size != mes_size) {x
            printf("Can\'t write all string to pipe\n");
            fflush(NULL);
            exit(-1);
        }
        if (close(fd[1]) < 0) {
            printf("parent: Can\'t close writing side of pipe\n");
            fflush(NULL);
            exit(-1);
        }
        FILE* a = fopen("1.txt", "w");

        fprintf(a, "\n\nWriter exit\n\n");

        fflush(NULL);
        fclose(a);
        wait(NULL);
    }

    return 0;
}