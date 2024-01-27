#include "values.h"

/*Vedi se il job che entra nel sistema va in biglietteria o no*/
static int getTypeOfPayment(){
  int type;
  if(Random()<=0.65){  //ipotizzo che il 65% delle persone va in biglietteria ad acquistare il biglietto
    type = 1; //Se il tipo è 1 allora vado in biglietteria
  }else{
    type = 0;  //Se il tipo è 0, il biglietto è stato preso online
  }
   
  return type; 
}

static int chooseGame(){
  int type;

  if(Random()<= 0.6){  //ipotizzo che il 60% delle persone va da Jumanji
    type = 1; //Se il tipo è 1 il cliente si mette in coda da Jumanji
  }else{
    type = 0;  //Se il tipo è 0 il cliente si mette in coda da Atlantide
  }
   
  return type; 
}

static int getTypeOfQueue(){  //Assumo che una piccola parte della clientela (il 10%) ha i pass saltacoda
  if(Random()<=0.1){
    return FastQueue;
  }else{
    return SlowQueue;
  }

}

/*Vedi quale evento bisogna gestire ora*/
static int getNextEvent(event_type events[10]){
 int e;                                    
 int i = 0;

  while (events[i].status == 0){      /* find the index of the first 'active' */
   i++;                        /* element in the event list            */ 
  }       
  e = i;

  while (i < 10) {         /* now, check the others to find which  */
                    /* event type is most imminent          */
    if ((events[i].status == 1) && (events[i].time < events[e].time)){
        e = i;

    }
   i++;
   
  }

  return (e);

}


static int getIdleServer(multiserver biglietteriaMultiServ[serversNum]){
  int server = 0;
  int i;
  int time = INFINITY;
  for(i=0;i<serversNum;i++){
    if(biglietteriaMultiServ[i].occupied == 0){ /*Trova il primo server libero*/
      if(biglietteriaMultiServ[i].service<= time){
         time = biglietteriaMultiServ[i].service;
         server = i;
      }
    }
  }
 
  return server;
}

static void resetStation(station *cent, double numServ){

  cent->node = 0.0;           
  cent->queue = 0.0;  
  cent->service = 0.0;             
  cent->index = 0.0;                     
  cent->number = 0.0;                    
  cent->servers = numServ;
  cent->firstArrival = INFINITY;
  cent->lastArrival = 0.0;
  cent->lastService = 0.0;
 
}

