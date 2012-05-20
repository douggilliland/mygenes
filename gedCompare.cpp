/////////////////////////////////////////////////////////////////////////
// gedCompare.cpp
// Compare two GEDCOMs
// Dump differences (missing parents) between files.
/////////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gedCompare.h"

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

FILE * ged1FilePtr;
FILE * ged2FilePtr;

struct gedStruc {
    int iNumber;
    int father;
    int mother;
    int otherTreeIndiv;
    char iName[256];
    char iBirthDate[256];
};

gedStruc array1[32768];
gedStruc array2[32768];

char ged1Line[32768];
char ged2Line[32768];

int file1State;
int file2State;

int file1Size;
int file2Size;
int family1Size;
int family2Size;

int fatherINum;
int motherINum;

/////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////

int openFiles(void);
int processFiles(void);
int closeFiles(void);
int parseLine(int);
int fillArrays(void);
int reportStats(void);
int checkFirstName(void);
int compareTree1vs2(void);
int searchString(char *, char *);
int printStruct(void);

/////////////////////////////////////////////////////////////////////////
// int printStruct(void)
/////////////////////////////////////////////////////////////////////////

int printStruct(void)
{
    int loopCount;
    for (loopCount = 0; loopCount < file1Size; loopCount++)
    {
        printf("Record Num : %d\n", array1[loopCount].iNumber);
        printf("Other Rec  : %d\n", array1[loopCount].otherTreeIndiv);
        printf("Individual : %s\n", array1[loopCount].iName);
        printf("Birthdate  : %s\n", array1[loopCount].iBirthDate);
        if (array1[loopCount].father != -1)
        {
            printf("Father: %d %s\n", array1[loopCount].father, array1[array1[loopCount].father - 1].iName);
        }
        if (array1[loopCount].mother != -1)
        {
            printf("Mother: %d %s\n", array1[loopCount].mother, array1[array1[loopCount].mother - 1].iName);
        }
        printf("\n");
    }
    for (loopCount = 0; loopCount < file2Size; loopCount++)
    {
        printf("Record Num : %d\n", array2[loopCount].iNumber);
        printf("Other Rec  : %d\n", array2[loopCount].otherTreeIndiv);
        printf("Individual : %s\n", array2[loopCount].iName);
        printf("Birthdate  : %s\n", array2[loopCount].iBirthDate);
        if (array2[loopCount].father != -1)
        {
            printf("Father: %d %s\n", array2[loopCount].father, array2[array2[loopCount].father - 1].iName);
        }
        if (array2[loopCount].mother != -1)
        {
            printf("Mother: %d %s\n", array2[loopCount].mother, array2[array2[loopCount].mother - 1].iName);
        }
        printf("\n");
    }
    return NOERR;
}

/////////////////////////////////////////////////////////////////////////
// int searchString(char * stringToSearch, char * searchForString)
// Search a string
// Return 0 if no match
// Return 1 offset to end of string
// 1234567890asdfghjkl searchingString
// ......7890......... searchKey
// 0123456789^ returns 10
/////////////////////////////////////////////////////////////////////////

int searchString(char * searchingString, char * searchKey)
{
    int lenSearchingString;
    int lenSearchKey;
    int offsetSearchKey = 0;
    int offsetSearchingString = 0;
    int matched;

    lenSearchKey = strlen(searchKey);
    lenSearchingString = strlen(searchingString);

    if (lenSearchKey > lenSearchingString)
        return 0;

    do
    {
        if (searchingString[offsetSearchingString] == searchKey[offsetSearchKey])
        {
            matched = 1;
            offsetSearchingString++;
            offsetSearchKey++;
        }
        else
        {
            matched = 0;
            offsetSearchKey = 0;
            offsetSearchingString++;
        }
        if (offsetSearchKey == lenSearchKey)
            return(offsetSearchingString);
    }
    while (offsetSearchingString < lenSearchingString);
    return 0;
}

