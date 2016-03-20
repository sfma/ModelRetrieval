/*
 * svdcmp.h
 *
 *  Created on: Mar 19, 2016
 *      Author: sfma
 */

#ifndef SVDCMP_H_
#define SVDCMP_H_

int svdcmp(double **a, int nRows, int nCols, double *w, double **v);

// prints an arbitrary size matrix to the standard output
void printMatrix(double **a, int rows, int cols);

#endif /* SVDCMP_H_ */
