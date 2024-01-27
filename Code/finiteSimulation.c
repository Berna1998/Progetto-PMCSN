#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rngs.h"                      /* the multi-stream generator     */
#include "rvgs.h"
#include "rvms.h"
#include "struct.h"
#include "functions.h"
#include "values.h"
#include "finiteSimulation.h"


static double getArrival(){ 
  SelectStream(0);
  arrival+=Exponential(1.0/lambda);
  
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

static void updateIntegrals(station *station, multiserver multiServ[]){
  if(station->number > 0){
    station->node += (t.next - t.current)* station->number;

    if(t.next != t.current){
          if(station->number > station->servers){
            station->queue   += (t.next - t.current) * (station->number - station->servers);
          }
          if(station->servers == 1){
            station->service += (t.next - t.current);
          }else if(station->servers > 1){
            for(int j=0; j< (station->servers); j++){
              if(multiServ[j].occupied==1){
                station->service += (t.next - t.current);
              }
            }
          }
        }              
  }
  
}

outputValues getOutputValues(station *cent){
  outputValues output;

  output.interarrival = (cent->lastArrival - cent->firstArrival) / cent->index;
  output.wait = cent->node / cent->index;
  output.delay = cent->queue / cent->index;
  output.service = (cent->service/cent->servers) / cent->index;
  output.Ns = cent->node/(cent->lastService - cent->firstArrival);
  output.Nq = cent->queue / (cent->lastService - cent->firstArrival); 
  output.utilization = cent->service / (cent->servers*(cent->lastService - cent->firstArrival));
  return output;
}


void finiteHorizon(outputValues results[5], int replicaNum, int orario) {

  int biglietteriaCount = 0;
  int jumanjiFastCount = 0;
  int jumanjiSlowCount = 0;  
  int atlantideSlowCount = 0;
  int atlantideFastCount = 0; 
  int i;
  arrival = 0.0;
  
  if(orario == 1){
    lambda = LAMBDA1;
  }else{
    lambda = LAMBDA2;
  }
   
  /*Diverse inizializzazioni*/
  
  /*Inizializzo il multi server*/
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
  
  t.current = START;   /* set the clock*/
  t.next = 0.0;
  t.last = 0.0;
  
  events[0].time = getArrival();    //arriva il primo evento ed event[0] è arrivo nel sistema
  events[0].status = 1;
  park.firstArrival = events[0].time;
  
  int nexEv;

  while (events[0].time<STOPF || (jumanjiFast.number+jumanjiSlow.number+biglietteria.number+atlantideFast.number+atlantideSlow.number)>0) {  
    nexEv = getNextEvent(events);  //ottengo l'evento da gestire
    t.next = events[nexEv].time;          /* next event time   */
                                        /* update integrals  */
    updateIntegrals(&biglietteria,biglietteriaMultiServ);   
    updateIntegrals(&jumanjiFast,gameServer);
    updateIntegrals(&jumanjiSlow,gameServer);
    updateIntegrals(&atlantideFast,gameServer);
    updateIntegrals(&atlantideSlow,gameServer);    
    updateIntegrals(&park,NULL);

    t.current = t.next;                    /* advance the clock */
    
    int typeEvent = nexEv;

    if(typeEvent == 0){
       if(getTypeOfPayment() == online){
         park.number++;
         park.lastArrival = t.current;
         double time = events[0].time;
         events[0].time = getArrival();      
       
         if(events[0].time> STOPF){   
            events[0].status = 0;
            t.last = t.current;
         }
         
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
         
         if(events[0].time > STOPF){
           t.last = t.current;
           park.lastArrival = t.current;
           events[0].status = 0;
         }
         
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

         int free = 0;
         for(i=0;i<serversNum;i++){
           if(leaveBiglietteria[i].time == tempo){
             free = i;
           }
         }
         /*succede se ho gente in coda*/
         if(biglietteria.number>=serversNum){
           double service = getService(1,1);
           
           events[1].time = t.current + getService(1,1);
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

  } 
   

  biglietteriaCount = 0;
  jumanjiFastCount = 0;
  jumanjiSlowCount = 0;
  atlantideFastCount = 0;
  atlantideSlowCount = 0;
  
  outputValues biglietteriaOutput = getOutputValues(&biglietteria);
  outputValues jumanjiFastOutput = getOutputValues(&jumanjiFast);
  outputValues jumanjiSlowOutput = getOutputValues(&jumanjiSlow);
  outputValues atlantideFastOutput = getOutputValues(&atlantideFast);
  outputValues atlantideSlowOutput = getOutputValues(&atlantideSlow);
    
  results[0] = biglietteriaOutput;
  results[1] = jumanjiFastOutput;
  results[2] = jumanjiSlowOutput;
  results[3] = atlantideFastOutput;
  results[4] = atlantideSlowOutput;
  
}


