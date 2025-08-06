#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    struct thread_data *data = (struct thread_data*)thread_param;
    int rc;

    usleep(data->wait_to_obtain_ms * 1000);

    rc = pthread_mutex_lock(data->mutex);
    if(rc!=0){
        ERROR_LOG("Unable to lock mutex");
    }
    else{
        usleep(data->wait_to_release_ms * 1000);
        pthread_mutex_unlock(data->mutex);
        data->thread_complete_success = true;
    }

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    int rc;
    struct thread_data *params = malloc(sizeof(struct thread_data));
    if(!params){
        ERROR_LOG("Unable to malloc memory");
        return false;
    }

    params->wait_to_obtain_ms = wait_to_obtain_ms;
    params->wait_to_release_ms = wait_to_release_ms;
    params->mutex = mutex;
    params->id = thread;
    params->thread_complete_success = false;

    // Set up thread
    rc = pthread_create(params->id, NULL, threadfunc, (void*)params);
    if(rc!=0){
        ERROR_LOG("Unable to create thread");
        free(params);
        return false;
    }

    return true;
}

