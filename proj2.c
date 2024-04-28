/**
 * @file proj2.c
 * @brief IOS – projekt 2 (synchronizace)
 *
 * Zadání je inspirováno knihou Allen B. Downey: The Little Book of Semaphores (The Senate Bus
 * problem)
 *
 * @author Timotej Mikula, xmikult00
 * @date 28.4.2024
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
#include <signal.h>
#include <semaphore.h>
#include "proj2.h"

sem_t *mutex_output; // mutex for output
sem_t *boarding;     // mutex for boarding
sem_t *leaving;      // mutex for unboarding/skier leaves bus
FILE *file;
int *action_id;
int seed;
int *count_in_bus;
int *alldepartedskiers;

BusStop busstop[11]; // 0 - final 1-10 - stops

pid_t parent_pid;

/**
 * @brief Kills all child processes.
 *
 * This function is called when the parent process receives a signal.
 * It sends the same signal to all child processes to terminate them.
 *
 * @param sig The signal received by the parent process.
 */
void kill_children(int sig)
{
    (void)sig;
    if (getpid() == parent_pid)
    {
        kill(0, SIGKILL);
    }
}

/**
 * @brief Parses the command line arguments.
 *
 * This function takes the command line arguments and parses them into an Arg structure.
 *
 * @param argc The number of command line arguments.
 * @param argv The array of command line arguments.
 * @return Returns an Arg structure containing the parsed arguments.
 */
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

/**
 * @brief Handles program exit due to an error.
 *
 * This function is called when the program encounters an error and needs to exit.
 * It performs necessary cleanup operations before terminating the program.
 *
 * @param error_message The error message to be displayed before exiting.
 */
void exit_error(char *msg, int errcode)
{
    fprintf(stderr, "ERROR: %s Exit code: %d\n", msg, errcode);
    exit(errcode);
}

/**
 * @brief Checks if a string represents a valid integer.
 *
 * This function takes a string as input and checks if it represents a valid integer.
 * It returns true if the string can be converted to an integer, and false otherwise.
 *
 * @param str The string to be checked.
 * @return Returns true if the string represents a valid integer, and false otherwise.
 */
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

/**
 * @brief Returns the minimum of two integers.
 *
 * This function takes two integers as input and returns the smaller one.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return Returns the smaller of the two input integers.
 */
int min(int a, int b)
{
    return (a < b) ? a : b;
}

/**
 * @brief Generates a random integer within a given range.
 *
 * This function generates a random integer that falls within the range specified by the two input parameters.
 *
 * @param min The lower bound of the range.
 * @param max The upper bound of the range.
 * @return Returns a random integer within the specified range.
 */
int random_int(int lower, int upper)
{
    seed += getpid() * 3696;

    srand(seed);
    int randn = (int)rand() % (upper - lower + 1) + lower;

    // generate random integer in the range <lower, upper>
    return randn;
}

/**
 * @brief Suspends execution for a random interval.
 *
 * This function suspends the execution of the calling thread for an interval that is randomly chosen within the specified range.
 *
 * @param min_microseconds The lower bound of the range, in microseconds.
 * @param max_microseconds The upper bound of the range, in microseconds.
 */
void usleep_random_in_range(int lower, int upper)
{
    int rand_n = random_int(lower, upper);
    usleep(rand_n);
}

/**
 * @brief Clears and opens the output file.
 *
 * This function clears the contents of the output file if it exists, and then opens it for writing.
 * If the file does not exist, it creates a new file.
 */
void clear_and_open_output_file(void)
{
    // Clear the output file.

    file = fopen("proj2.out", "w");
    if (file == NULL)
    {
        exit_error("Fopen failed.\n", 1);
    }

    if (fclose(file) == EOF)
    {
        exit_error("fclose close.\n", 1);
    }

    file = fopen("proj2.out", "a");
    if (file == NULL)
    {
        exit_error("Fopen 2 failed.\n", 1);
    }
}

/**
 * @brief Initializes the semaphores used in the program.
 *
 * This function initializes the semaphores used for synchronization in the program.
 * The number and types of semaphores initialized depend on the arguments passed to the function.
 *
 * @param args The arguments that determine which semaphores to initialize.
 */
