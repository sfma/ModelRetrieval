/*
 * pointSVD.c
 *
 *  Created on: Mar 18, 2016
 *      Author: sfma
 *
 *  Description:
 *  	pointSVD to normalize the model.
 *  	"Multi-Fourier spectra descriptor and augmentation with spectral clustering for 3D shape retrieval" by Atsushi Tatsuma and Masaki Aono
 */

#include "Ds.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include "Rotate.h"
#include "svdcmp.h"

#define sign(a)  (((a)<0) ? -1: ((a)>0))

//Function declaration
double AreaOfTri(pVer vertex, Tri ATriangle);
Ver FindCenter(pVer pSamples, int m);
Ver generatePoints(pVer vertex, Tri ATriangle);
void pointSVD(int m, pVer vertex, int NumVer, pTri triangle, int NumTri, pVer newVertex);


double AreaOfTri(pVer vertex, Tri ATriangle){
	/*
	 * Calculate the area of a triangle.
	 */
	vector v1,v2,v3;
	v1.x=vertex[ATriangle.v[0]].coor[0];
	v1.y=vertex[ATriangle.v[0]].coor[1];
	v1.z=vertex[ATriangle.v[0]].coor[2];

	v2.x=vertex[ATriangle.v[1]].coor[0];
	v2.y=vertex[ATriangle.v[1]].coor[1];
	v2.z=vertex[ATriangle.v[1]].coor[2];

	v3.x=vertex[ATriangle.v[2]].coor[0];
	v3.y=vertex[ATriangle.v[2]].coor[1];
	v3.z=vertex[ATriangle.v[2]].coor[2];

	vector vec1, vec2;
	vec1.x=v3.x-v1.x; vec1.y=v3.y-v1.y; vec1.z=v3.z-v1.z;
	vec2.x=v3.x-v2.x; vec2.y=v3.y-v2.y; vec2.z=v3.z-v2.z;

	vector vec=cross(vec1, vec2);
	double area;
	area=pow(vec.x,2)+pow(vec.y,2)+pow(vec.z,2);
	area=0.5*sqrt(area);
	return area;
}

Ver FindCenter(pVer pSamples, int m){
	//Find the center of the samples.
	Ver center;
	center.coor[0]=0;
	center.coor[1]=0;
	center.coor[2]=0;
	int i;
	for(i=0;i<m;i++){
		center.coor[0]+=pSamples[i].coor[0];
		center.coor[1]+=pSamples[i].coor[1];
		center.coor[2]+=pSamples[i].coor[2];
	}
	center.coor[0]/=m;
	center.coor[1]/=m;
	center.coor[2]/=m;
	return center;
}

Ver generatePoints(pVer vertex, Tri ATriangle){
	double r1,r2;
	//function rand returns an int number.
	r1=(double)rand();
	r1/=RAND_MAX;
	r2=(double)rand();
	r2/=RAND_MAX;
	Ver point;
	double c1, c2, c3;
	c1=1-sqrt(r1);
	c2=sqrt(r1)*(1-r2);
	c3=sqrt(r1)*r2;
	point.coor[0]=c1*vertex[ATriangle.v[0]].coor[0]+c2*vertex[ATriangle.v[1]].coor[0]+c3*vertex[ATriangle.v[2]].coor[0];
	point.coor[1]=c1*vertex[ATriangle.v[0]].coor[1]+c2*vertex[ATriangle.v[1]].coor[1]+c3*vertex[ATriangle.v[2]].coor[1];
	point.coor[2]=c1*vertex[ATriangle.v[0]].coor[2]+c2*vertex[ATriangle.v[1]].coor[2]+c3*vertex[ATriangle.v[2]].coor[2];
	return point;
}

