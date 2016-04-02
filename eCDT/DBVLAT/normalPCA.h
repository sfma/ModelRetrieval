/*
 * normalPCA.h
 *
 *  Created on: Apr 2, 2016
 *      Author: sfma
 */

#ifndef NORMALPCA_H_
#define NORMALPCA_H_

double *FindNormalCenter(double **normals, int m);
void normalPCA(int m, pVer vertex, int NumVer, pTri triangle, int NumTri, pVer newVertex);

#endif /* NORMALPCA_H_ */
