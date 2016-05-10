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
#include "kmeans.h"
#include "vlad.h"

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

	//variables declaration must be placed outsize the switch case statement
	float *feature[NumModel][NumCam];
	int numKeypoints[NumModel][NumCam];

	unsigned char *dphImages[NumCam]; //dphimage is an array of pointers
	Ver cameras[NumCam];
	//Specify coordinate values of cameras.
	//Coordinate values of cameras are already written in a file. Read the file store the positions.
	//printf("prepare to read cameras.\n");
	char fcam[100]="cameras";
	//I need to write a function of my own to read camera positions. ReadObj does not always work because in some algorithms, cameras not only placed
	//on vertices, but also placed on edges or faces.
	readCameras(fcam, cameras);

	int i,j,k,l,m,n,p,q;
	float *fimage;
	char flist[100]="mlist.txt";
	char line[200];
	FILE *fptList;
	int size=winWidth*winHeight;
	int descriptorSize;
	//Use float data and the L2 distance for clustering
	float *features1d=NULL;
	float *centers;
	VlKMeans *kmeans=vl_kmeans_new(VlDistanceL2, VL_TYPE_FLOAT);
	int numCenters=32;
	float *distances; //used when a feature is assigned to a cluster
	vl_uint32 *indexes;
	float *assignments;
	//float *enc; //encoding in vlad
	int numData;
	float *data; //features for one image
	int dimension; //to be consistent with vlfeat tutorial name conventions
	float *imageSig[NumModel][NumCam]; //image signatures for all image
	char fnameout[100];
	FILE *fptout=NULL;
	int sigDim; //signature dimension

	switch(key){
	case 'n':

		//First, read a model from a file
		for(i=0;i<NumCam;i++){
			dphImages[i]=(unsigned char *)malloc(winWidth*winHeight*sizeof(unsigned char));
		}

		fptList=fopen(flist,"r");
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
				//printf("Depth images done.\n");
				//feature extraction: dsift
				//First, initialize a dsift filter object by vl_dsift_new.
				VlDsiftFilter *filter=vl_dsift_new(winWidth, winHeight);
				//Customze the descriptor paramters
				//vl_dsift_set_bounds(filter,9,9,(winWidth-1-9),(winHeight-1-9));
				vl_dsift_set_steps(filter,4,4);
				/*
				VlDsiftDescriptorGeometry *geom;
				geom->numBinT=36;
				geom->binSizeX=19;
				geom->binSizeY=19;
				geom->numBinX=12; //winWidth/(patchSizeX+step)*4
				geom->numBinY=12;
				vl_dsift_set_geometry(filter, geom);
				*/

				//Process an image by vl_dsift_process


				fimage=(float *)malloc(winWidth*winHeight*sizeof(float));

				for(j=0;j<size;j++){
					fimage[j]=(float)dphImages[i][j];
				}
				vl_dsift_process(filter,fimage); //the second parameter of this function is float const *
				//Retrieve the number of keypoints, keypoints, and their descriptors
				numKeypoints[k][i]=vl_dsift_get_keypoint_num(filter);
				//feature[i]=(float *)malloc(numKeypoints[i]*128);
				descriptorSize=filter->descrSize;
				dimension=descriptorSize;
				feature[k][i]=(float *)malloc(numKeypoints[k][i]*filter->descrSize*sizeof(float));
				printf("k=%d,i=%d\n",k,i);
				if(feature[k][i]){
					//successfully allocate space for features
				}
				else{
					printf("Allocating space for features failed.\n");
				}
				//VlDsiftKeypoint const *pkeypoint=vl_dsift_get_keypoints(filter);
				vl_dsift_get_keypoints(filter);
				feature[k][i]=vl_dsift_get_descriptors(filter);
				free(fimage);

				//delte the dsift filter
				vl_dsift_delete(filter);
			}
			k++;
		}

		printf("k=%d,i=%d\n",k,i);

		//features for images from all models extracted
		//Now, k-means clustering
		int numFeatures=0; //used for clustering and its value must be calculated during dsift calculation
		for(m=0;m<NumModel;m++){
			for(n=0;n<NumCam;n++){
				numFeatures+=numKeypoints[m][n];
			}
		}

		features1d=(double *)malloc(numFeatures*descriptorSize*sizeof(float));
		//multidimensional variable "feature" to 1D "features1d"
		for(m=0;m<NumModel;m++){
			for(n=0;n<NumCam;n++){
				for(p=0;p<numKeypoints;p++){
					for(q=0;q<descriptorSize;q++){
						features1d=feature[m][n][p*descriptorSize+q];
					}
				}
			}
		}
		//Use Lloyd algorithm
		vl_kmeans_set_algorithm(kmeans, VlKMeansLloyd);
		//Initialize the cluster centers by randomly sampling the data
		vl_kmeans_init_centers_with_rand_data(kmeans, features1d, descriptorSize, numFeatures, numCenters);
		//Run at most 100 iterations of cluster refinement using Lloytd algorithm
		vl_kmeans_set_max_num_iterations(kmeans, 100);
		vl_kmeans_refine_centers(kmeans, features1d, numFeatures);
		//Obtain the cluster centers
		centers=vl_kmeans_get_centers(kmeans);
		//print centers: according to http://stackoverflow.com/questions/28438212/vlfeat-kmeans-c-api-explanation
		for(m=0;m<numCenters;m++){
			printf("center # %d: \n",m);
			for(n=0;n<descriptorSize;n++){
				printf("coord[%d]=%f\n",n,centers[m][n]);
			}
		}
		//Get signatures for each image
		for(m=0;m<NumModel;m++){
			for(n=0;n<NumCam;n++){
				numData=numKeypoints[m][n];
				//data=(float *)malloc(numData*dimension*sizeof(float));
				//asign data
				data=feature[m][n]; //data here is different from that in vlfeat VLAD tutorial.
				//In tutorial, data means all the data used to do the clustering
				//here, data is data to encode, i.e., original features for one image
				//kmeans clustering
				indexes=(vl_uint32 *)malloc(sizeof(vl_uint32)*numData);
				distances=vl_malloc(sizeof(float)*numData);
				vl_kmeans_quantize(kmeans, indexes, distances, data, numData);
				//VLAD
				assignments=vl_malloc(sizeof(float)*numData*numCenters);
				memset(assignments, 0, sizeof(float)*numData*numCenters);
				for(l=0;l<numData;l++){
					assignments[i*numCenters+indexes[i]]=1.;
				}
				//allocate space for this image signature, which is a vector
				//enc=vl_malloc(sizeof(float)*dimension*numCenters);
				imageSig[m][n]=vl_malloc(sizeof(float)*dimension*numCenters);
				//do the encoding job
				//vl_vlad_encode(enc, VL_TYPE_FLOAT,centers,dimension,numCenters,data, numData);
				vl_vlad_encode(imageSig[m][n], VL_TYPE_FLOAT,centers,dimension,numCenters,data, numData);
			}
		}

		//write feature files
		sigDim=dimension*numCenters;
		for(m=0;m<NumModel;m++){
			//save features for this model
			//one file correspond to one model, and each line for one image
			//first, give the output name and open the file for writing
			fnameout="";
			fptout=fopen(fnameout,"w");
			if(!fptout){
				printf("error writing feature file for model %d.\n",m);
			}else{
				for(n=0;n<NumCam;n++){
					for(l=0;l<sigDim;l++){
						fprintf(fptout,"%f\t",imageSig[m][n][l]);
					}
					fprintf(fptout,"\n");
				}
			}
		}

		//freeing variable feature lead to error: pointer being freed was not allocated.
		//I know something like this the part below is wrong because k does not neccessarily be NumModel. And when k is not equal to NumModel,
		//feature[index(a value larger than k)][j] is not allocated.
		/*for(i=0;i<NumModel;i++){
			for(j=0;j<NumCam;j++){

				free(feature[i][j]);
			}
		}

		for(i=0;i<NumCam;i++){
			free(dphImages[i]);
		}*/

		//why just freeing feature[0][0] cause the error?
		//free(feature[0][0]);
		/*for(m=0;m<k;m++){ //k models
			for(n=0;n<NumCam;n++){
				printf("m=%d,n=%d\n",m,n);
				free(feature[m][n]);
			}
		}*/

		for(i=0;i<NumCam;i++){
			free(dphImages[i]);
		}

		break;
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





