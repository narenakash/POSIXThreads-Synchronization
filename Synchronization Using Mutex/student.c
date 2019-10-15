#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "restaurant.h"

void * pupil_thread(void * args)
{
  Student * pupil = (Student*)args;
  pthread_mutex_t * mutex_ptr = &(pupil->restaurant->mutex);
  pthread_cond_t * cv_1_ptr = &(pupil->restaurant->cv_1);
  pthread_cond_t * cv_2_ptr = &(pupil->restaurant->cv_2);

  /* Student waiting to get allocated */
  /* pupil_wait_for_sTable */
  pthread_mutex_lock(mutex_ptr);
  pupil->status = STUDENT_WAITING;
  while(pupil->status == STUDENT_WAITING)
    pthread_cond_wait(cv_1_ptr, mutex_ptr);
  pthread_mutex_unlock(mutex_ptr);

  ServingTable * sTable = pupil->sTable;

  /* pupil is in the slot. waiting for sTable to start voting phase. */
  /* pupil_in_slot */
  pthread_mutex_lock(mutex_ptr);
  while(sTable->flag == 0)
    pthread_cond_wait(cv_1_ptr, mutex_ptr);
  (sTable->number_of_slots)--;
  printf(ANSI_COLOR_GREEN "\tPupil Number: %d finished eating from Serving Table Number: %d\n", pupil->idx + 1, sTable->idx + 1);
  pthread_cond_broadcast(cv_2_ptr);
  pthread_mutex_unlock(mutex_ptr);

  return NULL;
}

Student* pupil_init(Student * pupil, int idx, Restaurant * restaurant)
{
  pupil = (Student*)malloc(sizeof(Student));
  pupil->idx = idx;
  pupil->restaurant = restaurant;
  pupil->status = STUDENT_CAME;
  pupil->sTable = NULL;
  return pupil;
}
