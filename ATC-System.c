#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    newB->ETA_End = (Time *)malloc(sizeof(Time));

    newB->BucketID = id;
    newB->ETA_Beg = Beg;
    newB->ETA_End->hr = Beg->hr + ((Beg->min + 59) / 60);
    newB->ETA_End->min = Beg->min + 59;
    if (newB->ETA_End->min > 60)
        newB->ETA_End->min -= 60;
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
        retVal = 1;
    else if (A->hr > B->hr)
        retVal = -1;
    else {
        if (A->min < B->min)
            retVal = 1;
        else if (A->min > B->min)
            retVal = -1;
    }
    return retVal;
}

void addNewFP(Bucket **dashboard, Flight_Plan *newFP) {
    Bucket *curr = *dashboard;
    Bucket *prev = NULL;
    
    while (curr && (curr->ETA_Beg->hr < newFP->arrival->hr)) {
        prev = curr;
        curr = curr->next;
    }

    if (!prev || !curr) {
        Time *t = makeTime(newFP->arrival->hr, 0);
        Bucket *newB = makeBucketNode(++b_id, t);
        newB->FlightList = newFP;
        newB->next = curr;
        *dashboard = newB;
    }
    
    else {
        if (curr->ETA_Beg->hr == newFP->arrival->hr) {
            Flight_Plan *ptr = curr->FlightList;
            Flight_Plan *fprev = NULL;

            while (ptr && maxTime(ptr->depart, newFP->depart) != -1) {
                fprev = ptr;
                ptr = ptr->next;
            }

            fprev->next = newFP;
            newFP->next = ptr;
        }

        else {
            Time *t = makeTime(newFP->arrival->hr, 0);
            Bucket *newB = makeBucketNode(++b_id, t);
            newB->FlightList = newFP;
            prev->next = newB;
            newB->next = curr;
        }
    }
}

void cancelFP(Bucket **dashboard, int FPid) {
    Bucket *curr = *dashboard;
    Bucket *prev = NULL;
    Flight_Plan *fpcurr, *fpprev;
    int flag = 0;

    while (curr && !flag) {
        fpcurr = curr->FlightList;
        fpprev = NULL;
        while (fpcurr && (fpcurr->flightID != FPid)) {
            fpprev = fpcurr;
            fpcurr = fpcurr->next;
        }
        if (!fpcurr) {
            prev = curr;
            curr = curr->next;
        }
        else {
            flag = 1;
            if (fpprev)
                fpprev->next = fpcurr->next;
            else {
                curr->FlightList = fpcurr->next;
                if (!curr->FlightList) {
                    if (prev)
                        prev->next = curr->next;
                    else    
                        *dashboard = curr->next;
                    free(curr);
                }
            }
            free(fpcurr);
        }
    }
    if (!curr && !flag)
        printf("No such flight with id = %d found", FPid);
}

void showStatus(Bucket *dashboard, int FPid) {
    Bucket *curr = dashboard;
    int flag = 0;
    Flight_Plan *fpcurr;

    while (curr && !flag) {
        fpcurr = curr->FlightList;
        while (fpcurr && (fpcurr->flightID != FPid)) {
            fpcurr = fpcurr->next;
        }
        if (!fpcurr)
            curr = curr->next;
        else {
            flag = 1;
            printf("For flight id %d:\n-Departure Time: %d:%d Hrs\n-Arrival Time: %d:%d Hrs", fpcurr->flightID, fpcurr->depart->hr, fpcurr->depart->min, fpcurr->arrival->hr, fpcurr->arrival->min);
        }
    }
    if (!curr && !flag)
        printf("No such flight with id = %d found", FPid);
}

void insertSorted(Bucket **BuckPtr, Flight_Plan *fp) {
    Bucket *Buck = *BuckPtr;
    Flight_Plan *curr, *prev;

    if (!Buck->FlightList) {
        Buck->FlightList = fp;
    }
    else {
        curr = Buck->FlightList;
        prev = NULL;
        
        while (curr && maxTime(curr->depart, fp->depart) != -1) {
            prev = curr;
            curr = curr->next;
        }

        if (!prev) {
            fp->next = Buck->FlightList;
            Buck->FlightList = fp;
        }
        else {
            prev->next = fp;
            fp->next = curr;
        }
    }
    *BuckPtr = Buck;
}

void BucketRearrange(Bucket **dashboard, Time *newT) {
    Bucket *curr = *dashboard;
    Bucket *prev = NULL;
    Flight_Plan *fpcurr;
    Flight_Plan *fpprev;

    while (curr && newT->hr > curr->ETA_Beg->hr) {
        fpcurr = curr->FlightList;
        fpprev = NULL;

        while (fpcurr) {
            fpprev = fpcurr;
            fpcurr = fpcurr->next;
            free(fpprev);
        }

        curr->FlightList = NULL;
        prev = curr;
        curr = curr->next;
        free(prev);
    }

    *dashboard = curr;
    if (curr->ETA_Beg->hr == newT->hr) {
        fpcurr = curr->FlightList;
        fpprev = NULL;

        while (fpcurr) {
            if (fpcurr->arrival->min < newT->min) {
                if (!fpprev)  {
                    fpprev = fpcurr;
                    fpcurr = fpcurr->next;
                    free(fpprev);
                    fpprev = NULL;
                    curr->FlightList = fpcurr;
                }
                else {
                    fpprev->next = fpcurr->next;
                    free(fpcurr);
                    fpcurr = fpprev->next;
                }
            }
            else {
                fpprev = fpcurr;
                fpcurr = fpcurr->next;
            }
        }
        if (!curr->FlightList) {
            prev = curr;
            curr = curr->next;
            free(prev);
            *dashboard = curr;
        }
    }
    
    curr = *dashboard;
    while (curr) {
        curr->ETA_Beg->min = newT->min;
        curr->ETA_End->min = newT->min - 1;
        curr = curr->next;
    }

    curr = *dashboard;
    prev = curr;
    curr = curr->next;
    Flight_Plan *toCorrect;

    while (curr) {
        fpcurr = curr->FlightList;
        fpprev = NULL;

        while (fpcurr) {
            int diff = timeDiff(fpcurr->arrival, curr->ETA_Beg);

            if (diff >= 0) {
                fpprev = fpcurr;
                fpcurr = fpcurr->next;
            }
            else {
                if (!fpprev) {
                    curr->FlightList = fpcurr->next;
                    toCorrect = fpcurr;
                    fpcurr = fpcurr->next;
                    toCorrect->next = NULL;
                }
                else {
                    fpprev->next = fpcurr->next;
                    toCorrect = fpcurr;
                    fpcurr = fpcurr->next;
                    toCorrect->next = NULL;
                }

                int diff_prev = timeDiff(toCorrect->arrival, prev->ETA_Beg);

                if (diff_prev < 60) {
                    insertSorted(&prev, toCorrect);
                }
                else {
                    Time *TforNewB = makeTime(toCorrect->arrival->hr, newT->min);
                    Bucket *newB = makeBucketNode(++b_id, TforNewB);
                    prev->next = newB;
                    newB->next = curr;
                    prev = newB;
                    insertSorted(&prev, toCorrect);
                }
            }
        }
        prev = curr;
        curr = curr->next;
    }
}