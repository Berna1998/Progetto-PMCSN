#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rngs.h"                      /* the multi-stream generator     */
#include "rvgs.h"
#include "rvms.h"
#include "struct.h"
#include "values.h"
#include "infiniteSimulation.h"
#include "finiteSimulation.h"


char *statsNames[7] = {"Interarrival Time avg","Wait avg","Delay avg","Service Time avg","Num Node avg","Num Queue avg","Utilization avg"};

void writeOnFile(double interval[7*5][5], char *fileName){
  FILE *fp;
  fp = fopen(fileName,"w");
  
  for(int i=0; i<7*5; i++){
        int station = interval[i][0];
        double w = interval[i][1];
        double mean = interval[i][2];
        if(station == 0){
          fprintf(fp, "%s,%s,%.3f,%.3f\n", "Biglietteria",statsNames[i%7], mean, w);   
        }else if (station==1){
           fprintf(fp, "%s,%s,%.3f,%.3f\n", "Coda Veloce Jumanji",statsNames[i%7], mean, w); 
        }else if(station == 2){
           fprintf(fp, "%s,%s,%.3f,%.3f\n", "Coda Lenta Jumanji",statsNames[i%7], mean, w); 
        }else if(station == 3){
           fprintf(fp, "%s,%s,%.3f,%.3f\n", "Coda Veloce Atlantide",statsNames[i%7], mean, w); 
        }else{            
           fprintf(fp, "%s,%s,%.3f,%.3f\n", "Coda Lenta Atlantide",statsNames[i%7], mean, w);        
        }
  }
  
  fclose(fp);  
  
}

