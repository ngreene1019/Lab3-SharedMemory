#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <unistd.h>
#include  <sys/wait.h>
#include  <time.h>

void  ClientProcess(int []);
void  ParentProcess(int []);

//Pair Programming with Dr. Burge

// these two integers will be shared between processes
// only one process can modify these values at one time
int turn = 0;
int bankAccount = 0;


int  main(int  argc, char *argv[])
{
	int    ShmID;
	int    *ShmPTR; // this will point to array that holds the values that are passed in via argv[]
	pid_t  pid;
	int    status;
	int i = 0;

	srand (time(NULL));	// seeds the rand num generator

	//taking commandline args
	if (argc != 5) {
		printf("Use: %s #1 #2 #3 #4\n", argv[0]); // four integers
		exit(1);
	}

	ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
	if (ShmID < 0) {
		printf("*** shmget error (server) ***\n");
		exit(1);
	}
	printf("Server has received a shared memory of four integers...\n");

	ShmPTR = (int *) shmat(ShmID, NULL, 0);
	if (*ShmPTR == -1) {
		printf("*** shmat error (server) ***\n");
		exit(1);
	}
	printf("Server has attached the shared memory...\n");

	// storing the args into your array
	ShmPTR[0] = 0; //turn variable
	ShmPTR[1] = 0; //bankaccount variable

	printf("Server has filled %d %d in shared memory...\n",
		ShmPTR[0], ShmPTR[1]);

	printf("Server is about to fork a child process...\n");
	pid = fork();  // this forkes the first process
	if (pid < 0) {
		printf("*** fork error (server) ***\n");
		exit(1);
	}
	else if (pid == 0) {
		for (i = 0; i < 25; i++){
			ClientProcess(ShmPTR);  // this when the child gets control
		}
		
		exit(0);
	}

	for (i = 0; i < 25; i++){
  	ParentProcess(ShmPTR);  // parent gets control
	}
	wait(&status); // parent waits for child
	printf("Server has detected the completion of its child...\n");
	shmdt((void *) ShmPTR);
	printf("Server has detached its shared memory...\n");
	shmctl(ShmID, IPC_RMID, NULL);
	printf("Server has removed its shared memory...\n");



	printf("Server exits...\n");
	exit(0);
}

void  ClientProcess(int  ShmPTR[])
{
	int account = 0;  // this is a copy of accountBalance (the shared value)
	int balance = 0; //  this is the random amount the child wants
	int asleep = 0; // this is the random amount of sleep time
  
	asleep = rand() % 5;
  
	// sleep 0 -5 s
	sleep(asleep);

	// upon waking
  while(ShmPTR[0] != 1);

	account = ShmPTR[1];

	// randomly general int between 0 - 50
	balance = rand() % 50;
  printf("Poor Student needs $%d\n", balance);



	if (balance <= account){
		account = account - balance;
    printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
	}
	else
		printf("Poor Student: Not Enough Cash ($%d)\n", account);

	ShmPTR[1] = account;

	ShmPTR[0] = 0;
  
}

void  ParentProcess(int  ShmPTR[])
{
	int account = 0;  // this is a copy of accountBalance (the shared value)
	int balance = 0; //  this is the random amount the parent wants
	int asleep = 0; // this is the random amount of sleep time
  
	asleep = rand() % 5;

	// sleep 0 -5 s
	sleep(asleep);
 
  while(ShmPTR[0] != 0);
  
  // upon waking
	account = ShmPTR[1];
  
  
  if (account <= 100){
    balance = rand() % 100;
    // deposit 
		if (balance % 2 == 0){
			account += balance;
			printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
		}else{
			printf("Dear old Dad: Doesn't have any money to give\n");
		} 
    ShmPTR[1] = account;
  
  }else{
		printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
	}
      
  
  // set turn back to 0 since we're done
	ShmPTR[0] = 1;
}