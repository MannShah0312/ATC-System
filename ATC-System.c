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
    struct FlightPlan_Node *prev;
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
    new->prev = NULL;

    return new;
}

int timeDiff(Time A, Time B) {
    int diff = (A.hr - B.hr) * 60 + (A.min - B.min);
    if (A.meridiem == am && B.meridiem == pm)
        diff -= 12 * 60;
    else if (A.meridiem == pm && B.meridiem == am)
        diff += 12 * 60;
    return diff;
}

int maxTime(Time A, Time B) {
    int retVal = 0;
    if (A.meridiem == B.meridiem) {
        if (A.hr < B.hr)
            retVal = -1;
        else if (A.hr > B.hr)
            retVal = 1;
        else {
            if (A.min < B.min)
                retVal = -1;
            else if (A.min > B.min)
                retVal = 1;
        }
    }
    else if (A.meridiem == am && B.meridiem == pm)
        retVal = -1;
    else
        retVal = 1;
    return retVal;
}

void addNewFlightPlan(Bucket **dashboard, Flight_Plan *newFP) {
    Bucket *curr = *dashboard;
    Bucket *prev;

    while (curr && curr) {}
}