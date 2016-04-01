/*
 * GenerateRandomPoints.c
 *
 *  Created on: Mar 21, 2016
 *      Author: sfma
 */
#include "Ds.h"
#include "Rotate.h"
#include <math.h>
#include <stdlib.h>
#include "GenerateRandomPoints.h"
//Function declaration
double AreaOfTri(pVer vertex, Tri ATriangle);
Ver FindCenter(pVer pSamples, int m);
Ver generatePoints(pVer vertex, Tri ATriangle);

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
	//vec1.x=v3.x-v1.x; vec1.y=v3.y-v1.y; vec1.z=v3.z-v1.z;
	//vec2.x=v3.x-v2.x; vec2.y=v3.y-v2.y; vec2.z=v3.z-v2.z;

	vec1.x=v2.x-v1.x; vec1.y=v2.y-v1.y; vec1.z=v2.z-v1.z;
	vec2.x=v3.x-v1.x; vec2.y=v3.y-v1.y; vec2.z=v3.z-v1.z;

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
	//print r1, r2; c1, c2, c3
	//printf("=============================\n");
	//printf("r1=%f\tr2=%f\t\n",r1,r2);
	//printf("c1=%f\tc2=%f\tc3=%f\t\n",c1,c2,c3);
	//printf("=============================\n\n");
	point.coor[0]=c1*vertex[ATriangle.v[0]].coor[0]+c2*vertex[ATriangle.v[1]].coor[0]+c3*vertex[ATriangle.v[2]].coor[0];
	point.coor[1]=c1*vertex[ATriangle.v[0]].coor[1]+c2*vertex[ATriangle.v[1]].coor[1]+c3*vertex[ATriangle.v[2]].coor[1];
	point.coor[2]=c1*vertex[ATriangle.v[0]].coor[2]+c2*vertex[ATriangle.v[1]].coor[2]+c3*vertex[ATriangle.v[2]].coor[2];
	return point;
}
