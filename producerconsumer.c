#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

typedef int buffer_item;

#define BUFFER_SIZE 5
#define TRUE 1


// The circular buffer
buffer_item buffer[BUFFER_SIZE];

// Global variable for the buffer counter
int counter;

// Semaphore for number of empty slots in the buffer
sem_t empty;

// Semaphore for number of full slots in the buffer
sem_t full;

// Mutual exclusion lock
pthread_mutex_t mutex;


// Inserts an item into the buffer
// Returns 0 if successful, otherwise -1 indicating an error occurred
int insert_item(buffer_item item) {

    if (counter < BUFFER_SIZE) {
        buffer[counter] = item;
        counter++;
        return 0;
    }
    return -1;
}

// Removes an object from the buffer, placing it in `item`
// Returns 0 if successful, otherwise -1 indicating an error occurred
int remove_item(buffer_item *item) {

    if (counter > 0) {
        *item = buffer[--counter];
        return 0;
    }
    return -1;
}

// Producer thread
void* producer(void *param) {

    buffer_item item;

    while (TRUE) {

        // Sleep for a random amount of time between 1 and 4 seconds
        sleep(rand() % 4 + 1);

        // Create the item to be inserted
        item = rand();

        // Decrement the empty semaphore
        sem_wait(&empty);

        // Acquire mutex lock before critical section
        pthread_mutex_lock(&mutex);

        // Critical section, insert the item into buffer
        if (insert_item(item) == 0) {
            printf("Producer inserted item %d.\n", item);
        } else {
            printf("Producer error.\n");
        }

        // Critical section finished, release the mutex lock
        pthread_mutex_unlock(&mutex);

        // Increment the full semaphore, signalling that consumer process may start
        sem_post(&full);
    }
}

// Consumer thread
void* consumer(void *param) {

    buffer_item item;

    while (TRUE) {

        // Sleep for a random amount of time between 1 and 4 seconds
        sleep(rand() % 4 + 1);

        // Decrement the full semaphore
        sem_wait(&full);

        // Acquire mutex lock before critical section
        pthread_mutex_lock(&mutex);

        // Critical section, insert the item into buffer
        if (remove_item(&item) == 0) {
            printf("Consumer removed item %d.\n", item);
        } else {
            printf("Consumer error.\n");
        }

        // Critical section finished, release the mutex lock
        pthread_mutex_unlock(&mutex);

        // Increment the empty semaphore, signalling that producer process may start
        sem_post(&empty);
    }
}

// Initializes the semaphores and mutex lock
void init_locks() {

    // Initialize mutex lock
    pthread_mutex_init(&mutex, NULL);

    // Initialize full semaphore to 0 and empty semaphore to BUFFER_SIZE
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);
}

int main(int argc, char* argv[]) {

    // Check correct number of arguments passed by command line
    if (argc != 4) {
        printf("Error: Incorrect number of arguments.\n");
        return -1;
    }

    // Get parameters
    int sleep_duration = atoi(argv[1]);
    int num_producer_threads = atoi(argv[2]);
    int num_consumer_threads = atoi(argv[3]);

    // Initialize buffer
    counter = 0;

    // Initialize semaphores and mutex lock
    init_locks();

    // Create producer and consumer threads
    int i=0;
    pthread_t thread_id;

    for (i=0; i<num_producer_threads; i++) {
        pthread_create(&thread_id, NULL, producer, NULL);
    }

    for (i=0; i<num_consumer_threads; i++) {
        pthread_create(&thread_id, NULL, consumer, NULL);
    }

    // Main program - how long to run before terminating
    sleep(sleep_duration);

    // Exit program
    printf("Producer & consumer demo complete.\n\n");
    exit(0);
    return 0;
}
