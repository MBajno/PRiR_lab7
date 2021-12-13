#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "mpi.h"
#define REZERWA 500
#define DWORZEC 1
#define WYJAZD 2
#define JAZDA 3
#define KONIEC_TRASY 4
#define WYPADEK 5
#define TANKUJ 5000


int STOP = 1, KONTYNUACJA = 0;
int liczba_pr;
int nr_pr;
int ilosc_autobusow;
int ilosc_tras = 4;
int ilosc_zajetych_tras = 0;
int paliwo = 5000;
int tag = 1;
int wyslij[2];
int odbierz[2];
MPI_Status mpi_status;

void Wyslij(int nr_autobusu, int stan)
{
  wyslij[0] = nr_autobusu;
  wyslij[1] = stan;
  MPI_Send(&wyslij, 2, MPI_INT, 0, tag, MPI_COMM_WORLD);
  sleep(1);
}

void Dworzec(int liczba_pr){
  int nr_autobusu, status;
  ilosc_autobusow = liczba_pr - 1;
  printf("Witam na dworcu \n");
  if(rand()%2 == 0){
    printf("Warunki na trasach sa w wysmienitym stanie\n");
  }
  else{
   printf("W obecnych warunkach moga wystapic utrudnienia na trasach\n");
  }
  printf("Dysponujemy %d trasami\n", ilosc_tras);
  sleep(2);

  while(ilosc_tras <= ilosc_autobusow){
    MPI_Recv(&odbierz, 2, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &mpi_status);
    nr_autobusu = odbierz[0];
    status = odbierz[1];
    if(status == 1){
      printf("Autobus nr %d stoi na zajezdni\n", nr_autobusu);
    }
    if(status == 2){
      printf("Autobus nr %d pozwolenie na wyjazd na trase nr %d\n", nr_autobusu, ilosc_zajetych_tras);
      ilosc_zajetych_tras--;
    }
    if(status == 3){
      printf("Autobus nr %d jedzie\n", nr_autobusu);
    }
    if(status == 4){
       if(ilosc_zajetych_tras < ilosc_tras){
          ilosc_zajetych_tras++;
          MPI_Send(&STOP, 1, MPI_INT, nr_autobusu, tag, MPI_COMM_WORLD);
        }
    else{
      MPI_Send(&KONTYNUACJA, 1, MPI_INT, nr_autobusu, tag, MPI_COMM_WORLD);
        }
      }
    if(status == 5){
      ilosc_autobusow--;
      printf("Ilosc autobusow %d\n", ilosc_autobusow);
      }
    }
   printf("Program zakonczyl dzialanie:)\n");
  }
void Autobus(){
  int stan, suma, i;
  stan = JAZDA;
  while(1){
    if(stan == 1){
      if(rand()%2 == 1){
        stan = WYJAZD;
        paliwo = TANKUJ;
        printf("Prosze o pozwolenie na wyjazd, autobus %d\n",nr_pr);
        Wyslij(nr_pr, stan);
    }
    else{
      Wyslij(nr_pr, stan);
    }
  }
    else if(stan == 2){
        printf("Wyjechalem, autobus %d\n",nr_pr);
        stan = JAZDA;
        Wyslij(nr_pr, stan);
    }
    else if(stan == 3){
      paliwo -= rand()%500;
      if(paliwo <= REZERWA){
        stan = KONIEC_TRASY;
        printf("prosze o pozwolenie na powrot do zajezdni\n");
        Wyslij(nr_pr, stan);
    }
    else{
      for(i=0; rand()%10000; i++);
      }
    }
    else if(stan == 4){
    int temp;
    MPI_Recv(&temp, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &mpi_status);
    if(temp == STOP){
      stan = DWORZEC;
      printf("Wrocilem, autobus %d\n", nr_pr);
    }
    else
    {
      paliwo -= rand()%500;
    if(paliwo>0){
    Wyslij(nr_pr, stan);
    }
    else{
    stan = WYPADEK;
    printf("Wypadek\n");
    Wyslij(nr_pr, stan);
    return;
    }
  }
 }
}
}
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &nr_pr);
    MPI_Comm_size(MPI_COMM_WORLD, &liczba_pr);
    srand(time(NULL));
    if(nr_pr == 0){
        Dworzec(liczba_pr);
    }
    else{
        Autobus();
    }
    MPI_Finalize();
    return 0;
}
