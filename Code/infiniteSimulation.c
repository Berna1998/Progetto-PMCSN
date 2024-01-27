#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rngs.h"                      /* the multi-stream generator     */
#include "rvgs.h"
#include "rvms.h"
#include "struct.h"
#include "functions.h"
#include "values.h"
#include "infiniteSimulation.h"


int contatoree = 0;


static double getArrival(){
  SelectStream(0);
  arrival+=Exponential(1/lambda);
  return arrival;
}

static double getService(int stream, int type){ 
  SelectStream(stream);
  double service;
  if(type == 1){
    service = Exponential(SERVICEB);
  }else if(type == 2){
    service = Exponential(SERVICEJ);
  }else{
    service = Exponential(SERVICEA);
  }
  return service;
}

static void updateIntegrals(station *station, multiserver multiServ[], double next, double current){
  if(station->number > 0){
    station->node += (next - current)* station->number;

    if(next != current){
          if(station->number > station->servers){
            station->queue   += (next - current) * (station->number - station->servers);
          }
          
          if(station->servers == 1){

            station->service += (next - current);
          }else if(station->servers > 1){
            for(int j=0; j< (station->servers); j++){
              if(multiServ[j].occupied==1){
                station->service += (next - current);
              }
            }
          }
        } 
             
  }
}


void saveBatchesStats(station *cent, int areaNum, outputValues results[K][5]){ 
  outputValues output;

  output.interarrival = (cent->lastArrival - cent->firstArrival) / cent->index;
  output.wait = cent->node / cent->index;
  output.delay = cent->queue / cent->index;
  output.service = (cent->service/cent->servers) / cent->index;
  output.Ns = cent->node/(cent->lastService - cent->firstArrival);
  output.Nq = cent->queue / (cent->lastService - cent->firstArrival); 
  output.utilization = cent->service / (cent->servers*(cent->lastService - cent->firstArrival));

  if(areaNum < 3){
      results[areaBatches[areaNum]][areaNum-1] = output;      
      areaBatches[areaNum]++;   
  }else if (areaNum == 3){
      results[areaBatches[areaNum-1]-1][areaNum-1] = output;
  }else if (areaNum == 4){
      results[areaBatches[areaNum-1]][areaNum-1] = output;
      areaBatches[areaNum-1]++;    
  }else{
      results[areaBatches[areaNum-2]-1][areaNum-1] = output;  
  }
  
}

int factorial(int n){
  int i=1;
  int fattoriale = 1;
  if(n!=0 && n!=1){
     
    while(i < n){
      fattoriale=fattoriale*i;
      i++;
    }   
  }
  
  return fattoriale;
}

double sommation(int n, double ro){
  int i;
  int sum = 0;
  
  for(i=0;i<n;i++){
    sum+=(pow((n*ro),i))/factorial(i);
  }
  return sum;

}

double theoricalDelay(station *cent, int typeOfServer){
  double delay =0.0;
  double service, ro;
  double lambdaT;
  if(typeOfServer == 1){
    
    lambdaT = lambda *0.65;
  
    /* E(s) = E(si)/serversNum    */
    service = SERVICEB/serversNum;
  
    /* ro = lambda * E(s) */
    ro = lambdaT*service;
    
    double pZero = 1 / (  ((pow((serversNum*ro),serversNum)) / ((1-ro)*(factorial(serversNum))) ) + sommation(serversNum, ro));
    double pQ = pZero * ((pow((serversNum*ro),serversNum)) / (factorial(serversNum)*(1-ro)));

    // E(Tq) = Pq*E(s) / (1-ro)
    delay = (pQ * service) / (1-ro);

  }else if(typeOfServer == 2){

    lambdaT = lambda * 0.1*0.6;
    service = SERVICEJ;

    /* ro = lambda * E(s) */
    ro = lambdaT*service;

    // E(Tq) = ro*E(s) / (1-ro)
    delay = (ro*service)/(1-ro);
        
  }else if(typeOfServer == 3){
    lambdaT = lambda * 0.9*0.6;
    service = SERVICEJ;

    /* ro = lambda * E(s) */
    ro = lambdaT*service;

    // E(Tq) = ro*E(s) / (1-ro)
    delay = (ro*service)/(1-ro);    
  
  }else if(typeOfServer == 4){
    lambdaT = lambda * 0.1*0.4;
    service = SERVICEA;

    /* ro = lambda * E(s) */
    ro = lambdaT*service;

    // E(Tq) = ro*E(s) / (1-ro)
    delay = (ro*service)/(1-ro);    

  }else{
    lambdaT = lambda * 0.9*0.4;
    service = SERVICEA;

    /* ro = lambda * E(s) */
    ro = lambdaT*service;

    // E(Tq) = ro*E(s) / (1-ro)
    delay = (ro*service)/(1-ro);    
  }
  
  return delay;
}

