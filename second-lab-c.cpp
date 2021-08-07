#include <iostream>
#include <thread>
#include <unistd.h>

using namespace std;

int pravo;
bool zastavica[2];

void udi_u_KO(int i, int j)
{
         zastavica[i] = true;
         while(zastavica[j] == true)
         {
                 if(pravo == j)
                 {
                         zastavica[i] = false;
                         while(pravo == j);
                 }
                 zastavica[i] = true;
         }
}

void izadi_iz_KO(int i, int j)
{
         pravo = j;
         zastavica[i] = false;
         sleep(1);
}

void *dretva(void  *i)
{
         int ip = *((int*)i);
         for(int k = 1; k <= 5; ++k)
         {
                udi_u_KO(ip, 1 - ip);
                
                for(int m = 1; m <= 5; ++m)
                         cout << "Dretva: " << ip + 1 << ", K.O. br: " << k << " (" << m << "/5)" << endl;
                
				izadi_iz_KO(ip, 1 - ip);
         }
}
int main()
{
         int i = 0;
         int j = 1;
         pthread_t id, id2;
         pthread_create(&id,NULL,dretva,&i);
         pthread_create(&id2,NULL,dretva,&j);
         pthread_join(id,NULL);
         pthread_join(id2,NULL);
         pthread_kill(id,SIGINT);
         return 0;
}

