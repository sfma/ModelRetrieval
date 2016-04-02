/*
 * RWOFF.c
 *
 *  Created on: Apr 2, 2016
 *      Author: sfma
 *  Description: Read and write off file format.
 *  off file format:
 *  OFF
 *  NumVer NumTri 0
 *  v0.x v0.y v0.z
 *  ...
 *  triangle0.nodename triangle0.0 triangle0.1 ...
 *  ...
 */
#include "Ds.h"
#include <stdio.h>
#include "RWOFF.h"

int writeOff(char *filename, pVer vertex, int NumVer, pTri triangle, int NumTri);

int writeOff(char *filename, pVer vertex, int NumVer, pTri triangle, int NumTri){
	printf("writeOff function.\n");
	FILE *fpt;
	fpt=fopen(filename,"w");
	if(fpt==NULL){
		printf("Write failed.\n");
		return -1;
	}
	fprintf(fpt,"OFF\n");
	fprintf(fpt,"%d %d %d\n",NumVer, NumTri,0);
	//write vertices
	int i,j;
	for(i=0;i<NumVer;i++){
		fprintf(fpt,"%f %f %f\n",vertex[i].coor[0],vertex[i].coor[1],vertex[i].coor[2]);
	}
	//write faces
	for(i=0;i<NumTri;i++){
		fprintf(fpt,"%d ",triangle[i].NodeName);
		for(j=0;j<triangle[i].NodeName;j++){
			fprintf(fpt,"%d ",triangle[i].v[j]);
		}
		fprintf(fpt,"\n");
	}
	fclose(fpt);
	return 0;
}
