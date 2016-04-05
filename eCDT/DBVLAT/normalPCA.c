/*
 * normalPCA.c
 *
 *  Created on: Apr 2, 2016
 *      Author: sfma
*   Description: normalPCA, well, normalSVD proposed in "Multi-Fourier spectra descriptor and augmentation with
*   spectral clustering for 3D shape retrieval", whose implementation has much in common to pointPCA. Because I
*   cannot find a stable svd implementation in c for now, I use pca instead.
 */

#include "Ds.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Rotate.h"
#include "eispack.h"
#include "GenerateRandomPoints.h"
//#include "svd.h"

#define sign(a)  (((a)<0) ? -1: ((a)>0))

//Function declaration
double *FindNormalCenter(double **normals, int m);
void verticesInfo(pVer vertex, int NumVer);
void normalPCA(int m, pVer vertex, int NumVer, pTri triangle, int NumTri, pVer newVertex);

double *FindNormalCenter(double **normals, int m){
	static double center[3]={0,0,0};
	int i;
	for(i=0;i<m;i++){
		center[0]+=normals[0][i];
		center[1]+=normals[1][i];
		center[2]+=normals[2][i];
	}
	center[0]/=m;
	center[1]/=m;
	center[2]/=m;
	return center;
}

void verticesInfo(pVer vertex, int NumVer){
	//Print coordinates of vertices
	int i,j;
	for(i=0;i<NumVer;i++){
		for(j=0;j<3;j++){
			printf("%f\t",vertex[i].coor[j]);
		}
		printf("\n");
	}
}

