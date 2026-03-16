#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define READERS 3
#define WRITERS 2

sem_t mutex, wrt;

int readcount = 0;
int waiting_readers = 0;
int waiting_writers = 0;

/* -------- AI Decision Function -------- */
int ai_decision(int r, int w)
{
    if(w > r)
        return 1;   // give writer priority
    else
        return 0;   // give reader priority
}

/* -------- Reader Function -------- */
void *reader(void *arg)
{
    int id = *(int*)arg;

    waiting_readers++;

    int decision = ai_decision(waiting_readers, waiting_writers);

    if(decision == 1)
    {
        printf("AI delaying Reader %d because writers waiting\n", id);
        sleep(2);
    }

    sem_wait(&mutex);
    readcount++;

    if(readcount == 1)
        sem_wait(&wrt);

    sem_post(&mutex);

    waiting_readers--;

    printf("\nReader %d is reading file\n", id);

    FILE *fp = fopen("shared.txt","r");
    if(fp != NULL)
    {
        char ch;
        while((ch = fgetc(fp)) != EOF)
            putchar(ch);
        fclose(fp);
    }

    printf("\nReader %d finished reading\n", id);

    sem_wait(&mutex);
    readcount--;

    if(readcount == 0)
        sem_post(&wrt);

    sem_post(&mutex);

    pthread_exit(NULL);
}

/* -------- Writer Function -------- */
void *writer(void *arg)
{
    int id = *(int*)arg;

    waiting_writers++;

    int decision = ai_decision(waiting_readers, waiting_writers);

    if(decision == 0)
    {
        printf("AI delaying Writer %d because many readers waiting\n", id);
        sleep(2);
    }

    sem_wait(&wrt);

    waiting_writers--;

    printf("\nWriter %d is writing to file\n", id);

    FILE *fp = fopen("shared.txt","a");
    if(fp != NULL)
    {
        time_t t = time(NULL);
        fprintf(fp,"\nWritten by Writer %d at %s", id, ctime(&t));
        fclose(fp);
    }

    printf("Writer %d finished writing\n", id);

    sem_post(&wrt);

    pthread_exit(NULL);
}

/* -------- Main Function -------- */
int main()
{
    pthread_t r[READERS], w[WRITERS];

    int rid[READERS];
    int wid[WRITERS];

    sem_init(&mutex,0,1);
    sem_init(&wrt,0,1);

    printf("AI Based Reader Writer Simulation\n");

    for(int i=0;i<READERS;i++)
    {
        rid[i] = i+1;
        pthread_create(&r[i],NULL,reader,&rid[i]);
    }

    for(int i=0;i<WRITERS;i++)
    {
        wid[i] = i+1;
        pthread_create(&w[i],NULL,writer,&wid[i]);
    }

    for(int i=0;i<READERS;i++)
        pthread_join(r[i],NULL);

    for(int i=0;i<WRITERS;i++)
        pthread_join(w[i],NULL);

    sem_destroy(&mutex);
    sem_destroy(&wrt);

    printf("\nProgram finished\n");

    return 0;
}
