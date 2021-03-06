#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define OK    0
#define ERROR 1

#define FALSE 0
#define TRUE  1

#define VIDE       0
#define NEUTRALISE 100

#define FIRST_TEST 9
#define SIZE_NUMBER_ARRAY 128

#define NB_THREADS 2

unsigned long long nbAll[NB_THREADS][SIZE_NUMBER_ARRAY];
unsigned long long nbIrr[NB_THREADS][SIZE_NUMBER_ARRAY];
unsigned long long nbMM[NB_THREADS][SIZE_NUMBER_ARRAY];
unsigned long long nb1M[NB_THREADS][SIZE_NUMBER_ARRAY];
unsigned long long nbM1[NB_THREADS][SIZE_NUMBER_ARRAY];
unsigned long long nb11[NB_THREADS][SIZE_NUMBER_ARRAY];

typedef struct {
    int cellsNb;
    int firstX;
    int lastX;
    int nbExt;
    int nextTransitionReductible;
} SLICE;

SLICE sliceDef = {0, 128, -1, 0, FALSE};

typedef struct {
    int nbLig;
    int nbCol;
    char **tabLig;
    char *buffer;
    int nbSlices;
    SLICE *slices;
    int polySize;
    int targetSize;
    int maxY;
    int minX;
    int maxX;
    int firstSlice;
    int lastSlice;
} ARRAY;

ARRAY *arrayList[NB_THREADS][SIZE_NUMBER_ARRAY];

ARRAY *Create_Array(const int targetSize) {
    ARRAY *array = (ARRAY *) malloc(sizeof(ARRAY));

    if (array == NULL) {
        printf("Impossible d'allouer un objet de type ARRAY");
        return NULL;
    }
    array->targetSize = targetSize;
    array->nbLig = targetSize;
    array->nbCol = (2*targetSize-1);
    array->tabLig = (char **) malloc(array->nbLig * sizeof(char *));
    array->buffer = (char *) malloc(array->nbCol * array->nbLig * sizeof(char));
    array->nbSlices = array->nbCol+array->nbLig - 1;
    array->slices = (SLICE *) malloc(sizeof(SLICE) * array->nbSlices);

    if (array->tabLig == NULL) {
        printf("Impossible d'allouer tab de taille %d char*\n", array->nbLig);
    }
    else if (array->buffer == NULL) {
        printf("Impossible d'allouer buffer de taille %d char\n", array->nbCol * array->nbLig);
    }
    else if (array->slices == NULL) {
        printf("Impossible d'allouer slices de taille %d SLICES\n", array->nbSlices);
    }
    else {
        int i;

        for (i = 0; i < array->nbLig; i++) {
            array->tabLig[i] = array->buffer + i*array->nbCol;
        }

        array->polySize = 0;
        return array;
    }
    return NULL;
}


int Init_Array(ARRAY *array) {
    int size = array->nbLig;
    int status = OK;
    int s;

    memset(array->tabLig[0], NEUTRALISE, size-1);
    memset(array->tabLig[0]+size-1, VIDE, array->nbCol*size-(size-1));

    for (s = 0; s < array->nbSlices; s++)
        array->slices[s] = sliceDef;
    //array->sizePoly = 0;
    //array->maxY = 0;
    //array->minX = 0;
    //array->maxX = 0;
    return status;
}

int Copy_Array(ARRAY *arrayOut, ARRAY *arrayIn) {
    int status = OK;

    memcpy(arrayOut->buffer, arrayIn->buffer, arrayOut->nbCol*arrayIn->nbLig);

    arrayOut->targetSize = arrayIn->targetSize;
    arrayOut->polySize = arrayIn->polySize;
    arrayOut->maxY = arrayIn->maxY;
    arrayOut->minX = arrayIn->minX;
    arrayOut->maxX = arrayIn->maxX;
    arrayOut->firstSlice = arrayIn->firstSlice;
    arrayOut->lastSlice = arrayIn->lastSlice;

    memcpy(arrayOut->slices, arrayIn->slices, arrayOut->nbSlices*sizeof(SLICE));

    return status;
}

