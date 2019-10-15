#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "restaurant.h"

void * sTable_thread(void * args)
{
  ServingTable * sTable = (ServingTable*)args;
  Restaurant * restaurant = sTable->restaurant;

  pthread_mutex_t * mutex_ptr = &(restaurant->mutex);
  pthread_cond_t * cv_1_ptr = &(restaurant->cv_1);
  pthread_cond_t * cv_2_ptr = &(restaurant->cv_2);

  int i, j, max_;

  while(1)
  {
    pthread_mutex_lock(mutex_ptr);
      if(restaurant->done_pupils == restaurant->number_of_pupils)
      {
        pthread_mutex_unlock(mutex_ptr);
        break;
      }
    pthread_mutex_unlock(mutex_ptr);

    /* sTable getting free slots and waiting for pupils to get assigned */
    /* polling_ready_sTable */
    j = 0;
    pthread_mutex_lock(mutex_ptr);

    // max_ = rand()%(restaurant->max_slots_in_sTable) + 1;
    
    for(int k = 0; k < restaurant->number_of_chefs; k++) {
      if(restaurant->chefs[k]->flag) {
        (restaurant->chefs[k]->flag)--;
        max_ = restaurant->chefs[k]->feedingCapacity;
        break;
      }

      if(k == restaurant->number_of_chefs -1) {
        k = -1;
      }
    }

    sTable->number_of_slots = max_;
    sTable->flag = 0;
    pthread_mutex_unlock(mutex_ptr);
    printf(ANSI_COLOR_MAGENTA "   Serving Table Number: %d Free Slots Available: %d\n",sTable->idx + 1, max_);

    while(j<max_)
    {
      i = rand()%restaurant->number_of_pupils;

      pthread_mutex_lock(mutex_ptr);
      if(restaurant->pupils[i]->status == STUDENT_WAITING)
      {
        restaurant->pupils[i]->status = STUDENT_ASSIGNED;
        restaurant->pupils[i]->sTable = sTable;
        j++;
        (restaurant->done_pupils)++;
        printf(ANSI_COLOR_CYAN "\tServing Table Number: %d is alloted to Student Number: %d.\n", sTable->idx + 1, i + 1);
      }
      if(restaurant->done_pupils == restaurant->number_of_pupils)
      {
        pthread_mutex_unlock(mutex_ptr);
        break;
      }
      pthread_mutex_unlock(mutex_ptr);
    }

    if(j==0)break;

    /* sTable executing voting phase. */
    printf(ANSI_COLOR_YELLOW "   Serving Table Number: %d Filled With Biryani!\n",sTable->idx + 1);

    pthread_mutex_lock(mutex_ptr);
    sTable->number_of_slots = j;
    sTable->flag = 1;
    pthread_cond_broadcast(cv_1_ptr);
    while(sTable->number_of_slots)
      pthread_cond_wait(cv_2_ptr, mutex_ptr);
    pthread_mutex_unlock(mutex_ptr);

    printf(ANSI_COLOR_RED "   Serving Table Number: %d A Group of Students Left\n",sTable->idx + 1);
  }

  return NULL;
}

ServingTable* sTable_init(ServingTable * sTable, int idx, Restaurant * restaurant)
{
  sTable = (ServingTable*)malloc(sizeof(ServingTable));
  sTable->idx = idx;
  sTable->restaurant = restaurant;
  sTable->number_of_slots = 0;
  sTable->flag = 0;
  return sTable;
}