/////////////////////////////////////////////////////////////////////////
// int compareTree1vs2(void)
// int file1Size;
// int file2Size;
/*struct gedStruc {
    int iNumber;
    int father;
    int mother;
    int otherTreeFather;
    int otherTreeMother;
    char iName[256];
    char iBirthDate[256];
};
gedStruc array1[16384];
gedStruc array2[16384];*/
/////////////////////////////////////////////////////////////////////////

int compareTree1vs2(void)
{
    int fileLoopCount = 0;
#ifdef DEBUG2
    printf("Comparing the trees 1 vs 2\n");
#endif
    fileLoopCount = 0;
    array1[0].otherTreeIndiv = 1; // both start with 1
    do
    {
        // check fathers
        if ((array1[fileLoopCount].father != -1) &&
                (array2[(array1[fileLoopCount].otherTreeIndiv) - 1].father != -1))
        {
            array1[fileLoopCount].otherTreeIndiv = (array2[array1[fileLoopCount].otherTreeIndiv].iNumber) - 1;
            if (strcmp(array1[fileLoopCount].iName, array2[(array1[fileLoopCount].otherTreeIndiv) - 1].iName) != 0)
            {
                printf("*** Mismatched Names ***\n");
                printf("Tree 1 has name for this individual %d, %s\n", array1[fileLoopCount].iNumber, array1[fileLoopCount].iName);
                printf("Tree 2 has name for this individual %d, %s\n", array1[fileLoopCount].otherTreeIndiv, array2[(array1[fileLoopCount].otherTreeIndiv) - 1].iName);
            }
#ifdef DEBUG
            else
                printf("Names Matched\n");
#endif
#ifdef DEBUG
            printf("Both trees have fathers for this individual %d, %s\n", array1[fileLoopCount].iNumber, array1[fileLoopCount].iName);
            printf("Both trees have fathers for this individual %d, %s\n", array1[fileLoopCount].otherTreeIndiv, array2[(array1[fileLoopCount].otherTreeIndiv) - 1].iName);
#endif
            array1[(array1[fileLoopCount].father) - 1].otherTreeIndiv = array2[(array1[fileLoopCount].father) - 1].iNumber;
        } else
        {
#ifdef DEBUG2
            printf("*** Missing parent ***\n");
            printf("First file has no father for individual %d, %s\n", array1[fileLoopCount].iNumber, array1[fileLoopCount].iName);
            printf("Second file has father for individual %d, %s\n", (array1[fileLoopCount].otherTreeIndiv), array2[(array1[fileLoopCount].otherTreeIndiv) - 1].iName);
#endif
            //            return(OPENBRANCHFOUND);
        }
        // check mothers
        if ((array1[fileLoopCount].mother != -1) &&
                (array2[(array1[fileLoopCount].otherTreeIndiv) - 1].mother != -1))
        {
            array1[fileLoopCount].otherTreeIndiv = (array2[array1[fileLoopCount].otherTreeIndiv].iNumber) - 1;

#ifdef DEBUG
            
                printf("Names Matched\n");
#endif
#ifdef DEBUG
            printf("Both trees have mothers for this individual %d, %s\n", array1[fileLoopCount].iNumber, array1[fileLoopCount].iName);
            printf("Both trees have mothers for this individual %d, %s\n", array1[fileLoopCount].otherTreeIndiv, array2[(array1[fileLoopCount].otherTreeIndiv) - 1].iName);
#endif
            array1[(array1[fileLoopCount].mother) - 1].otherTreeIndiv = array2[(array1[fileLoopCount].mother) - 1].iNumber;
        } else
        {
#ifdef DEBUG2
            printf("*** Missing parent ***\n");
            printf("First file has no mother for individual %d, %s\n", array1[fileLoopCount].iNumber, array1[fileLoopCount].iName);
            printf("Second file has mother for individual %d, %s\n", (array1[fileLoopCount].otherTreeIndiv), array2[(array1[fileLoopCount].otherTreeIndiv) - 1].iName);
#endif
            return (OPENBRANCHFOUND);
        }

        fileLoopCount++;
    } while (fileLoopCount < file1Size);
    return (NOERR);
}


