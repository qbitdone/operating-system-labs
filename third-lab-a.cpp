#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>

using namespace std;

struct dijeljenaMemorija 
{
        long long int rnd_broj; 
        sem_t generiraj, racunaj;
};

dijeljenaMemorija *var;

int segId;

void generiraj(int y) 
{
        for(int i=0;i<y;i++)
		{
                var->rnd_broj=rand()%1000000000;
                cout<<"Generiran broj "<<var->rnd_broj<<endl;
                sem_post(&var->generiraj);
                sem_wait(&var->racunaj);
        }
        var->rnd_broj=0;
}

void racunaj(int i)
{
    do {
        if(var->rnd_broj==0) break;
        sem_wait(&var->generiraj);
        long long int broj;
        long long int suma = 0;
        broj=var->rnd_broj;
        cout<<"Proces "<<i<<". preuzeo zadatak "<<broj<<endl;
        sem_post(&var->racunaj);
        for(long long int j=0;j<broj;j++)   suma = suma + j;
        cout<<"Proces "<<i<<". zadatak = "<<broj<<" zbroj= "<<suma<<endl;
        } while(true);
}

void brisi(int sig) 
{
        shmdt(var);
        shmctl(segId, IPC_RMID, NULL);
        sem_destroy(&var->generiraj);
        sem_destroy(&var->racunaj);
        exit(0);
}

int main(int argc, char *argv[]) 
{
        if(argc != 3) 
		{
            cout << "Unijeli ste " << argc-1 << " argumenata, a ovaj program prima 2" << endl;
            exit(-1);
        }
            
        sigset(SIGINT, brisi);
        srand(time(NULL));
        segId=shmget(IPC_PRIVATE, sizeof(struct dijeljenaMemorija), 0600);
        var = (dijeljenaMemorija*) shmat(segId, NULL, 0);

        int x=atoi(argv[1]); 
        int y=atoi(argv[2]); 

        sem_init(&var->generiraj,1,0);
        sem_init(&var->racunaj,1,0);

        if(fork() == 0) 
		{
            cout<<"Proces koji generira zadatke poceo je s radom. Broj zadataka = "<<y<<endl;
            generiraj(y);
            exit(0);
        }
            
        int pom=0;
        for(int i=0;i<=x;i++) 
		{
                if(fork()==0) 
				{
                    racunaj(i);
                    exit(0);
                }
                pom++;
        }
        do {
            wait(NULL);
            pom--;
        } while(pom+1!=0);
        
		brisi(0);
        return 0;
}
