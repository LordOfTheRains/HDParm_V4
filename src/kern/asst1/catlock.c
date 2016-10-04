/*
 * catlock.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use LOCKS/CV'S to solve the cat syncronization problem in
 * this file.
 */


/*
 *
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>


/*
 *
 * Constants
 *
 */

/*
 * Number of food bowls.
 */

#define NFOODBOWLS 2

/*
 * Number of cats.
 */

#define NCATS 6

/*
 * Number of mice.
 */

#define NMICE 2

#define ANY_TURN 2
#define CATS_TURN 1
#define MICES_TURN 0

#define CATS_QUOTA
#define MICE_QUOTA

/*
 *
 * Function Definitions
 *
 */


/*
 * catlock()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS -
 *      1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using locks/cv's.
 *
 */
/*
static volatile cats_turns = 2;
static volatile mice_turns = 2;
static struct lock *mutex;
static struct cv *done_cv;
static struct cv *turn_cv;
/* wait for the cat or mouse turn */
/* wait here for process done */
/* Initialize mutex and two condition variables */
/*
mutex = lock_create("catlock mutex");
turn_cv = cv_create("catlock turn cv");
done_cv = cv_create("catlock done cv");
*/

static volatile int bowl_count;
static struct lock *kitchen_mutex;
static struct lock *bowl_mutex;

static struct lock *cats_queue_mutex;
static struct cv *cats_queue_cv;

static struct lock *cats_wait_mutex;
static volatile int *cats_waiting;

static struct lock *mice_queue_mutex;
static struct cv *mice_queue_cv;

static struct lock *mice_wait_mutex;
static volatile int *mice_waiting;


static struct lock *current_turn_mutex;
static volatile int current_turn;

static volatile int cats_quota;
static volatile int mice_quota;

static
void
lock_init(){
  //initialize bowl counts;
  bowl_count = NFOODBOWLS;
  bowl_mutex = lock_create("lock on available dishes(bowl_count)");
  kitchen_mutex = lock_create("lock on kitchen");
  //bowl_1_mutex = lock_create("lock on dish 1");
  //bowl_2_mutex = lock_create("lock on dish 2");

  //cats_quota = CATS_QUOTA;
  cats_queue_mutex = lock_create("lock on cats queue CV");
  cats_queue_cv = cv_create("queue for all waiting cats");

  cats_wait_mutex = lock_create("lock on cats wait ");
  cats_waiting = 0;
  //refill_cats_quota();
  //refill_mice_quota();
  mice_queue_mutex = lock_create("lock on mice queue CV");
  mice_queue_cv = cv_create("queue for all waiting mice");
  mice_wait_mutex = lock_create("lock on mice wait");
  mice_waiting = 0;


  current_turn_mutex = lock_create("lock on current_turn");
  current_turn = ANY_TURN;
}



static
void
catlock(void * unusedpointer,
        unsigned long catnumber)
{
  //sleep if no bowl is available or is mices turn;
  if(bowl_count==0 || current_turn == MICES_TURN ){
    kprintf("cat %lu, is waiting outside.\n", catnumber);
    lock_acquire(cats_wait_mutex);
    cats_waiting++;
    lock_release(cats_wait_mutex);
    cv_wait(cats_queue_cv,cats_queue_mutex);
  };
  //bowl is available and is cats turn
  kprintf("cat %lu, is going to the kitchen.\n", catnumber);
  lock_acquire(kitchen_mutex);

  lock_acquire(cats_wait_mutex);
  cats_waiting--;
  lock_release(cats_wait_mutex);

  kprintf("cat %lu, is in kitchen.\n", catnumber);
  if(bowl_count==0){
    panic("no bowls available in kitchen!!!!\n");
  }
  //there's another cat eating;
  if(bowl_count==1){
    kprintf("cat %lu, is in kitchen.-- 1 bowl\n", catnumber);
    lock_acquire(bowl_mutex);
    bowl_count--;
    lock_release(bowl_mutex);


    lock_acquire(mice_wait_mutex);
    if(mice_waiting > 0){
      lock_acquire(current_turn_mutex);
      current_turn = MICES_TURN;
      lock_release(current_turn_mutex);

      clocksleep(1);

      kprintf("cat %lu, is in kitchen.-- 1 bowl, done eating\n", catnumber);

      lock_release(kitchen_mutex);
      //wake up all mice
      cv_broadcast(mice_queue_cv,mice_queue_mutex);
    }
    else{
      lock_acquire(current_turn_mutex);
      current_turn = ANY_TURN;
      lock_release(current_turn_mutex);

      clocksleep(1);

      kprintf("cat %lu, is in kitchen.-- 1 bowl, done eating\n", catnumber);
      lock_release(kitchen_mutex);
      //wake up all cats
      cv_broadcast(cats_queue_cv,cats_queue_mutex);
    }
    lock_release(mice_wait_mutex);


  }
  // you are the first catttt
  else if(bowl_count==2){

    kprintf("cat %lu, is in kitchen.-- 2 bowls\n", catnumber);
    lock_acquire(bowl_mutex);
    bowl_count--;
    lock_release(bowl_mutex);
    kprintf("cat %lu, is in kitchen.-- 2 bowls, took 1\n", catnumber);

    //check if another cat is waiting
    lock_acquire(cats_wait_mutex);
    if(cats_waiting > 0){
      lock_acquire(current_turn_mutex);
      current_turn = CATS_TURN;
      lock_release(current_turn_mutex);

      lock_release(kitchen_mutex);
      //wake up next cat
      kprintf("cat %lu, is in kitchen.-- 2 bowl, woke next cat\n", catnumber);
      cv_signal(cats_queue_cv,cats_queue_mutex);

      clocksleep(1);

      kprintf("cat %lu, is in kitchen.-- 2 bowl, done eating\n", catnumber);

    }
    //not cats are waiting... set the turn to open
    else{
      lock_acquire(current_turn_mutex);
      current_turn = ANY_TURN;
      lock_release(current_turn_mutex);

      clocksleep(1);

      kprintf("cat %lu, is in kitchen.-- 2 bowl, done eating\n", catnumber);
      lock_release(kitchen_mutex);
      //wake up all cats
      kprintf("cat %lu, is in kitchen.-- 2 bowl, woke next catc\n", catnumber);
      cv_broadcast(mice_queue_cv,mice_queue_mutex);
    }
    lock_release(cats_wait_mutex);

  }
  else {
    panic("magical bowls in kitchen!!!");
  }
        /*
         * Avoid unused variable warnings.
         */




         /*
        (void) unusedpointer;
        (void) catnumber;
        */
}
static void refill_cats_quota(){
    //cats_quota = CATS_QUOTA;
}
static void refill_mice_quota(){
    //mice_quota = MICE_QUOTA;
}
/*
 * mouselock()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long mousenumber: holds the mouse identifier from 0 to
 *              NMICE - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using locks/cv's.
 *
 */