/////////////////////////////////////////////////////////////////////////
// int compareTree2vs1(void)
// int file1Size;
// int file2Size;
/*struct gedStruc {
    int iNumber;
    int father;
    int mother;
    int otherTreeFather;
    int otherTreeMother;
    char iName[256];
    char iBirthDate[256];
};
gedStruc array1[16384];
gedStruc array2[16384];*/
/////////////////////////////////////////////////////////////////////////

int compareTree2vs1(void)
{
    int fileLoopCount = 0;
#ifdef DEBUG2
    printf("Comparing the trees 2 vs 1\n");
#endif
    fileLoopCount = 0;
    array2[0].otherTreeIndiv = 1; // both start with 1
    do
    {
        // check fathers
        if ((array2[fileLoopCount].father > 0) &&
                (array1[(array2[fileLoopCount].otherTreeIndiv) - 1].father > 0))
        {
            array2[fileLoopCount].otherTreeIndiv = (array1[array2[fileLoopCount].otherTreeIndiv].iNumber) - 1;
            if (strcmp(array2[fileLoopCount].iName, array1[(array2[fileLoopCount].otherTreeIndiv) - 1].iName) != 0)
            {
                printf("*** Mismatched Names ***\n");
                printf("Tree 1 has name for this individual %d, %s\n", array2[fileLoopCount].iNumber, array2[fileLoopCount].iName);
                printf("Tree 2 has name for this individual %d, %s\n", array2[fileLoopCount].otherTreeIndiv, array1[(array2[fileLoopCount].otherTreeIndiv) - 1].iName);
            }
#ifdef DEBUG
            else
                printf("Names Matched\n");
#endif
#ifdef DEBUG
            printf("Both trees have fathers for this individual %d, %s\n", array2[fileLoopCount].iNumber, array2[fileLoopCount].iName);
            printf("Both trees have fathers for this individual %d, %s\n", array2[fileLoopCount].otherTreeIndiv, array1[(array2[fileLoopCount].otherTreeIndiv) - 1].iName);
#endif
            array2[(array2[fileLoopCount].father) - 1].otherTreeIndiv = array1[(array2[fileLoopCount].father) - 1].iNumber;
        } else if (array2[fileLoopCount].father != -1)
        {
#ifdef DEBUG2
            printf("*** Missing parent ***\n");
            printf("First file has no father for individual %d, %s\n", array2[fileLoopCount].iNumber, array2[fileLoopCount].iName);
            printf("Second file has father for individual %d, %s\n", (array2[fileLoopCount].otherTreeIndiv), array1[(array2[fileLoopCount].otherTreeIndiv) - 1].iName);
#endif
            //            return(OPENBRANCHFOUND);
        }
        // check mothers
        if ((array2[fileLoopCount].mother > 0) &&
                (array1[(array2[fileLoopCount].otherTreeIndiv) - 1].mother > 0))
        {
            array2[fileLoopCount].otherTreeIndiv = (array1[array2[fileLoopCount].otherTreeIndiv].iNumber) - 1;

#ifdef DEBUG

                printf("Names Matched\n");
#endif
#ifdef DEBUG
            printf("Both trees have mothers for this individual %d, %s\n", array2[fileLoopCount].iNumber, array2[fileLoopCount].iName);
            printf("Both trees have mothers for this individual %d, %s\n", array2[fileLoopCount].otherTreeIndiv, array1[(array2[fileLoopCount].otherTreeIndiv) - 1].iName);
#endif
            array2[(array2[fileLoopCount].mother) - 1].otherTreeIndiv = array1[(array2[fileLoopCount].mother) - 1].iNumber;
        } else if (array2[fileLoopCount].mother != -1)
        {
#ifdef DEBUG2
            printf("*** Missing parent ***\n");
            printf("First file has no mother for individual %d, %s\n", array2[fileLoopCount].iNumber, array2[fileLoopCount].iName);
            printf("Second file has mother for individual %d, %s\n", (array2[fileLoopCount].otherTreeIndiv), array1[(array2[fileLoopCount].otherTreeIndiv) - 1].iName);
#endif
            //            return (OPENBRANCHFOUND);
        }

        fileLoopCount++;
    } while (fileLoopCount < file2Size);
    return (NOERR);
}


