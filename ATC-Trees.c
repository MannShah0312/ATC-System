#include <stdio.h>
#include <stdlib.h>
#define ORDER 3
#define MINORDER 2
#define CAPACITY 4
#define MIN 2

typedef enum {false, true} bool;
typedef enum {inter, leaf} nodeType;
static int b_id = 0;

struct FlightTreeNode;

typedef struct Time {
    int hr;
    int min;
} Time;

typedef struct FlightPlan {
    int flightId;
    Time *arr;
    Time *dep;
} FlightPlan;

typedef struct Bucket {
    int buckId;
    int height;
    Time *ETA_Beg;
    Time ETA_End;
    struct FlightTreeNode *root;
    struct Bucket *left;
    struct Bucket *right;
} Bucket;

typedef struct FlightDLLNode {
    FlightPlan *fList[CAPACITY + 1];
    int FDLLsize;
    struct FlightDLLNode *next;
    struct FlightDLLNode *prev;
    struct FlightTreeNode *parent;
    int parentIndex;
} FlightDLLNode;

typedef struct FlightTreeNode {
    Time *TimeNodes[ORDER - 1];
    int size;
    union {
        struct FlightTreeNode *next[ORDER];
        FlightDLLNode *data[ORDER];
    } childPtr;
    nodeType UTag;
    int parentIndex;
    struct FlightTreeNode *parent;
} FlightTreeNode;

