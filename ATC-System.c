#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int f_id = 0;
static int b_id = 0;

typedef struct Time_Node {
    int hr;
    int min;
} Time;

typedef struct FlightPlan_Node {
    int flightID;
    Time *depart;
    Time *arrival;
    struct FlightPlan_Node *next;
} Flight_Plan;

typedef struct Bucket_Node {
    Time *ETA_Beg;
    Time *ETA_End;
    int BucketID;
    Flight_Plan *FlightList;
    struct Bucket_Node *next;
} Bucket;

Time *makeTime(int hr, int min) {
    Time *t = (Time *)malloc(sizeof(Time));

    t->hr = hr;
    t->min = min;

    return t;
}

Bucket *makeBucketNode(int id, Time *Beg) {
    Bucket *newB = (Bucket *)malloc(sizeof(Bucket));

    newB->BucketID = id;
    newB->ETA_Beg = Beg;
    newB->ETA_End = Beg;
    newB->ETA_End->min = 59;
    newB->FlightList = NULL;
    newB->next = NULL;

    return newB;
}

Flight_Plan *makeNewFlightPlan(int id, Time *dep, Time *arr) {
    Flight_Plan *new = (Flight_Plan *)malloc(sizeof(Flight_Plan));

    new->flightID = id;
    new->depart = dep;
    new->arrival = arr;
    new->next = NULL;

    return new;
}

int timeDiff(Time *A, Time *B) {
    int diff = (A->hr - B->hr) * 60 + (A->min - B->min);
    return diff;
}

int maxTime(Time *A, Time *B) {
    int retVal = 0;
    if (A->hr < B->hr)
        retVal = -1;
    else if (A->hr > B->hr)
        retVal = 1;
    else {
        if (A->min < B->min)
            retVal = -1;
        else if (A->min > B->min)
            retVal = 1;
    }
    return retVal;
}

void addNewFlightPlan(Bucket **dashboard, Flight_Plan *newFP) {
    Bucket *curr = *dashboard;
    Bucket *prev = NULL;
    
    while (curr && (curr->ETA_Beg->hr < newFP->arrival->hr)) {
        prev = curr;
        curr = curr->next;
    }

    if (!prev) {
        Time *t = (Time *)malloc(sizeof(Time));
        t = makeTime(newFP->arrival->hr, 0);
        Bucket *newB = (Bucket *)malloc(sizeof(Bucket));
        newB = makeBucketNode(++b_id, t);
        newB->FlightList = newFP;
        newB->next = curr;
        *dashboard = newB;
    }
    else {
        
    }
}