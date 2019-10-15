#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "restaurant.h"

void * chef_thread(void * args) {

    RobotChef * chef = (RobotChef*) args;

    while(chef->restaurant->done_pupils < chef->restaurant->number_of_pupils) {
        chef->seconds = rand() % 4 + 2;
        chef->vessels = rand() % 10 + 1;
        chef->feedingCapacity = rand() % 26 + 25;
        printf(ANSI_COLOR_RESET "   Robot Chef Number: %d preparing %d Biryani Vessels each of which can feed %d students\n", chef->idx + 1, chef->vessels, chef->feedingCapacity);
        sleep(chef->seconds);
        printf(ANSI_COLOR_RESET "   Robot Chef Number: %d prepared %d Biryani Vessels\n", chef->idx + 1, chef->vessels);
        chef->flag = chef->vessels;
        while(chef->flag) {
            if(chef->restaurant->done_pupils >= chef->restaurant->number_of_pupils) {
                break;
            }
        }
        printf(ANSI_COLOR_RESET "   Robot Chef Number: %d served %d Biryani Vessels and resumes cooking!\n", chef->idx + 1, chef->vessels);
    }

    pthread_exit(0);
}

RobotChef* chef_init(RobotChef * chef, int idx, Restaurant * restaurant)
{
  chef = (RobotChef*)malloc(sizeof(RobotChef));
  chef->idx = idx;
  chef->flag = 0;
  chef->restaurant = restaurant;
  return chef;
}