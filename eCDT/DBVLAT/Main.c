/*
 * depthImage.c
 *
 *  Created on: Mar 22, 2016
 *      Author: sfma
 *  Description: Create depth images.
 */
#include "Ds.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "dsift.h"
#include "RWOBJ.h"
#include "TranslateScale.h"

void display();

#define NumModel 907 //# of models in Princeton test database
#define NumCam 18
#define MAXLINELEN 100//maximum length of one line of the file which specifies the coordinate values of cameras
#define Dimension 128//feature dimension

int winWidth=256, winHeight=256;
pVer vertex=NULL;
pTri triangle=NULL;
int NumVer=0, NumTri=0; //used in gl functions, such as display

pVer vertex1,vertex2;
pTri triangle1, triangle2;
int NumVer1, NumTri1, NumVer2, NumTri2;

Ver Translate1,Translate2;
double Scale1, Scale2;


void readCameras(char *fname, pVer cameras){
	/*
	 * This function supposes that each line of the file specifies the coordinate values of one camera.
	 */
	//printf("readCameras function.\n");
	FILE *fpt;
	fpt=fopen(fname,"r");
	if(!fpt){
		printf("error opening file %s.\n",fname);
		exit(1);
	}
	char *strline=(char *)malloc(MAXLINELEN+1);
	//while(getline(&strline,)!=-1)
	fgets(strline,MAXLINELEN,fpt);
	strline[strlen(strline)-1]='\0';
	if(strcmp(strline,"cameras")!=0){
		printf("Bad file format.\n");
	}
	int k=0;
	while(fgets(strline,MAXLINELEN,fpt)){
		strline[strlen(strline)-1]='\0'; //remove the new line character
		//puts(strline);
		while(isspace(strline) && (*++strline!='\0')) ;//skip white spaces
		if(*strline=='\0'){
			continue;
		}
		if((strline[0]=='/') &&(strline[1]=='/')){
			continue; //skip the comment lines
		}
		sscanf(strline,"%lf %lf %lf",&cameras[k].coor[0],&cameras[k].coor[1],&cameras[k].coor[2]);
		//printf("%f %f %f\n\n",cameras[k].coor[0],cameras[k].coor[1],cameras[k].coor[2]);
		k++;
		continue;
	}
	if(k!=NumCam){
		printf("Error reading camera positions.\n");
	}
	free(strline);
	fclose(fpt);
}


void init(){
	glClearColor(1.0,1.0,1.0,0.0); //set background color to white
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void myRenderToMem(unsigned char *dImage,pVer camera, pVer ver, pTri tri, int nv, int nt){
	/*
	 * @parameters:
	 * 	dImages: depth image rendered for a model specified by the last four parameters
	 * 	camera: position of a camera
	 */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera->coor[0],camera->coor[1],camera->coor[2],
			0,0,0,
			0,1,0);
	vertex=ver;
	triangle=tri;
	NumVer=nv;
	NumTri=nt;

	display();
	glReadBuffer(GL_BACK);
	glReadPixels(0,0,winWidth,winHeight,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,dImage);
}

void keyboard(unsigned char key, int x, int y){
	//Just read an obj file to test if the opengl part works.
	/*char test[100]="/Users/sfma/benchmark/ptonobj/m0";
	ReadObj(test,&vertex,&triangle,&NumVer,&NumTri);
	if(!vertex){
		printf("vertex still Null.\n");
	}
	//printf("NumVer: %d.\n",NumVer);
	//printf("NumTri: %d.\n",NumTri);
	display();*/
	//Good. It works.

	//printf("keyboard function.\n");

	float feature[NumCam][Dimension];

	unsigned char *dphImages[NumCam]; //dphimage is an array of pointers
	Ver cameras[NumCam];
	//Specify coordinate values of cameras.
	//Coordinate values of cameras are already written in a file. Read the file store the positions.
	//printf("prepare to read cameras.\n");
	char fcam[100]="cameras";
	//I need to write a function of my own to read camera positions. ReadObj does not always work because in some algorithms, cameras not only placed
	//on vertices, but also placed on edges or faces.
	readCameras(fcam, cameras);



	int i,k;
	switch(key){
	case 'n':
		//First, read a model from a file
		for(i=0;i<NumCam;i++){
			dphImages[i]=(unsigned char *)malloc(winWidth*winHeight*sizeof(unsigned char));
		}
		char flist[100]="mlist.txt";
		char line[200];
		FILE *fptList=fopen(flist,"r");
		if(fptList==NULL){
			printf("Error open list file.\n");
			exit(1);
		}
		k=0;
		while(fgets(line, 200, fptList)){
			line[strlen(line)-1]='\0';
			//printf("Model location: %s\n",line);
			//read the model file,render depth images and extract features
			ReadObj(line,&vertex1,&triangle1,&NumVer1,&NumTri1);
			//printf("NumVer1: %d.\n",NumVer1);
			//printf("NumTri1: %d.\n",NumTri1);
			//printf("Model read.\n");

			//Translation and scale normalization
			TranslateScale(vertex1, NumVer1, triangle1, NumTri1, line, &Translate1, &Scale1);

			for(i=0;i<NumCam;i++){
				myRenderToMem(dphImages[i],cameras+i,vertex1,triangle1,NumVer1,NumTri1);
				//feature extraction: dsift
				//First, initialize a dsift filter object by vl_dsift_new.
				VlDsiftFilter *filter=vl_dsift(winWidth, winHeight);
				//Customze the descriptor paramters
				vl_dsift_set_steps(filter,4,4);
				VlDsiftDescriptorGeometry *geom;
				geom->numBinT=36;
				geom->numBinX=20;
				geom->numBinY=20;
				geom->binSizeX=19;
				geom->binSizeY=19;
				vl_dsift_set_geometry(filter, geom);
				//Process an image by vl_dsift_process
				vl_dsift_process(filter,dphImages[i]);
				//Retrieve the number of keypoints, keypoints, and their descriptors
				int numKeypoint;
				numKeypoint=vl_dsift_get_keypoint_num(filter);
				VlDsiftKeypoint const *pkeypoint=vl_dsift_get_keypoints(filter);
				feature[i]=vl_dsift_get_descriptors(filter);
				//delte the dsift filter
				vl_dsift_delete(filter);
			}
		}


		for(i=0;i<NumCam;i++){
			free(dphImages[i]);
		}
	}
}

void display(){
	int i,j;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
		glColor3f(0.0,0.0,0.0); //set paint color to black
		for(i=0;i<NumTri;i++){
			glBegin(GL_POLYGON);
				for(j=0;j<triangle[i].NodeName;j++){
					glVertex3d(vertex[triangle[i].v[j]].coor[0],vertex[triangle[i].v[j]].coor[1],vertex[triangle[i].v[j]].coor[2]);
				}
			glEnd();
		}
	glPopMatrix();
	glutSwapBuffers();
}

void reshape(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1,1,-1,1,0.0,2.0);
	glViewport(0,0,(GLsizei)winWidth,(GLsizei)winHeight);

	//I do not know why this part.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(1,0,0,0,0,0,0,1,0);
}


int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(200,200);
	glutInitWindowSize(winWidth,winHeight);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}




