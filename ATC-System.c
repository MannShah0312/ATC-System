#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum {am, pm} AntePost;

typedef struct Time_Node {
    int hr;
    int min;
    AntePost meridiem;
} Time;

typedef struct FlightPlan_Node {
    int flightID;
    Time depart;
    Time Arrival;
    struct FlightPlan_Node *next;
} Flight_Plan;

typedef struct Bucket_Node {
    Time Beg_ETA;
    Time End_ETA;
    int BucketID;
    Flight_Plan *FlightList;
    struct Bucket_Node *next;
} Bucket;

