
typedef struct{
  double node;                    /* time integrated number in the node  */
  double queue;                   /* time integrated number in the queue */
  double service;                 /* time integrated number in service   */
  double index;                     /* used to count departed jobs */
  double number;                    /* number in the node */
  double servers;
  double firstArrival;
  double lastArrival;
  double lastService;

}station;

typedef struct {
    double service;
    int served; //controlla cosa farci
    int occupied;
} multiserver;


typedef struct {
  int jobs;
  double interarrival;
  double wait;
  double delay;
  double service;
  double Ns;
  double Nq;
  double utilization;
} outputValues;


typedef struct{
	double time;  
	int status;   /*se è 0 è libero, se 1 è occupato in quel momento*/	
} event_type;

typedef struct {
  double arrival;                 /* next arrival time                   */
  double completion;              /* next completion time                */
  double current;                 /* current time                        */
  double next;                    /* next (most imminent) event time     */
  double last;                    /* last arrival time                   */
} time;

