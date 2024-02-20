#define START         0.0              /* initial time                   */
#define STOP      20000000.0             /* terminal (close the door) time */
#define STOPF    10800.0
#define INFINITY   (100.0 * STOP)      /* must be much larger than STOP  */
#define serversNum 6
#define FastQueue 2
#define SlowQueue 3
#define online 0
#define LOC 0.95                   /* level of confidence, */
#define SERVICEB 15
#define SERVICEJ 6.67
#define SERVICEA 3
#define LAMBDA1 0.65
#define LAMBDA2 0.51
#define K 128
#define B 1024
#define replicheNum 128

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

