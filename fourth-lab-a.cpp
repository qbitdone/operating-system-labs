#include<iostream>
#include<cstdlib>
#include<pthread.h>

using namespace std;

struct podaci {
    int brZapisa, brMonitora;
    pthread_mutex_t monitor;
    pthread_cond_t cond;
} pod;

void *dretva(void *argument){
        pthread_mutex_lock(&pod.monitor);
        int n = *(int*)argument;
        int unos;
        pod.brZapisa++;
        cout<<"Dretva "<<n<<". unosi broj"<<endl;
        cin>>unos;
        if (pod.brZapisa == pod.brMonitora) {
                    pthread_cond_broadcast(&pod.cond);
        } 
        else {
            pthread_cond_wait(&pod.cond,&pod.monitor);
        }
        cout<<"Dretva "<<n<<". je unijela broj "<<unos<<endl;
        pthread_mutex_unlock(&pod.monitor);
}

int brisi() {
        pthread_mutex_destroy(&pod.monitor);
        pthread_cond_destroy(&pod.cond);
}

int main(int argc, char *argv[]){
        pod.brMonitora = atoi(argv[1]);
        pthread_mutex_init(&pod.monitor, NULL);
        pthread_cond_init(&pod.cond, NULL);
        pthread_t dretve[pod.brMonitora];
        int dretArg[pod.brMonitora];
        for (int i=0; i<pod.brMonitora; i++) {
            dretArg[i] = i;
        }
        int i=-1;
        while(i<pod.brMonitora-1) {
            i++;
            pthread_create(&dretve[i], NULL, dretva, &dretArg[i]);
        }
        while(i>-1) {
            pthread_join(dretve[i], NULL);
            i--;
        }
        brisi();
        return 0;
}