/////////////////////////////////////////////////////////////////////////
// int openFiles(void)
/////////////////////////////////////////////////////////////////////////

int openFiles(void)
{
    int retVal = NOERR;

    ged1FilePtr = fopen("testData/ged1.ged", "r");
    if (ged1FilePtr == NULL)
    {
        perror("Error opening file 1");
        return ERRORLOADINGGED1;
    }
    ged2FilePtr = fopen("testData/ged2.ged", "r");
    if (ged2FilePtr == NULL)
    {
        perror("Error opening file 2");
        fclose(ged1FilePtr);
        return ERRORLOADINGGED2;
    }
    return (NOERR);
}

/////////////////////////////////////////////////////////////////////////
// int readLine(int bufferNumber)
/////////////////////////////////////////////////////////////////////////

int readLine(int bufferNumber)
{
    int retVal = NOERR;
    int lineOffset = 0;
    if (bufferNumber == 0)
    {
        do
        {
            retVal = fgetc(ged1FilePtr);
            if (retVal == EOF)
            {
                ged1Line[lineOffset] = 0;
                return (EOFREACHED);
            } else if (retVal == '\n')
            {
                ged1Line[lineOffset] = 0;
                if (strlen(ged1Line) > 0)
                    if (ged1Line[lineOffset - 1] == '\r')
                        ged1Line[lineOffset - 1] = 0;
                return NOERR;
            } else
            {
                ged1Line[lineOffset] = retVal;
            }
            lineOffset++;
        } while (lineOffset < 4095);
    } else
    {
        do
        {
            retVal = fgetc(ged2FilePtr);
            if (retVal == EOF)
            {
                ged2Line[lineOffset] = 0;
                return (EOFREACHED);
            } else if (retVal == '\n')
            {
                ged2Line[lineOffset] = 0;
                if (strlen(ged2Line) > 0)
                    if (ged2Line[lineOffset - 1] == '\r')
                        ged2Line[lineOffset - 1] = 0;
                return NOERR;
            } else
            {
                ged2Line[lineOffset] = retVal;
            }
            lineOffset++;
        } while (lineOffset < 4095);
    }
    return NOERR;
}

/////////////////////////////////////////////////////////////////////////
// int parseLine(int)
//  int file1State;
//  int file2State;
//      STARTSTATE
//      LOOKINGFORINDNUM
//      LOOKINGFORNAME
//      LOOKINGFORBDATE

/////////////////////////////////////////////////////////////////////////