void Print_Tableau(ARRAY *array, const int afficheExt) {
    const int size = array->nbLig;
    char **tab = array->tabLig;
    int x, y;

    printf("----\n");
    if (afficheExt) {
        for (y = 0; y <= array->maxY+1; y++) {
            for (x = 0; x < 2*size-1; x++) {
                if (tab[y][x] == NEUTRALISE)
                    printf("..");
                else if (tab[y][x] > 0)
                    printf("%2d", tab[y][x]);
                else if (tab[y][x] < 0) {
                    printf("XX");
                }
                else
                    printf("  ");
            }
            printf("|\n");
        }
    }
    else {
        for (x = 0; x < 2*size-1; x++) {
            printf("%2d",x);
        }
        printf("\n");
        for (y = 0; y < size; y++) {
            for (x = 0; x < 2*size-1; x++) {
                if (tab[y][x] > 0 && tab[y][x] != NEUTRALISE)
                    printf("%2d", tab[y][x]);
                else if (tab[y][x] < 0) {
                    printf("  ");
                }
                else
                    printf("  ");
            }
            printf("|\n");
        }
    }

    for (x = array->firstSlice-1; x <= array->lastSlice+1; x++) {
        printf("  slice[%d] = (%d, %d, %d, %d, %d)\n", x, array->slices[x].cellsNb, array->slices[x].firstX,
               array->slices[x].lastX, array->slices[x].nbExt, array->slices[x].nextTransitionReductible);
    }
    printf("----\n");
}

#define NB_TESTS_DEBUG 7
unsigned long long nbDebug[NB_TESTS_DEBUG][SIZE_NUMBER_ARRAY];

void Init_Debug() {
    int t,s;

    for (t = 0; t < NB_TESTS_DEBUG; t++)
        for (s = 0; s < SIZE_NUMBER_ARRAY; s++)
            nbDebug[t][s] = 0;
}

#define SIZE_MIN_DEBUG 10
int IsArray_Debug(ARRAY *array)
{
    int size = array->targetSize;

    if (array->polySize >= SIZE_MIN_DEBUG &&
        array->tabLig[1][size-1] == 2 &&
        array->tabLig[1][size-2] == 3 &&
        array->tabLig[1][size] == 4 &&
        array->tabLig[2][size-1] == 5 &&
        array->tabLig[1][size-3] == 6 &&
        array->tabLig[1][size-4] == 7 &&
        array->tabLig[1][size-5] == 8 &&
        array->tabLig[2][size-5] == 9 &&
        array->tabLig[2][size-6] == 10) {
        return TRUE;
    }
    else
        return FALSE;
}

#define RUN_DEBUG
void Test_Debug(ARRAY *array) {
    int size = array->targetSize;
    int t;

    if (IsArray_Debug(array) == FALSE)
        return;

    if (array->tabLig[1][size-6] == 10) {
        t=0;
    }
    else if (array->tabLig[2][size-6] == 10) {
        t=1;
    }
    else if (array->tabLig[3][size-5] == 10) {
        t=2;
    }
    else if (array->tabLig[3][size-1] == 10) {
        t=3;
    }
    else if (array->tabLig[2][size] == 10) {
        t=4;
    }
    else if (array->tabLig[1][size+1] == 10) {
        t=5;
    }
    else {
        t=NB_TESTS_DEBUG-1;
    }

    nbDebug[t][array->polySize]++;
}

void Trace_Debug(int max) {
    int t;
    int s;

    for (t = 0; t < NB_TESTS_DEBUG; t++) {
        for (s = SIZE_MIN_DEBUG; s <= max; s++) {
            printf(" debug [%d][%d] = %d\n", t, s, nbDebug[t][s]);
        }
    }
}

