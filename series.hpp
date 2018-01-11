#ifndef SERIES_H
#define SERIES_H


void Mult(unsigned long long *ptrOut, const unsigned long long *ptrOp1, const unsigned long long *ptrOp2, const int nb);
void MultAdd(unsigned long long *ptrOut, const unsigned long long *ptrOp1, const unsigned long long *ptrOp2, const int nb);

void PInv(unsigned long long *ptrPInv, const unsigned long long *ptrOrig, const int offset, unsigned long long coeff, const int nb);

void Mult3(unsigned long long *ptrOut, const unsigned long long *ptrOp1, const unsigned long long *ptrOp2, const unsigned long long *ptrOp3, const int nb);

void Mult3Add(unsigned long long *ptrOut, const unsigned long long *ptrOp1, const unsigned long long *ptrOp2, const unsigned long long *ptrOp3, const int offset, const int nb);



#endif // SERIES_H
