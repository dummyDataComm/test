#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

// Globals
typedef struct 
{
    char *str;
    int num;
    double dbl;
} ThreadInfo;

pthread_mutex_t producerLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t consumerLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t producerCondition;
pthread_cond_t consumerCondition;

int loop_count;

void* producerFunc(void *info_ptr) {
    pthread_mutex_lock(&producerLock);
    for (int i = 0; i < loop_count; i++) {
        char buffer[100];
        int i;
        double d;
        ThreadInfo *info = (ThreadInfo *)info_ptr;
        printf("Enter values to write <string> <integer> <data>\n");
        scanf("%s%d%lf", buffer, &i, &d);
        info->str = buffer;
        info->num = i;
        info->dbl = d;
        pthread_cond_signal(&consumerCondition);
        pthread_cond_wait(&producerCondition, &consumerLock);
    }
    pthread_mutex_unlock(&producerLock);
    return NULL;
}

void* consumerFunc(void *info_ptr) {
    pthread_mutex_lock(&consumerLock);
    for (int i = 0; i < loop_count; i++) {
        ThreadInfo *info = (ThreadInfo *)info_ptr;
        pthread_cond_wait(&consumerCondition, &producerLock);
        printf("consumer reading:\n");
        printf("%s, %i, %lf\n", info->str, info->num, info->dbl);
        pthread_cond_signal(&producerCondition);
    }
    return NULL;
    pthread_mutex_unlock(&consumerLock);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s <int - number of times to loop>\n", argv[0]);
        exit(0);
    }
    if ((loop_count =atoi(argv[1])) == 0) {
        printf("usage: %s <int - number of times to loop>\n", argv[0]);
        exit(0);
    }
    
    ThreadInfo *Info_Ptr;
    if ((Info_Ptr = malloc (sizeof (ThreadInfo))) == NULL)
    {
        perror ("malloc");
        exit (1);
    }
    Info_Ptr->str = "zero";
    pthread_t producer;
    pthread_t consumer;
    pthread_create(&producer, NULL, producerFunc, (void*)Info_Ptr);
    pthread_create(&consumer, NULL, consumerFunc, (void*)Info_Ptr);
    pthread_join(consumer, NULL);
    exit(0);
}