void theoricalWait(station *cent, int typeOfServer){
  double delay;
  double wait;
  double ro;
  double numNode;
  if(typeOfServer == 1){
    delay = theoricalDelay(cent,1);
    ro = lambda*0.65*SERVICEB/serversNum;
    numNode = lambda*0.65*delay+serversNum*ro;
    wait = delay + SERVICEB;
    printf("Valori teorici Biglietteria\n");
    printf("-----------------------------------------\n");
    printf("Wait           |           %f\n",wait);
    printf("Num Node       |           %f\n",numNode);
  }else if(typeOfServer == 2 || typeOfServer == 3){
    delay = theoricalDelay(cent,typeOfServer);
    wait = delay+ SERVICEJ;

    if(typeOfServer == 2){
      ro = lambda * 0.1 * 0.6 * SERVICEJ;
      numNode = lambda*0.1*0.6 * delay+ro;
      printf("Valori teorici Coda Veloce Jumanji\n");
      printf("-----------------------------------------\n");
      printf("Wait           |           %f\n",wait);
      printf("Num Node       |           %f\n",numNode); 
    }else{
      ro = lambda * 0.9 * 0.6 * SERVICEJ;
      numNode = lambda*0.9*0.6 * delay+ro;    
      printf("Valori teorici Coda Lenta Jumanji\n");
      printf("-----------------------------------------\n");
      printf("Wait           |           %f\n",wait);
      printf("Num Node       |           %f\n",numNode);    
    }
  }else{
    delay = theoricalDelay(cent,typeOfServer);
    wait = delay+ SERVICEA;

    if(typeOfServer == 4){
      ro = lambda * 0.1 * 0.4 * SERVICEA;
      numNode = lambda*0.1*0.4 * delay+ro;
      printf("Valori teorici Coda Veloce Atlantide\n");
      printf("-----------------------------------------\n");
      printf("Wait           |           %f\n",wait);
      printf("Num Node       |           %f\n",numNode); 
    }else{
      ro = lambda * 0.9 * 0.4 * SERVICEA;
      numNode = lambda*0.9* 0.4 * delay+ro;    
      printf("Valori teorici Coda Lenta Atlantide\n");
      printf("-----------------------------------------\n");
      printf("Wait           |           %f\n",wait);
      printf("Num Node       |           %f\n",numNode);    
    }  
  
  }

}


