#include "/comp/111/assignments/aardvarks/anthills.h" 
#include <semaphore.h>
#define TRUE 1
#define FALSE 0

int initialized=FALSE; // semaphores and mutexes are not initialized 

// define your mutexes and semaphores here 
// they must be global variables. 
pthread_mutex_t mut; //a mutex lock for each of the 3 aardvarks in each of the 3 anthills
sem_t anthill[ANTHILLS]; //using semaphore to allow 3 aardvarks to access an anthill 
int ant_counter[ANTHILLS]; //to keep track of ants in each anthill
double timestamp[ANTHILLS][3]; //to keep track of aardvarks in each anthill

// first thread initializes mutexes 
void *aardvark(void *input) { 
    char aname = *(char *)input; 
    // first caller needs to initialize the mutexes!
    pthread_mutex_lock(&init_lock); 
    if (!initialized++) { // this succeeds only for one thread
        // initialize your mutexes and semaphores here
        for(int i=0;i<ANTHILLS;i++){
            for(int j=0;j<3;j++){
                timestamp[i][j]=-2; //setting the initial time_stamp value of each aardvark of a hill
            }
            sem_init(&anthill[i], 0, AARDVARKS_PER_HILL); //initializing the semaphore for the anthill
            ant_counter[i] = 100; //setting the number of ants in each hill to be 100 as mentioned in question
        }
        pthread_mutex_init(&mut, NULL);
    } 
    pthread_mutex_unlock(&init_lock); 

    double timer = 0;
    double diff = 0;
    // now be an aardvark
    while (chow_time()) { 
        for(int i=0;i<ANTHILLS;i++){
            //goes into a hill
            for(int j=0;j<3;j++){
                //to know when 'that' one second is up
                timer = elapsed();
                //locking the hill with a semaphore which ensures only 3 aardvarks can slurp at a time
                sem_wait(&anthill[i]);
                diff = timer - timestamp[i][j];
                if(diff >= 1.1 && ant_counter[i] -- >0){
                    timestamp[i][j] = timer; 
                    sem_post(&anthill[i]); 
                    // try to slurp
                    if(slurp(aname,i) == -1){ //if slurp is not successful adding back the ant to the hill
                        pthread_mutex_lock(&mut);
                        ant_counter[i]--;
                        pthread_mutex_unlock(&mut);
                    } 
                }else{ //if 'that' one second is not up yet or there aren't enough ants to slurp
                    sem_post(&anthill[i]);
                }
            }
        }
    } 
    return NULL; 
} 