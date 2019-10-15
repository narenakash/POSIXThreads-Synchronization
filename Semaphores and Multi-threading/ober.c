#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

const int CabEmpty = 0;
const int CabPremier = 1;
const int CabPoolOne = 2;
const int CabPoolTwo = 3;

struct arg_struct {
    int riderNumber;
    int cabNumber;
    int maxWait;
    int rideTime;
};

void *bookRide(void *args);
void riding(struct arg_struct *args);
void scheduler(struct arg_struct *args);
void *serverInit(void *args);
void bookingPaymentServer(struct arg_struct *args);
void paying(struct arg_struct *args, int sNumber);

sem_t Cabs[100];
sem_t Servers[100];
sem_t mutex;

int cabStatus[100] = {0};
int numCars = 1, numRiders = 1, numServers = 1, count = 0;

int main(void) {  
    printf(ANSI_COLOR_RESET "Please enter the number of cars, number of riders, and the number of payment servers.\n\n");
    scanf("%d %d %d", &numCars, &numRiders, &numServers);
    struct arg_struct args[100];
    
    pthread_t id[100];
    pthread_t sid[100];
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    for(int i = 0; i < numCars; i++) {
        sem_init(&Cabs[i],0,1);
    }

    for(int i = 0; i < numServers; i++) {
        sem_init(&Servers[i],0,1);
    }

    sem_init(&mutex, 0, 1);
    
    for(int i = 0; i < numServers; i++) {
        pthread_create(&sid[i], &attr, serverInit, (void *) &i);
    }

    sleep(2);
    printf("\n");

    while(numRiders--) {
        int rideType;
        printf(ANSI_COLOR_RESET "   Rider Number: %d Arrived.\n", ++count);
        args[count-1].riderNumber = count;
        scanf("%d %d %d", &rideType, &args[count-1].maxWait, &args[count-1].rideTime); //rideType 0:Pool 1:Premium
        fflush(stdin);
        char rides[2][10] = {"Pool", "Premium"};
        printf("   Rider Number: %d Looking for %s Ride\n", count, rides[rideType]);

        for(int i = 0; i < numCars; i++) {
            if(rideType) {
                if(cabStatus[i] == 0) {
                    args[count-1].cabNumber = i + 1;
                    cabStatus[i] = 1;
                    break;
                }
            } else {
                if(cabStatus[i] == 0) {
                    args[count-1].cabNumber = i + 1;
                    cabStatus[i] = 2;
                    break;
                } else if(cabStatus[i] == 2) {
                    args[count-1].cabNumber = i + 1;
                    cabStatus[i] = 3;
                    break;
                }
            }
            if(i == numCars -1) {
                i = -1;
            }
        }

        pthread_create(&id[count-1], &attr, bookRide, (void *) &args[count-1]);
    }

    for(int i = 0; i < count; i++) {
        pthread_join(id[i], NULL);
    }

    for(int i = 0; i < numServers; i++) {
        pthread_join(sid[i], NULL);
    }

    for(int i = 0; i < numCars; i++) {
        sem_destroy(&Cabs[i]);
    }

    for(int i = 0; i < numServers; i++) {
        sem_destroy(&Servers[i]);
    }

    printf(ANSI_COLOR_RESET "\nExiting the system.\n");
    return 0;
}

void *serverInit(void *args) {
    int *arg = (int *) args;
    printf(ANSI_COLOR_RESET "Payment Server Number: %d initiated.\n", *arg);
}

void *bookRide(void *args) {
    int s;
    struct arg_struct *arg = (struct arg_struct *) args;
    
    if(cabStatus[arg->cabNumber - 1] != 3) {
        if((s = sem_trywait(&Cabs[arg->cabNumber - 1])) == -1) {
            scheduler(arg);
        }
    }
    printf(ANSI_COLOR_BLUE "   Rider Number: %d Booking Cab %d" ANSI_COLOR_RESET "\n", arg->riderNumber, arg->cabNumber);
    riding(args);
}

void paying(struct arg_struct *args, int sNumber) {
    printf(ANSI_COLOR_CYAN "\tRider Number: %d is alloted Server Number: %d for payment" ANSI_COLOR_RESET "\n", args->riderNumber, sNumber);
    sleep(2);
    printf(ANSI_COLOR_GREEN "\tRider Number: %d successfully completed their payment" ANSI_COLOR_RESET "\n", args->riderNumber);
    sem_post(&Servers[sNumber-1]);
}

void bookingPaymentServer(struct arg_struct *args) {

    for(int i = 0; i < numServers; i++) {
        
        int s;
        if((s = sem_wait(&Servers[i])) != -1) {
            paying(args, i+1);
            return;
        }

        if(i == numServers -1) {
            i = -1;
        }
    }
}

void riding(struct arg_struct *args) {
    sleep(args->rideTime);
    printf(ANSI_COLOR_MAGENTA "   Rider Number: %d finished the ride in Cab Number %d" ANSI_COLOR_RESET "\n", args->riderNumber, args->cabNumber);

    int carNumber = args->cabNumber - 1;
    if(cabStatus[carNumber] == 1 || cabStatus[carNumber] == 2) {
        cabStatus[carNumber] = 0;
    } else if(cabStatus[carNumber] == 3) {
        cabStatus[carNumber] = 2;
    }

    sem_post(&Cabs[args->cabNumber - 1]);
    bookingPaymentServer(args);
    pthread_exit(0);
}

void scheduler(struct arg_struct *args) {
    struct timespec ts;
    int s;

    if(clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
    }

    ts.tv_sec += args->maxWait;

    printf(ANSI_COLOR_YELLOW "\tRider Number: %d Waiting for Cab %d" ANSI_COLOR_RESET "\n", args->riderNumber, args->cabNumber);
    if((s = sem_timedwait(&Cabs[args->cabNumber - 1], &ts)) == -1) {
        if(errno == ETIMEDOUT) {
            printf(ANSI_COLOR_RED "\tRider Number: %d left the system. TIMED OUT" ANSI_COLOR_RESET "\n", args->riderNumber);
            int carNumber = args->cabNumber;
            if(cabStatus[carNumber] == 1 || cabStatus[carNumber] == 2) {
                cabStatus[carNumber] = 0;
            } else if(cabStatus[carNumber] == 3) {
                cabStatus[carNumber] = 2;
            }
            pthread_exit(0);
        }
    }
    printf(ANSI_COLOR_BLUE "   Rider Number: %d Booking Cab %d" ANSI_COLOR_RESET "\n", args->riderNumber, args->cabNumber);
    riding(args);
}



/****************************************************
 Naren Akash, R J: 2018111020
 International Institute of Information Technology
 Hyderabad, INDIA
****************************************************/





