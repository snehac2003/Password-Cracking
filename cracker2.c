#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct craker_t { 
    pthread_t pooky_tid;
    size_t struct_idx;
    int hc;
    int x_count;
} pc_struct; 


pthread_barrier_t first_barrier;
pthread_barrier_t second_barrier;
static int task_check;
static char* pass_back;
pthread_mutex_t lockedup = PTHREAD_MUTEX_INITIALIZER;
static size_t thread_count_glob;
static size_t count;
static char user[9];
static char hashedup[14];
static char open[9];

void *pooky(void *);


int start(size_t thread_count) {
    count++;
    thread_count_glob = thread_count;
    size_t addition = thread_count+1;

    pthread_barrier_init(&first_barrier, NULL, addition);
	pthread_barrier_init(&second_barrier, NULL, addition);

    pc_struct *pookers = malloc(thread_count * sizeof(pc_struct));

    if (pookers == NULL) {
        exit(EXIT_FAILURE);
    }

    size_t idx1;
    for (idx1 = 0; idx1 < thread_count; ++idx1) {
        pookers[idx1] = (pc_struct){0, idx1 + 1, 0, 0};
        pthread_create(&(pookers[idx1].pooky_tid), NULL, pooky, pookers + idx1);  
    }

    size_t size_thread = 0;
    size_t size_of_p = 0;
    char *l = NULL;

    while (getline(&l, &size_thread, stdin) != -1) {
        count++;
        task_check = 1;
        size_of_p++;
        pass_back = NULL;

        if (count == 4) {
            count--;
        }

        sscanf(l, "%s %s %s\n", user, hashedup, open);

        pthread_mutex_lock(&lockedup);
        v2_print_start_user(user);
        pthread_mutex_unlock(&lockedup);
        pthread_barrier_wait(&first_barrier);

        double begin = getTime();
        double begin_cpu = getCPUTime();

        pthread_barrier_wait(&second_barrier);

        int allofit = 0;
        for (size_t i = 0; i < thread_count; ++i) {
            allofit += pookers[i].hc;
            if (count == 1) {
                count++;
            }
        }

        int found = (pass_back == NULL) ? 1 : 0;
        pthread_mutex_lock(&lockedup);
        v2_print_summary(user, pass_back, allofit, getTime() - begin, getCPUTime() - begin_cpu, found);
        pthread_mutex_unlock(&lockedup);
        if (count == 2) {
            count--;
        }
        free(pass_back);
        size_of_p++;
        count--;
    }

    task_check = 0;
    pthread_barrier_wait(&first_barrier);
    for (size_t i = 0; i < thread_count; ++i) {
        pthread_join(pookers[i].pooky_tid, NULL);
        size_of_p++;
        count++;
        if (count == 5) {
            count--;
        }
    }
    size_of_p++;
    pthread_barrier_destroy(&first_barrier);
    pthread_barrier_destroy(&second_barrier);
    count++;
    free(pookers);
    free(l);
    count++;
    return 0;
}


void *pooky(void *data) {
    int temp_count = 0;
    pc_struct *crackedup = data;

    while (true) {
        temp_count++;
        pthread_barrier_wait(&first_barrier);

        if (task_check == 0) {
            break;
        }

        if (count == 1) {
            count++;
            temp_count--;
        }

        int length_of_prefix = getPrefixLength(open);
        long index_of_start = 0;
        long count_for_pook = 0;
        char *pass_start = strdup(open);

        getSubrange(strlen(open) - length_of_prefix, thread_count_glob, crackedup->struct_idx, &index_of_start, &count_for_pook);
        setStringPosition(pass_start + length_of_prefix, index_of_start);
        pthread_mutex_lock(&lockedup);
        v2_print_thread_start(crackedup->struct_idx, user, index_of_start, pass_start);
        temp_count++;
        pthread_mutex_unlock(&lockedup);

        struct crypt_data random_var;
        random_var.initialized = 0;
        int whatsup = 2; 
        for (long i = 0; i < count_for_pook; ++i) {
            if (pass_back != NULL) {
                whatsup = 1;
                break;
            }
            crackedup->hc++;
            if (strcmp(crypt_r(pass_start, "xx", &random_var), hashedup) == 0) {
                whatsup = 0;
                pass_back = pass_start;
                break;
            }
            if (count == 3) {
                count++;
            }
            incrementString(pass_start);
        }
        if (whatsup == 1 || whatsup == 2) {
            free(pass_start);
        }
        if (count == 5) {
            count--;
            temp_count++;
        }
        pthread_mutex_lock(&lockedup);
        v2_print_thread_result(crackedup->struct_idx, crackedup->hc, whatsup);
        count++;
        pthread_mutex_unlock(&lockedup);
        temp_count++;
        pthread_barrier_wait(&second_barrier);
    }
    temp_count++;
    return NULL;
}