static
void
mouselock(void * unusedpointer,
          unsigned long mousenumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) unusedpointer;
        (void) mousenumber;
        //sleep if no bowl is available or is mices turn;
        if(bowl_count==0 || current_turn == CATS_TURN ){

            kprintf("mouse %lu, is waiting outside.\n", mousenumber);
            lock_acquire(mice_wait_mutex);
            mice_waiting++;
            lock_release(mice_wait_mutex);
          cv_wait(mice_queue_cv,mice_queue_mutex);
        };
        //bowl is available and is cats turn
        kprintf("mouse %lu, is going to the kitchen.\n", mousenumber);
        lock_acquire(kitchen_mutex);

        lock_acquire(mice_wait_mutex);
        mice_waiting--;
        lock_release(mice_wait_mutex);


        kprintf("Mouse %lu, is in kitchen.\n", mousenumber);
        if(bowl_count==0){
          panic("no bowls available in kitchen!!!!");
        }
        if(bowl_count==1){
          kprintf("mouse %lu, is in kitchen.-- 1 bowl\n", mousenumber);
          lock_acquire(bowl_mutex);
          bowl_count--;
          lock_release(bowl_mutex);

          lock_acquire(current_turn_mutex);
          current_turn = CATS_TURN;
          lock_release(current_turn_mutex);

          clocksleep(1);
          cv_broadcast(cats_queue_cv,cats_queue_mutex);
        }
        else if(bowl_count==2){

          kprintf("mouse %lu, is in kitchen.-- 2 bowls\n", mousenumber);
          lock_acquire(bowl_mutex);
          bowl_count--;
          lock_release(bowl_mutex);

          lock_acquire(current_turn_mutex);
          current_turn = CATS_TURN;
          lock_release(current_turn_mutex);
          cv_signal(mice_queue_cv,mice_queue_mutex);

          clocksleep(1);

          cv_broadcast(mice_queue_cv,mice_queue_mutex);
        }
        else {
          panic("magical bowls in kitchen!!!");
        }

        lock_release(kitchen_mutex);



}


/*
 * catmouselock()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catlock() and mouselock() threads.  Change
 *      this code as necessary for your solution.
 */

int
catmouselock(int nargs,
             char ** args)
{
        int index, error;

        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;

        /*
         * Start NCATS catlock() threads.
         */
         kprintf("starting locksssssss!!\n");
         lock_init();
        for (index = 0; index < NCATS; index++) {

                error = thread_fork("catlock thread",
                                    NULL,
                                    index,
                                    catlock,
                                    NULL
                                    );

                /*
                 * panic() on error.
                 */

                if (error) {

                        panic("catlock: thread_fork failed: %s\n",
                              strerror(error)
                              );
                }
        }

        /*
         * Start NMICE mouselock() threads.
         */

        for (index = 0; index < NMICE; index++) {

                error = thread_fork("mouselock thread",
                                    NULL,
                                    index,
                                    mouselock,
                                    NULL
                                    );

                /*
                 * panic() on error.
                 */

                if (error) {

                        panic("mouselock: thread_fork failed: %s\n",
                              strerror(error)
                              );
                }
        }

        return 0;
}

/*
 * End of catlock.c
 */