void writeAfter(outputValues results [K][5], int typeSim){

  long nTot[7]; /* counts data points */
  double sum[7];
  double mean[7];
  double data[7];
  double stdev[7];
  double u[7], t[7], w[7];
  double diff[7];
  int batch;
  int values;
  double intervalStats[7*5][5];
  int j;
  char *string;
  if(typeSim == 1){
    string = "statsInfinite.csv";
  }else{
    string = "statsFinite.csv";
  }
  
  for(j=0;j<7;j++){
    nTot[j] = 0;
    sum[j] = 0.0;
    mean[j] = 0.0;
  }
  int offset = 0;
  for(j=0;j<5;j++){
    for(batch=0;batch<K;batch++){
      data[0] = results[batch][j].interarrival;
      data[1] = results[batch][j].wait;
      data[2] = results[batch][j].delay;
      data[3] = results[batch][j].service;
      data[4] = results[batch][j].Ns;
      data[5] = results[batch][j].Nq;
      data[6] = results[batch][j].utilization;
      values = 0;
      while (values<7) { /* use Welford's one-pass method */
        /*metti condizione whle, roba da mmettere prima del while, i vari dati d< prende cioè*/
        nTot[values]++; /* and standard deviation */
        diff[values] = data[values] - mean[values];
        sum[values] += diff[values] * diff[values] * (nTot[values] - 1.0) / nTot[values];
        mean[values] += diff[values] / nTot[values];

        values++;
      }
    }
    
    for(values = 0;values<7;values++){
      stdev[values] = sqrt(sum[values] / nTot[values]);
      /*Vedi pure qua cosa tocca cambia*/
      if (nTot[values] > 1) {
        u[values] = 1.0 - 0.5 * (1.0 - LOC); /* interval parameter */
        t[values] = idfStudent(nTot[values] - 1, u[values]); /* critical value of t */
        w[values] = t[values] * stdev[values] / sqrt(nTot[values] - 1); /* interval half width */
        printf("\nbased upon %ld data points", nTot[values]);
        printf(" and with %d%% confidence\n", (int) (100.0 * LOC + 0.5));
        printf("the expected value is in the interval");
        printf("%10.2f +/- %6.2f\n", mean[values], w[values]); 
       
        /*Salva in una matrice valori dell'intervallo*/
        intervalStats[j*4+values+offset][0] = j;
        intervalStats[j*4+values+offset][1] = w[values];
        intervalStats[j*4+values+offset][2] = mean[values];
      }
    }
    
    if(j==0){
        printf("Valori Biglietteria\n");
        printf("-----------------------------------------\n");
        printf("Interarrival   |           %10.2f +/- %6.2f\n",intervalStats[j*4+offset][2], intervalStats[j*4+offset][1]);
        printf("Wait           |           %10.2f +/- %6.2f\n",intervalStats[j*4+1+offset][2], intervalStats[j*4+1+offset][1]);
        printf("Delay          |           %10.2f +/- %6.2f\n",intervalStats[j*4+2+offset][2], intervalStats[j*4+2+offset][1]);
        printf("Service        |           %10.2f +/- %6.2f\n",intervalStats[j*4+3+offset][2], intervalStats[j*4+3+offset][1]); 
        printf("Num Node       |           %10.2f +/- %6.2f\n",intervalStats[j*4+4+offset][2], intervalStats[j*4+4+offset][1]); 
        printf("Num Queue      |           %10.2f +/- %6.2f\n",intervalStats[j*4+5+offset][2], intervalStats[j*4+5+offset][1]); 
        printf("Utilization    |           %10.2f +/- %6.2f\n",intervalStats[j*4+6+offset][2], intervalStats[j*4+6+offset][1]);                                                  
    }else if(j==1){
        printf("Valori Coda Veloce Jumanji\n");
        printf("-----------------------------------------\n");
        printf("Interarrival   |           %10.2f +/- %6.2f\n",intervalStats[j*4+offset][2], intervalStats[j*4+offset][1]);
        printf("Wait           |           %10.2f +/- %6.2f\n",intervalStats[j*4+1+offset][2], intervalStats[j*4+1+offset][1]);
        printf("Delay          |           %10.2f +/- %6.2f\n",intervalStats[j*4+2+offset][2], intervalStats[j*4+2+offset][1]);
        printf("Service        |           %10.2f +/- %6.2f\n",intervalStats[j*4+3+offset][2], intervalStats[j*4+3+offset][1]); 
        printf("Num Node       |           %10.2f +/- %6.2f\n",intervalStats[j*4+4+offset][2], intervalStats[j*4+4+offset][1]); 
        printf("Num Queue      |           %10.2f +/- %6.2f\n",intervalStats[j*4+5+offset][2], intervalStats[j*4+5+offset][1]); 
        printf("Utilization    |           %10.2f +/- %6.2f\n",intervalStats[j*4+6+offset][2], intervalStats[j*4+6+offset][1]);           
    }else if(j==2){
        printf("Valori Coda Lenta Jumanji\n");
        printf("-----------------------------------------\n");
        printf("Interarrival   |           %10.2f +/- %6.2f\n",intervalStats[j*4+offset][2], intervalStats[j*4+offset][1]);
        printf("Wait           |           %10.2f +/- %6.2f\n",intervalStats[j*4+1+offset][2], intervalStats[j*4+1+offset][1]);
        printf("Delay          |           %10.2f +/- %6.2f\n",intervalStats[j*4+2+offset][2], intervalStats[j*4+2+offset][1]);
        printf("Service        |           %10.2f +/- %6.2f\n",intervalStats[j*4+3+offset][2], intervalStats[j*4+3+offset][1]); 
        printf("Num Node       |           %10.2f +/- %6.2f\n",intervalStats[j*4+4+offset][2], intervalStats[j*4+4+offset][1]); 
        printf("Num Queue      |           %10.2f +/- %6.2f\n",intervalStats[j*4+5+offset][2], intervalStats[j*4+5+offset][1]); 
        printf("Utilization    |           %10.2f +/- %6.2f\n",intervalStats[j*4+6+offset][2], intervalStats[j*4+6+offset][1]);          
        
    }else if(j==3){
        printf("Valori Coda Veloce Atlantide\n");
        printf("-----------------------------------------\n");
        printf("Interarrival   |           %10.2f +/- %6.2f\n",intervalStats[j*4+offset][2], intervalStats[j*4+offset][1]);
        printf("Wait           |           %10.2f +/- %6.2f\n",intervalStats[j*4+1+offset][2], intervalStats[j*4+1+offset][1]);
        printf("Delay          |           %10.2f +/- %6.2f\n",intervalStats[j*4+2+offset][2], intervalStats[j*4+2+offset][1]);
        printf("Service        |           %10.2f +/- %6.2f\n",intervalStats[j*4+3+offset][2], intervalStats[j*4+3+offset][1]); 
        printf("Num Node       |           %10.2f +/- %6.2f\n",intervalStats[j*4+4+offset][2], intervalStats[j*4+4+offset][1]); 
        printf("Num Queue      |           %10.2f +/- %6.2f\n",intervalStats[j*4+5+offset][2], intervalStats[j*4+5+offset][1]); 
        printf("Utilization    |           %10.2f +/- %6.2f\n",intervalStats[j*4+6+offset][2], intervalStats[j*4+6+offset][1]);          
           
    }else{
        printf("Valori Coda Lenta Atlantide\n");
        printf("-----------------------------------------\n");
        printf("Interarrival   |           %10.2f +/- %6.2f\n",intervalStats[j*4+offset][2], intervalStats[j*4+offset][1]);
        printf("Wait           |           %10.2f +/- %6.2f\n",intervalStats[j*4+1+offset][2], intervalStats[j*4+1+offset][1]);
        printf("Delay          |           %10.2f +/- %6.2f\n",intervalStats[j*4+2+offset][2], intervalStats[j*4+2+offset][1]);
        printf("Service        |           %10.2f +/- %6.2f\n",intervalStats[j*4+3+offset][2], intervalStats[j*4+3+offset][1]); 
        printf("Num Node       |           %10.2f +/- %6.2f\n",intervalStats[j*4+4+offset][2], intervalStats[j*4+4+offset][1]); 
        printf("Num Queue      |           %10.2f +/- %6.2f\n",intervalStats[j*4+5+offset][2], intervalStats[j*4+5+offset][1]); 
        printf("Utilization    |           %10.2f +/- %6.2f\n",intervalStats[j*4+6+offset][2], intervalStats[j*4+6+offset][1]);       
    }
         
    offset+=3;
    
    /*Funzione scrittura valori in csv*/    
    writeOnFile(intervalStats, string);
    
    /*Ri-azzero*/
    
    for(values=0;values<7;values++){
      nTot[values] = 0;
      sum[values] = 0.0;
      mean[values] = 0.0;
    }
   
  }
  
}

int main(){
  int sceltaSim;
  int orario;
  outputValues results [K][5];
  
ret1:  
  printf("Scegli la fascia oraria, 1 per la fascia 16-19 e 2 per la fascia 10-13: ");  
  scanf("%d", &orario); 
  if(orario == 1){
    printf("Hai scelto 16-19\n");
  }else if (orario == 2){
    printf("Hai scelto 10-13\n");
  }else{
    printf("Inserisci un numero valido\n\n");
    goto ret1;
  }
  
  PlantSeeds(12345);
ret2: 
  printf("Scegli il tipo di simulazione, 1 per lo stato stazionario e 2 per il transiente: ");
  scanf("%d",&sceltaSim);

  if(sceltaSim == 1){
    infiniteHorizon(results,orario);
    writeAfter(results,1);
  }else if (sceltaSim == 2){
    for (int i=0;i<replicheNum;i++){
      finiteHorizon(results[i], i,orario);
    }
    writeAfter(results, 2);
  }else{
    printf("inserisci un numero valido\n\n");
    goto ret2;
  }
  
  return 0;
}
