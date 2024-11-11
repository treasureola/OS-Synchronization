#include "types.h"
#include "stat.h"
#include "user.h"
#include "lock.h"

int
main(void){

	printf(1, "Test 5 Begin\n");

	int pid, lockid, lockid2;

	lockid = lock_create(LOCK_SPIN);
	printf(1, "Lock created at: %d\n", lockid);

	if(lockid < 0){
		printf(1, "Lock creation error\n");
		exit();
	}

	pid = fork();
	if(pid == 0){
		// Child
		lock_take(lockid);
		lock_release(lockid);
		lock_delete(lockid);
		exit();
	}else{
		// Parent
		wait();
		lock_take(lockid);
		lock_release(lockid);
		lock_delete(lockid);
		lockid2 = lock_create(LOCK_SPIN);
		printf(1, "Lock created at: %d\n", lockid2);
	}
	if(lockid2 == lockid){
		printf(1, "Passed Test 5\n");
	}else{
		printf(1, "Lock ids do not match\n");
	}

	lock_delete(lockid2);

	exit();
	return 0;
}