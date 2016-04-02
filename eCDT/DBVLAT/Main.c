/*
 * depthImage.c
 *
 *  Created on: Mar 22, 2016
 *      Author: sfma
 *  Description: Create depth images.
 */
#include "Ds.h"
#include <GL/glut.h>

int winWidth=256, winHeight=256;

void init(){
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void myRenderToMem(unsigned char *dphImage,pVer camera){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera->coor[0],camera->coor[1],camera->coor[2],
			0,0,0,
			0,1,0);
	glReadPixels(0,0,winWidth,winHeight,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,dphImage);
}

void keyboard(unsigned char key, int x, int y){
	unsigned char *dphImage;
	pVer camera;
	switch(key){
	case 'n':
		//First, read a model from a file
		;
		//Depth image rendering and feature extraction
	}
}

void display(){

}

void reshape(){

}

/*
int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
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
*/



