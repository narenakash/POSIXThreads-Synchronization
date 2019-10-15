#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "restaurant.h"

void * restaurant_thread(void* args)
{
  Restaurant * restaurant = (Restaurant*)args;
  
  for(int i=0; i<restaurant->number_of_servingTables; i++)
    restaurant->servingTables[i] = sTable_init(restaurant->servingTables[i], i, restaurant);
  for(int i=0; i<restaurant->number_of_pupils; i++)
    restaurant->pupils[i] = pupil_init(restaurant->pupils[i], i, restaurant);
  for(int i=0; i<restaurant->number_of_chefs; i++)
    restaurant->chefs[i] = chef_init(restaurant->chefs[i], i, restaurant);

  for(int i=0; i<restaurant->number_of_servingTables; i++)
    pthread_create(&(restaurant->servingTables[i]->sTable_thread_id), NULL, sTable_thread, restaurant->servingTables[i]);
  for(int i=0; i<restaurant->number_of_pupils; i++)
    pthread_create(&(restaurant->pupils[i]->pupil_thread_id), NULL, pupil_thread, restaurant->pupils[i]);
  for(int i=0; i<restaurant->number_of_chefs; i++)
    pthread_create(&(restaurant->chefs[i]->chef_thread_id), NULL, chef_thread, restaurant->chefs[i]);

  for(int i=0; i<restaurant->number_of_servingTables; i++)
    pthread_join(restaurant->servingTables[i]->sTable_thread_id, 0);
  for(int i=0; i<restaurant->number_of_pupils; i++)
    pthread_join(restaurant->pupils[i]->pupil_thread_id, 0);
  for(int i=0; i<restaurant->number_of_chefs; i++)
    pthread_join(restaurant->chefs[i]->chef_thread_id, 0);

  restaurant_cleanup(restaurant);
  return NULL;
}

Restaurant* restaurant_init( Restaurant * restaurant, int idx,
  int number_of_servingTables, int max_slots_in_sTable, int number_of_pupils, int number_of_chefs)
{
  restaurant = (Restaurant*)malloc(sizeof(Restaurant));
  restaurant->idx = idx;
  restaurant->number_of_servingTables = number_of_servingTables;
  restaurant->number_of_chefs = number_of_chefs;
  restaurant->max_slots_in_sTable = max_slots_in_sTable;
  restaurant->number_of_pupils = number_of_pupils;
  restaurant->done_pupils = 0;
  restaurant->servingTables = (ServingTable**)malloc(sizeof(ServingTable*)*number_of_servingTables);
  restaurant->chefs = (RobotChef**)malloc(sizeof(RobotChef*)*number_of_chefs);
  restaurant->pupils = (Student**)malloc(sizeof(Student*)*number_of_pupils);

  pthread_mutex_init(&(restaurant->mutex), NULL);
  pthread_cond_init(&(restaurant->cv_1), NULL);
  pthread_cond_init(&(restaurant->cv_2), NULL);

  return restaurant;
}

void restaurant_cleanup(Restaurant * restaurant)
{
  for(int i=0; i<restaurant->number_of_servingTables; i++)
    free(restaurant->servingTables[i]);
  for(int i=0; i<restaurant->number_of_pupils; i++)
    free(restaurant->pupils[i]);
  free(restaurant->servingTables);
  free(restaurant->pupils);
}
