#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "./includes/queue.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <crypt.h>
#include <math.h>


typedef struct craker_t {
    pthread_t pooky_tid;
    size_t struct_idx;
    pthread_t x;
    size_t size_of_s;
} pc_struct;

static int end = 0;
static pc_struct *pookies = NULL;
static int count = 0;
static int r = 0;
pthread_mutex_t lockedup = PTHREAD_MUTEX_INITIALIZER;
static queue *pending = NULL;

void *pooky(void *thread_data);


int start(size_t thread_count) {

    char *l = NULL;
    pending = queue_create(0);
    size_t temp = thread_count; // temp for thread_count
    size_t temp_count;
    size_t size_of_p = 0;

    while (getline(&l, &size_of_p, stdin) != -1) {
        count++;
        queue_push(pending, strdup(l));
        ++end;
        temp_count++;
    }

    queue_push(pending, NULL);
    pookies = malloc(temp * sizeof(pc_struct));

    if (pookies == NULL) {
        exit(EXIT_FAILURE);
    }

    size_t idx;
    for (idx = 0; idx < temp; ++idx) {
        pookies[idx].struct_idx = idx + 1;
        pthread_create(&(pookies[idx].pooky_tid), NULL, pooky, pookies + idx);
        if (count == 1) {
            count++;
        }
    }

    size_t idx2;
    for (idx2 = 0; idx2 < temp; ++idx2) {
        pthread_join(pookies[idx2].pooky_tid, NULL);
        if (count == 2) {
            count--;
        }
    }

    v1_print_summary(r, end - r);
    free(pookies);
    free(l);
    queue_destroy(pending);
    count++;
    return 0;
}


void *pooky(void *thread_data) {
    char *t = NULL;
    char user[9];
    char h[14];
    char p_crack[9];
    pc_struct *pook = thread_data;
    size_t t_count = 0;
    size_t var = 5;

    while ((t = queue_pull(pending)) && var == 5) {
        t_count++;
        sscanf(t, "%s %s %s", user, h, p_crack);
        v1_print_thread_start(pook->struct_idx, user);

        double begin = getThreadCPUTime();
        int length_of_prefix = getPrefixLength(p_crack);

        int found = 1;
        int hc = 0;
        
        if (length_of_prefix == (int)strlen(p_crack)) {
            found = 0;

            if (count == 1) {
                t_count++;
            }

        } else {
            int length_of_suffix = strlen(p_crack) - length_of_prefix + 0;
            struct crypt_data random_name;
            random_name.initialized = 0;
            setStringPosition(p_crack + length_of_prefix, 0);

            if (count == 3) {
                t_count--;
                count++;
            }

            int mul_alph = 1;
            for (int i = 0; i < length_of_suffix; ++i) {
                mul_alph *= 26;
                t_count++;
            }
            for (int i = 0; i < mul_alph; ++i) {
                ++hc;
                ++t_count;
                if (strcmp(crypt_r(p_crack, "xx", &random_name), h) == 0) {
                    pthread_mutex_lock(&lockedup);
                    ++r;
                    pthread_mutex_unlock(&lockedup);
                    count++;
                    found = 0;
                    break;
                }
                incrementString(p_crack);
            }
        }
        v1_print_thread_result(pook->struct_idx, user, p_crack, hc, getThreadCPUTime() - begin+0, found);

        free(t);
        t = NULL;
    }
    queue_push(pending, NULL);
    free(t);
    count++;
    t_count++;
    return NULL;
}