void Destroy_Array(ARRAY *array) {
    free(array->tabLig);
    free(array->buffer);
    free(array);

    return;
}



void Test_Array(ARRAY *array, int idThread) {
    const int nbCells = array->polySize;
    const int size = (const int) array->nbLig;
    const int lgTab = (const int) array->nbCol;

    int slice, x, y;
    int nbFirst = -1;
    int nbLast = -1;
    int isIrreductible = TRUE;
    int idSliceOneInside = -1;
    int idLastSlice = -1;
    int minXPrec = -1;
    int maxXPrec = -1;
    int nbInSlicePrec = -1;


    nbFirst = array->slices[array->firstSlice].cellsNb;
    nbLast  = array->slices[array->lastSlice].cellsNb;
    if (array->slices[array->firstSlice].nextTransitionReductible)
        isIrreductible = FALSE;
    else {
        for (slice = array->firstSlice+1; slice < array->lastSlice; slice++) {
            if (array->slices[slice].nextTransitionReductible) {
                isIrreductible = FALSE;
                break;
            }
            if (array->slices[slice].cellsNb == 1) {
                idSliceOneInside = slice;
                break;
            }
        }
    }

    if (isIrreductible == TRUE) {
        nbIrr[idThread][nbCells]++;

        if (idSliceOneInside == -1 /*|| idSliceOneInside == idLastSlice*/) {
//if (array->polySize == 4) Print_Tableau(array, TRUE);

            if (nbLast == 1) {
                if (nbFirst == 1) {
                    nb11[idThread][nbCells]++;
                }
                else {
                    nbM1[idThread][nbCells]++;
                }
            }
            else {
                if (nbFirst == 1) {
                    nb1M[idThread][nbCells]++;
                    //Test_Debug(array);
                }
                else {
                    nbMM[idThread][nbCells]++;
                }
            }
            //if (nbCells == 5)
            //    Print_Tableau(array, array->nbLig, FALSE);
        }
    }

    nbAll[idThread][nbCells]++;
}

