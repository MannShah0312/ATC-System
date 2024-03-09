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
    
    if (curr) {
        if (curr->ETA_Beg->hr > newFP->arrival->hr) {
            Time *t = makeTime(newFP->arrival->hr, 0);
            Bucket *newB = makeBucketNode(++b_id, t);
            newB->FlightList = newFP;
            newB->next = curr;
            if (prev)
                prev->next = newB;
            else
                *dashboard = newB;
        }
        else {
            Flight_Plan *fpcurr = curr->FlightList;
            Flight_Plan *fpprev = NULL;

            while (fpcurr && maxTime(fpcurr->depart, newFP->depart) != -1) {
                fpprev = fpcurr;
                fpcurr = fpcurr->next;
            }
            
            if (fpprev)
                fpprev->next = newFP;
            else
                curr->FlightList = newFP;
            newFP->next = fpcurr;
        }
    }
    else {
        if (!prev) {
            Time *t = makeTime(newFP->arrival->hr, 0);
            Bucket *newB = makeBucketNode(++b_id, t);
            newB->FlightList = newFP;
            *dashboard = newB;
        }
        else {
            Time *t = makeTime(newFP->arrival->hr, 0);
            Bucket *newB = makeBucketNode(++b_id, t);
            newB->FlightList = newFP;
            prev->next = newB;
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
                    free(curr->ETA_Beg);
                    free(curr->ETA_End);
                    free(curr);
                }
            }
            free(fpcurr->arrival);
            free(fpcurr->depart);
            free(fpcurr);
        }
    }
    if (!curr && !flag)
        printf("No flight with ID %d present, so it cannot be cancelled\n", FPid);
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
            printf("For flight id %d:\n   -Departure Time: %d:%d Hrs\n   -Arrival Time: %d:%d Hrs\n", fpcurr->flightID, fpcurr->depart->hr, fpcurr->depart->min, fpcurr->arrival->hr, fpcurr->arrival->min);
        }
    }
    if (!curr && !flag)
        printf("No such flight with id %d found\n", FPid);
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
            free(fpprev->arrival);
            free(fpprev->depart);
            free(fpprev);
        }

        curr->FlightList = NULL;
        free(curr->ETA_Beg);
        free(curr->ETA_End);
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
                    free(fpprev->arrival);
                    free(fpprev->depart);
                    free(fpprev);
                    fpprev = NULL;
                    curr->FlightList = fpcurr;
                }
                else {
                    fpprev->next = fpcurr->next;
                    free(fpcurr->arrival);
                    free(fpcurr->depart);
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
            free(prev->ETA_Beg);
            free(prev->ETA_End);
            free(prev);
            *dashboard = curr;
        }
    }
    
    curr = *dashboard;

    if (newT->min + 59 >= 60) {
        while (curr) {
            curr->ETA_Beg->min = newT->min;
            curr->ETA_End->min = newT->min - 1;
            if(curr->ETA_End->hr < 23)
                curr->ETA_End->hr += 1;
            else
                curr->ETA_End->hr = 0;
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
}

void displayDashboard(Bucket *dashboard, char *fileName) {
    FILE *newF = fopen(fileName, "w");

    Bucket *curr = dashboard;
    Flight_Plan *fpcurr;
    int i;

    while (curr) {
        fprintf(newF, "Bucket info: ID: %d -- ETA_Beginng: %d:%d -- ETA_Ending: %d:%d\n", curr->BucketID, curr->ETA_Beg->hr, curr->ETA_Beg->min, curr->ETA_End->hr, curr->ETA_End->min);
        fprintf(newF, "Flight List:\n");
        fpcurr = curr->FlightList;
        i = 1;

        while (fpcurr) {
            fprintf(newF, "\t%d. Flight ID: %d, Departure: %d:%d, Arrival: %d:%d\n", i, fpcurr->flightID, fpcurr->depart->hr, fpcurr->depart->min, fpcurr->arrival->hr, fpcurr->arrival->min);
            fpcurr = fpcurr->next;
            i++;
        }
        fprintf(newF, "\n");
        curr = curr->next;
    }

    fclose(newF);
}

Bucket* input(Bucket *dashboard, char *fileName) {
    FILE *f = fopen(fileName, "r");
    int id, depHr, depMin, arrHr, arrMin;
    fscanf(f, "%*s,%*s,%*s,%*s,%*s\n");
    while (!feof(f)) {
        fscanf(f, "%d,%d,%d,%d,%d\n", &id, &depHr, &depMin, &arrHr, &arrMin);
        Time *dep = makeTime(depHr, depMin);
        Time *arr = makeTime(arrHr, arrMin);
        Flight_Plan *fp = makeNewFlightPlan(id, dep, arr);
        addNewFP(&dashboard, fp);
    }

    fclose(f);
    return dashboard;
}

int main()
{
    Bucket *dashboard = NULL;
    dashboard = input(dashboard, "flights.csv");
    displayDashboard(dashboard, "flights.txt");
    showStatus(dashboard, 100);
    cancelFP(&dashboard, 14);
    Time *newT = makeTime(2, 30);
    BucketRearrange(&dashboard, newT);
    displayDashboard(dashboard, "output.txt");
    cancelFP(&dashboard, 85);
    displayDashboard(dashboard, "output1.txt");
}