void init_semaphores(Arg args)
{
    init_sem(&mutex_output, 1);
    init_sem(&boarding, 1);
    init_sem(&leaving, 0);

    for (int i = 0; i <= args.Z; i++)
    {
        init_sem(&busstop[i].semaphore, 0);
        busstop[i].count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
        if (busstop[i].count == MAP_FAILED)
        {
            exit_error("Mmap failed.\n", 1);
        }
        *(busstop[i].count) = 0;
    }

    action_id = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (action_id == MAP_FAILED)
    {
        exit_error("Mmap failed.\n", 1);
    }

    count_in_bus = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (count_in_bus == MAP_FAILED)
    {
        exit_error("Mmap failed.\n", 1);
    }

    alldepartedskiers = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (alldepartedskiers == MAP_FAILED)
    {
        exit_error("Mmap failed.\n", 1);
    }

    *action_id = 0;
    *count_in_bus = 0;
    *alldepartedskiers = 0;
}

/**
 * @brief Waits for a semaphore to become available.
 *
 * This function blocks the calling process until the specified semaphore is available.
 *
 * @param sem A pointer to the semaphore to wait for.
 */
void wait_sem(sem_t **sem)
{
    if (sem_wait(*sem) == -1)
        exit_error("Wait sem failed.\n", 1);
}

/**
 * @brief Increments the semaphore count.
 *
 * This function increments the count of the specified semaphore and wakes up a process waiting on the semaphore, if any.
 *
 * @param sem A pointer to the semaphore to be incremented.
 */
void post_sem(sem_t **sem)
{
    if (sem_post(*sem) == -1)
        exit_error("Post sem failed.\n", 1);
}

/**
 * @brief Initializes a semaphore with a given value.
 *
 * This function initializes the specified semaphore with the given initial value.
 *
 * @param sem A pointer to the semaphore to be initialized.
 * @param value The initial value for the semaphore.
 */
void init_sem(sem_t **sem, int value)
{
    *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (*sem == MAP_FAILED)
    {
        exit_error("Mmap failed.\n", 1);
    }

    if (sem_init(*sem, 1, value) == -1)
    {
        exit_error("Init sem failed.\n", 1);
    }
}

/**
 * @brief Destroys a semaphore.
 *
 * This function destroys the specified semaphore, freeing any resources it might hold.
 *
 * @param sem A pointer to the semaphore to be destroyed.
 */
void destroy_sem(sem_t **sem)
{
    if (sem_destroy(*sem) == -1)
        exit_error("Destroy sem failed.\n", 1);

    if (munmap((*sem), sizeof(sem_t)) == -1)
        exit_error("Munmap sem failed.\n", 1);
}

/**
 * @brief Cleans up the semaphores used in the program.
 *
 * This function cleans up the semaphores used for synchronization in the program.
 * The number and types of semaphores cleaned up depend on the arguments passed to the function.
 *
 * @param args The arguments that determine which semaphores to clean up.
 */
void cleanup_semaphores(Arg args)
{
    destroy_sem(&mutex_output);
    destroy_sem(&boarding);
    destroy_sem(&leaving);

    for (int i = 0; i <= args.Z; i++)
    {
        destroy_sem(&busstop[i].semaphore);
        if (munmap(busstop[i].count, sizeof(int)) == -1)
        {
            exit_error("Munmap failed.\n", 1);
        }
    }

    if (munmap(action_id, sizeof(int)) == -1)
    {
        exit_error("Munmap failed.\n", 1);
    }
    if (munmap(count_in_bus, sizeof(int)) == -1)
    {
        exit_error("Munmap failed.\n", 1);
    }
    if (munmap(alldepartedskiers, sizeof(int)) == -1)
    {
        exit_error("Munmap failed.\n", 1);
    }
}

/**
 * @brief Outputs a message based on the action type, id, and bus stop.
 *
 * This function generates and outputs a message based on the action type, id, and bus stop passed as parameters.
 *
 * @param action_type The type of action to be included in the output message.
 * @param id The id to be included in the output message.
 * @param busstop The bus stop to be included in the output message.
 */
