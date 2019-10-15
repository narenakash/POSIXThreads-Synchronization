#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "restaurant.h"

const int STUDENT_CAME=0;
const int STUDENT_WAITING=1;
const int STUDENT_ASSIGNED=2;
const int STUDENT_DONE=3;

int main()
{
  int _number_of_chefs;
  int _number_of_servingTables;
  int _max_slots_in_sTable = 10;
  int _number_of_pupils;
  
  scanf("%d %d %d", &_number_of_chefs, &_number_of_servingTables, &_number_of_pupils);

  printf("RESTAURANT OPEN\n\n");

  Restaurant *dining = (Restaurant*) malloc(sizeof(Restaurant*));
 
  dining = restaurant_init( dining, 1, _number_of_servingTables, _max_slots_in_sTable, _number_of_pupils, _number_of_chefs);

  pthread_create(&(dining->restaurant_thread_id), NULL, restaurant_thread, dining);

  pthread_join(dining->restaurant_thread_id, 0);

  printf(ANSI_COLOR_RESET "\nRESTAURANT CLOSED\n\n");

  return 0;
}
