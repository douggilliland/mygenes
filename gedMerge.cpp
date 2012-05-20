/////////////////////////////////////////////////////////////////////////
// gedCompare.cpp
// Merge two GEDCOMs
/////////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gedCompare.h"

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

extern FILE * ged1FilePtr;
extern FILE * ged2FilePtr;

struct gedStruc {
    int iNumber;
    int father;
    int mother;
    int otherTreeIndiv;
    char iName[256];
    char iBirthDate[256];
};

extern gedStruc array1[32768];
extern gedStruc array2[32768];

extern char ged1Line[32768];
extern char ged2Line[32768];

extern int file1State;
extern int file2State;

extern int file1Size;
extern int file2Size;
extern int family1Size;
extern int family2Size;

extern int fatherINum;
extern int motherINum;

extern int printStruct(void);

/////////////////////////////////////////////////////////////////////////
// int gedMerge(void)
/////////////////////////////////////////////////////////////////////////

int gedMerge(void)
{
#ifdef DEBUG2
    printf("Reached gedMerge\n");
    printStruct();
#endif
    return NOERR;
}

