
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdbool.h>

#define SHOWER_TIME 2

/*

  So these threads do their best thinking in the shower.

  Threads want to use the shower but only one thread can use the
  shower at once.  Once in the shower a thread showers for at least
  SHOWER_TIME.  Then the thread checks to see if any other threads
  want to use the shower, if yes the thread finishes in the shower and
  exits.

  If no other threads want to use the shower, the thread will stay in
  the shower and begin the process again.  This means that a thread
  will never leave the shower unless another thread arrives.

  Hint: this problem can actually be solved with just mutexes, but a
  cv solution is also good.

  Example output:

thread 1 wants to use the shower
thread 1 is using shower
thread 1 decides to stay in the shower a little longer
thread 3 wants to use the shower
thread 2 wants to use the shower
thread 4 wants to use the shower
thread 1 is finished with shower
thread 3 is using shower
thread 3 is finished with shower
thread 2 is using shower
thread 2 is finished with shower
thread 4 is using shower
thread 4 decides to stay in the shower a little longer
thread 4 decides to stay in the shower a little longer
<would run forever but main quits>

 */

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t shower_occupied_cond = PTHREAD_COND_INITIALIZER;

int num_waiting = 0;
int thread_in_shower = 0;

void *thread(void *arg)
{
    int num = (int) arg; //ignore the sketchy way I'm passing a parameter here
    printf("thread %d wants to use the shower\n", num);
    // I reccommend you leave the above print statement outside of
    // any lock, otherwise a certian kind of bug will be less obvious
    pthread_mutex_lock(&lock);
    if (thread_in_shower) {
        num_waiting++;
        while (thread_in_shower) {
            pthread_cond_wait(&shower_occupied_cond, &lock);
        }
        num_waiting--;
    }
    thread_in_shower = 1;
    printf("thread %d is using shower\n", num);
    pthread_mutex_unlock(&lock);
    while(1) {
        sleep(SHOWER_TIME);
        pthread_mutex_lock(&lock);
        if(num_waiting > 0) {
           printf("thread %d is finished with shower\n", num);
           thread_in_shower = 0;
           pthread_cond_signal(&shower_occupied_cond);
           pthread_mutex_unlock(&lock);
           break;
        }
        pthread_mutex_unlock(&lock);
        printf("thread %d decides to stay in the shower a little longer\n", num);
    
    }
}



int main()
{
    
    pthread_t tid[100];
    int i = 0;

    
    pthread_create(&tid[i++],NULL, thread, (void*) 1);
    sleep(3);
    pthread_create(&tid[i++],NULL, thread, (void*) 2);
    pthread_create(&tid[i++],NULL, thread, (void*) 3);
    sleep(1);
    pthread_create(&tid[i++],NULL, thread, (void*) 4);
    sleep(10);

    //note that we don't wait for the last thread, otherwise this
    //program would never be done
    for(int j = 0; j < i - 1; j++) {
        pthread_join(tid[j],NULL);
    }


    return 0;
}
