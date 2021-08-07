#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

using namespace std;

#define SEM_PRAZAN 0
#define SEM_PUN 1
#define SEM_PISI 2

int id_podatka; //id segmenta

struct spremnik 
{
	int ULAZ;
	int IZLAZ;
	int UKUPNO;
	char M[5];
	int N;
};

spremnik *podatci;

int SemId;

void SemGet(int n) {  /* dobavi skup semafora sa ukupno n semafora */
   SemId = semget(IPC_PRIVATE, n, 0600);
   if (SemId == -1) {
      printf("Nema semafora!\n");
      exit(1);
   }
}

int SemSetVal(int SemNum, int SemVal) {  /* postavi vrijednost semafora SemNum na SemVal */
   return semctl(SemId, SemNum, SETVAL, SemVal);
}

int SemOp(int SemNum, int SemOp) {  /* obavi operaciju SemOp sa semaforom SemNum */
   struct sembuf SemBuf;

   SemBuf.sem_num = SemNum;
   SemBuf.sem_op  = SemOp;
   SemBuf.sem_flg = 0;
   return semop(SemId, & SemBuf, 1);
}

void SemRemove() {  /* uništi skup semafora */
   semctl(SemId, 0, IPC_RMID, 0);
}

int proizvodjac (int id)
{
	for (int i = 0; i<podatci->N; i++)
	{
		
		SemOp(SEM_PUN, -1);
		SemOp(SEM_PISI, -1);
		podatci->M[podatci->ULAZ] = rand() %1000;
		cout << "Proizvoðaè broj " << id << " šalje " << podatci->M[podatci->ULAZ] << endl;
		podatci->ULAZ = (podatci->ULAZ + 1) % 5;
		SemOp(SEM_PISI, 1);
		SemOp(SEM_PRAZAN, 1);
		i = i + 1;
		cout << "Proizvoðaè s brojem " << id << " je završio!" << endl;
		sleep(1);
	}
}

int potrosac()
{
	int zbroj = 0;
	for (int i = 0; i < podatci->UKUPNO; i++)
	{
		SemOp(SEM_PRAZAN, -1);
		cout << "Potrosac prima " << podatci->M[podatci->IZLAZ] << endl;
		zbroj = zbroj + podatci->M[podatci->IZLAZ];
		podatci->IZLAZ = (podatci->IZLAZ + 1) % 5;
		SemOp(SEM_PUN, 1);
		cout << "Potrosac - zbroj primljenih brojeva " << zbroj << endl;
		sleep(1);
	}	
	
}

void izlaz() 
{
    shmctl(id_podatka, IPC_RMID, NULL);
    SemRemove();
}

int main (int arg_count, char *argv[])
{
 	
     if(arg_count != 3 || atoi(argv[1]) < 1)
     {
         printf("parametri krivi\n");
         exit(1);
     }
     
	id_podatka = shmget(IPC_PRIVATE, sizeof(struct spremnik), 0600);
	if (id_podatka == -1)
	{
		cout << "Memorija kriva" << endl;
		exit(1);	
	}    
    
    podatci = (struct spremnik *) shmat(id_podatka, NULL, 0);
    int broj_procesa = atoi(argv[1]);
    podatci->N = atoi(argv[2]); //broj slucajnih brojeva
    int zbroj_svih = podatci->N * broj_procesa;
    podatci->UKUPNO = zbroj_svih;
    
	SemGet(3);
    SemSetVal(SEM_PISI, 1);
    SemSetVal(SEM_PRAZAN, 0);
    SemSetVal(SEM_PUN, 5); 
    srand(time(NULL));
    
    for (int i = 1; i<broj_procesa+1; i++)
    {
    	switch(fork())
         {
              case -1:
                  printf("fork krivi\n");
                  exit(1); 
                  break;
              case 0:
                  proizvodjac(i);
                  exit(0);
                  break;
              default:
                  break;
          }
	}
     
    //potrosac
    potrosac();
    for(int i = 0; i < broj_procesa; ++i)
        wait(NULL);
     
     izlaz();
     
     return 0;
     
     
}