void pointSVD(int m, pVer vertex, int NumVer, pTri triangle, int NumTri, pVer newVertex){
	/*
	 * m is the total number of points sampled on the surface.
	 */
	//First, triangle the model
	pTri myTriangle; // for all the triangles
	double *area; //area for each triangle
	double totalArea=0;
	int i,j,k;
	int NumMyTri=0; //total triangles when the polygon mesh is triangled.
	for(i=0;i<NumTri;i++){
		NumMyTri+=triangle[i].NodeName-2;
	}
	myTriangle=(pTri)malloc(NumMyTri*sizeof(Tri));
	area=(double *)malloc(NumMyTri*sizeof(double));
	j=0;
	for(i=0;i<NumTri;i++){
		myTriangle[j].v[0]=triangle[i].v[0];
		myTriangle[j].v[1]=triangle[i].v[1];
		for(k=3;k<triangle[i].NodeName;k++){
			myTriangle[j].v[2]=triangle[i].v[k];
			//to calculate the area for a triangle
			area[j]=AreaOfTri(vertex, myTriangle[j]);
			totalArea+=area[j];
			if(k==(triangle[i].NodeName-1)){
				j++;
			}
			else{
				j++;
				myTriangle[j].v[0]=triangle[i].v[0];
				myTriangle[j].v[1]=triangle[i].v[1];
			}
		}
	}
	//Randomly sampling on the surface of the polygon mesh
	pVer pSamples=(pVer)malloc(m*sizeof(Ver));
	k=0;
	for(i=0;i<NumMyTri;i++){
		int numSamples=(int)(area[i]/totalArea);
		for(j=0;j<numSamples;j++){
			pSamples[k]=generatePoints(vertex,myTriangle[i]);
			k++;
		}
	}
	for(;k<m;k++){
		int randindx=(int)(rand()/RAND_MAX*(NumMyTri-1));
		pSamples[k]=generatePoints(vertex, myTriangle[randindx]);
	}
	//Points generated.

	//Find the center of the samples
	Ver center=FindCenter(pSamples, m);

	//Generate matrix P.
	double *matrixP[3];
	for(i=0;i<3;i++){
		matrixP[i]=(double *)malloc(m*sizeof(double));
	}
	int indxOuter, indxInner;
	/*
	for(indxOuter=0;indxOuter<m;indxOuter++){
		for(indxInner=0;indxInner<3;indxInner++){
			matrixP[indxOuter][indxInner]=pSamples[indxOuter].coor[indxInner]-center.coor[indxInner];
		}
	}*/
	for(indxOuter=0;indxOuter<3;indxOuter++){
		for(indxInner=0;indxInner<m;indxInner++){
			matrixP[indxOuter][indxInner]=pSamples[indxInner].coor[indxOuter]-center.coor[indxOuter];
		}
	}

	//free pSamples
	free(pSamples);
	//SVD
	double *w=(double *)malloc(m);
	double *v[m];
	for(i=0;i<m;i++)
	{
		v[i]=(double *)malloc(m*sizeof(double));
	}
	//before call function svdcmp, make a copy of matrix P.
	double *copyP[3];
	for(i=0;i<3;i++){
		copyP[i]=(double *)malloc(m*sizeof(double));
		for(j=0;j<m;j++){
			copyP[i][j]=matrixP[i][j];
		}
	}
	svdcmp(matrixP,3,m,w,v);
	free(w);
	for(i=0;i<m;i++){
		free(v[i]);
	}
	//Now matrixP is a 3*3 matrix. Transpose it to get Q.
	double Q[3][3];
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++){
			Q[j][i]=matrixP[i][j];
		}
	}
	for(i=0;i<3;i++){
		free(matrixP[i]);
	}

	//Create matrix P'
	double *P1[3];//P'
	for(i=0;i<3;i++){
		P1[i]=(double *)malloc(m*sizeof(double));
	}
	for(i=0;i<3;i++){
		for(j=0;j<m;j++)
		{
			double sum=0;
			for(k=0;k<3;k++){
				sum+=Q[i][k]*copyP[k][j];
			}
			P1[i][j]=sum;
		}
	}

	double f[3]={0,0,0};
	for(i=0;i<3;i++){
		for(j=0;j<m;j++){
			f[i]+=sign(P1[i][j])*pow(P1[i][j],2);
		}
	}


	double F[3][3];
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			if(j==i){
				F[i][j]=f[j];
			}
			else{
				F[i][j]=0;
			}
		}
	}

	//free P'
	for(i=0;i<3;i++){
		free(P1[i]);
	}

	//matrix V and V'
	double *V[3], *V1[3];
	for(i=0;i<3;i++){
		V[i]=(double *)malloc(NumVer*sizeof(double));
		V1[i]=(double *)malloc(NumVer*sizeof(double));
	}

	for(i=0;i<3;i++)
	{
		for(j=0;j<NumVer;j++){
			V[i][j]=vertex[j].coor[i]-center.coor[i];
		}
	}

	double tmp[3][3];
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			double sum=0;
			for(k=0;k<3;k++){
				sum+=F[i][k]*Q[k][j];
			}
			tmp[i][j]=sum;
		}
	}
	for(i=0;i<3;i++){
		for(j=0;j<NumVer;j++){
			double sum=0;
			for(k=0;k<3;k++){
				sum+=tmp[i][k]*V[k][j];
			}
			V1[i][j]=sum;
		}
	}

	//pVer newVertex=(pVer)malloc(NumVer*sizeof(Ver));
	for(i=0;i<NumVer;i++){
		newVertex[i].coor[0]=V1[0][i];
		newVertex[i].coor[1]=V1[1][i];
		newVertex[i].coor[2]=V1[2][i];
	}

	for(i=0;i<3;i++){
		free(V[i]);
		free(V1[i]);
	}
	free(myTriangle);
	free(area);
}


