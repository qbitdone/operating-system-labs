#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

int N;
int M;
int shrsegId;

struct dijeljena {
pthread_mutex_t monitor;
pthread_mutexattr_t monitorski;
pthread_cond_t uvjet[2];
pthread_condattr_t uvjetni;
int uRestoranu;
int uRestoranu_gotovo;
int brProgramera[2];
int vrstaUR = -1;
} *dat;

void ispis(int ulazni, int smjer) {
	
	cout << "Red Linux:";
	for(int i=0; i<M; i++)
		if(i<dat->brProgramera[0])
			cout << "L";
		else
			cout << "-";
	
	cout << " Red Microsoft:";
	for(int i=0; i<M; i++)
		if(i<dat->brProgramera[1])
			cout << "M";
		else
			cout << "-";
	
	cout << " Restoran:";
	if(dat->vrstaUR==0)
		for(int i=0; i<dat->uRestoranu; i++)
			cout << "L";
    if(dat->vrstaUR==1)
		for(int i=0; i<dat->uRestoranu; i++)
			cout << "M";
	
	cout << " --> ";
    if(ulazni==0) {
        if(smjer==0)
            cout << "L iz restorana";
        if(smjer==1)
            cout << "L u restoran";
        if(smjer==2)
            cout << "L u uvjet cekanja";
        }
	else {
        if(smjer==0)
            cout << "M iz restorana";
        if(smjer==1)
            cout << "M u restoran";
        if(smjer==2)
            cout << "M u uvjet cekanja";
        }
	cout << endl;
}

void udji(int vrsta) {
	pthread_mutex_lock(&dat->monitor);
	dat->brProgramera[vrsta]++;
	while(dat->vrstaUR == 1-vrsta || dat->uRestoranu_gotovo >= N) {
		ispis(vrsta, 2);
		pthread_cond_wait(&dat->uvjet[vrsta], &dat->monitor);
	}
	dat->brProgramera[vrsta]--;
	dat->vrstaUR = vrsta;
	dat->uRestoranu++;
	if(dat->brProgramera[1-vrsta] > 0)
		dat->uRestoranu_gotovo++;
	ispis(vrsta, 1);
	pthread_mutex_unlock(&dat->monitor);
}


void izadji(int vrsta) {
	pthread_mutex_lock(&dat->monitor);
	dat->uRestoranu--;
	if(dat->uRestoranu==0) {
		if(dat->brProgramera[1-vrsta] > 0) {
			dat->vrstaUR = 1-vrsta;
			pthread_cond_broadcast(&dat->uvjet[1-vrsta]);
		} else {
			dat->vrstaUR = -1;
		}
		dat->uRestoranu_gotovo = 0;
	}
	ispis(vrsta, 0);
	pthread_mutex_unlock(&dat->monitor);
}

void jedi() {
	usleep(rand()%100000);
}

void programiraj() {
	usleep(rand()%100000);
}

void programeri(int vrsta) {
    programiraj();
	udji(vrsta);
    jedi();
	izadji(vrsta);
}

void pocetak() {
    shrsegId = shmget(IPC_PRIVATE, sizeof(dijeljena), 0600);
    dat = (dijeljena *) shmat(shrsegId, NULL, 0);
    pthread_mutexattr_init(&dat->monitorski);
    pthread_mutexattr_setpshared(&dat->monitorski, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&dat->monitor, &dat->monitorski);
    pthread_condattr_init(&dat->uvjetni);
    pthread_condattr_setpshared(&dat->uvjetni, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&dat->uvjet[0], &dat->uvjetni);
    pthread_cond_init(&dat->uvjet[1], &dat->uvjetni);
}


void kraj() {
    pthread_mutexattr_destroy(&dat->monitorski);
	pthread_mutex_destroy(&dat->monitor);
	pthread_condattr_destroy(&dat->uvjetni);
	pthread_cond_destroy(&dat->uvjet[0]);
	pthread_cond_destroy(&dat->uvjet[1]);
	shmdt(dat);
    shmctl(shrsegId, IPC_RMID, NULL);
}

int main(int argc, char** argv) {
	if(argc<3) {
		cout << "Netocan broj argumenata!" << endl;
		exit(0);
	}
	
	N = atoi(argv[1]);
	M = atoi(argv[2]);
	
    pocetak();
	
	for(int i=0; i<M*2; i++) {
        if(fork() == 0) {
            programeri(i%2);
            exit(0);
            }
        }
	
	for(int i=M*2; i>0; i--)
	    wait(NULL);
	
    kraj();
	return 0;
}
