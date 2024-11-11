#include "types.h"
#include "user.h"

#define NCHILDREN  8		/* should be > 1 to have contention */
#define CRITSECTSZ 3
#define DATASZ (1024 * 32 / NCHILDREN)

#define LOCKS_IMPLEMENTED 	/* uncomment this when you have a working lock implementation */
#ifdef LOCKS_IMPLEMENTED
#include "lock.h"
#define LOCK_CREATE(x)  lock_create(x)
#define LOCK_TAKE(x)    lock_take(x)
#define LOCK_RELEASE(x) lock_release(x)
#define LOCK_DEL(x)     lock_delete(x)
#else
int compilehack = 0;
#define LOCK_CREATE(x)  compilehack++
#define LOCK_TAKE(x)    compilehack++
#define LOCK_RELEASE(x) compilehack++
#define LOCK_DEL(x)     compilehack++
#endif


void
child(int lockid, int pipefd, char tosend)
{
	int i, j;

	for (i = 0 ; i < DATASZ ; i += CRITSECTSZ) {
		/*
		 * If the critical section works, then each child
		 * writes CRITSECTSZ times to the pipe before another
		 * child does.  Thus we can detect race conditions on
		 * the "shared resource" that is the pipe.
		 */
		LOCK_TAKE(lockid);
		for (j = 0 ; j < CRITSECTSZ ; j++) {
			write(pipefd, &tosend, 1);
		}
		LOCK_RELEASE(lockid);
	}
	exit();
}

int
main(void)
{
	int i;
	int pipes[2];
	char data[NCHILDREN], first = 'a';
	int lockid;

	for (i = 0 ; i < NCHILDREN ; i++) {
		data[i] = (char)(first + i);
	}

	if (pipe(&pipes[0])) {
		printf(1, "Pipe error\n");
		exit();
	}

	if ((lockid = LOCK_CREATE(LOCK_SPIN)) < 0) {
		printf(1, "Lock creation error\n");
		exit();
	}

	for (i = 0 ; i < NCHILDREN ; i++) {
		if (fork() == 0) child(lockid, pipes[1], data[i]);
	}
	close(pipes[1]);

	while (1) {
		char fst, c;
		int cnt;

		fst = '_';
		for (cnt = 0 ; cnt < CRITSECTSZ ; cnt++) {
			if (read(pipes[0], &c, 1) == 0) goto done;

			if (fst == '_') {
				fst = c;
			} else if (fst != c) {
				printf(1, "RACE!!!\n");
			}
		}
	}
done:

	for (i = 0 ; i < NCHILDREN ; i++) {
		if (wait() < 0) {
			printf(1, "Wait error\n");
			exit();
		}
	}

	LOCK_DEL(lockid);

	exit();
}