int Limits_Array(ARRAY *array, int nvFirstSlice) {
    int tooMuchSlices = FALSE;
    int nbSurnum = 0;
    int maxSlicesNb = (array->targetSize-nbSurnum) / 2 + 1;
    int nbLeft = array->targetSize - array->polySize;
    int minCellsToAdd = 0;
    int minSliceExt = array->slices[array->firstSlice-1].nbExt > 0 ? array->firstSlice-1 : -1;
    int maxSliceExt = minSliceExt;
    int minSliceOne = -1;
    int maxSliceOne = -1;
    int minSliceRed = -1;
    int maxSliceRed = -1;
    int neededDist = 0;
    int slice = -1;
    int xDeb, x;

    if (array->polySize <= maxSlicesNb)
        return tooMuchSlices;

    for (slice = array->firstSlice; slice < array->lastSlice; slice++) {
        if (array->slices[slice].nbExt) {
            maxSliceExt = slice;
            if (minSliceExt == -1) minSliceExt = slice;
        }
        if (slice > array->firstSlice && array->slices[slice].cellsNb == 1 &&
            !array->slices[slice].nextTransitionReductible) {
            minCellsToAdd++;
            maxSliceOne = slice;
            if (minSliceOne == -1) minSliceOne = slice;
        }
        else if (array->slices[slice].nextTransitionReductible) {
            int ReducTransitionSegLg = 0;
            int OnesInSeg = 0;

            if (minSliceRed == -1) minSliceRed = slice;
            while (slice < array->lastSlice && array->slices[slice].nextTransitionReductible) {
                maxSliceRed = slice;
                ReducTransitionSegLg++;
                if (slice > array->firstSlice && array->slices[slice].cellsNb == 1) OnesInSeg++;
                slice++;
                if (array->slices[slice].nbExt) {
                    maxSliceExt = slice;
                    if (minSliceExt == -1) minSliceExt = slice;
                }
            }
            if (OnesInSeg >= (ReducTransitionSegLg+1)/2) minCellsToAdd += OnesInSeg;
            else {
                    minCellsToAdd += (ReducTransitionSegLg+1)/2;
                    //Print_Tableau(array, TRUE);
            }
            //minCellsToAdd += OnesInSeg;
        }
    }
    if (minCellsToAdd > nbLeft)
        return TRUE;

    if (array->slices[array->lastSlice].nbExt) {
        maxSliceExt = slice;
        if (minSliceExt == -1) minSliceExt = slice;
    }
    if (array->slices[array->lastSlice+1].nbExt > 0) {
        maxSliceExt = array->lastSlice+1;
        if (minSliceExt == -1) minSliceExt = array->lastSlice+1;
    }

    if (maxSliceExt != -1 && maxSliceExt < maxSliceOne &&
        minSliceOne != -1 && minSliceOne < minSliceExt) {
        neededDist = maxSliceOne-maxSliceExt+1 +minSliceExt-minSliceOne+1;
        if (neededDist == nbLeft) {
#ifdef RUN_DEBUG
        for (slice = minSliceExt+1; slice < maxSliceExt; slice++) {
            xDeb = (slice >= array->nbLig) ? slice - array->nbLig + 1: 0;
            for (x = xDeb; x <= slice ; x++) {
                if (array->tabLig[slice-x][x] < 0 ) {
                    array->slices[slice].nbExt--;
                    array->tabLig[slice -x][x] = NEUTRALISE;
                }
            }
        }
#endif
        }
        else if (neededDist > nbLeft) {
            return TRUE;
        }
    }
    else if (maxSliceExt != -1 && maxSliceExt < maxSliceOne) {
        neededDist = maxSliceOne-maxSliceExt+1;
        if (neededDist == nbLeft) {
        for (slice = minSliceExt; slice < maxSliceExt; slice++) {
            xDeb = (slice >= array->nbLig) ? slice - array->nbLig + 1: 0;
            for (x = xDeb; x <= slice ; x++) {
                if (array->tabLig[slice-x][x] < 0 ) {
                    array->slices[slice].nbExt--;
                    array->tabLig[slice -x][x] = NEUTRALISE;
                }
            }
        }
        }
        else if (neededDist > nbLeft) {
            return TRUE;
        }
    }
    else if (minSliceOne != -1 && minSliceOne < minSliceExt) {
        neededDist = minSliceExt-minSliceOne+1;
        if (neededDist == nbLeft) {
/*if (IsArray_Debug(array) && array->polySize <= 12) {
    printf(" maxSliceExt %d maxSliceOne %d\n", maxSliceExt, maxSliceOne);
    Print_Tableau(array, TRUE);
}
*/
        for (slice = minSliceExt+1; slice <= maxSliceExt; slice++) {
            xDeb = (slice >= array->nbLig) ? slice - array->nbLig + 1: 0;
            for (x = xDeb; x <= slice ; x++) {
                if (array->tabLig[slice-x][x] < 0 ) {
                    array->slices[slice].nbExt--;
                    array->tabLig[slice -x][x] = NEUTRALISE;
                }
            }
        }
/*
if (IsArray_Debug(array) && array->polySize <= 12) {
//    printf(" maxSliceExt %d maxSliceOne %d", maxSliceExt, maxSliceOne);
//    Print_Tableau(array, TRUE);
}
*/
        }
        else if (neededDist > nbLeft) {
            return TRUE;
        }
    }

    if (maxSliceExt != -1 && maxSliceExt < maxSliceRed) {
        neededDist = maxSliceRed-maxSliceExt+1;
        if (neededDist == nbLeft) {
        for (slice = minSliceExt; slice < maxSliceExt; slice++) {
            xDeb = (slice >= array->nbLig) ? slice - array->nbLig + 1: 0;
            for (x = xDeb; x <= slice ; x++) {
                if (array->tabLig[slice-x][x] < 0 ) {
                    array->slices[slice].nbExt--;
                    array->tabLig[slice -x][x] = NEUTRALISE;
                }
            }
        }
        }
        else if (neededDist > nbLeft) {
            return TRUE;
        }
    }
    else if (minSliceRed != -1 && minSliceRed < minSliceExt) {
        neededDist = minSliceExt-minSliceRed;
        if (neededDist == nbLeft) {
/*if (IsArray_Debug(array) && array->polySize <= 12) {
    printf(" maxSliceExt %d maxSliceOne %d\n", maxSliceExt, maxSliceOne);
    Print_Tableau(array, TRUE);
}
*/
        for (slice = minSliceExt+1; slice <= maxSliceExt; slice++) {
            xDeb = (slice >= array->nbLig) ? slice - array->nbLig + 1: 0;
            for (x = xDeb; x <= slice ; x++) {
                if (array->tabLig[slice-x][x] < 0 ) {
                    array->slices[slice].nbExt--;
                    array->tabLig[slice -x][x] = NEUTRALISE;
                }
            }
        }
/*
if (IsArray_Debug(array) && array->polySize <= 12) {
//    printf(" maxSliceExt %d maxSliceOne %d", maxSliceExt, maxSliceOne);
//    Print_Tableau(array, TRUE);
}
*/
        }
        else if (neededDist > nbLeft) {
            return TRUE;
        }
    }

    if (array->slices[array->firstSlice].cellsNb > 2) nbSurnum += (array->slices[array->firstSlice].cellsNb-2);
    for (slice = array->firstSlice+1; slice < array->lastSlice; slice++) {
        if (array->slices[slice].cellsNb > 2) nbSurnum += (array->slices[slice].cellsNb-2);
    }

    if (array->slices[array->lastSlice].cellsNb > 2) nbSurnum += (array->slices[array->lastSlice].cellsNb-2);

    maxSlicesNb = (array->targetSize-nbSurnum) / 2 + 1;

    if (nvFirstSlice > array->firstSlice && nvFirstSlice < array->lastSlice) {
    }
    else {
        if (nvFirstSlice == array->firstSlice) {
            slice = array->firstSlice + maxSlicesNb;
        }
        else if (nvFirstSlice == array->lastSlice) {
            slice = array->lastSlice - maxSlicesNb;
        }

        xDeb = (slice >= array->nbLig) ? slice - array->nbLig + 1: 0;
        for (x = xDeb; x <= slice ; x++) {
            if (array->tabLig[slice-x][x] != NEUTRALISE && array->tabLig[slice-x][x] > 0) {
                tooMuchSlices = TRUE;
                //printf("s");
                break;
            }
            if (array->tabLig[slice -x][x] < 0) array->slices[slice].nbExt--;
            array->tabLig[slice -x][x] = NEUTRALISE;
        }
    }
    return tooMuchSlices;
}

