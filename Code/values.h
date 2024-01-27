#define START         0.0              /* initial time                   */
#define STOP      20000000.0             /* terminal (close the door) time */
#define STOPF    10800.0
#define INFINITY   (100.0 * STOP)      /* must be much larger than STOP  */
#define serversNum 7
#define FastQueue 2
#define SlowQueue 3
#define online 0
#define LOC 0.95                   /* level of confidence, */
#define SERVICEB 15
#define SERVICEA 3.6
#define SERVICEJ 8.83
#define LAMBDA1 0.83
#define LAMBDA2 0.66
#define K 64
#define B 256
#define replicheNum 64

/*Variabili globali*/
static event_type events[10];
static int areaBatches[4];
static double arrival;		 
static station park;
static station biglietteria;
static station jumanjiFast;
static station jumanjiSlow;
static station atlantideFast;
static station atlantideSlow;
static multiserver biglietteriaMultiServ[serversNum];
static multiserver gameServer[4];
static time t;
static double lambda;

