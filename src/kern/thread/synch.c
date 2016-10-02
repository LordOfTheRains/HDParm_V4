/*
 * Synchronization primitives.
 * See synch.h for specifications of the functions.
 */
#define true 1
#define false 0
#include <types.h>
#include <lib.h>
#include <synch.h>
#include <thread.h>
#include <curthread.h>
#include <machine/spl.h>

////////////////////////////////////////////////////////////
//
// Semaphore.

struct semaphore *
sem_create(const char *namearg, int initial_count)
{
	struct semaphore *sem;

	sem = kmalloc(sizeof(struct semaphore));
	if (sem == NULL) {
		return NULL;
	}

	sem->name = kstrdup(namearg);
	if (sem->name == NULL) {
		kfree(sem);
		return NULL;
	}

	sem->count = initial_count;
	return sem;
}

void
sem_destroy(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	spl = splhigh();
	assert(thread_hassleepers(sem)==0);
	splx(spl);

	/*
	 * Note: while someone could theoretically start sleeping on
	 * the semaphore after the above test but before we free it,
	 * if they're going to do that, they can just as easily wait
	 * a bit and start sleeping on the semaphore after it's been
	 * freed. Consequently, there's not a whole lot of point in
	 * including the kfrees in the splhigh block, so we don't.
	 */

	kfree(sem->name);
	kfree(sem);
}

void
P(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	assert(in_interrupt==0);

	spl = splhigh();
	while (sem->count==0) {
		thread_sleep(sem);
	}
	assert(sem->count>0);
	sem->count--;
	splx(spl);
}

void
V(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);
	spl = splhigh();
	sem->count++;
	assert(sem->count>0);
	thread_wakeup(sem);
	splx(spl);
}

////////////////////////////////////////////////////////////
//
// Lock.

struct lock *
lock_create(const char *name)
{

	struct lock *lock;

	lock = kmalloc(sizeof(struct lock));
	if (lock == NULL) {
		return NULL;
	}

	lock->name = kstrdup(name);
	if (lock->name == NULL) {
		kfree(lock);
		return NULL;
	}
	// add stuff here as needed

	lock->isHeld = false;
	lock->holder = NULL;

	return lock;
}

void
lock_destroy(struct lock *lock)
{
	/*
	int spl;
	if(lock == NULL){
		panic("cannot destroy lock: lock does not exist, you idiot.");
	}

  spl = splhigh();
	if(!lock_do_i_hold(lock)){
		panic("cannot destroy lock: YOU DO NOT OWN ME, you idiot.");
	}
	*/
	// lock_release(lock);
	// add stuff here as needed
	kfree(lock->name);
	kfree(lock);
	//splx(spl);
}

void
lock_acquire(struct lock *lock)
{
	// Write this
	int spl;
	assert(lock!=NULL);
  spl = splhigh();
	if(lock_do_i_hold(lock)){
		panic("cannot get the lock: dont double dip, you idiot.");
	}
	//edited this
	while(lock->isHeld){
		thread_sleep(lock);
	};

	lock->isHeld = true;
	lock->holder = curthread;
	splx(spl);
	// (void)lock;  // suppress warning until code gets written
}

void
lock_release(struct lock *lock)
{
	// Write this
	int spl;
	assert(lock!=NULL);
	spl = splhigh();
	if(!lock_do_i_hold(lock)){
		panic("cannot release lock: YOU DONT OWN ME, you idiot.");
	}
	else {
		lock->holder = NULL;
		lock->isHeld = false;
		thread_wakeup(lock);
	}
	splx(spl);
	// (void)lock;  // suppress warning until code gets written
}

int
lock_do_i_hold(struct lock *lock)
{
	/*
	// Write this
	int spl, result;
	if(lock == NULL){
		panic("cannot check lock holder: lock does not exist, you idiot.");
	}
	spl = splhigh();
	if (lock->isHeld){
		result = (lock->holder == curthread);
	}
	else {
		result = 0;
	}
	// (void)lock;  // suppress warning until code gets written
	splx(spl);*/
	return (lock->holder == curthread);    // dummy until code gets written
}

////////////////////////////////////////////////////////////
//
// CV


struct cv *
cv_create(const char *name)
{
	struct cv *cv;

	cv = kmalloc(sizeof(struct cv));
	if (cv == NULL) {
		return NULL;
	}

	cv->name = kstrdup(name);
	if (cv->name==NULL) {
		kfree(cv);
		return NULL;
	}

	// add stuff here as needed

	return cv;
}

void
cv_destroy(struct cv *cv)
{
	assert(cv != NULL);

	// add stuff here as needed

	kfree(cv->name);
	kfree(cv);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}

void
cv_signal(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}
