#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>

void swap(int *aa, int *bb)
{
    int tmp = *aa;
    usleep(1);
    *aa = *bb;
    *bb = tmp;
}

int isInteger(char *str)
{
    int length = strlen(str);
    for (int i = 0; i < length; i++)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

int *c;
sem_t *mutex;

int main(int argc, char **argv)
{
    if (argc != 6)
    {
        fprintf(stderr, "Invalid count of parameters.\n");
        return 1;
    }
    for (int i = 1; i < argc; i++)
    {
        if (!isInteger(argv[i]))
        {
            fprintf(stderr, "Invalid input parameters. All parameters should be integers.\n");
            return 1;
        }
    }

    unsigned L = atoi(argv[1]);
    unsigned Z = atoi(argv[2]);
    unsigned K = atoi(argv[3]);
    unsigned TL = atoi(argv[4]);
    unsigned TB = atoi(argv[5]);
    if (L >= 20000 || Z > 10 || K < 10 || K > 100 || TL > 10000 || TB > 1000)
    {
        fprintf(stderr, "Invalid input parameters.\n");
        return 1;
    }

    c = mmap(NULL, sizeof(int) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if ((c == MAP_FAILED) || (mutex == MAP_FAILED))
    {
        printf("mmap failed\n");
        exit(1);
    }

    c[0] = 1;
    c[1] = 2;
    int res;
    sem_init(mutex, 1, 1);

    res = fork();
    if (res < 0)
    {
        return 1;
    }
    for (int i = 0; i < 100; i++)
    {
        sem_wait(mutex);
        swap(&(c[0]), &(c[1]));
        sem_post(mutex);
        printf(">>> %i %i\n", c[0], c[1]);
    }
    if (res > 0)
    {
        wait(NULL);
        sem_destroy(mutex);
    }
    printf("konec\n");
    munmap(c, sizeof(int) * 2);
    munmap(mutex, sizeof(sem_t));
    return 0;
}