void normalPCA(int m, pVer vertex, int NumVer, pTri triangle, int NumTri, pVer newVertex){
	/*
	 * m is the total number of points sampled on the surface.
	 */

	//verticesInfo(vertex, NumVer);
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
	//printf("Triangle model written.\n");



	//Randomly sampling is slightly different from pointPCA. What they have in common is that they both sample points.
	 // The difference is that, normalPCA also use vertices on the trianle.
	 //
	//printf("here? Before normals.\n");
	double *normals[3];
	for(i=0;i<3;i++){
		normals[i]=(double *)malloc(m*sizeof(double));
	}
	pVer pSamples=(pVer)malloc(m*sizeof(Ver));
	k=0;
	//printf("totalarea of %d faces is %f.\n",NumMyTri,totalArea);
	for(i=0;i<NumMyTri;i++){
		//printf("Area of %dth triangle is %f.\n",i,area[i]);
		int numSamples=(int)(area[i]/totalArea*m);
		for(j=0;j<numSamples;j++){
			//if(i<100){
			//	printf("This time the algorithm selects face %d, its area is %f, while total area is %f.\n",i,area[i],totalArea);
			//}
			pSamples[k]=generatePoints(vertex,myTriangle[i]);
			//use the sampled point and two vertices of a triangle to generate the normal
			vector tmpvec1, tmpvec2;
			vector normVec;
			double norm=0;

			tmpvec1.x=vertex[myTriangle[i].v[0]].coor[0]-pSamples[k].coor[0];
			tmpvec1.y=vertex[myTriangle[i].v[0]].coor[1]-pSamples[k].coor[1];
			tmpvec1.z=vertex[myTriangle[i].v[0]].coor[2]-pSamples[k].coor[2];
			tmpvec2.x=vertex[myTriangle[i].v[1]].coor[0]-pSamples[k].coor[0];
			tmpvec2.y=vertex[myTriangle[i].v[1]].coor[1]-pSamples[k].coor[1];
			tmpvec2.z=vertex[myTriangle[i].v[1]].coor[2]-pSamples[k].coor[2];

			normVec=cross(tmpvec1, tmpvec2);
			norm=pow(normVec.x,2)+pow(normVec.y,2)+pow(normVec.z,2);
			norm=sqrt(norm);
			normVec.x/=(norm);
			normVec.y/=(norm);
			normVec.z/=(norm);
			//printf("norm:%f \n",norm);
			//printf("%f\t%f\t%f\n",normVec.x,normVec.y,normVec.z);

			normals[0][k]=normVec.x;
			normals[1][k]=normVec.y;
			normals[2][k]=normVec.z;
			k++;
		}
	}
	for(;k<m;k++){
		int randindx=(int)(((double)rand())/RAND_MAX*(NumMyTri-1));
		pSamples[k]=generatePoints(vertex, myTriangle[randindx]);

		vector tmpvec1, tmpvec2;
		vector normVec;
		double norm=0;

		tmpvec1.x=vertex[myTriangle[randindx].v[0]].coor[0]-pSamples[k].coor[0];
		tmpvec1.y=vertex[myTriangle[randindx].v[0]].coor[1]-pSamples[k].coor[1];
		tmpvec1.z=vertex[myTriangle[randindx].v[0]].coor[2]-pSamples[k].coor[2];
		tmpvec2.x=vertex[myTriangle[randindx].v[1]].coor[0]-pSamples[k].coor[0];
		tmpvec2.y=vertex[myTriangle[randindx].v[1]].coor[1]-pSamples[k].coor[1];
		tmpvec2.z=vertex[myTriangle[randindx].v[1]].coor[2]-pSamples[k].coor[2];


		//printf("%f\t%f\t%f\n",tmpvec1.x,tmpvec1.y,tmpvec1.z);
		//printf("%f\t%f\t%f\n",tmpvec2.x,tmpvec2.y,tmpvec2.z);
		normVec=cross(tmpvec1, tmpvec2);
		norm=pow(normVec.x,2)+pow(normVec.y,2)+pow(normVec.z,2);
		norm=sqrt(norm);
		normVec.x/=(norm);
		normVec.y/=(norm);
		normVec.z/=(norm);
		//printf("norm:%f \n",norm);
		//printf("%f\t%f\t%f\n",normVec.x,normVec.y,normVec.z);

		normals[0][k]=normVec.x;
		normals[1][k]=normVec.y;
		normals[2][k]=normVec.z;
	}
	free(myTriangle);
	free(area);
	//Points generated.
	//write the samples to an off file and open the off file to see if it maintain the object' shape.
	//char fSamples[100]="/Users/sfma/Desktop/samples.off";
	//FILE *fsam=fopen(fSamples,"w");
	//fprintf(fsam,"OFF\n");
	//fprintf(fsam,"%d %d %d\n",m, NumMyTri,0);
	//for(i=0;i<m;i++){
	//	fprintf(fsam,"%f %f %f\n",pSamples[i].coor[0],pSamples[i].coor[1],pSamples[i].coor[2]);
	//}
	//fclose(fsam);//

	//Find the center of the normals
	double *normalCenter;
	normalCenter=FindNormalCenter(normals, m);
	for(i=0;i<3;i++){
		printf("%f\t",normalCenter[i]);
	}
	printf("\n");

	for(i=0;i<3;i++){
		for(j=0;j<m;j++){
			normals[i][j]=normals[i][j]-normalCenter[i];
		}
	}


	//generate covariance matrix
	double covMat[3][3];
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			covMat[i][j]=0;
		}
	}

	//use matrix multiplication to get covariance matrix
	double *normalsTrans[m];
	for(i=0;i<m;i++){
		normalsTrans[i]=(double *)malloc(3*sizeof(double));
	}
	for(i=0;i<m;i++){
		for(j=0;j<3;j++){
			normalsTrans[i][j]=normals[j][i];
		}
	}
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			double sum=0;
			for(k=0;k<m;k++){
				sum+=normals[i][k]*normalsTrans[k][j];
			}
			covMat[i][j]=sum;
		}
	}
	printf("==========covMat============\n");
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			printf("%f\t",covMat[i][j]);
		}
		printf("\n");
	}


	for(i=0;i<m;i++){
		free(normalsTrans[i]);
	}

	//free pSamples
	free(pSamples);

	//before function svdcmp destroy matrix P, copy it
	double *copyN[3];
	for(i=0;i<3;i++){
		copyN[i]=(double *)malloc(m*sizeof(double));
		for(j=0;j<m;j++){
			copyN[i][j]=normals[i][j];
		}
	}


	//PCA to get eigenvectors
	double A[3*3];
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			A[i*3+j]=covMat[i][j];
		}
	}
	double W[3];
	double Z[3][3];
	double Z1[3*3];
	rs(3,A,W,1,Z1);
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			Z[i][j]=Z1[i*3+j];
		}
	}
	printf("====W====\n");
	for(i=0;i<3;i++){
		printf("%f\n",W[i]);
	}
	//test the eigenvalues and eigenvectors
	int row=0;
	//step1. covMat * Z[0] and W[0]*Z[0]
	double tempVec1[3], tempVec2[3];
	for(i=0;i<3;i++){
		tempVec1[i]=0;
		for(j=0;j<3;j++){
			tempVec1[i]+=covMat[i][j]*Z[row][j];
		}
	}
	for(i=0;i<3;i++){
		tempVec2[i]=W[row]*Z[row][i];
	}
	printf("Au and lambda*u\n");
	for(i=0;i<3;i++){
		printf("%f %f\n",tempVec1[i],tempVec2[i]);
	}
	//So I see that the eigenvectors are organized by rows.
	//Note that eigenvalues are in ascending order.

	//get Q.
	double Q[3][3];
	double length[3]={0,0,0};
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			Q[i][j]=Z[2-i][j];
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
				sum+=Q[i][k]*copyN[k][j];
			}
			P1[i][j]=sum;
		}
	}

	//free matrix P (since we have Q now) and copyP
	for(i=0;i<3;i++){
		free(normals[i]);
		free(copyN[i]);
	}

	double f[3]={0,0,0};
	for(i=0;i<3;i++){
		for(j=0;j<m;j++){
			f[i]+=sign(P1[i][j])*pow(P1[i][j],2);
		}
	}

	//printf("=======F=============\n");
	double F[3][3];
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			if(j==i){
				F[i][j]=sign(f[j]);
				//printf("%f\t",F[i][j]);
			}
			else{
				F[i][j]=0;
			}
		}
	}
	//printf("\n");

	//free P'
	for(i=0;i<3;i++){
		free(P1[i]);
	}



	//matrix V and V'
	//int i,j,k;
	double *V[3];
	double *V1[3];
	for(i=0;i<3;i++){
		V[i]=(double *)malloc(NumVer*sizeof(double));
		V1[i]=(double *)malloc(NumVer*sizeof(double));
		if(V[i]==NULL){
			printf("Space allocation for V[%d] failed.\n",i);
		}
		if(V1[i]==NULL){
			printf("Space allocation for V1[%d] failed.\n",i);
		}
	}
	printf("Successfully allocate space for V and V1.\n");
	printf("=============V initialized===================\n");
	printf("First we need to identify if vertex is still valid.\n");
	if(vertex==NULL){
		printf("Invalid pointer vertex.\n");
	}
	for(i=0;i<3;i++){
		for(j=0;j<NumVer;j++){
			V[i][j]=vertex[j].coor[i];
			printf("%f\t",V[i][j]);
		}
		printf("\n");
	}
	double *verticesCenter;
	verticesCenter=FindNormalCenter(V,NumVer);

	//double verticesCenter[3]={0,0,0};
	printf("================V=================\n");
	for(i=0;i<3;i++)
	{
		for(j=0;j<NumVer;j++){
			V[i][j]=vertex[j].coor[i]-verticesCenter[i];
			printf("%f\t",V[i][j]);
		}
		printf("\n");
	}



	printf("tmp=FQ\n");
	double tmp[3][3];
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			double sum=0;
			for(k=0;k<3;k++){
				sum+=F[i][k]*Q[k][j];
			}
			tmp[i][j]=sum;
			printf("%f\t",tmp[i][j]);
		}
		printf("\n");
	}
	printf("============V1==============\n");
	for(i=0;i<3;i++){
		//printf("i=%d\n",i);
		for(j=0;j<NumVer;j++){
			//printf("j=%d\n",j);
			double sum=0;
			for(k=0;k<3;k++){
				sum+=tmp[i][k]*V[k][j];
			}
			V1[i][j]=sum;
			printf("%f\t",V1[i][j]);
		}
		printf("\n");
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
	printf("============V1 normalized=========\n");
	for(i=0;i<3;i++){
		for(j=0;j<NumVer;j++){
			V1[i][j]/=maxDist;
			printf("%f\t",V1[i][j]);
		}
		printf("\n");
	}

	/*char fv1Name[100]="/Users/sfma/Desktop/v1.off";
	FILE *fv1=fopen(fv1Name,"w");
	fprintf(fv1,"OFF\n");
	fprintf(fv1,"%d %d %d\n",m, 0,0);
	for(i=0;i<m;i++){
		fprintf(fv1,"%f %f %f\n",V1[0][i],V1[1][i],V1[2][i]);
	}
	fclose(fv1);*/

	//pVer newVertex=(pVer)malloc(NumVer*sizeof(Ver));  //newVertex has already been malloced in the caller function.
	for(i=0;i<NumVer;i++){
		newVertex[i].coor[0]=V1[0][i];
		newVertex[i].coor[1]=V1[1][i];
		newVertex[i].coor[2]=V1[2][i];
	}


	printf("About to free V and V1.\n");
	for(i=0;i<3;i++){
		printf("i=%d\n",i);
		free(V[i]);
		printf("V[%d] freed.\n",i);
	}

	for(i=0;i<3;i++){
		printf("i=%d\n",i);
		free(V1[i]);
		printf("V1[%d] freed.\n",i);
	}

	printf("End of normalPCA.\n");

}
