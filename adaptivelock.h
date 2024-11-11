struct adaptivelock{
	struct spinlock lk;
	uint locked;
	uint adaptivecount; //if . 1024
	uint adaptivelock; //if
	char * name;
	int pid;
};