int parseLine(int fileNumber)
{
    int lineOffset = 0;
//    int indNum;
    char tempString[256];
    int childINum;

    if (fileNumber == 0)
    {
        switch (file1State) {
            case STARTSTATE:
                file1Size = 0; // current record number
                if (searchString(ged1Line,"0 HEAD") > 0)
                {
#ifdef DEBUG
                    printf("Got header 1\n");
#endif
                    file1State = LOOKINGFORINDNUM;
                }
                else
                    return(NOHEADER1);
                break;
            case LOOKINGFORINDNUM:
                //0 @I3@ INDI
                if (searchString(ged1Line,"0 @I") > 0)
                {
                    do
                    {
                        tempString[lineOffset] = ged1Line[4 + lineOffset];
                        lineOffset++;
                    } while ((lineOffset < 32)
                            && (ged1Line[4 + lineOffset] != 0)
                            && (ged1Line[4 + lineOffset] != '@'));
                    tempString[lineOffset] = 0;
                    array1[file1Size].iNumber = atoi(tempString);
                    array1[file1Size].father = -1;
                    array1[file1Size].mother = -1;
                    file1State = LOOKINGFORNAME;
                }
                else if (searchString(ged1Line,"0 @F") > 0)
                {
                    fatherINum = -1;
                    motherINum = -1;
                    file1State = LOOKINGFORFAMILY;
                    family1Size++;
                }
                break;
                // 0 @F1@ FAM
                // 1 _UID 1C24472F4C95924C8ED3E3DEEFF5954A5A64
                // 1 HUSB @I2@
                // 1 WIFE @I3@
                // 1 CHIL @I1@
            case LOOKINGFORFAMILY:
            {
                if (searchString(ged1Line,"1 H") > 0)
                {
                    do
                    {
                        tempString[lineOffset] = ged1Line[9 + lineOffset];
                        lineOffset++;
                    } while ((lineOffset < 32)
                            && (ged1Line[9 + lineOffset] != 0)
                            && (ged1Line[9 + lineOffset] != '@'));
                    tempString[lineOffset] = 0;
                    fatherINum = atoi(tempString);
                }
                else if (searchString(ged1Line,"1 W") > 0)
                {
                    do
                    {
                        tempString[lineOffset] = ged1Line[9 + lineOffset];
                        lineOffset++;
                    } while ((lineOffset < 32)
                            && (ged1Line[9 + lineOffset] != 0)
                            && (ged1Line[9 + lineOffset] != '@'));
                    tempString[lineOffset] = 0;
                    motherINum = atoi(tempString);
                }
                else if (searchString(ged1Line,"1 C") > 0)
                {
                    do
                    {
                        tempString[lineOffset] = ged1Line[9 + lineOffset];
                        lineOffset++;
                    } while ((lineOffset < 32)
                            && (ged1Line[9 + lineOffset] != 0)
                            && (ged1Line[9 + lineOffset] != '@'));
                    tempString[lineOffset] = 0;
                    childINum = atoi(tempString);
                    array1[childINum - 1].father = fatherINum;
                    array1[childINum - 1].mother = motherINum;
#ifdef DEBUG
                    printf("Individual %d, Father %d, Mother %d\n", childINum, fatherINum, motherINum);
#endif
                }
                else if (searchString(ged1Line,"0 @F"))
                {
                    fatherINum = -1;
                    motherINum = -1;
                    family1Size++;
                }
                break;
            }
            case LOOKINGFORNAME:
                if (searchString(ged1Line, "1 NAME") > 0)
                {
                    strcpy(array1[file1Size].iName, (char *) & ged1Line[7]);
                    file1State = LOOKINGFORBDATE;
                }
                break;
            case LOOKINGFORBDATE:
                if (searchString(ged1Line, "1 BIRT") > 0)
                {
#ifdef DEBUG
                    printf("bdflag set\n");
#endif
                    file1State = LOOKINGFORBDVAL;
                }//                1 _UID 125798D09A0BC74EB538DB12DD0BF16FADE0

                else if (searchString(ged1Line,"1 _UID") > 0)
                {
                    file1State = LOOKINGFORINDNUM;
                    file1Size++;
                }
                break;
            case LOOKINGFORBDVAL:
            {
                strcpy(array1[file1Size].iBirthDate, (char *) & ged1Line[7]);
#ifdef DEBUG
                printf("Individual number: %d\n", array1[file1Size].iNumber);
                printf("Name: %s\n", array1[file1Size].iName);
                printf("birthday: %s\n", array1[file1Size].iBirthDate);
#endif
                file1Size++;
                file1State = LOOKINGFORINDNUM;
            }

        }
    } else
    {
        switch (file2State) {
            case STARTSTATE:
                file2Size = 0; // current record number
                if (searchString(ged2Line,"0 HEAD") > 0)
                {
#ifdef DEBUG
                    printf("Got header 2\n");
#endif
                    file2State = LOOKINGFORINDNUM;
                }
                else
                    return (NOHEADER2);
                break;
            case LOOKINGFORINDNUM:
                //0 @I3@ INDI
                if (searchString(ged2Line,"0 @I") > 0)
                {
                    do
                    {
                        tempString[lineOffset] = ged2Line[4 + lineOffset];
                        lineOffset++;
                    } while ((lineOffset < 32)
                            && (ged2Line[4 + lineOffset] != 0)
                            && (ged2Line[4 + lineOffset] != '@'));
                    tempString[lineOffset] = 0;
                    array2[file2Size].iNumber = atoi(tempString);
                    array2[file2Size].father = -1;
                    array2[file2Size].mother = -1;
                    file2State = LOOKINGFORNAME;
                }
                else if (searchString(ged2Line,"0 @F") > 0)
                {
                    fatherINum = -1;
                    motherINum = -1;
                    family2Size++;
                    file2State = LOOKINGFORFAMILY;
                }
                break;
            case LOOKINGFORFAMILY:
            {
                lineOffset = 0;
                if (searchString(ged2Line,"1 H") > 0)
                {
                    do
                    {
                        tempString[lineOffset] = ged2Line[9 + lineOffset];
                        lineOffset++;
                    } while ((lineOffset < 32)
                            && (ged2Line[9 + lineOffset] != 0)
                            && (ged2Line[9 + lineOffset] != '@'));
                    tempString[lineOffset] = 0;
                    fatherINum = atoi(tempString);
                }
                else if (searchString(ged2Line,"1 W") > 0)
                {
                    do
                    {
                        tempString[lineOffset] = ged2Line[9 + lineOffset];
                        lineOffset++;
                    } while ((lineOffset < 32)
                            && (ged2Line[9 + lineOffset] != 0)
                            && (ged2Line[9 + lineOffset] != '@'));
                    tempString[lineOffset] = 0;
                    motherINum = atoi(tempString);
                }
                else if (searchString(ged2Line,"1 C") > 0)
                {
                    do
                    {
                        tempString[lineOffset] = ged2Line[9 + lineOffset];
                        lineOffset++;
                    } while ((lineOffset < 32)
                            && (ged2Line[9 + lineOffset] != 0)
                            && (ged2Line[9 + lineOffset] != '@'));
                    tempString[lineOffset] = 0;
                    childINum = atoi(tempString);
                    array2[childINum - 1].father = fatherINum;
                    array2[childINum - 1].mother = motherINum;
#ifdef DEBUG
                    printf("Parents of Individual %d, Father %d, Mother %d\n", childINum, fatherINum, motherINum);
#endif
                } 
                else if (searchString(ged2Line,"0 @F") > 0)
                {
                    fatherINum = -1;
                    motherINum = -1;
                    family2Size++;
                }
                break;
            }
            case LOOKINGFORNAME:
                if (searchString(ged2Line,"1 NAME") > 0)
                {
                    strcpy(array2[file2Size].iName, (char *) & ged2Line[7]);
                    file2State = LOOKINGFORBDATE;
                }
                break;
            case LOOKINGFORBDATE:
                if (searchString(ged2Line, "1 BIRT") > 0)
                {
#ifdef DEBUG
                    printf("bdflag set\n");
#endif
                    file2State = LOOKINGFORBDVAL;
                }//                1 _UID 125798D09A0BC74EB538DB12DD0BF16FADE0

                else if (searchString(ged2Line,"1 _UID") > 0)
                {
                    file2State = LOOKINGFORINDNUM;
                    file2Size++;
                }
                break;
            case LOOKINGFORBDVAL:
            {
                strcpy(array2[file2Size].iBirthDate, (char *) & ged2Line[7]);
#ifdef DEBUG
                printf("Individual number: %d\n", array2[file2Size].iNumber);
                printf("Name: %s\n", array2[file2Size].iName);
                printf("birthday: %s\n", array2[file2Size].iBirthDate);
#endif
                file2Size++;
                file2State = LOOKINGFORINDNUM;
            }

        }

    }
    return NOERR;
}