void UpdateSlice(ARRAY *array, int y, int x) {
    SLICE *slice = &array->slices[y+x];
    SLICE *nextSlice = &array->slices[y+x+1];
    SLICE *precSlice = &array->slices[y+x-1];

    slice->cellsNb++;
    if (slice->cellsNb == 1) {
        slice->firstX = slice->lastX = x;
    }
    else if (x > slice->lastX) slice->lastX = x;
    else if (x < slice->firstX) slice->firstX = x;

    if (y+x < array->firstSlice) array->firstSlice = y+x;
    else if (y+x > array->lastSlice) array->lastSlice = y+x;

    if (nextSlice->cellsNb > 0) {
        if (slice->firstX == nextSlice->lastX ||
            slice->lastX == nextSlice->firstX -1)
            slice->nextTransitionReductible = TRUE;
        else
            slice->nextTransitionReductible = FALSE;
    }
    if (precSlice->cellsNb > 0) {
        if (precSlice->firstX == slice->lastX ||
            precSlice->lastX == slice->firstX -1)
            precSlice->nextTransitionReductible = TRUE;
        else
            precSlice->nextTransitionReductible = FALSE;
    }

}

int Add_Element_Array(ARRAY *arrayIn, int extMaxIn, int idThread) {
    int status = OK;
    int x, y;

    if (arrayIn->polySize >= FIRST_TEST) {
        Test_Array(arrayIn, idThread);
    }


    if (arrayIn->polySize == arrayIn->nbLig - 1) {
        int fSliceIn = arrayIn->firstSlice;
        int lSliceIn = arrayIn->lastSlice;
        SLICE sliceIn;
        int precSliceReductible;

        arrayIn->polySize++;
        for (y = 0; y <= arrayIn->maxY+1; y++) {
            for (x = arrayIn->minX-1; x <= arrayIn->maxX+1; x++) {
                if (arrayIn->tabLig[y][x] < 0) {
                    int v = arrayIn->tabLig[y][x];

                    arrayIn->firstSlice = fSliceIn;
                    arrayIn->lastSlice = lSliceIn;

                    arrayIn->tabLig[y][x] = arrayIn->polySize;

                    sliceIn = arrayIn->slices[y+x];
                    precSliceReductible = arrayIn->slices[y+x-1].nextTransitionReductible;
                    UpdateSlice(arrayIn, y, x);

                    Test_Array(arrayIn, idThread);
                    arrayIn->tabLig[y][x] = v;
                    arrayIn->slices[y+x] = sliceIn;
                    arrayIn->slices[y+x-1].nextTransitionReductible = precSliceReductible;
                }
            }
        }
    }
    else
        {
        ARRAY *arrayNv = arrayList[idThread][arrayIn->polySize+1]; // Create_Array(arrayIn->nbLig);

        {
            int xx, yy;

            for (y = 0; y <= arrayIn->maxY+1; y++) {
                for (x = arrayIn->minX-1; x <= arrayIn->maxX+1; x++) {
                    int tooMuchSlices = FALSE;
                //for (x = arrayIn->nbLig-(arrayIn->sizePoly-y+1); x < arrayIn->nbLig+(arrayIn->sizePoly-y+1); x++) {
                    if (arrayIn->tabLig[y][x] < 0) {
                        int extMax = extMaxIn;
                        char inVal = arrayIn->tabLig[y][x];

                        status = Copy_Array(arrayNv, arrayIn);

                        for (yy = 0; yy <= arrayIn->maxY+1; yy++) {
                            //for (xx = arrayIn->nbLig-(arrayIn->sizePoly-yy+1); xx < arrayIn->nbLig+(arrayIn->sizePoly-yy+1); xx++) {
                            for (xx = arrayIn->minX-1; xx <= arrayIn->maxX+1; xx++) {
                                if (arrayNv->tabLig[yy][xx] < 0 && arrayNv->tabLig[yy][xx] > inVal) {
                                    arrayNv->tabLig[yy][xx] = NEUTRALISE;
                                    arrayNv->slices[yy+xx].nbExt--;
                                }
                            }
                        }

                        arrayNv->tabLig[y][x] = arrayIn->polySize + 1;
                        arrayNv->slices[y+x].nbExt--;
                        arrayNv->polySize = arrayIn->polySize + 1;
                        if (y > arrayNv->maxY) arrayNv->maxY = y;
                        if (x > arrayNv->maxX) arrayNv->maxX = x;
                        else if (x < arrayNv->minX) arrayNv->minX = x;

                        UpdateSlice(arrayNv, y, x);

//Print_Tableau(arrayNv->tabLig, arrayNv->nbLig, TRUE);
                        xx = x;
                        yy = y - 1;
                        if (yy >= 0 && arrayNv->tabLig[yy][xx] == VIDE) {
                            extMax++;
                            arrayNv->tabLig[yy][xx] = -extMax;
                            arrayNv->slices[yy+xx].nbExt++;
                        }
                        xx = x - 1;
                        yy = y;
                        if (xx >= 0 && arrayNv->tabLig[yy][xx] == VIDE) {
                            extMax++;
                            arrayNv->tabLig[yy][xx] = -extMax;
                            arrayNv->slices[yy+xx].nbExt++;
                        }
                        xx = x + 1;
                        yy = y;
                        if (xx < arrayNv->nbCol && arrayNv->tabLig[yy][xx] == VIDE) {
                            extMax++;
                            arrayNv->tabLig[yy][xx] = -extMax;
                            arrayNv->slices[yy+xx].nbExt++;
                        }
                        xx = x;
                        yy = y + 1;
                        if (yy < arrayIn->nbLig && arrayNv->tabLig[yy][xx] == VIDE) {
                            extMax++;
                            arrayNv->tabLig[yy][xx] = -extMax;
                            arrayNv->slices[yy+xx].nbExt++;
                        }

                        tooMuchSlices = Limits_Array(arrayNv, x+y);
                        if (tooMuchSlices == TRUE)
                            continue;

                        status = Add_Element_Array(arrayNv, extMax, idThread);
                    }
                }
            }
        }
    }

    return status;
}


