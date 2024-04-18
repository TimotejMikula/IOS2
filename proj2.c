/**
 * @file proj2.c
 * @brief IOS – projekt 2 (synchronizace)
 *
 * Zadání je inspirováno knihou Allen B. Downey: The Little Book of Semaphores (The Senate Bus
 * problem)
 *
 * @author Timotej Mikula, xmikult00
 * @date 16.4.2024
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include "proj2.h"

sem_t *mutex_output;
sem_t *mutex_bus_stop;
sem_t *mutex_queue_update;
Bus_Stop busstop1;
Bus_Stop busstop2;
Bus_Stop busstop3;
Bus_Stop busstop4;
Bus_Stop busstop5;
Bus_Stop busstop6;
Bus_Stop busstop7;
Bus_Stop busstop8;
Bus_Stop busstop9;
Bus_Stop busstop10;
FILE *file;
bool *post_open;
int *action_id;
int seed;

Arg ParseArgs(int argc, char *const argv[])
{
    Arg args;

    if (argc != 6)
    {
        exit_error("Incorrect count of args!\n", 1);
    }
    for (int i = 1; i < argc; i++)
    {
        if (!isInteger(argv[i]))
        {
            exit_error("Invalid input parameters. All parameters should be integers.\n", 1);
        }
    }

    args.L = atoi(argv[1]);
    args.Z = atoi(argv[2]);
    args.K = atoi(argv[3]);
    args.TL = atoi(argv[4]);
    args.TB = atoi(argv[5]);

    if (args.L >= 20000 || args.Z > 10 || args.K < 10 || args.K > 100 || args.TL > 10000 || args.TB > 1000)
    {
        exit_error("Invalid input parameters.\n", 1);
    }

    return args;
}

void exit_error(char *msg, int errcode)
{
    fprintf(stderr, "ERROR: %s Exit code: %d\n", msg, errcode);
    exit(errcode);
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

void skibus(Arg args)
{
    output(BUS_STARTED, NONE, NONE);
    // for (int i = 1; i <= args.Z; i++)
    // {
    //     usleep_random_in_range(0, args.TB);
    //     output(BUS_ARRIVED_TO, NONE, i);
    //     wait_sem(&mutex_bus_stop);
    //     // if (bus_at_stop(i))
    //     // {
    //     //     //
    //     // }
    //     output(BUS_LEAVING, NONE, i);
    // }
    exit(0);
}

void skier_busstop(int id, int idz)
{
    wait_sem(&mutex_queue_update);
    if (idz == 1)
    {
        (*(busstop1.count))++;
        wait_sem(&(busstop1.queue));
        output(L_ARRIVED_TO, id, idz);
    }

    if (idz == 2)
    {
        (*(busstop2.count))++;
        wait_sem(&(busstop2.queue));
        output(L_ARRIVED_TO, id, idz);
    }

    if (idz == 3)
    {
        (*(busstop3.count))++;
        wait_sem(&(busstop3.queue));
        output(L_ARRIVED_TO, id, idz);
    }

    if (idz == 4)
    {
        (*(busstop4.count))++;
        wait_sem(&(busstop4.queue));
        output(L_ARRIVED_TO, id, idz);
    }

    if (idz == 5)
    {
        (*(busstop5.count))++;
        wait_sem(&(busstop5.queue));
        output(L_ARRIVED_TO, id, idz);
    }

    if (idz == 6)
    {
        (*(busstop6.count))++;
        wait_sem(&(busstop6.queue));
        output(L_ARRIVED_TO, id, idz);
    }

    if (idz == 7)
    {
        (*(busstop7.count))++;
        wait_sem(&(busstop7.queue));
        output(L_ARRIVED_TO, id, idz);
    }

    if (idz == 8)
    {
        (*(busstop8.count))++;
        wait_sem(&(busstop8.queue));
        output(L_ARRIVED_TO, id, idz);
    }

    if (idz == 9)
    {
        (*(busstop9.count))++;
        wait_sem(&(busstop9.queue));
        output(L_ARRIVED_TO, id, idz);
    }

    if (idz == 10)
    {
        (*(busstop10.count))++;
        wait_sem(&(busstop10.queue));
        output(L_ARRIVED_TO, id, idz);
    }
    post_sem(&mutex_queue_update);
    bus_at_stop(idz);
}

bool bus_at_stop(int busstop)
{
    bool result = true;

    if (busstop == ANY)
    {
        if (*(busstop1.count) == 0 && *(busstop2.count) == 0 && *(busstop3.count) == 0 && *(busstop4.count) == 0 && *(busstop5.count) == 0 && *(busstop6.count) == 0 && *(busstop7.count) == 0 && *(busstop8.count) == 0 && *(busstop9.count) == 0 && *(busstop10.count) == 0)
        {
            result = false;
        }
    }
    else if (busstop == 1 && *(busstop1.count) == 0)
    {
        result = false;
    }
    else if (busstop == 2 && *(busstop2.count) == 0)
    {
        result = false;
    }
    else if (busstop == 3 && *(busstop3.count) == 0)
    {
        result = false;
    }
    else if (busstop == 4 && *(busstop4.count) == 0)
    {
        result = false;
    }
    else if (busstop == 5 && *(busstop5.count) == 0)
    {
        result = false;
    }
    else if (busstop == 6 && *(busstop6.count) == 0)
    {
        result = false;
    }
    else if (busstop == 7 && *(busstop7.count) == 0)
    {
        result = false;
    }
    else if (busstop == 8 && *(busstop8.count) == 0)
    {
        result = false;
    }
    else if (busstop == 9 && *(busstop9.count) == 0)
    {
        result = false;
    }
    else if (busstop == 10 && *(busstop10.count) == 0)
    {
        result = false;
    }

    return result;
}

void skier(Arg args, int id, int idz)
{
    output(L_STARTED, id, NONE);
    usleep_random_in_range(0, args.TL);
    // skier_busstop(id, idz);
    exit(0);
}

bool check_any_skier(int busstop)
{
    bool result = true;

    if (busstop == ANY)
    {
        if (*(busstop1.count) == 0 && *(busstop2.count) == 0 && *(busstop3.count) == 0 && *(busstop4.count) == 0 && *(busstop5.count) == 0 && *(busstop6.count) == 0 && *(busstop7.count) == 0 && *(busstop8.count) == 0 && *(busstop9.count) == 0 && *(busstop10.count) == 0)
        {
            result = false;
        }
    }
    else if (busstop == 1 && *(busstop1.count) == 0)
    {
        result = false;
    }
    else if (busstop == 2 && *(busstop2.count) == 0)
    {
        result = false;
    }
    else if (busstop == 3 && *(busstop3.count) == 0)
    {
        result = false;
    }
    else if (busstop == 3 && *(busstop4.count) == 0)
    {
        result = false;
    }
    else if (busstop == 3 && *(busstop5.count) == 0)
    {
        result = false;
    }
    else if (busstop == 3 && *(busstop6.count) == 0)
    {
        result = false;
    }
    else if (busstop == 3 && *(busstop7.count) == 0)
    {
        result = false;
    }
    else if (busstop == 3 && *(busstop8.count) == 0)
    {
        result = false;
    }
    else if (busstop == 3 && *(busstop9.count) == 0)
    {
        result = false;
    }
    else if (busstop == 3 && *(busstop10.count) == 0)
    {
        result = false;
    }
    return result;
}

void skier_going_to_ski(int id)
{
    output(L_GOING_TO_SKI, id, NONE);
    exit(0);
}

void init_semaphores(void)
{

    init_sem(&mutex_bus_stop, 1);
    init_sem(&mutex_output, 1);
    init_sem(&mutex_queue_update, 1);
    init_sem(&busstop1.queue, 0);
    init_sem(&busstop2.queue, 0);
    init_sem(&busstop3.queue, 0);
    init_sem(&busstop4.queue, 0);
    init_sem(&busstop5.queue, 0);
    init_sem(&busstop6.queue, 0);
    init_sem(&busstop7.queue, 0);
    init_sem(&busstop8.queue, 0);
    init_sem(&busstop9.queue, 0);
    init_sem(&busstop10.queue, 0);

    action_id = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (action_id == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }
    busstop1.count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (busstop1.count == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }
    busstop2.count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (busstop2.count == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }
    busstop3.count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (busstop3.count == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }
    busstop4.count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (busstop4.count == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }
    busstop5.count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (busstop5.count == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }
    busstop6.count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (busstop6.count == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }
    busstop7.count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (busstop7.count == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }
    busstop8.count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (busstop8.count == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }
    busstop9.count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (busstop9.count == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }
    busstop10.count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (busstop10.count == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }

    *(busstop1.count) = 0;
    *(busstop2.count) = 0;
    *(busstop3.count) = 0;
    *(busstop4.count) = 0;
    *(busstop5.count) = 0;
    *(busstop6.count) = 0;
    *(busstop7.count) = 0;
    *(busstop8.count) = 0;
    *(busstop9.count) = 0;
    *(busstop10.count) = 0;
    *action_id = 0;
}

void wait_sem(sem_t **sem)
{
    if (sem_wait(*sem) == -1)
        exit_error("Wait sem failed.", 1);
}

void post_sem(sem_t **sem)
{
    if (sem_post(*sem) == -1)
        exit_error("Post sem failed.", 1);
}

void init_sem(sem_t **sem, int value)
{
    *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (*sem == MAP_FAILED)
        exit_error("Mmap failed.", 1);

    if (sem_init(*sem, 1, value) == -1)
        exit_error("Init sem failed.", 1);
}

void destroy_sem(sem_t **sem)
{
    if (sem_destroy(*sem) == -1)
        exit_error("Destroy sem failed.", 1);

    if (munmap((*sem), sizeof(sem_t)) == -1)
        exit_error("Munmap sem failed.", 1);
}

void cleanup_semaphores(void)
{
    destroy_sem(&mutex_output);
    destroy_sem(&mutex_queue_update);
    destroy_sem(&mutex_bus_stop);

    if (munmap(action_id, sizeof(int)) == -1)
    {
        exit_error("Munmap sem failed.\n", 1);
    }
}

int random_int(int lower, int upper)
{
    seed += getpid() * 3696;

    srand(seed);
    int randn = (int)rand() % (upper - lower + 1) + lower;

    // generate random integer in the range <lower, upper>
    return randn;
}

void usleep_random_in_range(int lower, int upper)
{
    int rand_n = random_int(lower, upper);
    usleep(rand_n);
}

void clear_and_open_output_file(void)
{
    // Clear the output file.

    file = fopen("proj2.out", "w");
    if (file == NULL)
    {
        exit_error("Fopen failed.", 1);
    }

    if (fclose(file) == EOF)
    {
        exit_error("fclose close.", 1);
    }

    file = fopen("proj2.out", "a");
    if (file == NULL)
    {
        exit_error("Fopen 2 failed.", 1);
    }
}

void output(int action_type, int id, int busstop)
{
    wait_sem(&mutex_output);
    *action_id += 1;
    switch (action_type)
    {
    case BUS_STARTED:
        fprintf(file, "%d: BUS: started\n", *action_id);
        break;
    case BUS_ARRIVED_TO:
        fprintf(file, "%d: BUS: arrived to %d\n", *action_id, busstop);
        break;
    case BUS_LEAVING:
        fprintf(file, "%d: BUS: leaving %d\n", *action_id, busstop);
        break;
    case BUS_ARRIVED_TO_FINAL:
        fprintf(file, "%d: BUS: arrived to final\n", *action_id);
        break;
        // postman
    case BUS_LEAVING_FINAL:
        fprintf(file, "%d: BUS: leaving final\n", *action_id);
        break;
    case BUS_FINISH:
        fprintf(file, "%d: BUS: finish\n", *action_id);
        break;
    case L_STARTED:
        fprintf(file, "%d: L %d: started\n", *action_id, id);
        break;
    case L_ARRIVED_TO:
        fprintf(file, "%d: U %d: arrived to %d\n", *action_id, id, busstop);
        break;
    case L_BOARDING:
        fprintf(file, "%d: U %d: boarding\n", *action_id, id);
        break;
    case L_GOING_TO_SKI:
        fprintf(file, "%d: U %d: going to ski\n", *action_id, id);
        break;
    default:
        exit_error("Internal error: Unknown output action_type.", 1);
    }

    post_sem(&mutex_output);
}

int main(int argc, char **argv)
{
    Arg args = ParseArgs(argc, argv);
    init_semaphores();
    clear_and_open_output_file();
    setbuf(file, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    pid_t skibus_id = fork();
    if (skibus_id < 0)
    {
        exit_error("skibus fork error\n", 1);
    }
    if (skibus_id == 0)
    {
        output(BUS_STARTED, NONE, NONE);
    }

    for (int i = 1; i <= args.L; i++)
    {
        pid_t skier_id = fork();
        if (skier_id < 0)
        {
            exit_error("Fork skier failed.\n", 1);
        }
        if (skier_id == 0)
        {
            skier(args, i, random_int(1, args.Z));
        }
    }

    while (wait(NULL) > 0)
        ;
    if (fclose(file) == EOF)
        exit_error("Final fclose close.\n", 1);

    cleanup_semaphores();
    return 0;
}