/////////////////////////////////////////////////////////////////////////
// int fillArrays(void)
/////////////////////////////////////////////////////////////////////////

int fillArrays(void)
{
    int retVal, retVal2;
    file1State = STARTSTATE;
    file2State = STARTSTATE;
    family1Size = 0;
    family2Size = 0;

#ifdef DEBUG
    printf("*** Processing First File ***\n");
#endif
    do
    {
        retVal = readLine(0);
        if (retVal > NOERR)
            return (retVal);
        retVal2 = parseLine(0);
        if (retVal2 != NOERR)
            return (retVal2);
    } while (retVal == NOERR);
#ifdef DEBUG
    printf("*** Processing Second File ***\n");
#endif
    do
    {
        retVal = readLine(1);
        if (retVal > NOERR)
            return (retVal);
        retVal2 = parseLine(1);
        if (retVal2 != NOERR)
            return (retVal2);
    } while (retVal == NOERR);
    return (NOERR);
}

/////////////////////////////////////////////////////////////////////////
// int reportStats(void)
/////////////////////////////////////////////////////////////////////////

int reportStats(void)
{
    printf("Statistics\n");
    printf("File 1 has %d individuals\n", file1Size);
    printf("File 1 has %d families\n", family1Size);
    printf("File 2 has %d individuals\n", file2Size);
    printf("File 2 has %d families\n", family2Size);
	return (NOERR);
}

