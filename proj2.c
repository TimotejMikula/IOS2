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

void skier_waiting_on_bus_stop(int id, int busstop)
{
    if (busstop == 1)
    {
        wait_sem(&(busstop1.queue));
        output(L_ARRIVED_TO, id, busstop);
    }
    else if (busstop == 2)
    {
        wait_sem(&(busstop1.queue));
        output(L_ARRIVED_TO, id, busstop);
    }
    else if (busstop == 3)
    {
        wait_sem(&(busstop1.queue));
        output(L_ARRIVED_TO, id, busstop);
    }
    else if (busstop == 4)
    {
        wait_sem(&(busstop1.queue));
        output(L_ARRIVED_TO, id, busstop);
    }
    else if (busstop == 5)
    {
        wait_sem(&(busstop1.queue));
        output(L_ARRIVED_TO, id, busstop);
    }
    else if (busstop == 6)
    {
        wait_sem(&(busstop1.queue));
        output(L_ARRIVED_TO, id, busstop);
    }
    else if (busstop == 7)
    {
        wait_sem(&(busstop1.queue));
        output(L_ARRIVED_TO, id, busstop);
    }
    else if (busstop == 8)
    {
        wait_sem(&(busstop1.queue));
        output(L_ARRIVED_TO, id, busstop);
    }
    else if (busstop == 9)
    {
        wait_sem(&(busstop1.queue));
        output(L_ARRIVED_TO, id, busstop);
    }
    else if (busstop == 10)
    {
        wait_sem(&(busstop1.queue));
        output(L_ARRIVED_TO, id, busstop);
    }
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
    usleep_random_in_range(0, args.TB);
    output(BUS_ARRIVED_TO, NONE, NONE);
    wait_sem(&mutex_bus_stop);
    output(BUS_LEAVING, NONE, NONE);
}

void skier_busstop(int id, Arg args, int idz)
{
    // int rand_busstop = random_int(1, args.Z);
    wait_sem(&mutex_queue_update);
    if (idz == 1)
        (*(busstop1.count))++;

    if (idz == 2)
        (*(busstop2.count))++;

    if (idz == 3)
        (*(busstop3.count))++;

    if (idz == 4)
        (*(busstop4.count))++;

    if (idz == 5)
        (*(busstop5.count))++;

    if (idz == 6)
        (*(busstop6.count))++;

    if (idz == 7)
        (*(busstop7.count))++;

    if (idz == 8)
        (*(busstop8.count))++;

    if (idz == 9)
        (*(busstop9.count))++;

    if (idz == 10)
        (*(busstop10.count))++;

    post_sem(&mutex_queue_update);
}

void skier(Arg args, int id, int idz)
{
    output(L_STARTED, id, NONE);
    usleep_random_in_range(0, args.TL);
    skier_busstop(id, args, idz);
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

    action_id = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (action_id == MAP_FAILED)
    {
        exit_error("Mmap failed.", 1);
    }

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

void output(int action_type, int id, int service)
{
    wait_sem(&mutex_output);
    *action_id += 1;
    switch (action_type)
    {
    case BUS_STARTED:
        fprintf(file, "%d: BUS: started\n", *action_id);
        break;
    case BUS_ARRIVED_TO:
        fprintf(file, "%d: Z %d: going home\n", *action_id, id);
        break;
    case BUS_LEAVING:
        fprintf(file, "%d: Z %d: entering office for a service %d\n", *action_id, id, service);
        break;
    case BUS_ARRIVED_TO_FINAL:
        fprintf(file, "%d: Z %d: called by office worker\n", *action_id, id);
        break;
        // postman
    case BUS_LEAVING_FINAL:
        fprintf(file, "%d: U %d: started\n", *action_id, id);
        break;
    case BUS_FINISH:
        fprintf(file, "%d: U %d: serving a service of type %d\n", *action_id, id, service);
        break;
    case L_STARTED:
        fprintf(file, "%d: U %d: service finished\n", *action_id, id);
        break;
    case L_ARRIVED_TO:
        fprintf(file, "%d: U %d: taking break\n", *action_id, id);
        break;
    case L_BOARDING:
        fprintf(file, "%d: U %d: break finished\n", *action_id, id);
        break;
    case L_GOING_TO_SKI:
        fprintf(file, "%d: U %d: going home\n", *action_id, id);
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

    pid_t skibus = fork();
    if (skibus < 0)
    {
        exit_error("skibus fork error\n", 1);
    }
    if (skibus == 0)
    {
        output(BUS_STARTED, NONE, NONE);
    }

    for (int i = 1; i < args.L + 1; i++)
    {
        pid_t skier_id = fork();
        int rand_busstop = random_int(1, args.Z);
        if (skier_id < 0)
            exit_error("Fork skier failed.", 1);

        if (skier_id == 0)
        {
            skier(args, i, rand_busstop);
        }
    }

    while (wait(NULL) > 0)
        ;
    if (fclose(file) == EOF)
        exit_error("Final fclose close.", 1);

    cleanup_semaphores();
    return 0;
}
