/*
 * pointSVD.h
 *
 *  Created on: Mar 20, 2016
 *      Author: sfma
 */

#ifndef POINTSVD_H_
#define POINTSVD_H_

double AreaOfTri(pVer vertex, Tri ATriangle);
Ver FindCenter(pVer pSamples, int m);
Ver generatePoints(pVer vertex, Tri ATriangle);
void pointSVD(int m, pVer vertex, int NumVer, pTri triangle, int NumTri, pVer newVertex);

#endif /* POINTSVD_H_ */