int max(int a, int b) {
    return (a > b) ? a : b;
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

Time* makeTime(int hr, int min) {
    Time *t = (Time *)malloc(sizeof(Time));
    t->hr = hr;
    t->min = min;
    return t;
}

FlightPlan* makeFlightPlan(int id, Time *dep, Time *arr) {
    FlightPlan *newFP = (FlightPlan *)malloc(sizeof(FlightPlan));
    newFP->flightId = id;
    newFP->dep = dep;
    newFP->arr = arr;
    return newFP;
}

Bucket* makeBucket(Time *beg) {
    Bucket *newB = (Bucket *)malloc(sizeof(Bucket));
    newB->buckId = ++b_id;
    newB->height = 1;
    newB->ETA_Beg = beg;
    newB->ETA_End.hr = beg->hr;
    newB->ETA_End.min = beg->min + 59;
    if (newB->ETA_End.min >= 60)
        newB->ETA_End.min -= 60;
    newB->root = NULL;
    newB->left = NULL;
    newB->right = NULL;
    return newB;
}

FlightDLLNode* makeFlightDLLNode() {
    FlightDLLNode *newFDLL = (FlightDLLNode *)malloc(sizeof(FlightDLLNode));
    newFDLL->FDLLsize = 0;
    newFDLL->next = NULL;
    newFDLL->prev = NULL;
    newFDLL->parent = NULL;
    newFDLL->parentIndex = -1;
    for (int i = 0; i < CAPACITY; i++) {
        newFDLL->fList[i] = NULL;
    }
    return newFDLL;
}

FlightTreeNode* makeFlightTreeNode(nodeType nt, Time *t) {
    FlightTreeNode *newFTNode = (FlightTreeNode *)malloc(sizeof(FlightTreeNode));
    newFTNode->size = 1;
    newFTNode->UTag = nt;
    newFTNode->parent = NULL;
    newFTNode->parentIndex = -1;
    newFTNode->TimeNodes[0] = t;
    if(newFTNode->UTag == leaf) {
        int i;
        for (i = 0; i < ORDER - 2; i++) {
            newFTNode->TimeNodes[i + 1] = NULL;
        }
        for (i = 0; i < CAPACITY + 1; i++) {
            newFTNode->childPtr.data[i] = NULL;
        }
        newFTNode->UTag = leaf;
    }
    else {
        int i;
        for (i = 0; i < ORDER - 2; i++) {
            newFTNode->TimeNodes[i + 1] = NULL;
        }
        for (i = 0; i < ORDER; i++) {
            newFTNode->childPtr.next[i] = NULL;
        }
        newFTNode->UTag = inter;
    }
    return newFTNode;
}

void printTime(Time *A) {
    printf("hr: %d, min: %d\n", A->hr, A->min);
}

int getHeight(Bucket *root) {
    if (!root)
        return 0;
    return root->height;
}

Bucket* rotateRight(Bucket *root) {
    Bucket *temp = root;
    if (!root || !root->left)
        printf("ERROR\n");
    else {
        temp = root->left;
        root->left = temp->right;
        temp->right = root;
        root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
        temp->height = max(getHeight(temp->left), getHeight(temp->right)) + 1;   
    }
    return temp;
}

Bucket* rotateLeft(Bucket *root) {
    Bucket *temp = root;
    if (!root || !root->right)
        printf("ERROR\n");
    else {
        temp = root->right;
        root->right = temp->left;
        temp->left = root;
        root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
        temp->height = max(getHeight(temp->left), getHeight(temp->right)) + 1;   
    }
    return temp;
}

Bucket* insertNewBuck(Bucket *root, Bucket *nptr) {
    if (!nptr)
        return root;
    if (!root)
        return nptr;
    else {
        if (nptr->ETA_Beg->hr < root->ETA_Beg->hr)
            root->left = insertNewBuck(root->left, nptr);
        else
            root->right = insertNewBuck(root->right, nptr);

        root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
        int diff = getHeight(root->right) - getHeight(root->left);

        if (diff < -1 && nptr->ETA_Beg->hr < root->left->ETA_Beg->hr)
            return rotateRight(root);

        if (diff > 1 && nptr->ETA_Beg->hr > root->right->ETA_Beg->hr)
            return rotateLeft(root);

        if (diff < -1 && nptr->ETA_Beg->hr > root->left->ETA_Beg->hr) {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }

        if (diff > 1 && nptr->ETA_Beg->hr < root->right->ETA_Beg->hr) {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }

        return root;
    }
}

Bucket* findBucketAndAdd(Bucket *root, int findHr, Bucket **ogRoot) {
    if (!root) {
        Time *t = makeTime(findHr, 0);
        Bucket *nptr = makeBucket(t);
        *ogRoot = insertNewBuck(*ogRoot, nptr);
        return nptr;
    }
    else {
        if (findHr < root->ETA_Beg->hr)
            return findBucketAndAdd(root->left, findHr, ogRoot);
        else if (findHr > root->ETA_Beg->hr)
            return findBucketAndAdd(root->right, findHr, ogRoot);
        else
            return root;
    }
}

Bucket* justFindBucket(Bucket *root, int findHr) {
    if (root) {
        if (findHr < root->ETA_Beg->hr)
            return justFindBucket(root->left, findHr);
        else if (findHr > root->ETA_Beg->hr)
            return justFindBucket(root->right, findHr);
        else
            return root;
    }
    else 
        return NULL;
}

FlightDLLNode* findSpot(FlightTreeNode *perfRoot, Time *FPdep) {
    int toGoIndex = 0;
    int size = perfRoot->size;
    while (toGoIndex < size && timeDiff(perfRoot->TimeNodes[toGoIndex], FPdep) <= 0) {
        toGoIndex++;
    }
    if (perfRoot->UTag == leaf) {
        FlightDLLNode *ret = perfRoot->childPtr.data[toGoIndex];
        if(!ret) {
            ret = makeFlightDLLNode();
            ret->parent = perfRoot;
            ret->parentIndex = toGoIndex;
            if (toGoIndex != ORDER - 1)
                ret->next = perfRoot->childPtr.data[toGoIndex + 1];
            if (toGoIndex != 0)
                ret->prev = perfRoot->childPtr.data[toGoIndex - 1];
            perfRoot->childPtr.data[toGoIndex] = ret;
        }
        return ret;
    }
    else
        return findSpot(perfRoot->childPtr.next[toGoIndex], FPdep);
}

void addIntoNonFull(FlightDLLNode *DLLNode, FlightPlan *newFP) {
    if (!DLLNode->fList[0]) {
        DLLNode->fList[0] = newFP;
        DLLNode->FDLLsize++;
        return;
    }
    int insertIndex, size;
    insertIndex = 0;
    size = DLLNode->FDLLsize;
    DLLNode->FDLLsize++;
    while (insertIndex < size && timeDiff(DLLNode->fList[insertIndex]->dep, newFP->dep) <= 0) {
        insertIndex++;
    }
    if (insertIndex == size) {
        DLLNode->fList[size] = newFP;
        return;
    }
    FlightPlan *temp1 = DLLNode->fList[insertIndex];
    FlightPlan *temp2;
    DLLNode->fList[insertIndex] = newFP;
    int i = insertIndex + 1;
    size++;
    while (i < size) {
        temp2 = temp1;
        temp1 = DLLNode->fList[i];
        DLLNode->fList[i] = temp2;
        i++;
    }
    return;
}

FlightTreeNode *splitParent(FlightTreeNode *root, Time *toPush, FlightTreeNode *gp, FlightTreeNode *kn1, FlightTreeNode *kn2) {
    if (!gp) {
        FlightTreeNode *newRoot = makeFlightTreeNode(inter, toPush);

        newRoot->childPtr.next[0] = kn1;
        kn1->parent = newRoot;
        kn1->parentIndex = 0;

        newRoot->childPtr.next[1] = kn2;
        kn2->parent = newRoot;
        kn2->parentIndex = 1;

        return newRoot;
    }
    else if (gp->size < ORDER - 1) {
        gp->size++;
        if (kn1->parentIndex == 0) {
            gp->TimeNodes[1] = gp->TimeNodes[0];
            gp->TimeNodes[0] = toPush;

            gp->childPtr.next[2] = gp->childPtr.next[1];
            gp->childPtr.next[2]->parentIndex = 2;

            gp->childPtr.next[1] = kn2;
            kn2->parentIndex = 1;

            gp->childPtr.next[0] = kn1;
            kn1->parentIndex = 0;
        }
        else if (kn1->parentIndex == 1){
            gp->TimeNodes[1] = toPush;

            gp->childPtr.next[1] = kn1;
            kn1->parentIndex = 1;

            gp->childPtr.next[2] = kn2;
            kn2->parentIndex = 2;
        }
        kn1->parent = gp;
        kn2->parent = gp;
        return root;
    }
    else {
        FlightTreeNode *newKn1, *ggp;
        ggp = gp->parent;
        if (kn1->parentIndex == 0) {
            newKn1 = makeFlightTreeNode(inter, gp->TimeNodes[1]);
            gp->TimeNodes[1] = NULL;
            gp->size--;
            Time *temp = gp->TimeNodes[0];
            gp->TimeNodes[0] = toPush;
            toPush = temp;
            gp->childPtr.next[0] = kn1;
            kn1->parent = gp;
            kn1->parentIndex = 0;

            newKn1->childPtr.next[0] = gp->childPtr.next[1];
            newKn1->childPtr.next[0]->parent = newKn1;
            newKn1->childPtr.next[0]->parentIndex = 0;
            gp->childPtr.next[1] = kn2;
            kn2->parent = gp;
            kn2->parentIndex = 1;

            newKn1->childPtr.next[1] = gp->childPtr.next[2];
            newKn1->childPtr.next[1]->parent = newKn1;
            newKn1->childPtr.next[1]->parentIndex = 1;
            gp->childPtr.next[2] = NULL;
        }
        else if (kn1->parentIndex == 1) {
            newKn1 = makeFlightTreeNode(inter, gp->TimeNodes[1]);
            gp->TimeNodes[1] = NULL;
            gp->size--;

            gp->childPtr.next[1] = kn1;
            kn1->parent = gp;
            kn1->parentIndex = 1;

            newKn1->childPtr.next[0] = kn2;
            kn2->parent = newKn1;
            kn2->parentIndex = 0;

            newKn1->childPtr.next[1] = gp->childPtr.next[2];
            newKn1->childPtr.next[1]->parent = newKn1;
            newKn1->childPtr.next[1]->parentIndex = 1;
            gp->childPtr.next[2] = NULL;
        }
        else {
            newKn1 = makeFlightTreeNode(inter, toPush);
            toPush = gp->TimeNodes[1];
            newKn1->childPtr.next[0] = kn1;
            kn1->parent = newKn1;
            kn1->parentIndex = 0;
            
            newKn1->childPtr.next[1] = kn2;
            kn2->parent = newKn1;
            kn2->parentIndex = 1;

            gp->size--;
            gp->TimeNodes[1] = NULL;
            gp->childPtr.next[2] = NULL;
        }
        return splitParent(root, toPush, ggp, gp, newKn1);
    }
}

FlightTreeNode* addIntoFull(FlightDLLNode *DataNode, FlightPlan *newFP, FlightTreeNode *root) {
    FlightDLLNode *newDataNode = (FlightDLLNode *)malloc(sizeof(FlightDLLNode));
    FlightPlan *temp1, *temp2;
    int i = 0;
    while (i < CAPACITY && timeDiff(DataNode->fList[i]->dep, newFP->dep) < 0)
        i++;
    temp1 = DataNode->fList[i];
    DataNode->fList[i] = newFP;
    i++;
    while (i < CAPACITY + 1) {
        temp2 = temp1;
        temp1 = DataNode->fList[i];
        DataNode->fList[i] = temp2;
        i++;
    }
    i = 2;
    newDataNode->FDLLsize = 3;
    while (i < CAPACITY + 1) {
        newDataNode->fList[i - 2] = DataNode->fList[i];
        DataNode->fList[i] = NULL;
        i++;
    }
    newDataNode->next = DataNode->next;
    newDataNode->prev = DataNode;
    DataNode->next = newDataNode;
    if (newDataNode->next)
        newDataNode->next->prev = newDataNode;
    // newDataNode->parent = DataNode->parent;
    // newDataNode->parentIndex = DataNode->parentIndex + 1;
    DataNode->FDLLsize = 2;

    if (DataNode->parent->size < ORDER - 1) {
        if (DataNode->parentIndex == 0) {
            DataNode->parent->childPtr.data[2] = newDataNode->next;
            newDataNode->next->parentIndex = 2;
            DataNode->parent->childPtr.data[1] = newDataNode;
            newDataNode->parent = DataNode->parent;
            newDataNode->parentIndex = 1;
            DataNode->parent->TimeNodes[1] = DataNode->parent->TimeNodes[0];
            Time *t = makeTime(newDataNode->fList[0]->dep->hr, newDataNode->fList[0]->dep->min);
            DataNode->parent->TimeNodes[0] = t;
        }
        else if (DataNode->parentIndex == 1) {
            DataNode->parent->childPtr.data[2] = newDataNode;
            newDataNode->parentIndex = 2;
            newDataNode->parent = DataNode->parent;
            Time *t = makeTime(newDataNode->fList[0]->dep->hr, newDataNode->fList[0]->dep->min);
            DataNode->parent->TimeNodes[1] = t;
        }
        DataNode->parent->size++;
        return root;
    }
    else {
        FlightTreeNode *keyNode1, *par, *grandpar;
        Time *toPush;
        par = DataNode->parent;
        grandpar = par->parent;
        if (DataNode->parentIndex == 0) {
            keyNode1 = makeFlightTreeNode(leaf, par->TimeNodes[1]);
            toPush = par->TimeNodes[0];
            Time *t = makeTime(newDataNode->fList[0]->dep->hr, newDataNode->fList[0]->dep->min);
            par->TimeNodes[0] = t;
            par->TimeNodes[1] = NULL;
            keyNode1->size = 1;

            keyNode1->childPtr.data[0] = par->childPtr.data[1];
            keyNode1->childPtr.data[0]->parent = keyNode1;
            keyNode1->childPtr.data[0]->parentIndex = 0;
            par->childPtr.data[1] = NULL;
            
            keyNode1->childPtr.data[1] = par->childPtr.data[2];
            keyNode1->childPtr.data[1]->parent = keyNode1;
            keyNode1->childPtr.data[1]->parentIndex = 1;
            par->childPtr.data[2] = NULL;

            par->childPtr.data[1] = newDataNode;
            par->childPtr.data[1]->parent = par;
            par->childPtr.data[1]->parentIndex = 1;
            par->size--;
        }
        else if (DataNode->parentIndex == 1) {
            keyNode1 = makeFlightTreeNode(leaf, par->TimeNodes[1]);
            par->TimeNodes[1] = NULL;
            keyNode1->size = 1;
            Time *t = makeTime(newDataNode->fList[0]->dep->hr, newDataNode->fList[0]->dep->min);
            toPush = t;

            keyNode1->childPtr.data[0] = newDataNode;
            newDataNode->parent = keyNode1;
            newDataNode->parentIndex = 0;

            keyNode1->childPtr.data[1] = par->childPtr.data[2];
            keyNode1->childPtr.data[1]->parent = keyNode1;
            keyNode1->childPtr.data[1]->parentIndex = 1;
            par->childPtr.data[2] = NULL;

            par->size--;
        }
        else {
            Time *t = makeTime(newDataNode->fList[0]->dep->hr, newDataNode->fList[0]->dep->min);
            keyNode1 = makeFlightTreeNode(leaf, t);
            toPush = par->TimeNodes[1];
            par->TimeNodes[1] = NULL;

            keyNode1->childPtr.data[0] = DataNode;
            keyNode1->childPtr.data[0]->parent = keyNode1;
            keyNode1->childPtr.data[0]->parentIndex = 0;
            par->childPtr.data[2] = NULL;

            keyNode1->childPtr.data[1] = newDataNode;
            keyNode1->childPtr.data[1]->parent = keyNode1;
            keyNode1->childPtr.data[1]->parentIndex = 1;

            par->size--;
        }
        return splitParent(root, toPush, grandpar, par, keyNode1);
    }
}

Bucket* addNewFlight(Bucket *AVLroot, FlightPlan *newFP) {
    Bucket *perfBuck = findBucketAndAdd(AVLroot, newFP->arr->hr, &AVLroot);
    FlightTreeNode *perfRoot = perfBuck->root;
    if (!perfRoot) {
        Time *t = makeTime(newFP->dep->hr, newFP->dep->min);
        FlightTreeNode *newFTNode = makeFlightTreeNode(leaf, t);
        newFTNode->childPtr.data[1] = makeFlightDLLNode();
        newFTNode->childPtr.data[1]->FDLLsize++;
        newFTNode->childPtr.data[1]->fList[0] = newFP;
        newFTNode->childPtr.data[1]->parent = newFTNode;
        newFTNode->childPtr.data[1]->parentIndex = 1;
        perfBuck->root = newFTNode;
    }
    else {
        FlightDLLNode *perfDLLNode = findSpot(perfRoot, newFP->dep);
        if (perfDLLNode->FDLLsize < CAPACITY) {
            addIntoNonFull(perfDLLNode, newFP);
        }   
        else {
            perfRoot = addIntoFull(perfDLLNode, newFP, perfRoot);
            perfBuck->root = perfRoot;
        }
    }
    return AVLroot;
}

FlightDLLNode* findNextCompleteSib(FlightDLLNode *dataNode, FlightTreeNode *par) {
    if (dataNode->FDLLsize > MIN)
        return dataNode;
    if (dataNode->parentIndex == par->size)
        return NULL;
    return findNextCompleteSib(dataNode->next, par);
}

FlightDLLNode* findPrevCompleteSib(FlightDLLNode *dataNode, FlightTreeNode *par) {
    if (dataNode->FDLLsize > MIN)
        return dataNode;
    if (dataNode->parentIndex == 0)
        return NULL;
    return findPrevCompleteSib(dataNode->prev, par);
}

bool borrowFromSib(FlightDLLNode *dataNode) {
    FlightDLLNode *nextSib = findNextCompleteSib(dataNode, dataNode->parent);
    FlightDLLNode *prevSib = findPrevCompleteSib(dataNode, dataNode->parent);
    if (!nextSib && !prevSib)
        return false;

    FlightDLLNode *ptr = dataNode;
    if (nextSib) {
        while (ptr != nextSib) {
            FlightDLLNode *next = ptr->next;
            ptr->fList[ptr->FDLLsize] = next->fList[0];
            int i = 0;
            while (i < next->FDLLsize - 1) {
                next->fList[i] = next->fList[i + 1];
                i++;
            }
            next->fList[next->FDLLsize] = NULL;
            next->FDLLsize--;
            ptr->FDLLsize++;
            ptr->parent->TimeNodes[ptr->parentIndex]->hr = next->fList[0]->dep->hr;
            ptr->parent->TimeNodes[ptr->parentIndex]->min = next->fList[0]->dep->min;
            ptr = ptr->next;
        }
    } else if (prevSib) {
        while (ptr != prevSib) {
            FlightDLLNode *prev = ptr->prev;
            int i = 0;
            while (i < ptr->FDLLsize - 1) {
                ptr->fList[i + 1] = ptr->fList[i];
                i++;
            }
            ptr->fList[0] = prev->fList[prev->FDLLsize];
            prev->fList[prev->FDLLsize] = NULL;
            prev->FDLLsize--;
            ptr->FDLLsize++;
            ptr->parent->TimeNodes[ptr->parentIndex - 1]->hr = ptr->fList[0]->dep->hr;
            ptr->parent->TimeNodes[ptr->parentIndex - 1]->min = ptr->fList[0]->dep->min;
            ptr = ptr->prev;
        }
    }
    return true;
}

FlightTreeNode* findNextCompleteParSib(FlightTreeNode *Node, FlightTreeNode *par) {
    printf("Finding next par sib\n");
    if (Node->size > MINORDER - 1) {
        printf("nextParSib found func\n");
        return Node;
    }
    if (Node->parentIndex == par->size) {
        printf("nextParSib not found\n");
        return NULL;
    }
    return findNextCompleteParSib(par->childPtr.next[Node->parentIndex + 1], par);
}

FlightTreeNode* findPrevCompleteParSib(FlightTreeNode *Node, FlightTreeNode *par) {
    printf("Finding prev par sib\n");
    printf("parentIndex: %d\n", Node->parentIndex);
    if (Node->size > MINORDER - 1) {
        printf("prevparsib found func\n");
        return Node;
    }
    if (Node->parentIndex == 0) {
        printf("prevpar sib not found\n");
        return NULL;
    }
    return findPrevCompleteParSib(par->childPtr.next[Node->parentIndex - 1], par);
}

FlightTreeNode* borrowFromParSib(FlightTreeNode *nptr, FlightTreeNode *parent) {
    FlightTreeNode *nextSib = findNextCompleteParSib(nptr, parent);
    printf("parentIndex: %d\n", nptr->parentIndex);
    FlightTreeNode *prevSib = findPrevCompleteParSib(nptr, parent);
    
    if (nextSib) {
        printf("nextParSib found!\n");
        if (nptr->UTag == leaf) {
            nptr->childPtr.data[1] = nextSib->childPtr.data[0];
            nptr->childPtr.data[1]->parent = nptr;
            nptr->childPtr.data[1]->parentIndex = 1;
            nptr->TimeNodes[0]->hr = nptr->childPtr.data[1]->fList[0]->dep->hr;
            nptr->TimeNodes[0]->min = nptr->childPtr.data[1]->fList[0]->dep->min;
            int i;
            for (i = 1; i < nextSib->size + 1; i++) {
                nextSib->childPtr.data[i - 1] = nextSib->childPtr.data[i];
                nextSib->childPtr.data[i - 1]->parentIndex = i - 1;
            }
            nextSib->childPtr.data[i - 1] = NULL;
        } 
        else {
            nptr->childPtr.next[1] = nextSib->childPtr.next[0];
            nptr->childPtr.next[1]->parent = nptr;
            nptr->childPtr.next[1]->parentIndex = 1;
            int i;
            for (i = 1; i < nextSib->size + 1; i++) {
                nextSib->childPtr.next[i - 1] = nextSib->childPtr.next[i];
                nextSib->childPtr.next[i - 1]->parentIndex = i - 1;
            }
            nextSib->childPtr.next[i - 1] = NULL;
            FlightTreeNode *traverse = nptr->childPtr.next[1];
            while (traverse->UTag == inter) {
                traverse = traverse->childPtr.next[0];
            }
            nptr->TimeNodes[0]->hr = traverse->childPtr.data[0]->fList[0]->dep->hr;
            nptr->TimeNodes[0]->min = traverse->childPtr.data[0]->fList[0]->dep->min;
        }
        nptr->size++;
        nextSib->size--;
        free(nextSib->TimeNodes[0]);
        nextSib->TimeNodes[0] = nextSib->TimeNodes[1];
        nextSib->TimeNodes[1] = NULL;
        FlightDLLNode *nextFirstData;
        while (nextSib->UTag != leaf) {
            nextSib = nextSib->childPtr.next[0];
        }
        nextFirstData = nextSib->childPtr.data[0];
        parent->TimeNodes[nptr->parentIndex]->hr = nextFirstData->fList[0]->dep->hr;
        parent->TimeNodes[nptr->parentIndex]->min = nextFirstData->fList[0]->dep->min;
        return nptr;
    }
    else if (prevSib) {
        printf("prevParSib found!\n");
        if (nptr->UTag == leaf) {
            nptr->childPtr.data[1] = nptr->childPtr.data[0];
            nptr->childPtr.data[0] = prevSib->childPtr.data[prevSib->size];
            prevSib->childPtr.data[prevSib->size] = NULL;
            nptr->TimeNodes[0]->hr = nptr->childPtr.data[1]->fList[0]->dep->hr;
            nptr->TimeNodes[0]->min = nptr->childPtr.data[1]->fList[0]->dep->min;
        } 
        else {
            nptr->childPtr.next[1] = nptr->childPtr.next[0];
            nptr->childPtr.next[0] = prevSib->childPtr.next[prevSib->size];
            prevSib->childPtr.next[prevSib->size] = NULL;
            FlightTreeNode *traverse = nptr->childPtr.next[1];
            while (traverse->UTag == inter) {
                traverse = traverse->childPtr.next[0];
            }
            nptr->TimeNodes[0]->hr = traverse->childPtr.data[0]->fList[0]->dep->hr;
            nptr->TimeNodes[0]->min = traverse->childPtr.data[0]->fList[0]->dep->min;
        }
        prevSib->TimeNodes[prevSib->size] = NULL;
        nptr->size++;
        prevSib->size--;
        FlightTreeNode *temp = nptr;
        FlightDLLNode *nextFirstData;
        while (temp->UTag != leaf) {
            temp = temp->childPtr.next[0];
        }
        nextFirstData = temp->childPtr.data[0];
        parent->TimeNodes[nptr->parentIndex - 1]->hr = nextFirstData->fList[0]->dep->hr;
        parent->TimeNodes[nptr->parentIndex - 1]->min = nextFirstData->fList[0]->dep->min;
        return nptr;
    }
    else {
        printf("nextParSib or prev NOT found!\n");
        if (nptr->parentIndex == 0) {
            printf("nptr Parentindex 0\n");
            nextSib = parent->childPtr.next[1];
            nextSib->TimeNodes[1] = nextSib->TimeNodes[0];
            nextSib->TimeNodes[0] = parent->TimeNodes[0];
            if (nextSib->UTag == inter) {
                nextSib->childPtr.next[2] = nextSib->childPtr.next[1];
                nextSib->childPtr.next[2]->parentIndex = 2;

                nextSib->childPtr.next[1] = nextSib->childPtr.next[0];
                nextSib->childPtr.next[1]->parentIndex = 1;
                
                nextSib->childPtr.next[0] = nptr->childPtr.next[0];
                nextSib->childPtr.next[0]->parent = nextSib;
                nextSib->childPtr.next[0]->parentIndex = 0;
            }
            else {
                nextSib->childPtr.data[2] = nextSib->childPtr.data[1];
                nextSib->childPtr.data[2]->parentIndex = 2;

                nextSib->childPtr.data[1] = nextSib->childPtr.data[0];
                nextSib->childPtr.data[1]->parentIndex = 1;
                
                nextSib->childPtr.data[0] = nptr->childPtr.data[0];
                nextSib->childPtr.data[0]->parent = nextSib;
                nextSib->childPtr.data[0]->parentIndex = 0;
            }
            printf("next sib children set\n");
            nextSib->parentIndex = 0;
            parent->childPtr.next[0] = nextSib;
            if (parent->size == 2) {
                parent->childPtr.next[1] = parent->childPtr.next[2];
                parent->childPtr.next[1]->parentIndex = 1;
                printf("midway\n");
                parent->childPtr.next[2] = NULL;
                parent->TimeNodes[0] = parent->TimeNodes[1];
                parent->TimeNodes[1] = NULL;
            }
            printf("parent children set\n");
            parent->size--;
            nextSib->size++;
            free(nptr->TimeNodes[0]);
            free(nptr);
            printf("nptr freed\n");
            if (parent->size == 0) {
                printf("parent size less than min order - 1\n");
                parent = borrowFromParSib(parent, parent->parent);
                if (!parent) {
                    nextSib->parent = NULL;
                    nextSib->parentIndex = -1;
                } else {
                    parent->childPtr.next[0] = nextSib;
                }
            }
            return nextSib;
        }
        else if (nptr->parentIndex == 1) {
            if (parent->size == 2) {
                printf("parentIndex 1 size 2\n");
                FlightTreeNode *nextSib = parent->childPtr.next[2];
                nextSib->TimeNodes[1] = nextSib->TimeNodes[0];
                nextSib->TimeNodes[0] = parent->TimeNodes[1];
                if (nextSib->UTag == inter) {
                    nextSib->childPtr.next[2] = nextSib->childPtr.next[1];
                    nextSib->childPtr.next[2]->parentIndex = 2;

                    nextSib->childPtr.next[1] = nextSib->childPtr.next[0];
                    nextSib->childPtr.next[1]->parentIndex = 1;

                    nextSib->childPtr.next[0] = nptr->childPtr.next[0];
                    nextSib->childPtr.next[0]->parent = nextSib;
                    nextSib->childPtr.next[0]->parentIndex = 0;
                }
                else {
                    nextSib->childPtr.data[2] = nextSib->childPtr.data[1];
                    nextSib->childPtr.data[2]->parentIndex = 2;

                    nextSib->childPtr.data[1] = nextSib->childPtr.data[0];
                    nextSib->childPtr.data[1]->parentIndex = 1;

                    nextSib->childPtr.data[0] = nptr->childPtr.data[0];
                    nextSib->childPtr.data[0]->parent = nextSib;
                    nextSib->childPtr.data[0]->parentIndex = 0;
                }
                nextSib->parentIndex = 1;
                parent->childPtr.next[1] = nextSib;
                parent->childPtr.next[2] = NULL;
                parent->TimeNodes[1] = NULL;
                parent->size--;
                nextSib->size++;
                free(nptr->TimeNodes[0]);
                free(nptr);
                return nextSib;
            }
            else {
                printf("ParentIndex 1 size 1\n");
                prevSib = parent->childPtr.next[0];
                prevSib->TimeNodes[1] = parent->TimeNodes[0];
                if (prevSib->UTag == inter) {
                    prevSib->childPtr.next[2] = nptr->childPtr.next[0];
                    prevSib->childPtr.next[2]->parent = prevSib;
                    prevSib->childPtr.next[2]->parentIndex = 2;
                }
                else {
                    prevSib->childPtr.data[2] = nptr->childPtr.data[0];
                    prevSib->childPtr.data[2]->parent = prevSib;
                    prevSib->childPtr.data[2]->parentIndex = 2;
                }
                prevSib->parent = parent->parent;
                if (parent->parent) {
                    parent->parent->childPtr.next[parent->parentIndex] = prevSib;
                    prevSib->parentIndex = parent->parentIndex;
                }
                else
                    prevSib->parentIndex = -1;
                prevSib->size++;
                free(nptr->TimeNodes[0]);
                free(nptr);
                free(parent);
                return prevSib;
            }
        }
        else {
            printf("parent index 2\n");
            prevSib = parent->childPtr.next[1];
            prevSib->TimeNodes[1] = parent->TimeNodes[1];
            if (prevSib->UTag == inter) {
                prevSib->childPtr.next[2] = nptr->childPtr.next[0];
                prevSib->childPtr.next[2]->parent = prevSib;
                prevSib->childPtr.next[2]->parentIndex = 2;
            }
            else {
                prevSib->childPtr.data[2] = nptr->childPtr.data[0];
                prevSib->childPtr.data[2]->parent = prevSib;
                prevSib->childPtr.data[2]->parentIndex = 2;
            }
            free(nptr->TimeNodes[0]);
            free(nptr);
            parent->TimeNodes[1] = NULL;
            parent->childPtr.next[2] = NULL;
            parent->size--;
            prevSib->size++;
            return prevSib;
        }
    }
}

FlightTreeNode* mergeSib(FlightDLLNode *dataNode, FlightTreeNode *par) {
    if (dataNode->parentIndex != par->size) {
        printf("data node is NOT at parent size %d data node: %d\n", par->size, dataNode->parentIndex);
        FlightDLLNode *next = dataNode->next;
        printf("Next first %d\n", next->fList[0]->flightId);
        int i = 0;
        while (i < next->FDLLsize) {
            dataNode->fList[dataNode->FDLLsize] = next->fList[i];
            dataNode->FDLLsize++;
            i++;
        }
        dataNode->next = next->next;
        if (next->next) {
            next->next->prev = dataNode;
        }
        next->parent->childPtr.data[next->parentIndex] = NULL;
        free(next);
        if (dataNode->parent->size == 2 && dataNode->parentIndex == 0) {
            dataNode->parent->TimeNodes[0] = dataNode->parent->TimeNodes[1];
            dataNode->parent->TimeNodes[1] = NULL;
            par->childPtr.data[1] = par->childPtr.data[2];
            par->childPtr.data[2] = NULL;
        } else if (dataNode->parentIndex == 0) {
            printf("data node index zero and parent not full\n");
            printf("parent timeNode: %d:%d\n", par->TimeNodes[0]->hr, par->TimeNodes[0]->min);
            printf("Data node size: %d\n", dataNode->FDLLsize);
            printf("datanode last timeNode: %d:%d\n", dataNode->fList[dataNode->FDLLsize - 1]->dep->hr, dataNode->fList[dataNode->FDLLsize - 1]->dep->min);
            // par = borrowFromParSib(par, par->parent);
            par->TimeNodes[0]->hr = dataNode->fList[dataNode->FDLLsize - 1]->dep->hr;
            par->TimeNodes[0]->min = dataNode->fList[dataNode->FDLLsize - 1]->dep->min + 1;
            printf("parent timeNode after set: %d:%d\n", par->TimeNodes[0]->hr, par->TimeNodes[0]->min);
            printf("Hola!!\n");
        }
        par->size--;
        printf("Parent size: %d\n", par->size);
    }
    else {
        printf("data node is at parent size %d\n", par->size);
        FlightDLLNode *prev = dataNode->prev;
        int i = 0;
        while (i < dataNode->FDLLsize) {
            prev->fList[prev->FDLLsize] = dataNode->fList[i];
            prev->FDLLsize++;
            i++;
        }
        prev->next = dataNode->next;
        if (dataNode->next) {
            dataNode->next->prev = prev;
        }
        dataNode->parent->childPtr.data[dataNode->parentIndex] = NULL;
        free(dataNode);
        free(prev->parent->TimeNodes[prev->parentIndex]);
        prev->parent->TimeNodes[prev->parentIndex] = NULL;
        par->size--;
        dataNode = prev;
    }
    if (par && par->size < MINORDER - 1) {
        printf("borrow from parent sib\n");
        par = borrowFromParSib(par, par->parent);
    }
    return par;
}

FlightDLLNode *findingSpot(FlightTreeNode *root, Time *t) {
    int toGo = 0;
    while (toGo < root->size && timeDiff(t, root->TimeNodes[toGo]) >= 0) {
        toGo++;
    }
    if (toGo != 0)
        printf("To the right of time %d:%d\n", root->TimeNodes[toGo - 1]->hr, root->TimeNodes[toGo - 1]->min);
    else
        printf("To the left of %d:%d\n", root->TimeNodes[toGo]->hr, root->TimeNodes[toGo]->min);
    if (root->UTag == leaf) {
        return root->childPtr.data[toGo];
    }
    else
        return findingSpot(root->childPtr.next[toGo], t);
}

Bucket* deleteFlightPlan(Bucket *AVLroot, FlightPlan *oldFP) {
    Bucket *perfBuck = justFindBucket(AVLroot, oldFP->arr->hr);
    if (!AVLroot || !perfBuck->root) {
        printf("1.Flight not found!\n");
        return NULL;
    }
    FlightTreeNode *perfRoot = perfBuck->root;
    FlightDLLNode *perfDLLNode = findingSpot(perfRoot, oldFP->dep);
    FlightPlan *perfPlan = NULL;
    int i = 0;
    while ((i < perfDLLNode->FDLLsize) && (oldFP->flightId != perfDLLNode->fList[i]->flightId)) {
        printf("Input: %d, Checking %d\n", oldFP->flightId, perfDLLNode->fList[i]->flightId);
        i++;
    }
    printf("i: %d\n", i);
    if (i < perfDLLNode->FDLLsize)
        printf("F id: %d, Dep TIme: %d:%d, Arr Time: %d:%d\n", perfDLLNode->fList[i]->flightId, perfDLLNode->fList[i]->dep->hr, perfDLLNode->fList[i]->dep->min, perfDLLNode->fList[i]->arr->hr, perfDLLNode->fList[i]->arr->min);
    if (i < perfDLLNode->FDLLsize)
        perfPlan = perfDLLNode->fList[i];
    else {
        printf("2.Flight not found!\n");
        return NULL;
    }
    free(perfPlan->arr);
    free(perfPlan->dep);
    free(perfPlan);
    printf("Size: %d i: %d\n", perfDLLNode->FDLLsize, i);
    while (i < perfDLLNode->FDLLsize - 1) {
        perfDLLNode->fList[i] = perfDLLNode->fList[i + 1];
        i++;
    }
    perfDLLNode->fList[i] = NULL;
    perfDLLNode->FDLLsize--;
    if (perfDLLNode->FDLLsize < MIN) {
        printf("Inside Min\n");
        bool flag = borrowFromSib(perfDLLNode);
        if (!flag) {
            printf("Could not borrow from sib\n");
            perfDLLNode->parent = mergeSib(perfDLLNode, perfDLLNode->parent);
        }
    }
    return AVLroot;
}

void ShowStatusFlight(FlightTreeNode* root, FlightPlan* fp){
    if(!root)
        printf("No such flight timings found\n");
    else{
        int i = 0;
        while(i<root->size && timeDiff(fp->dep,root->TimeNodes[i]) >= 0){
            i++;   
        }
        if(root->UTag == inter)
            ShowStatusFlight(root->childPtr.next[i],fp);
        else{ 
            FlightDLLNode *required = root->childPtr.data[i];
            int j = 0;
            while(j < required->FDLLsize && timeDiff(fp->dep,required->fList[j]->dep) > 0){
                j++;
            }
            if(timeDiff(fp->dep, required->fList[j]->dep) == 0 && fp->flightId == required->fList[j]->flightId){
                printf("Flight found :) \n");
                printf("Flight ID: %d \n",required->fList[j]->flightId);
                printf("Flight departure time %d:%d \n",required->fList[j]->dep->hr,required->fList[j]->dep->min);
                printf("Arrival Time %d:%d\n\n",required->fList[j]->arr->hr,required->fList[j]->arr->min);
            }
            else
                printf("No such flight schedule found :( ");
        }
    }
}

void ShowStatus(Bucket *buckroot,FlightPlan *fp){
    if(!buckroot)
        printf("No such bucket time found\n");
    else{
        if(fp->arr->hr == buckroot->ETA_Beg->hr)
            ShowStatusFlight(buckroot->root,fp);
        else if(fp->arr->hr > buckroot->ETA_Beg->hr)
            ShowStatus(buckroot->right,fp);
        else
            ShowStatus(buckroot->left,fp);
    }
}

FlightDLLNode* findHead(FlightTreeNode *root) {
    if (root->UTag == leaf) {
        int i = 0;
        while (!root->childPtr.data[i]) {
            i++;
        }
        return root->childPtr.data[i];
    }
    return findHead(root->childPtr.next[0]);
}

void ShowFlight(FlightPlan *fp){
    if(fp != NULL)
    {
        printf("Flight Arrival Time : %d:%d\n", fp->arr->hr,fp->arr->min);
        printf("Departure Time: %d:%d\n", fp->dep->hr,fp->arr->min);
        printf("Flight ID: %d\n",fp->flightId);
    }
    
}

void ShowRange(Bucket* bucketroot,Time* t1,Time* t2){
    FlightDLLNode* head = findHead(bucketroot->root);
    int i =0;
    while(head){
        i = 0;
        while(i < CAPACITY){
            if(t1->hr == t2->hr){
                if(head->fList[i] != NULL && head->fList[i]->arr->min > t1->min && head->fList[i]->arr->min < t2->min)
                    ShowFlight(head->fList[i]);
            }
            else{
                if(bucketroot->ETA_Beg->hr == t1->hr){
                    if(head->fList[i] != NULL && head->fList[i]->arr->min >= t1->min)
                        ShowFlight(head->fList[i]);  
                }
                else if(head->fList[i] != NULL && bucketroot->ETA_Beg->hr > t1->hr && bucketroot->ETA_Beg->hr < t2->hr){
                    ShowFlight(head->fList[i]);
                }
                else if(head->fList[i] != NULL && bucketroot->ETA_Beg->hr == t2->hr){
                    if(head->fList[i]->arr->min <= t2->min)
                        ShowFlight(head->fList[i]);  
                }
            }
            i++;
        }
        head = head->next;
    }     
}

void InorderTraversal(Bucket* buckroot, Time* t1, Time* t2) {
    if (buckroot) {
        InorderTraversal(buckroot->left,t1, t2);
        if (buckroot->ETA_Beg->hr >= t1->hr && buckroot->ETA_Beg->hr <= t2->hr)
            ShowRange(buckroot, t1, t2);
        InorderTraversal(buckroot->right, t1, t2);
    }
}

void findRange(Bucket* buckroot , Time* t1, Time* t2){
    InorderTraversal(buckroot,t1,t2);
}

void printFlightPlan(FlightTreeNode *root) {
    FlightDLLNode* head = findHead(root);
    while (head) {
        for (int i = 0; i < head->FDLLsize; i++) {
            printf("Flight ID: %d, Flight arr- %d:%d, Flight dep- %d:%d\n", head->fList[i]->flightId, head->fList[i]->arr->hr, head->fList[i]->arr->min, head->fList[i]->dep->hr, head->fList[i]->dep->min);
        }
        printf("\n");
        head = head->next;
    }
}

Bucket *readData(Bucket *root, char *fileName) {
    FILE *f = fopen(fileName, "r");
    int id, depHr, depMin, arrHr, arrMin;
    fscanf(f, "%*s,%*s,%*s,%*s,%*s\n");
    int i = 1;
    while (!feof(f)) {
        fscanf(f, "%d,%d,%d,%d,%d\n", &id, &depHr, &depMin, &arrHr, &arrMin);
        Time *dep = makeTime(depHr, depMin);
        Time *arr = makeTime(arrHr, arrMin);
        FlightPlan *fp = makeFlightPlan(id, dep, arr);
        root = addNewFlight(root, fp);
        i++;
    }

    fclose(f);
    return root;
}

void printAll(Bucket *dash) {
    if (dash) {
        printAll(dash->left);
        printFlightPlan(dash->root);
        printf("-x-x-x-x-x-x-x-x-x-x-x-x-\n\n");
        printAll(dash->right);
    }
}

void main() {
    Bucket *Broot = NULL;
    Broot = readData(Broot, "flights.csv");
    printf("%d", Broot->buckId);
    printAll(Broot);
    Time *t31 = makeTime(14, 58);
    Time *t32 = makeTime(5, 58);
    
    Time *t1 = makeTime(14, 24);
    Time *t2 = makeTime(0, 45);
    FlightPlan *fp = makeFlightPlan(12, t2, t1);
    printf("\n\n");
    Broot = deleteFlightPlan(Broot, fp);
    printAll(Broot);
    Time *t3 = makeTime(1, 54);
    Time *t4 = makeTime(5, 20);
    FlightPlan *fp1 = makeFlightPlan(134, t4, t3);
    printf("\n\n");
    Broot = deleteFlightPlan(Broot, fp1);
    printAll(Broot);
    Time *t5 = makeTime(14, 34);
    Time *t6 = makeTime(8, 45);
    Time *t7 = makeTime(0, 15);
    Time *t8 = makeTime(1, 45);
    FlightPlan *fp2 = makeFlightPlan(17, t6, t5);
    printf("\n");
    ShowStatus(Broot,fp2);
    printf("\n");
    findRange(Broot,t7,t8);
}