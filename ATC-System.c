#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum {am, pm} AntePost;

typedef struct TIME_NODE {
    int hr;
    int min;
    AntePost meridiem;
} TIME;

typedef struct Bucket_Node {
    
} Bucket;