void output(int action_type, int id, int busstop)
{
    wait_sem(&mutex_output);
    *action_id += 1;
    switch (action_type)
    {
        // bus
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
    case BUS_LEAVING_FINAL:
        fprintf(file, "%d: BUS: leaving final\n", *action_id);
        break;
    case BUS_FINISH:
        fprintf(file, "%d: BUS: finish\n", *action_id);
        break;
        // skier
    case L_STARTED:
        fprintf(file, "%d: L %d: started\n", *action_id, id);
        break;
    case L_ARRIVED_TO:
        fprintf(file, "%d: L %d: arrived to %d\n", *action_id, id, busstop);
        break;
    case L_BOARDING:
        fprintf(file, "%d: L %d: boarding\n", *action_id, id);
        break;
    case L_GOING_TO_SKI:
        fprintf(file, "%d: L %d: going to ski\n", *action_id, id);
        break;
    default:
        exit_error("Internal error: Unknown output action_type.\n", 1);
    }

    post_sem(&mutex_output);
}

/**
 * @brief Handles the ski bus operations.
 *
 * This function is responsible for managing the ski bus operations based on the arguments passed to it.
 *
 * @param args The arguments that determine the behavior of the ski bus.
 */
void skibus(Arg args)
{
    output(BUS_STARTED, NONE, NONE);
    for (int i = 1; i <= args.Z; i++)
    {
        usleep_random_in_range(0, args.TB);
        wait_sem(&boarding);
        output(BUS_ARRIVED_TO, NONE, i);
        if ((*busstop[i].count) > 0)
        {
            post_sem(&busstop[i].semaphore);
            wait_sem(&leaving);
        }
        output(BUS_LEAVING, NONE, i);
        post_sem(&boarding);
        if (i == args.Z)
        {
            i = 0;
            usleep_random_in_range(0, args.TB);
            wait_sem(&boarding);
            output(BUS_ARRIVED_TO_FINAL, NONE, NONE);
            if ((*count_in_bus) > 0)
            {
                post_sem(&busstop[i].semaphore);
                wait_sem(&leaving);
            }
            output(BUS_LEAVING_FINAL, NONE, NONE);
            post_sem(&boarding);
            if (args.L == (*alldepartedskiers))
            {
                output(BUS_FINISH, NONE, NONE);
                exit(0);
            }
        }
    }
}

/**
 * @brief Handles the skier operations.
 *
 * This function is responsible for managing the skier operations based on the arguments passed to it.
 *
 * @param args The arguments that determine the behavior of the skier.
 * @param id The id of the skier.
 * @param idz The id of the bus stop the skier is in.
 */
void skier(Arg args, int id, int idz)
{
    output(L_STARTED, id, NONE);
    usleep_random_in_range(0, args.TL);
    output(L_ARRIVED_TO, id, idz);
    (*busstop[idz].count)++;
    wait_sem(&busstop[idz].semaphore);
    output(L_BOARDING, id, NONE);
    (*count_in_bus)++;
    (*busstop[idz].count)--;
    if ((*busstop[idz].count == 0) || (*count_in_bus == args.K))
    {
        post_sem(&leaving);
    }
    else
    {
        post_sem(&busstop[idz].semaphore);
    }
    wait_sem(&busstop[0].semaphore);
    output(L_GOING_TO_SKI, id, NONE);
    (*alldepartedskiers)++;
    (*count_in_bus)--;
    if (*count_in_bus == 0)
    {
        post_sem(&leaving);
    }
    else
    {
        post_sem(&busstop[0].semaphore);
    }
    exit(0);
}

/**
 * @brief The main function of the program.
 *
 * This function is the entry point of the program. It takes command line arguments, 
 * performs initial setup, and starts the main logic of the program.
 *
 * @param argc The number of command line arguments.
 * @param argv The array of command line arguments.
 * @return Returns 0 if the program finishes successfully, and a non-zero error code otherwise.
 */
int main(int argc, char **argv)
{
    Arg args = ParseArgs(argc, argv);
    init_semaphores(args);
    clear_and_open_output_file();
    setbuf(file, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    parent_pid = getpid();
    signal(SIGUSR1, kill_children);

    pid_t skibus_id = fork();
    if (skibus_id < 0)
    {
        kill(parent_pid, SIGUSR1);
        exit_error("Skibus fork error\n", 1);
    }
    if (skibus_id == 0)
    {
        skibus(args);
    }

    for (int i = 1; i <= args.L; i++)
    {
        pid_t skier_id = fork();
        if (skier_id < 0)
        {
            kill(parent_pid, SIGUSR1);
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
    {
        exit_error("Final fclose close.\n", 1);
    }

    cleanup_semaphores(args);
    // exit(0);
    return 0;
}