void infiniteHorizon(outputValues results[K][5], int orario){

  int count = 0;

  int biglietteriaCount = 0;
  int jumanjiFastCount = 0;
  int jumanjiSlowCount = 0;
  int atlantideSlowCount = 0;
  int atlantideFastCount = 0;
  arrival = 0.0;
  int i;
  if(orario == 1){
    lambda = LAMBDA1;
  }else {
    lambda = LAMBDA2;
  }

  
  /*Diverse inizializzazioni*/
  
  /*Inizializzo il contatore delle batches*/
  for(i=0;i<4;i++){
    areaBatches[i] = 0;
  }
  
  /*Inizializzo i multi server*/
  for(i=0; i<serversNum; i++){
    biglietteriaMultiServ[i].served = 0;
    biglietteriaMultiServ[i].occupied = 0;
    biglietteriaMultiServ[i].service = 0.0;
  }

 
  /*Inizializzo i centri*/
  resetStation(&park,0);
  park.number = 0.0;
  resetStation(&biglietteria,serversNum);
  biglietteria.number = 0.0;
  resetStation(&jumanjiFast,1);
  jumanjiFast.number = 0.0;
  resetStation(&jumanjiSlow,1);
  jumanjiSlow.number = 0.0;
  resetStation(&atlantideFast,1);
  atlantideFast.number = 0.0;
  resetStation(&atlantideSlow,1);
  atlantideSlow.number = 0.0;
      
  /*Inizializzo gli eventi*/
  event_type leaveBiglietteria[serversNum];
  
  for(i=0; i<serversNum; i++){
      leaveBiglietteria[i].status = 0;
      leaveBiglietteria[i].time = INFINITY;
  }

   
  for(i=0; i<10; i++){
      events[i].status = 0;
      events[i].time = INFINITY;
  }
  
  t.current = 0.0;   /* set the clock*/
  t.next = 0.0;
  
  events[0].time = getArrival();    //arriva il primo evento ed event[0] è arrivo nel sistema
  events[0].status = 1;
  park.firstArrival = events[0].time;
  
  int n = K*3;
  
  int nexEv;

 
  while (count < n ) {  //parte la stato stazionario 
    nexEv = getNextEvent(events);  //ottengo l'evento da gestire
    t.next = events[nexEv].time;          /* next event time   */
                                        /* update integrals  */
    updateIntegrals(&biglietteria,biglietteriaMultiServ,t.next,t.current);
    updateIntegrals(&jumanjiFast,gameServer,t.next,t.current);
    updateIntegrals(&jumanjiSlow,gameServer,t.next,t.current);
    updateIntegrals(&atlantideFast,gameServer,t.next,t.current);
    updateIntegrals(&atlantideSlow,gameServer,t.next,t.current);     
    updateIntegrals(&park,NULL,t.next,t.current);

    t.current = t.next;                    /* advance the clock */
    
    int typeEvent = nexEv;

    if(typeEvent == 0){
       if(getTypeOfPayment() == online){
         park.number++;
         park.lastArrival = t.current;
         double time = events[0].time;
         events[0].time = getArrival();      
         
         if(chooseGame() == 1){
           /*Fai Jumanji*/
           if(getTypeOfQueue() == FastQueue){
             events[2].time = time;
             events[2].status = 1;
       
             typeEvent = 2; 
           }else{
             events[3].time = time;
             events[3].status = 1;
             typeEvent = 3;         
           }
         }else{
           /*Fai Atlantide*/
           if(getTypeOfQueue() == FastQueue){
             events[6].time = time;
             events[6].status = 1;
       
             typeEvent = 6; 
           }else{
             events[7].time = time;
             events[7].status = 1;
             typeEvent = 7;         
           }           
         }

       }
    }
    
    if(typeEvent == 0){
         /*Gestisco un nuovo arrivo nel sistema che va in biglietteria*/
         park.number++;
         park.lastArrival = t.current;
         
         biglietteria.number++;
         biglietteriaCount++;
         biglietteria.lastArrival = t.current;
         if(t.current < biglietteria.firstArrival){
           biglietteria.firstArrival = t.current;
         }
         events[0].time = getArrival();

         
         /*Se ho arrivi minori dei serventi continuo a riempire*/
         if(biglietteria.number <= serversNum){
           
           double service = getService(1,1);
           int freeServ = getIdleServer(biglietteriaMultiServ);
           
           biglietteriaMultiServ[freeServ].occupied = 1;
           
           biglietteriaMultiServ[freeServ].service+=service;
           
           biglietteriaMultiServ[freeServ].served++;
           
           leaveBiglietteria[freeServ].status = 1;
           leaveBiglietteria[freeServ].time = t.current+service;

           events[1].status = 1;
           double temps = INFINITY;
           for(i=0;i<serversNum;i++){
             if(leaveBiglietteria[i].status == 1 &&leaveBiglietteria[i].time<temps){
               temps = leaveBiglietteria[i].time;
             }     
           }
           events[1].time = temps;
           
         }
                  
    }else if(typeEvent == 1){
         /*Gestisco un cliente che esce dalla biglietteria*/
         biglietteria.index++;
         biglietteria.number--;
         biglietteria.lastService = t.current;
         
         
         double tempo = events[1].time;
         /*Riprendo il server libero*/
         int free = 0;
         for(i=0;i<serversNum;i++){
           if(leaveBiglietteria[i].time == tempo){
             free = i;
           }
         }
         /*succede se ho gente in coda*/
         if(biglietteria.number>=serversNum){
           double service = getService(1,1);
           
           events[1].time = t.current + service;
           events[1].status = 1;
           biglietteriaMultiServ[free].service+=service;
           biglietteriaMultiServ[free].served++;
           leaveBiglietteria[free].time = t.current + service;
           
         }else{
           /*succede se ho un server libero*/
           biglietteriaMultiServ[free].occupied = 0;
           leaveBiglietteria[free].time = INFINITY;
           leaveBiglietteria[free].status = 0;
           
           if(biglietteria.number == 0){
             events[1].status = 0;
             events[1].time = INFINITY;
           }
         }
         double temps = INFINITY;
         for(i=0;i<serversNum;i++){
           if(leaveBiglietteria[i].status == 1 &&leaveBiglietteria[i].time<temps){
             temps = leaveBiglietteria[i].time;
           }     
         }
         events[1].time = temps;
       
         if(chooseGame() == 1){
           /*Fai Jumanji*/
           if(getTypeOfQueue() == FastQueue){
             events[2].time = tempo;
             events[2].status = 1;
       
             typeEvent = 2; 
           }else{
             events[3].time = tempo;
             events[3].status = 1;
             typeEvent = 3;         
           }
         }else{
           /*Fai Atlantide*/
           if(getTypeOfQueue() == FastQueue){
             events[6].time = tempo;
             events[6].status = 1;
       
             typeEvent = 6; 
           }else{
             events[7].time = tempo;
             events[7].status = 1;
             typeEvent = 7;         
           }           
         }


    }else if(typeEvent == 2){
     /*è un arrivo nella coda più veloce di Jumanji*/      
         jumanjiFast.number++;
         jumanjiFast.lastArrival = t.current;
         
	 jumanjiFastCount++;
	 
         if(t.current < jumanjiFast.firstArrival){
           jumanjiFast.firstArrival = t.current;
         }

         events[2].status = 0;
         
         if(jumanjiFast.number == 1){
           events[4].status = 1;
           events[4].time = t.current + getService(4,2);
           typeEvent = 4;
         }

    }else if(typeEvent == 3){ 
     /*è un arrivo nella coda più lenta di Jumanji*/

         jumanjiSlow.number++;
         jumanjiSlow.lastArrival = t.current;
         jumanjiSlowCount++;
         
         if(t.current < jumanjiSlow.firstArrival){
           jumanjiSlow.firstArrival = t.current;
         }
         
         events[3].status = 0;
         
         if(jumanjiSlow.number == 1){
           events[5].status = 1;
           events[5].time = t.current + getService(5,2);
           typeEvent = 5;
         }
 
        
    } else if (typeEvent == 4){
      /*partenza da Jumanji con coda veloce*/
	 park.number--;
	 park.index++;    
	        
         jumanjiFast.index++;
         jumanjiFast.number--;
         jumanjiFast.lastService = t.current;

         /*succede se ho gente in coda*/
         if(jumanjiFast.number>=1){
           double service = getService(4,2);
           
           gameServer[0].service+=service;
           gameServer[0].served++;
           
           events[4].status = 1;
           events[4].time = t.current + service; 
           
         }else{
           /*succede se il server è libero*/
           gameServer[0].occupied = 0;                                
           events[4].status = 0;
         }
         
         if(jumanjiFast.number == 0){
           events[4].status = 0;
           events[4].time = INFINITY;
         }
         
   
    }else if(typeEvent == 5){
      /*partenza da Jumanji con coda lenta*/ 
	 park.number--;
	 park.index++;  
	        
         jumanjiSlow.index++;
         jumanjiSlow.number--;
         jumanjiSlow.lastService = t.current;
 
         /*succede se ho gente in coda*/
         if(jumanjiSlow.number>=1){
           double service = getService(5,2);
           
           gameServer[1].service+=service;
           gameServer[1].served++;
           
           events[5].status = 1;
           events[5].time = t.current + service; 
           
         }else{
           /*succede se il server è libero*/
           gameServer[1].occupied = 0;                                
           events[5].status = 0;
         }
                  
         if(jumanjiSlow.number == 0){
           events[5].status = 0;
           events[5].time = INFINITY;
         }
         

    } else if(typeEvent == 6){
    	/*Arrivo nella coda veloce di Atlantide*/
         atlantideFast.number++;
         atlantideFast.lastArrival = t.current;
         
	 atlantideFastCount++;
	 
         if(t.current < atlantideFast.firstArrival){
           atlantideFast.firstArrival = t.current;
         }

         events[6].status = 0;
         
         if(atlantideFast.number == 1){
           events[8].status = 1;
           events[8].time = t.current + getService(8,3);
           typeEvent = 8;
         }    	
 	    	
    
    } else if(typeEvent == 7){
    	/*Arrivo nella coda lenta di Atlantide*/
         atlantideSlow.number++;
         atlantideSlow.lastArrival = t.current;
         
	 atlantideSlowCount++;
	 
         if(t.current < atlantideSlow.firstArrival){
           atlantideSlow.firstArrival = t.current;
         }

         events[7].status = 0;
         
         if(atlantideSlow.number == 1){
           events[9].status = 1;
           events[9].time = t.current + getService(9,3);
           typeEvent = 9;
         }        	
  
    }else if(typeEvent == 8){
    	/*Partenza dalla coda veloce di Atlantide*/
 	 park.number--;
	 park.index++;  
	        
         atlantideFast.index++;
         atlantideFast.number--;
         atlantideFast.lastService = t.current;

         /*succede se ho gente in coda*/
         if(atlantideFast.number>=1){
           double service = getService(8,3);
           
           gameServer[2].service+=service;
           gameServer[2].served++;
           
           events[8].status = 1;
           events[8].time = t.current + service; 
           
         }else{
           /*succede se il server è libero*/
           gameServer[2].occupied = 0;                                
           events[8].status = 0;
         }
                  
         if(atlantideFast.number == 0){
           events[8].status = 0;
           events[8].time = INFINITY;
         }
            
    }else if(typeEvent == 9){
    	/*Partenza dalla coda lenta di Atlantide*/
	 park.number--;
	 park.index++;  
	        
         atlantideSlow.index++;
         atlantideSlow.number--;
         atlantideSlow.lastService = t.current;

         /*succede se ho gente in coda*/
         if(atlantideSlow.number>=1){
           double service = getService(9,3);
           
           gameServer[3].service+=service;
           gameServer[3].served++;
           
           events[9].status = 1;
           events[9].time = t.current + service; 
           
         }else{
           /*succede se il server è libero*/
           gameServer[3].occupied = 0;                                
           events[9].status = 0;
         }
                  
         if(atlantideSlow.number == 0){
           events[9].status = 0;
           events[9].time = INFINITY;
         }
         	  
    }

    /*Aggiorna le batches*/

    if((biglietteria.index == B) && (areaBatches[1] < K)){

      saveBatchesStats(&biglietteria, 1, results);
      resetStation(&biglietteria,serversNum);
      count++;
    }

    if(((jumanjiFast.index+jumanjiSlow.index) == B) && (areaBatches[2] < K)){

      saveBatchesStats(&jumanjiFast, 2, results);
      saveBatchesStats(&jumanjiSlow, 3, results);
      resetStation(&jumanjiFast,1);
      resetStation(&jumanjiSlow,1);

      count++;
            
      contatoree = 0;

    }
    if(((atlantideFast.index+atlantideSlow.index) == B) && (areaBatches[3] < K)){
      saveBatchesStats(&atlantideFast, 4, results);
      saveBatchesStats(&atlantideSlow, 5, results);
      resetStation(&atlantideFast,1);
      resetStation(&atlantideSlow,1);
      count++;
    }
    
    if((park.index == B) && (areaBatches[0] < K)){
 
      resetStation(&park,0);

      areaBatches[0]++;
      biglietteriaCount = 0;
      jumanjiFastCount = 0;
      jumanjiSlowCount = 0;
      atlantideFastCount = 0;      
      atlantideSlowCount = 0;

    }

  } 
  
  
  /*Parte valori teorici*/
  theoricalWait(&biglietteria,1);
  theoricalWait(&jumanjiFast,2);
  theoricalWait(&jumanjiSlow,3);
  theoricalWait(&atlantideFast,4);
  theoricalWait(&atlantideSlow,5);

  
} 

