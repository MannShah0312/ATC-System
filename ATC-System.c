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
    Time arrival;
    struct FlightPlan_Node *next;
} Flight_Plan;

typedef struct Bucket_Node {
    Time ETA_Beg;
    Time ETA_End;
    int BucketID;
    Flight_Plan *FlightList;
    struct Bucket_Node *next;
} Bucket;

Time makeTime(int hr, int min, AntePost meridiem) {
    Time t;

    t.hr = hr;
    t.min = min;
    t.meridiem = meridiem;

    return t;
}

Bucket *makeBucketNode(int id, Time Beg) {
    Bucket *newB = (Bucket *)malloc(sizeof(Bucket));

    newB->BucketID = id;
    newB->ETA_Beg = Beg;
    newB->ETA_End = Beg;
    newB->ETA_End.min = 59;
    newB->FlightList = NULL;
    newB->next = NULL;

    return newB;
}

Flight_Plan *makeNewFlightPlan(int id, Time dep, Time arr) {
    Flight_Plan *new = (Flight_Plan *)malloc(sizeof(Flight_Plan));

    new->flightID = id;
    new->depart = dep;
    new->arrival = arr;
    new->next = NULL;

    return new;
}

void addNewFlightPlan(Bucket **dashboard, Flight_Plan *newFP) {
    Bucket *curr = *dashboard;
    Bucket *prev;

    while (curr && curr) {}
}