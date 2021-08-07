#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>

int key;
int *a = new int[10000];

void create (int n, int m, int i)
{
    time_t vr;
    time(&vr);
    srand(vr+i);
    rand();
    usleep(100);

     for(int j = m*i; j<m *(i+1) && j<n; j++)
     {
         a[j] = rand()% 100;
         printf("%d. %d\n",j,a[j]);
     }

}

void calculate (int n, int m, int i)
{
     int sum = 0;
     
     for(int j = m*i; j<m *(i+1) && j<n; j++)
         sum = sum + a[j];
         
     sleep(1);
     int kraj = m*(i+1) - 1;
     printf("Zbroj (%d-%d) = %d\n",m*i,kraj>(n-1) ? n-1 : kraj,sum);

}
 
 
void res (int signal)
{
     shmdt(a);
     shmctl(key, IPC_RMID, NULL);
     exit(0);
}


int main (int arg_count, char *argv[])
{
 	
     if(arg_count != 3 || atoi(argv[1]) < 1)
     {
         printf("parametri krivi\n");
         exit(1);
     }
 
      key = shmget(IPC_PRIVATE, sizeof(int), 0600);

      if(key == -1)
      {
         printf("memorija kriva\n");
         exit(1);
      }
 
      a = (int *) shmat(key, NULL, 0);
      sigset(SIGINT, res);
      
      for(int i=0; i<=atoi(argv[2]); i++)
     {
          switch(fork())
         {
              case -1:
                  printf("fork krivi\n");
                  exit(1);
              case 0:
                  create(atoi(argv[1]), atoi(argv[2]), i);
                  calculate(atoi(argv[1]), atoi(argv[2]), i);
                  exit(0);
              default:
                  break;
          }
      }
      
      for(int i=0; i<atoi(argv[2]); i++)
          wait(NULL);
      
      
	  res(0);
	  wait(NULL);
      return 0;
}