/////////////////////////////////////////////////////////////////////////
// int checkFirstName(void)
/////////////////////////////////////////////////////////////////////////

int checkFirstName(void)
{
    int matches = 1;

    if (strcmp(array1[0].iName, array2[0].iName) != 0)
    {
        printf("Mismatched first names\n");
        printf("%s\n", array1[0].iName);
        printf("%s\n", array2[0].iName);
        matches = 0;
    }
    if (strcmp(array1[0].iBirthDate, array2[0].iBirthDate) != 0)
    {
        printf("Mismatched first birthdates\n");
        matches = 0;
    }
    if (matches == 0)
    {
        printf("Mismatched starting persons\n");
        return (MATCH1STERR);
    } else
    {
#ifdef DEBUG
        printf("Starting persons matched\n");
#endif
        return (NOERR);
    }
}

/////////////////////////////////////////////////////////////////////////
// int processFiles(void)
/////////////////////////////////////////////////////////////////////////

int processFiles(void)
{
    int retVal = NOERR;
    retVal = fillArrays();
    if (retVal != NOERR)
        return (retVal);
#ifdef DEBUG
    printf("Filled arrays\n");
#endif
    reportStats();
    if (checkFirstName() == MATCH1STERR)
    {
        printf("First person in the trees must match\n");
        return (NOERR);
    } else
    {
        compareTree1vs2();
        compareTree2vs1();

        return (NOERR);
    }
    return (NOERR);
}

/////////////////////////////////////////////////////////////////////////
// int closeFiles(void)
/////////////////////////////////////////////////////////////////////////

int closeFiles(void)
{
    int retVal = NOERR;
    fclose(ged1FilePtr);
    fclose(ged2FilePtr);
    return (NOERR);
}

/////////////////////////////////////////////////////////////////////////
// int gedCompare(void)
/////////////////////////////////////////////////////////////////////////

int gedCompare(void)
{
    int retVal = NOERR;

    retVal = openFiles();
    if (retVal != NOERR)
        return (retVal);
    retVal = processFiles();
    if (retVal != NOERR)
        return (retVal);
    retVal = closeFiles();
    if (retVal != NOERR)
        return (retVal);
    return NOERR;
}
