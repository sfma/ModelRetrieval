/*
 * testPSVD.c
 *
 *  Created on: Mar 20, 2016
 *      Author: sfma
 *  Description: Test the pointSVD.c.
 *               Read two obj models, do the transform and write the results to obj files. Visualize and see if they are aligned.
 */
#include "Ds.h"
#include "RWObj.h"
#include "pointPCA.h"
#include "normalPCA.h"
#include "RWOFF.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define m 1000

int testPSVD(){
//int main(){
	char prefix[100]="/Users/sfma/3Dretrieval/Executable/pton_obj_data/";
	char filename1[100];
	char filename2[100];
	strcpy(filename1,prefix);
	strcpy(filename2,prefix);
	strcat(filename1,"m34");
	strcat(filename2,"m35");
	pVer vertex1, vertex2;
	pTri triangle1, triangle2;
	int NumVer1, NumVer2;
	int NumTri1, NumTri2;
	ReadObj(filename1, &vertex1, &triangle1, &NumVer1, &NumTri1);
	ReadObj(filename2, &vertex2, &triangle2, &NumVer2, &NumTri2);


	//write the model just read to see if read successfully
	/*
	char out1[100],out2[100];
	char outPrefix[100]="/Users/sfma/Desktop/";
	strcpy(out1,outPrefix);
	strcpy(out2,outPrefix);
	strcat(out1,"out1.obj");
	strcat(out2,"out2.obj");
	SaveObj(out1,vertex1,triangle1,NumVer1,NumTri1);
	SaveObj(out2,vertex2,triangle2,NumVer2,NumTri2);
	free(vertex1);
	free(vertex2);
	*/

	pVer newVertex1=(pVer)malloc(NumVer1*sizeof(Ver));
	pVer newVertex2=(pVer)malloc(NumVer2*sizeof(Ver));
	//pointSVD(m, vertex1, NumVer1, triangle1, NumTri1, newVertex1);
	//pointSVD(m, vertex2, NumVer2, triangle2, NumTri2, newVertex2);

	pointPCA(m, vertex1, NumVer1, triangle1, NumTri1, newVertex1);
	pointPCA(m, vertex2, NumVer2, triangle2, NumTri2, newVertex2);
	//normalPCA(m, vertex1, NumVer1, triangle1, NumTri1, newVertex1);
	//normalPCA(m, vertex2, NumVer2, triangle2, NumTri2, newVertex2);
	free(vertex1);
	free(vertex2);
	char out1[100],out2[100];
	char outPrefix[100]="/Users/sfma/Desktop/";
	strcpy(out1,outPrefix);
	strcpy(out2,outPrefix);
	//strcat(out1,"out1.obj");
	//strcat(out2,"out2.obj");
	//SaveObj(out1,newVertex1,triangle1,NumVer1,NumTri1);
	//SaveObj(out2,newVertex2,triangle2,NumVer2,NumTri2);
	//write in off format instead of obj to take advantage of offviewer, which is in home directory.
	strcat(out1,"out1.off");
	strcat(out2,"out2.off");
	writeOff(out1,newVertex1,NumVer1,triangle1,NumTri1);
	writeOff(out2,newVertex2,NumVer2,triangle2,NumTri2);


	free(newVertex1);
	free(newVertex2);

	free(triangle1);
	free(triangle2);
	return 0;
}


