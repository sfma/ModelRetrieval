/*
 * CPCA.c
 *
 *  Created on: Mar 21, 2016
 *      Author: sfma
*   Description: This is very similar to pointSVD. The difference is that it use eigenvalue and eigenvector instead of SVD.
 */

#include "Ds.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Rotate.h"
#include "svdcmp.h"
#include "GenerateRandomPoints.h"
#include "eispack.h"
//#include "svd.h"

#define sign(a)  (((a)<0) ? -1: ((a)>0))

//Function declaration
void CPCA(int m, pVer vertex, int NumVer, pTri triangle, int NumTri, pVer newVertex);

void CPCA(int m, pVer vertex, int NumVer, pTri triangle, int NumTri, pVer newVertex){
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
		for(k=2;k<triangle[i].NodeName;k++){
			myTriangle[j].v[2]=triangle[i].v[k];
			myTriangle[j].NodeName=3;
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
	//write the triangle model to see if it is right
	/*char ftri[100]="/Users/sfma/Desktop/triangleModel.obj";
	SaveObj(ftri,vertex,myTriangle,NumVer,NumMyTri);*/

	//Randomly sampling on the surface of the polygon mesh
	pVer pSamples=(pVer)malloc(m*sizeof(Ver));
	k=0;
	//printf("totalarea of %d faces is %f.\n",NumMyTri,totalArea);
	for(i=0;i<NumMyTri;i++){
		//printf("Area of %dth triangle is %f.\n",i,area[i]);
		int numSamples=(int)(area[i]/totalArea*m);
		for(j=0;j<numSamples;j++){
			/*if(i<100){
				printf("This time the algorithm selects face %d, its area is %f, while total area is %f.\n",i,area[i],totalArea);
			}*/
			pSamples[k]=generatePoints(vertex,myTriangle[i]);
			k++;
		}
	}
	for(;k<m;k++){
		int randindx=(int)(((double)rand())/RAND_MAX*(NumMyTri-1));
		pSamples[k]=generatePoints(vertex, myTriangle[randindx]);
	}
	//Points generated.
	//write the samples to an off file and open the off file to see if it maintain the object' shape.
	/*char fSamples[100]="/Users/sfma/Desktop/samples.off";
	FILE *fsam=fopen(fSamples,"w");
	fprintf(fsam,"OFF\n");
	fprintf(fsam,"%d %d %d\n",m, NumMyTri,0);
	for(i=0;i<m;i++){
		fprintf(fsam,"%f %f %f\n",pSamples[i].coor[0],pSamples[i].coor[1],pSamples[i].coor[2]);
	}
	fclose(fsam);*/

	//Find the center of the samples
	Ver center=FindCenter(pSamples, m);

	//create matrix P
	double *matrixP[3];
	for(i=0;i<3;i++){
		matrixP[i]=(double *)malloc(m*sizeof(double));
		for(j=0;j<m;j++){
			matrixP[i][j]=pSamples[j].coor[i]-center.coor[i];
		}
	}


	//generate covariance matrix
	double covMat[3][3];
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			covMat[i][j]=0;
		}
	}
	for(i=0;i<m;i++){
		double vec[3];
		for(j=0;j<3;j++){
			vec[j]=pSamples[i].coor[j]-center.coor[j];
			for(k=0;k<3;k++){
				covMat[j][k]+=vec[j]*vec[k];
			}
		}
	}

	//free pSamples
	free(pSamples);

	//eigenvalue and eigenvector
	double A[9];
	k=0;
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			A[k]=covMat[i][j];
			k++;
		}
	}
	double w[3];
	double Z[9];
	rs(3,A,w,1,Z);

	//Get Q.
	double Q[3][3];
	k=0;
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			Q[i][j]=Z[k];
			k++;
		}
	}
	double length[3]={0,0,0};
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			length[i]+=pow(Q[i][j],2);
		}
		length[i]=sqrt(length[i]);
	}
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			Q[i][j]/=length[i];
			printf("Q[%d][%d]=%f\t",i,j,Q[i][j]);
		}
		printf("\n");
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
				sum+=Q[i][k]*matrixP[k][j];
			}
			P1[i][j]=sum;
		}
	}
	//Is P' normal? Create an off file to test it.
	char fp1Name[100]="/Users/sfma/Desktop/p1.off";
	FILE *fp1=fopen(fp1Name,"w");
	fprintf(fp1,"OFF\n");
	fprintf(fp1,"%d %d %d\n",m, 0,0);
	for(i=0;i<m;i++){
		fprintf(fp1,"%f %f %f\n",P1[0][i],P1[1][i],P1[2][i]);
	}
	fclose(fp1);

	//free matrix P (since we have Q now) and copyP
	for(i=0;i<3;i++){
		free(matrixP[i]);
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

	//Scale
	double dist,maxDist=0;
	for(i=0;i<NumVer;i++){
		dist=pow(V1[0][i],2)+pow(V1[1][i],2)+pow(V1[2][i],2);
		dist=sqrt(dist);
		if(dist>maxDist){
			maxDist=dist;
		}
	}
	for(i=0;i<3;i++){
		for(j=0;j<NumVer;j++){
			V1[i][j]/=maxDist;
		}
	}

	/*char fv1Name[100]="/Users/sfma/Desktop/v1.off";
	FILE *fv1=fopen(fv1Name,"w");
	fprintf(fv1,"OFF\n");
	fprintf(fv1,"%d %d %d\n",m, 0,0);
	for(i=0;i<m;i++){
		fprintf(fv1,"%f %f %f\n",V1[0][i],V1[1][i],V1[2][i]);
	}
	fclose(fv1);*/

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


