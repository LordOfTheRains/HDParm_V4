/*
 * catsem.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use SEMAPHORES to solve the cat syncronization problem in 
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

typedef int bool;
#define true 1
#define false 0



volatile bool all_dishes_available = true;
volatile bool dish1Busy = false;
volatile bool dish2Busy = false;

static struct semaphore *mutex;
static struct semaphore *dish_sem;
static struct semaphore *cat_sem;
static struct semaphore *mouse_sem;
static struct semaphore *done;
static struct semaphore *all_done;


volatile int cats_wait_count = 0;
volatile bool no_cat_eat = true;
volatile int mice_wait_count = 0;
volatile bool no_mouse_eat = true;
static volatile int cats_done_eating;
static volatile int mice_done_eating;

// volatile bool first_cat_eat = true;

/*
 * 
 * Function Definitions
 * 
 */


/*
 * catsem()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using semaphores.
 *
 */

static
void
catsem(void * unusedpointer, 
       unsigned long catnumber)
{
        
        /*
         * Avoid unused variable warnings.
         */

        (void) unusedpointer;
        (void) catnumber;

        bool first_cat_eating = false;
        bool another_cat_eating = false;
        int myDish;

        P(mutex);

        //Check if first cat
        if(all_dishes_available) {
                //this cat is first
                all_dishes_available = false;
                V(cat_sem);

                no_cat_eat = false;
                first_cat_eating = true;
        } else {
                //NOT first cat
                first_cat_eating = false;
        }

        cats_wait_count++;
        V(mutex);

        P(cat_sem); //let cat in

        P(mutex);

        V(mutex);
        if(first_cat_eating) {
                P(mutex);
                //If another cat is waiting let them eat
                if (cats_wait_count > 1) {
                        another_cat_eating = true;
                        V(cat_sem);
                }
                V(mutex);
        }
        
        kprintf("Cat %lu in the kitchen.\n", catnumber);


        //dish
        P(dish_sem);
        //check dish 1 not busy. 
        if(!dish1Busy){
                //take dish 1
                dish1Busy = true;
                myDish = 1;
        } else { //check dish 2 not busy
                assert(!dish2Busy);
                dish2Busy = true;
                myDish = 2;
        }
        V(dish_sem);

        //do the eating
        kprintf("Cat %lu eating.\n", catnumber);
        clocksleep(1);
        kprintf("Cat %lu done eating.\n", catnumber);

        P(dish_sem);
        //Release the dish gotten earlier
        if(myDish == 1){
                dish1Busy = false;
        } else {
                assert(myDish == 2);
                dish2Busy = false;
        }
        
        V(dish_sem);

        P(mutex);
        cats_wait_count--;
        V(mutex);

        //Leave kitchen
        if(first_cat_eating) {

                //If ther is another cat, wait for it to finish
                if (another_cat_eating) {
                        P(done);
                }

                kprintf("First cat, %lu, is now leaving.\n", catnumber);
                
                //No cats are eating
                P(mutex);
                no_cat_eat = true;
                V(mutex);

                //If mice are waiting, switch to mouse context.
                //else let more cats in
                P(mutex);
                if (mice_wait_count > 0) {
                        V(mouse_sem);
                } else if (cats_wait_count > 0) {
                        V(cat_sem);
                } else {
                        all_dishes_available = true;
                }
                V(mutex);
        } else {
              //Signal that second cat is leaving
              kprintf("Other cat, %lu, is leaving.\n", catnumber);  
              V(done);
        }

        //Signal that this fat kitty is done eating
        P(mutex);
        cats_done_eating++;
        V(all_done);
        V(mutex);
}
        

/*
 * mousesem()
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
 *      Write and comment this function using semaphores.
 *
 */

