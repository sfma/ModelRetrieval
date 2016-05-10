/*
 * Main.c
 *
 *  Created on: May 10, 2016
 *      Author: sfma
 *
*   Description: For now, this c program just compute pairwise distance for all models listed in the list file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NumModel 907
#define NumCam 18
#define MAX_LINE_LEN 10000
//distmat is a global variable
float distmat[NumModel][NumModel];

int main(){
	//First of all, calculate the distance matrix
	int i,j,k;
	//these variables are also available in DBVLAT project
	const int numCenters=32;
	const int dimension=128;
	int sigSize=numCenters*dimension;
	float featureSrc[NumCam][sigSize];
	float featureDst[NumCam][sigSize];
	char srcname[100];
	char dstname[100];
	FILE *fptsrc, *fptdst;
	char listname[100]="mlist.txt"; //list file name
	FILE *fptlist=fopen(listname, "r");
	FILE *fptlist1=fopen(listname,"r");
	if(!fptlist){
		printf("error opening list file.\n");
	}
	char line[MAX_LINE_LEN];
	int indexsrc=0, indexdst=0;
	while(fgets(line,MAX_LINE_LEN,fptlist)){
		line[strlen(line)-1]=0x00; //remove the ending '\n'
		srcname="";
		//read its feature file and get the features
		fptsrc=fopen(srcname,"r");
		if(!fptsrc){
			printf("error opening feature file %s.\n", srcname);
		}
		for(i=0;i<NumCam;i++){
			fgets(line,MAX_LINE_LEN,fptsrc);
			line[strlen(line)-1]=0x00;
			for(j=0;j<sigSize;j++){
				sscanf(line,"%f",featureSrc[i][j]);
			}
		}
		//one vs all
		while(fgets(line,MAX_LINE_LEN,fptlist)){
			line[strlen(line)-1]=0x00; //remove the ending '\n'
			dstname="";
			//read its feature file and get the features
			fptsrc=fopen(srcname,"r");
			if(!fptsrc){
				printf("error opening feature file %s.\n", srcname);
			}
			for(i=0;i<NumCam;i++){
				fgets(line,MAX_LINE_LEN,fptdst);
				line[strlen(line)-1]=0x00;
				for(j=0;j<sigSize;j++){
					sscanf(line,"%f",featureDst[i][j]);
				}
			}
			//calculate feature distance
			int tmpdist=0;
			for(i=0;i<NumCam;i++){
				for(j=0;j<sigSize;j++){
					tmpdist+=pow(featureSrc[i][j]-featureDst[i][j],2);
				}
			}
			distmat[indexsrc][indexdst]=tmpdist;
			indexdst+=1;
		}
		indexsrc+=1;
	}
	return 0;
}
