#include "types.h"
#include "user.h"
#include "lock.h"

int main(void)
{
	printf(1, "Stating test\n");
	int S_take1 = 2;
	int S_take2 = 2;
	int S_take3 = 2;
	printf(1, "CREATE LOCK 1\n");
	int S_create1 = lock_create(1); //create first lock
	printf(1, "%s %d\n", "Lockid = ", S_create1);
	printf(1, "CREATELOCK 1 SUCCESSFUL-1\n");
	printf(1, "------------------------------\n");
	printf(1, "\n");

	printf(1, "TAKE LOCK 1\n");
	S_take1 = lock_take(S_create1); //acquire the first lock
	if (S_take1 == 0){
		printf(1, "%s %d\n", "S_take1 = ", S_take1); //should return 0
		printf(1, "TAKE LOCK 1 SUCCESSFUL-1\n");
	} else{
		printf(1, "TAKE LOCK 1 UNSUCCESSFUL-1\n");
	}
	printf(1, "------------------------------\n");
	printf(1, "\n");

	printf(1, "FORK ANOTHER PROCESS\n");
	if (fork() == 0){ //create another process
		printf(1, "TAKE LOCK 1 AGAIN - SHOULD RETURN -1\n");
		S_take2 = lock_take(S_create1);  //try to acquire the lock again
		if (S_take2 == -1){
			printf(1, "%s %d\n", "S_take2 = ", S_take2); //should return -1 becuase the lock has been acquired
			printf(1, "TAKE LOCK 1 AGAIN SUCCESSFUL-1\n");
		} else{
			printf(1, "TAKE LOCK 1 AGAIN UNSUCCESSFUL-1\n");
		}
		printf(1, "------------------------------\n");
		printf(1, "\n");



		printf(1, "RELEASE LOCK 1\n");
		int S_release1 = lock_release(S_create1);
		printf(1, "%s %d\n", "S_realese1 = ", S_release1); //should return 0 becuase the lock has been released
		S_take2 = lock_take(S_create1);  //try to acquire the lock again
		if (S_take2 == 0){
			printf(1, "%s %d\n", "S_take2 = ", S_take2); //should return 0
			printf(1, "RELEASE SUCCESSFUL-1\n");
		} else{
			printf(1, "RELEASE UNSUCCESSFUL-1\n");
		}
		printf(1, "------------------------------\n");
		printf(1, "\n");
	}

	printf(1, "RELEASE LOCK 1 WITHIN SAME PROCESS\n");
	int S_release2 = lock_release(S_create1);
	printf(1, "%s %d\n", "S_realese2 = ", S_release2); //should return 0 becuase the lock has been released
	
	int S_release3 = lock_release(S_create1);
	printf(1, "%s %d\n", "S_realese3 = ", S_release3); //should return 0 becuase the lock has been released
	if (S_release3 == -1){
		printf(1, "%s %d\n", "S_take3 = ", S_take3); //should return 0
		printf(1, "RELEASE AGAIN SUCCESSFUL-1\n");
	} else{
		printf(1, "RELEASE AGAIN UNSUCCESSFUL-1\n");
	}
	printf(1, "------------------------------\n");
	printf(1, "\n");

	S_take3 = lock_take(S_create1);  //try to acquire the lock again
	if (S_take3 == 0){
		printf(1, "%s %d\n", "S_take3 = ", S_take3); //should return 0
		printf(1, "RELEASE SUCCESSFUL-1\n");
	} else{
		printf(1, "RELEASE UNSUCCESSFUL-1\n");
	}
	printf(1, "------------------------------\n");
	printf(1, "\n");

	wait();
	exit();
}