int Create_Polyominos(const int size) {
    ARRAY *array = NULL;
    int status = OK;
    int t, i;

    printf("Calcul des polyominos de taille %d\n", size);

    for (t = 0; t < NB_THREADS; t++) {
        for (i = 0; i < SIZE_NUMBER_ARRAY; i++) {
            nbAll[t][i] = 0;
            nbIrr[t][i] = 0;
            nbMM[t][i] = 0;
            nb1M[t][i] = 0;
            nbM1[t][i] = 0;
            nb11[t][i] = 0;
        }
    }

    if (NB_THREADS == 1) {
        array = arrayList[0][1];
        Init_Array(array);
        array->polySize = 1;
        array->tabLig[0][size-1] = 1;
        array->slices[0+size-1].cellsNb = 1;
        array->slices[0+size-1].firstX = array->slices[0+size-1].lastX = size-1;
        array->slices[0+size-1].nextTransitionReductible = FALSE;
        array->maxY = 0;
        array->maxX = array->minX = size-1;
        array->firstSlice = array->lastSlice = size - 1;
        array->tabLig[0][size] = -2;
        array->tabLig[1][size-1] = -3;
        array->slices[size].nbExt = 2;
        Limits_Array(array,size-1);
        Limits_Array(array, size);

        status = Add_Element_Array(array, 3, 0/* idThread*/);
    }
    else if (NB_THREADS == 2) {
        int t = 0;
        time_t td = time(NULL);
        time_t tf;

        array = arrayList[t][1];
        Init_Array(array);
        array->polySize = 2;
        array->tabLig[0][size-1] = 1;
        array->tabLig[0][size] = 2;
        array->slices[0+size-1].cellsNb = 1;
        array->slices[0+size].cellsNb = 1;
        array->slices[0+size-1].firstX = array->slices[0+size-1].lastX = size-1;
        array->slices[0+size].firstX = array->slices[0+size].lastX = size;
        array->slices[0+size-1].nextTransitionReductible = TRUE;
        array->maxY = 0;
        array->minX = size-1;
        array->maxX = size;
        array->firstSlice = size - 1;
        array->lastSlice  = size;
        array->tabLig[0][size+1] = -4;
        array->tabLig[1][size] = -5;
        array->tabLig[1][size-1] = -3;
        array->slices[size].nbExt = 1;
        array->slices[size+1].nbExt = 2;
        Limits_Array(array,size-1);
        Limits_Array(array, size);

        status = Add_Element_Array(array, 5, t);
        tf = time(NULL);
        for (i = FIRST_TEST; i <= size; i++) {
            printf("taille des polyominos : %d\n", i);
            printf("   nb All : %llu\n", nbAll[t][i]);
            printf("   nb Irreductible : %llu\n", nbIrr[t][i]);
            printf("   nb MM : %llu\n", nbMM[t][i]);
            printf("   nb M1 : %llu\n", nbM1[t][i]);
            printf("   nb 1M : %llu\n", nb1M[t][i]);
            printf("   nb 11 : %llu\n", nb11[t][i]);
        }
        printf(" duree premier thread %ld\n", tf-td);

        td = time(NULL);

        t++;
        array = arrayList[t][1];
        Init_Array(array);
        array->polySize = 2;
        array->tabLig[0][size-1] = 1;
        array->tabLig[1][size-1] = 2;
        array->slices[0+size-1].cellsNb = 1;
        array->slices[0+size].cellsNb = 1;
        array->slices[0+size-1].firstX = array->slices[0+size-1].lastX = size-1;
        array->slices[0+size].firstX = array->slices[0+size].lastX = size-1;
        array->slices[0+size-1].nextTransitionReductible = TRUE;
        array->maxY = 1;
        array->minX = size-1;
        array->maxX = size-1;
        array->firstSlice = size - 1;
        array->lastSlice  = size;
        array->tabLig[0][size] = NEUTRALISE;
        array->tabLig[1][size-2] = -3;
        array->tabLig[1][size] = -4;
        array->tabLig[2][size-1] = -5;
        array->slices[size-1].nbExt = 1;
        array->slices[size+1].nbExt = 2;

        Limits_Array(array,size-1);
        Limits_Array(array, size);

        status = Add_Element_Array(array, 5, t/* idThread*/);
        tf = time(NULL);

        for (i = FIRST_TEST; i <= size; i++) {
            printf("taille des polyominos : %d\n", i);
            printf("   nb All : %llu\n", nbAll[t][i]);
            printf("   nb Irreductible : %llu\n", nbIrr[t][i]);
            printf("   nb MM : %llu\n", nbMM[t][i]);
            printf("   nb M1 : %llu\n", nbM1[t][i]);
            printf("   nb 1M : %llu\n", nb1M[t][i]);
            printf("   nb 11 : %llu\n", nb11[t][i]);
        }
        printf(" duree second thread %ld\n", tf-td);
    }



    Destroy_Array(array);

    for (i = FIRST_TEST; i <= size; i++) {
        printf("taille des polyominos : %d\n", i);
        for (t = 1; t < NB_THREADS; t++) {
            nbAll[0][i] += nbAll[t][i];
            nbIrr[0][i] += nbIrr[t][i];
            nbMM[0][i] += nbMM[t][i];
            nbM1[0][i] += nbM1[t][i];
            nb1M[0][i] += nb1M[t][i];
            nb11[0][i] += nb11[t][i];
        }
        printf("   nb All : %llu\n", nbAll[0][i]);
        printf("   nb Irreductible : %llu\n", nbIrr[0][i]);
        printf("   nb MM : %llu\n", nbMM[0][i]);
        printf("   nb M1 : %llu\n", nbM1[0][i]);
        printf("   nb 1M : %llu\n", nb1M[0][i]);
        printf("   nb 11 : %llu\n", nb11[0][i]);
    }
    return status;
}


int main(int argc, char *argv[])
{
    int status = OK;
    time_t td = time(NULL);
    time_t tf;
    int max = 15;
    int t,s;

//Init_Debug();
    for (t = 0; t < NB_THREADS; t++) {
        for (s = 1; s <= max; s++) {
            arrayList[t][s] = Create_Array(max);
        }
    }

    Create_Polyominos(max);
    tf = time(NULL);
    printf("time %ld\n", tf-td);

//Trace_Debug(max);
    return status;
}
