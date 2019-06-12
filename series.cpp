#include <stdio.h>
#include <stdlib.h>

void Mult(unsigned long long *ptrOut, const unsigned long long *ptrOp1, const unsigned long long *ptrOp2, const int nb)
{
    int i, i1, i2;
    for (i = 0; i < nb; i++)
        ptrOut[i] = 0;

    for (i1 = 0; i1 < nb; i1++) {
        for (i2 = 0; i2 < nb-i1; i2++)
            ptrOut[i1+i2] += ptrOp1[i1] * ptrOp2[i2];
    }
}

void MultAdd(unsigned long long *ptrOut, const unsigned long long *ptrOp1, const unsigned long long *ptrOp2, const int nb)
{
    int i1, i2;

    for (i1 = 0; i1 < nb; i1++) {
        for (i2 = 0; i2 < nb-i1; i2++)
            ptrOut[i1+i2] += ptrOp1[i1] * ptrOp2[i2];
    }
}

void Mult3(unsigned long long *ptrOut, const unsigned long long *ptrOp1, const unsigned long long *ptrOp2, const unsigned long long *ptrOp3, const int nb)
{
    int i, i1, i2, i3;
    for (i = 0; i < nb; i++)
        ptrOut[i] = 0;

    for (i1 = 0; i1 < nb; i1++) {
        for (i2 = 0; i2 < nb-i1; i2++) {
            for (i3 = 0; i3 < nb-i1-i2; i3++) {
                ptrOut[i1+i2+i3] += ptrOp1[i1] * ptrOp2[i2] * ptrOp3[i3];
            }
        }
    }
}

void Mult3Add(unsigned long long *ptrOut, const unsigned long long *ptrOp1, const unsigned long long *ptrOp2, const unsigned long long *ptrOp3, const int offset, const int nb)
{
    int i, i1, i2, i3;

    for (i1 = 0; i1 < nb; i1++) {
        if (ptrOp1[i1] == 0) continue;

        for (i2 = 0; i2 < nb; i2++) {
            int begin_i3 = (0 > offset-i1-i2) ? 0 : offset-i1-i2;
            int end_i3 = (nb <= nb+offset-i1-i2) ? nb : nb+offset-i1-i2;

            if (ptrOp2[i2] == 0) continue;

            for (i3 = begin_i3; i3 < end_i3; i3++) {
                ptrOut[i1+i2+i3-offset] += ptrOp1[i1] * ptrOp2[i2] * ptrOp3[i3];
            }
        }
    }
}

void PInv(unsigned long long *ptrPInv, const unsigned long long *ptrOrig, const int offset, unsigned long long coeff, const int nb)
{
    int min = 0;
    unsigned long long exp = coeff;
    unsigned long long *ptrOrigOff = NULL;
    unsigned long long *ptrOrigN = NULL;
    unsigned long long *ptrOrigNP1 = NULL;
    int i, k;

    ptrPInv[0] = 1;
    for (i = 1; i<nb; i++) ptrPInv[i] = 0;

    if (ptrOrig[0] != 0) {
        printf("PInv error : ptrOrig must start with 0\n");
        return;
    }
    while (min < nb && ptrOrig[min] == 0) min++;

    if (min <= offset) {
        printf("PInv error ; ptrOrig must be zero until %d\n", offset);
    }

    ptrOrigOff = (unsigned long long *) malloc(sizeof(unsigned long long)*nb);
    ptrOrigN = (unsigned long long *) malloc(sizeof(unsigned long long)*nb);
    ptrOrigNP1 = (unsigned long long *) malloc(sizeof(unsigned long long)*nb);

    for (i = 0; i < nb - offset; i++)
        ptrOrigOff[i] = ptrOrig[i+offset];
    for ( ; i < nb; i++)
        ptrOrigOff[i] = 0;

    ptrOrigN[0] = 0;
    for (i = 1; i < nb; i++) {
        ptrOrigN[i] = ptrOrigOff[i];
        ptrPInv[i] = exp * ptrOrigOff[i];
    }
    min -= offset;
    k = 2;
    while (min*k < nb) {
        int i1,  i2;

        Mult(ptrOrigNP1, ptrOrigOff, ptrOrigN, nb);
        exp *= coeff;
        for (i = 0; i < nb; i++) {
            ptrPInv[i] += exp * ptrOrigNP1[i];
            ptrOrigN[i] = ptrOrigNP1[i];
        }
        k++;
    }

    free(ptrOrigOff);
    free(ptrOrigNP1);
    free(ptrOrigN);
    return;
}

