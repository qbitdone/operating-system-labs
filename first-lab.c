#include <stdio.h>
#include <signal.h>
  #include <time.h>
   #include <unistd.h>
 #define N 3    /* broj razina prekida */
  
   int PRIORITET[N];
   int TEKUCI_PRIORITET;
  
  void obrada_prekida(int j)
  {
 
     printf ("Poceo obradu prekida %d\n", j);
     /* obrada se simulira trošenjem vremena - 10 s */
     int brojac_pomocni = 0;
    while(brojac_pomocni<10)
  {
     sleep(1);
     brojac_pomocni++;
     printf("Prekid razine %d: %d/10\n",j,brojac_pomocni);
  }
 
     printf ("Zavrsio obradu prekida %d\n", j);
  }
 
  void prekidna_rutina (int sig)
  {
     int i;
     time_t t;
 
     time(&t);
     printf("Prekidna rutina pozvana u: %s", ctime(&t));
 
     switch (sig) {
     case SIGINT:
        i = 1;
     obrada_prekida(i);
     //sighold(SIGINT);
     break;
     case SIGQUIT:
     	    i = 2;
     sigignore(SIGINT);
     obrada_prekida(i);
        break;
     case SIGTSTP:
        i = 3;
     sigignore(SIGQUIT);
     sigignore(SIGINT);
     obrada_prekida(i);
        break;
     default:
        return;
     }
     printf ("Razina prekida: %d\n", i);
 
  }
 
  int main (void)
  {
 
     printf ("Poceo osnovni program\n");
     /* troši vrijeme da se ima šta prekinuti - 10 s */
   int brojac = 0;
    while(brojac<10)
  {
     sigset (SIGINT, prekidna_rutina);
     sigset (SIGQUIT, prekidna_rutina);
     sigset (SIGTSTP, prekidna_rutina);
 
     sleep(1);
    brojac++;
    printf("Glavni program: %d/10\n",brojac);
  }
     printf ("Zavrsio osnovni program\n");
 
     return 0;
  }


