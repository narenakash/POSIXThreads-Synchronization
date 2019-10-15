#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

extern const int STUDENT_CAME;
extern const int STUDENT_WAITING;
extern const int STUDENT_ASSIGNED;
extern const int STUDENT_DONE;

struct Restaurant;
typedef struct Restaurant Restaurant;

typedef struct ServingTable
{
  int idx;
  pthread_t sTable_thread_id;
  Restaurant * restaurant;
  int number_of_slots;
  int flag;
} ServingTable;

typedef struct Student
{
  int idx;
  pthread_t pupil_thread_id;
  Restaurant * restaurant;
  ServingTable * sTable;
  int status;
} Student;

typedef struct RobotChef
{
  int idx;
  pthread_t chef_thread_id;
  int seconds;
  int vessels;
  int feedingCapacity;
  int flag;
  Restaurant * restaurant;
} RobotChef;

struct Restaurant
{
  int idx;
  pthread_t restaurant_thread_id;
  int number_of_servingTables;
  int number_of_chefs;
  int max_slots_in_sTable;
  int number_of_pupils;
  int done_pupils;
  ServingTable ** servingTables;
  RobotChef ** chefs;
  Student ** pupils;
  pthread_mutex_t mutex;
  pthread_cond_t cv_1;
  pthread_cond_t cv_2;
};

Restaurant* restaurant_init( Restaurant * restaurant, int idx, int number_of_servingTables, int max_slots_in_sTable, int number_of_pupils, int number_of_chefs);
void * restaurant_thread(void * args);
void restaurant_cleanup(Restaurant * restaurant);

ServingTable* sTable_init(ServingTable * sTable, int idx, Restaurant * restaurant);
void * sTable_thread(void * args);

Student* pupil_init(Student * pupil, int idx, Restaurant * restaurant);
void * pupil_thread(void * args);

RobotChef* chef_init(RobotChef * sTable, int idx, Restaurant * restaurant);
void * chef_thread(void * args);