static
void
mousesem(void * unusedpointer, 
         unsigned long mousenumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) unusedpointer;
        (void) mousenumber;

        bool first_mouse_eating = false;
        bool another_mouse_eating = false;
        int myDish;

        P(mutex);

        //Check if first mouse
        if(all_dishes_available) {
                //this mouse is first
                all_dishes_available = false;
                V(mouse_sem);

                no_mouse_eat = false;
                first_mouse_eating = true;
        } else {
                //NOT first mouse
                first_mouse_eating = false;
        }

        mice_wait_count++;
        V(mutex);

        P(mouse_sem); //let cat in

        P(mutex);

        V(mutex);
        if(first_mouse_eating) {
                P(mutex);
                //If another cat is waiting le tthem eat
                if (mice_wait_count > 1) {
                        another_mouse_eating = true;
                        V(cat_sem);
                }
                V(mutex);
        }
        
        kprintf("Mouse %lu in the kitchen.\n", mousenumber);


        //dish
        P(dish_sem);
        //check dish 1 not busy. 
        if(!dish1Busy){
                //take dish 1
                dish1Busy = true;
                myDish = 1;
        } else { //check dish 2 not busy
                assert(!dish2Busy);
                dish2Busy = true;
                myDish = 2;
        }
        V(dish_sem);

        //do the eating
        kprintf("Mouse %lu eating.\n", mousenumber);
        clocksleep(1);
        kprintf("Mouse %lu done eating.\n", mousenumber);

        P(dish_sem);
        //Release the dish gotten earlier
        if(myDish == 1){
                dish1Busy = false;
        } else {
                assert(myDish == 2);
                dish2Busy = false;
        }
        
        V(dish_sem);

        P(mutex);
        mice_wait_count--;
        V(mutex);

        //Leave kitchen
        if(first_mouse_eating) {

                //If ther is another cat, wait for it to finish
                if (another_cat_eating) {
                        P(done);
                }

                kprintf("First Mouse, %lu, is now leaving.\n", catnumber);
                
                //No cats are eating
                P(mutex);
                no_cat_eat = true;
                V(mutex);

                //If cats are waiting, switch to cat context.
                //else let more mice in
                P(mutex);
                if (cats_wait_count > 0) {
                        V(cat_sem);
                } else if (cats_wait_count > 0) {
                        V(mouse_sem);
                } else {
                        all_dishes_available = true;
                }
                V(mutex);
        } else {
              //Signal that second cat is leaving
              kprintf("Other Mouse, %lu, is leaving.\n", catnumber);  
              V(done);
        }

        //Signal that this fat kitty is done eating
        P(mutex);
        mice_done_eating++;
        V(all_done);
        V(mutex);
}

/*
* init_catmousesem()
*
* Arguments:
*  none
*
* Returns:
*  none
*
* Notes:
*  Initializes all global variables and creates semaphores
*/
static
void
init_catmousesem() {
        all_dishes_available = true;
        dish1Busy = false;
        dish2Busy = false;

        mutex = sem_create("mutex", 0);
        dish_sem = sem_create("dish_sem", 1);
        cat_sem = sem_create("cat_sem", 0);
        mouse_sem = sem_create("mouse_sem", 0);
        done = sem_create("done", 0);
        all_done = sem_create("all_done", 0);

        cats_wait_count = 0;
        no_cat_eat = true;
        mice_wait_count = 0;
        no_mouse_eat = true;
        cats_done_eating = 0;
        mice_done_eating = 0;
}

/*
* end_catmousesem()
*
* Arguments:
*  none
*
* Returns:
*  none
*
* Notes:
*  Destroys all semaphores
*/
static
void
end_catmousesem() {
        sem_destroy(mutex);
        sem_destroy(dish_sem);
        sem_destroy(cat_sem);
        sem_destroy(mouse_sem);
        sem_destroy(done);
        sem_destroy(all_done);
        mutex = NULL;
        dish_sem = NULL;
        cat_sem = NULL;
        mouse_sem = NULL;
        done = NULL;
        all_done = NULL;
}

/*
 * catmousesem()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catsem() and mousesem() threads.  Change this 
 *      code as necessary for your solution.
 */

int
catmousesem(int nargs,
            char ** args)
{
        int index, error;
   
        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;
        
        //init the global variables and create the semaphores
        init_catmousesem();
        /*
         * Start NCATS catsem() threads.
         */

        for (index = 0; index < NCATS; index++) {
           
                error = thread_fork("catsem Thread", 
                                    NULL, 
                                    index, 
                                    catsem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
                 
                        panic("catsem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }
        
        /*
         * Start NMICE mousesem() threads.
         */

        for (index = 0; index < NMICE; index++) {
   
                error = thread_fork("mousesem Thread", 
                                    NULL, 
                                    index, 
                                    mousesem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
         
                        panic("mousesem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

        //Wait for the animals to feed
        while (cats_done_eating < NCATS || mice_done_eating < NMICE) {
                P(all_done);
        }
        //cleanup
        end_catmousesem();
        return 0;
}


/*
 * End of catsem.c
 */
