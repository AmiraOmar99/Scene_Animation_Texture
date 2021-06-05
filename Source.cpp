/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

 /*
  * robot.c
  * This program shows how to composite modeling transformations
  * to draw translated and rotated hierarchical models.
  * Interaction:  pressing the s and e keys (shoulder and elbow)
  * alters the rotation of the robot arm.
  */
#include <stdlib.h>
#include <iostream>
#include <GL/glut.h>
#include<math.h>
#include"imageloader.h"
#include"texture.h"
#include"startRendering.h"
#include "glm.h"




static int elbow[] = { 0, 0 };//right and left elbow angles
static int arm1[] = { 0, 0 };//right and left Shoulder angles
static int arm11[] = { 0,0 }; //ANOTHER angles of shoulder
static int arm2[] = { 0, 0 };//right and left arm angles
static int leg1[] = { 0, 0 };//right and left hip angles
static int leg11[] = { 0,0 }; // ANOTHER angles of hip
static int leg2[] = { 0, 0 };//right and left leg angles
static int knee[] = { 0, 0 };//right and left knee angles
static int flang[] = { 0, 0 };//flang angles
static int finger[] = { 0, 0 };//flang angles
float VRot = 0.0;
int set_state=0;
bool standing=true;
GLMmodel* pmodel;
GLMmodel* pmodel0;
GLMmodel* pmodel5;
GLMmodel* pmodel6;
GLMmodel* pmodel1;
GLMmodel* pmodel2 = glmReadOBJ("data/roundTable2.obj");
GLMmodel* pmodel3 = glmReadOBJ("data/flowers.obj");
GLMmodel* pmodel4 = glmReadOBJ("data/sedia.obj");
GLMmodel* pmodel7 = glmReadOBJ("data/taburet1_update.obj");
GLuint textureId;

        
double up[] = { 0.0,1.0,0.0};
double eye[] = { 0.0,0.5,9.0 }; 
double center[] = { 0.0,0.0,0.0 };
double speed = 0.1;

int moving, startx, starty;
float x_body=0.0;
float y_body=0.0;
float z_body=0.0;
int leg_state=1;
GLfloat angle = 0.0;   /* in degrees */
GLfloat angle2 = 0.0;   /* in degrees */
int mainBody=0;
int rCounter=0;
static float ground = 1.0;
static int jump_state = 1;
static float returningPosition = 1;
void Choose_texture(int id);

void init(void)
{
    glMatrixMode(GL_PROJECTION);
    gluPerspective(65.0, (GLfloat)1024 / (GLfloat)869, 1.0, 30.0);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
}
void rotatePoint(double a[], double theta, double p[])
{

    double temp[3];
    temp[0] = p[0];
    temp[1] = p[1];
    temp[2] = p[2];

    temp[0] = -a[2] * p[1] + a[1] * p[2];
    temp[1] = a[2] * p[0] - a[0] * p[2];
    temp[2] = -a[1] * p[0] + a[0] * p[1];

    temp[0] *= sin(theta);
    temp[1] *= sin(theta);
    temp[2] *= sin(theta);

    temp[0] += (1 - cos(theta)) * (a[0] * a[0] * p[0] + a[0] * a[1] * p[1] + a[0] * a[2] * p[2]);
    temp[1] += (1 - cos(theta)) * (a[0] * a[1] * p[0] + a[1] * a[1] * p[1] + a[1] * a[2] * p[2]);
    temp[2] += (1 - cos(theta)) * (a[0] * a[2] * p[0] + a[1] * a[2] * p[1] + a[2] * a[2] * p[2]);

    temp[0] += cos(theta) * p[0];
    temp[1] += cos(theta) * p[1];
    temp[2] += cos(theta) * p[2];

    p[0] = temp[0];
    p[1] = temp[1];
    p[2] = temp[2];

}
void crossProduct(double a[], double b[], double c[])
{
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(double a[])
{
    double norm;
    norm = a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
    norm = sqrt(norm);
    a[0] /= norm;
    a[1] /= norm;
    a[2] /= norm;
}
void MoveForward() {
    double direction[3];
    direction[0] = center[0] - eye[0];
    direction[1] = center[1] - eye[1];
    direction[2] = center[2] - eye[2];

    eye[0] += direction[0] * speed;
    eye[1] += direction[1] * speed;
    eye[2] += direction[2] * speed;
}
void MoveBackward() {
    double direction[3];
    direction[0] = center[0] - eye[0];
    direction[1] = center[1] - eye[1];
    direction[2] = center[2] - eye[2];

    eye[0] -= direction[0] * speed;
    eye[1] -= direction[1] * speed;
    eye[2] -= direction[2] * speed;

}
void LEFT() {
    rotatePoint(up, -0.157, eye);
}
void RIGHT() {
    rotatePoint(up, 0.157, eye);
}

void UP() {
    double Horizontal[3];
    double direction[3];
    direction[0] = center[0] - eye[0];
    direction[1] = center[1] - eye[1];
    direction[2] = center[2] - eye[2];
    crossProduct(up, direction, Horizontal);
    normalize(Horizontal);
    rotatePoint(Horizontal, 0.157, eye);
    rotatePoint(Horizontal, 0.157, up);
}
void DOWN() {
    double Horizontal[3];
    double direction[3];
    direction[0] = center[0] - eye[0];
    direction[1] = center[1] - eye[1];
    direction[2] = center[2] - eye[2];
    crossProduct(up, direction, Horizontal);
    normalize(Horizontal);
    rotatePoint(Horizontal, -0.157, eye);
    rotatePoint(Horizontal, -0.157, up);
}
void reset()
{
    double e[] = { 0.0, 0.5, 9.0 };
    double c[] = { 0.0, 0.0, 0.0 };
    double u[] = { 0.0, 1.0, 0.0 };
    for (int i = 0; i < 3; i++)
    {
        eye[i] = e[i];
        center[i] = c[i];
        up[i] = u[i];
    }
    angle = 0;
    angle2 = 0;
    x_body=0;
    y_body=0;
    z_body=0;
    mainBody=0;
}
// draw objects
void drawmodel1(void)
{
    if (!pmodel) {
        pmodel = glmReadOBJ("data/roundTable2.obj");
        if (!pmodel) exit(0);
        glmUnitize(pmodel);
        glmFacetNormals(pmodel);
        glmVertexNormals(pmodel, 90.0);
        glmScale(pmodel, .15);
    }
    glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
}
void drawmodel2(void)
{
    if (!pmodel0) {
        pmodel0 = glmReadOBJ("data/flowers.obj");
        if (!pmodel0) exit(0);
        glmUnitize(pmodel0);
        glmFacetNormals(pmodel0);
        glmVertexNormals(pmodel0, 90.0);
        glmScale(pmodel0, .15);
    }
    glmDraw(pmodel0, GLM_SMOOTH | GLM_MATERIAL);
}
void drawmodel3(void)
{
    if (!pmodel5) {
        pmodel5 = glmReadOBJ("data/sedia.obj");
        if (!pmodel5) exit(0);
        glmUnitize(pmodel5);
        glmFacetNormals(pmodel5);
        glmVertexNormals(pmodel5, 90.0);
        glmScale(pmodel5, .15);
    }
    glmDraw(pmodel5, GLM_SMOOTH | GLM_MATERIAL);
}
void drawmodel4(void)
{
    if (!pmodel6) {
        pmodel6 = glmReadOBJ("data/taburet1_update.obj");
        if (!pmodel6) exit(0);
        glmUnitize(pmodel6);
        glmFacetNormals(pmodel6);
        glmVertexNormals(pmodel6, 90.0);
        glmScale(pmodel6, .15);
    }
    glmDraw(pmodel6, GLM_SMOOTH | GLM_MATERIAL);
}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glShadeModel(GL_FLAT);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

   // two light sources and whight light
    GLfloat left_light_position[] =
            {0.0, 10.0, 1.0, 0.0}, right_light_position[] =
            {0.0, -10.0, 1.0, 0.0}, white_light[] =
            {1.0, 1.0, 1.0, 0.0};
    // Material Properties
    GLfloat mat_amb_diff[] = {0.643, 0.753, 0.934, 1.0};
    GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat shininess[] = {10.0};
    glPushMatrix();
        glLightfv(GL_LIGHT0, GL_POSITION, left_light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, white_light);
        glLightfv(GL_LIGHT1, GL_POSITION, right_light_position);
        glLightfv(GL_LIGHT1, GL_AMBIENT_AND_DIFFUSE, white_light);
    glPopMatrix();

    //materials properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,mat_amb_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);


    // Draw The Texture on The Floor
    glPushMatrix();
    glTranslatef(0,-3.75,0);
    drawFloorTexture(textureId);
    glPopMatrix();

    glPushMatrix();
    glRotatef(angle2, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);
    
    glScalef(0.75,0.75,0.75);
    glTranslatef(x_body,y_body,z_body);
    // glTranslatef (0.0, 0.0, -2.0);
    
    glRotatef ((GLfloat) mainBody, 0.0, 1.0, 0.0);
    
    // START OF RIGHT LEG
    glPushMatrix();

    glTranslatef(0.5, 0.0, 0.0);
    glRotatef((GLfloat)leg2[0], 0.0, 0.0, 1.0);
    glTranslatef(-0.5, 0.0, 0.0);

    glRotatef((GLfloat)leg1[0], 1.0, 0.0, 0.0);
    glRotatef((GLfloat)leg11[0], 0.0, 0.0, 1.0);
    glTranslatef(0.32, -1.0, 0.0);

    glPushMatrix();

    glScalef(0.35, 2.0, 0.5);
    glutWireCube(1.0);
    //glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.175, -1.0, 0.0);
    glRotatef(GLfloat(knee[0]), -1.0, 0.0, 0.0);
    glTranslatef(-0.175, -0.75, 0.0);

    glPushMatrix();
    glScalef(0.35, 1.5, 0.5);
    glutWireCube(1.0);
    //glutSolidCube(1.0);
    glPopMatrix();

    glTranslatef(0.0, -0.95, 0.0);
    glPushMatrix();
    glScalef(0.5, 0.25, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // END OF RIGHT LEG


    // START OF LEFT LEG
    glPushMatrix();

    glTranslatef(-0.5, 0.0, 0.0);
    glRotatef((GLfloat)leg2[1], 0.0, 0.0, 1.0);
    glTranslatef(0.5, 0.0, 0.0);

    glRotatef((GLfloat)leg1[1], 1.0, 0.0, 0.0);
    glRotatef((GLfloat)leg11[1], 0.0, 0.0, -1.0);
    glTranslatef(-0.32, -1.0, 0.0);

    glPushMatrix();
    glScalef(0.35, 2.0, 0.5);
    glutWireCube(1.0);
    //glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.175, -1.0, 0.0);
    glRotatef(GLfloat(knee[1]), -1.0, 0.0, 0.0);
    glTranslatef(-0.175, -0.75, 0.0);
    glPushMatrix();
    glScalef(0.35, 1.5, 0.5);
    glutWireCube(1.0);
    //glutSolidCube(1.0);
    glPopMatrix();

    glTranslatef(0.0, -0.95, 0.0);
    glPushMatrix();
    glScalef(0.5, 0.25, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
    // END OF LEFT LEG


    // START OF TRUNK
    glPushMatrix();
    glTranslatef(0.0, 1.0, 0.0);
    glScalef(1.0, 2.0, 0.5);
    glutWireCube(1.0);
    glPopMatrix();
    // END OF TRUNK


    // START OF HEAD
    glPushMatrix();
    glTranslatef(0.0, 2.5, 0.0);
    glutWireSphere(0.35, 16, 16);
    glPopMatrix();
    // END OF HEAD


    // START OF RIGHT ARM

    glPushMatrix();



    glTranslatef(1.0, 1.85, 0.02);


    glRotatef((GLfloat)arm2[0], 1.0, 0.0, 0.0);


    glTranslatef(-0.375, 0, 0);
    glRotatef(arm1[0], 0.0, 0.0, 1.0);
    glRotatef(arm11[0], 1.0, 0.0, 0.0);
    glTranslatef(0.375, 0, 0);

    // Draw the limb
    glPushMatrix();
    glScalef(0.75, 0.3, 0.3);
    glutWireCube(1.0);
    glPopMatrix();




    glTranslatef(0.75, 0, 0);

    glTranslatef(-0.375, 0, 0.0);
    glRotatef((GLfloat)elbow[0], 0.0, 0.0, 1.0);
    glTranslatef(0.375, 0, 0.0);
    glPushMatrix();
    glScalef(0.75, 0.3, 0.3);
    glutWireCube(1.0);
    glPopMatrix();



    // START OF FINGER 1
    glPushMatrix();

    glTranslatef(0.475, 0.05, 0.1125);

    glTranslatef(-0.1, 0, 0);
    glRotatef(finger[0], 0, 0, 1);
    glTranslatef(0.1, 0, 0);
    glPushMatrix();
    glScalef(0.2, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();




    glPushMatrix();
    glTranslatef(0.1, 0, 0);
    glTranslatef(-0.05, 0.0, 0.0);
    glRotatef(flang[0], 0, 0, 1);
    glTranslatef(0.05, 0.0, 0.0);

    glScalef(0.1, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    // END OF FINGER 1


    // START OF FINGER 2
    glPushMatrix();

    glTranslatef(0.475, 0.05, 0.0);


    glTranslatef(-0.1, 0, 0);
    glRotatef(finger[0], 0, 0, 1);
    glTranslatef(0.1, 0, 0);
    glPushMatrix();
    glScalef(0.2, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();



    glPushMatrix();
    glTranslatef(0.1, 0, 0);
    glTranslatef(-0.05, 0, 0);
    glRotatef(flang[0], 0, 0, 1);
    glTranslatef(0.05, 0, 0);
    glScalef(0.1, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    // END OF FINGER 2

    // START OF FINGER 3
    glPushMatrix();

    glTranslatef(0.475, 0.05, -0.1125);


    glTranslatef(-0.1, 0, 0);
    glRotatef(finger[0], 0, 0, 1);
    glTranslatef(0.1, 0, 0);
    glPushMatrix();
    glScalef(0.2, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();



    glPushMatrix();
    glTranslatef(0.1, 0, 0);
    glTranslatef(-0.05, 0, 0);
    glRotatef(flang[0], 0, 0, 1);
    glTranslatef(0.05, 0, 0);
    glScalef(0.1, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    // END OF FINGER 3





    // START OF FINGER 4
    glPushMatrix();

    glTranslatef(0.475, -0.1, 0);


    glTranslatef(-0.1, 0, 0);
    glRotatef(finger[0], 0, 0, 1);
    glTranslatef(0.1, 0, 0);
    glPushMatrix();
    glScalef(0.2, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.1, 0, 0);
    glTranslatef(-0.05, 0, 0);
    glRotatef(flang[0], 0, 0, 1);
    glTranslatef(0.05, 0, 0);
    glScalef(0.1, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    // END OF FINGER 4


    glPopMatrix();


    // END OF RIGHT ARM


    // START OF LEFT ARM

    glPushMatrix();



    glTranslatef(-1.0, 1.85, 0.02);
    glRotatef((GLfloat)arm2[1], 1.0, 0.0, 0.0);
    glTranslatef(0.375, 0, 0);
    glRotatef(arm1[1], 0.0, 0.0, 1.0);
    glRotatef(arm11[1], 1.0, 0.0, 0.0);
    glTranslatef(-0.375, 0, 0);

    // Draw the limb
    glPushMatrix();
    glScalef(0.75, 0.3, 0.3);
    glutWireCube(1.0);
    glPopMatrix();



    glTranslatef(-0.75, 0, 0);

    glTranslatef(0.375, 0, 0.0);
    glRotatef((GLfloat)elbow[1], 0.0, 0.0, -1.0);
    glTranslatef(-0.375, 0, 0.0);
    glPushMatrix();
    glScalef(0.75, 0.3, 0.3);
    glutWireCube(1.0);
    glPopMatrix();

    // START OF FINGER 1
    glPushMatrix();

    glTranslatef(-0.475, 0.05, 0.1125);

    glTranslatef(0.1, 0, 0);
    glRotatef(finger[1], 0, 0, 1);
    glTranslatef(-0.1, 0, 0);
    glPushMatrix();
    glScalef(0.2, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-0.1, 0, 0);
    glTranslatef(0.05, 0, 0);
    glRotatef(flang[1], 0, 0, -1);
    glTranslatef(-0.05, 0, 0);
    glScalef(0.1, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    // END OF FINGER 1


    // START OF FINGER 2
    glPushMatrix();

    glTranslatef(-0.475, 0.05, 0.0);

    glTranslatef(0.1, 0, 0);
    glRotatef(finger[1], 0, 0, 1);
    glTranslatef(-0.1, 0, 0);
    glPushMatrix();
    glScalef(0.2, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-0.1, 0, 0);
    glTranslatef(0.05, 0, 0);
    glRotatef(flang[1], 0, 0, -1);
    glTranslatef(-0.05, 0, 0);
    glScalef(0.1, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    // END OF FINGER 2

    // START OF FINGER 3
    glPushMatrix();

    glTranslatef(-0.475, 0.05, -0.1125);

    glTranslatef(0.1, 0, 0);
    glRotatef(finger[1], 0, 0, 1);
    glTranslatef(-0.1, 0, 0);
    glPushMatrix();
    glScalef(0.2, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-0.1, 0, 0);
    glTranslatef(0.05, 0, 0);
    glRotatef(flang[1], 0, 0, -1);
    glTranslatef(-0.05, 0, 0);
    glScalef(0.1, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    // END OF FINGER 3




    // START OF FINGER 4
    glPushMatrix();

    glTranslatef(-0.475, -0.1, 0);

    glTranslatef(0.1, 0, 0);
    glRotatef(finger[1], 0, 0, 1);
    glTranslatef(-0.1, 0, 0);
    glPushMatrix();
    glScalef(0.2, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();



    glPushMatrix();
    glTranslatef(-0.1, 0, 0);
    glTranslatef(0.05, 0, 0);
    glRotatef(flang[1], 0, 0, -1);
    glTranslatef(-0.05, 0, 0);
    glScalef(0.1, 0.1, 0.05);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    // END OF FINGER 4

    glPopMatrix();

    //END OF LEFT ARM
    
    //draw chair model
    glPopMatrix();
    glPushMatrix();
    glTranslatef(7.0, -3.75, -3.5);
    glRotatef(180, 0, 1, 0);
    glScalef(190, 190, 190);
    drawmodel3();
    glPopMatrix();
    //draw flowers model
    glPushMatrix();
    glTranslatef(7.0, -0.5, 1.75);
    glRotatef(VRot, 0, 1, 0);
    glScalef(30, 30, 30);
    drawmodel2();
    glPopMatrix();
    //draw table model
    glPushMatrix();
    glTranslatef(-5.0, -3.75, 2.5);
    glRotatef(90, 0, 1, 0);
    glScalef(12, 20, 15);
    drawmodel4();
    glPopMatrix();
    //draw circular table model
    glPopMatrix();
    glPushMatrix();
    glTranslatef(7.0, -3.75, 1.5);
    glRotatef(-90, 0, 1, 0);
    glScalef(15, 15, 15);
    drawmodel1();
    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(85.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
}
void rotateBodyRight(int value)
{// this function for animaton of rotating the body right
    if (mainBody > value) {
        mainBody = (mainBody - 1) % 360;
        rCounter += 1;
        glutTimerFunc(10, rotateBodyRight, mainBody + rCounter - 90);
    } else {
        rCounter = 0;
    }
    glutPostRedisplay();
}

void rotateBodyLeft(int value)
// this function for animaton of rotating the body left
{
    if (mainBody < value) {
        mainBody = (mainBody + 1) % 360;
        rCounter += 1;
        glutTimerFunc(10, rotateBodyLeft, mainBody - rCounter + 90);
    } else {
        rCounter = 0;
    }
    glutPostRedisplay();
}
void jumpOver(int heightValue)
{
    /*
     This function makes the robotic body jumps over the table for the animation interaction
     */

    switch (jump_state)
    {
        // Jumping
        case 1:
            if (y_body < float(heightValue))
            {
                y_body += 0.5;
                x_body -= 0.5;

                // Moving the knees
                knee[1] -= 1;
                knee[0] -= 1;
            } else {
                jump_state = 2;
            }
            glutTimerFunc(1000/60, jumpOver, heightValue);
            break;

        // Returning Back
        case 2:
            if(y_body > returningPosition)
            {
                y_body -= 0.5;
                x_body -= 0.5;

                knee[1] += 1;
                knee[0] += 1;
            } else if (y_body < returningPosition) {
                jump_state = 0;
            }
            glutTimerFunc(1000/60, jumpOver, heightValue);
            break;

        default:
            break;
    }
    glutPostRedisplay();
}
void walkForward(int value)
{
    /*
     Move the body in the forward direction
     */


    // Reached End of the floor
    if (z_body > 6.5 || z_body<-0.25 || x_body>6 || x_body<-4) {
        leg_state = 0;

        // Return Body to normal state
        leg1[0] = 0;
        knee[0] = 0;
        leg1[1] = 0;
        knee[1] = 0;

        
    }

    switch (leg_state)
    {
        case 1:
            if (leg1[0] < 30) {
                // Move Body
                // Check on movement's direction
                if (mainBody == 0) {
                    z_body += 0.03;
                } else if (mainBody == -90 || mainBody == 270) {
                    x_body -= 0.03;
                } else if (mainBody == -180 || mainBody == 180) {
                    z_body -= 0.03;
                } else if (mainBody == -270 || mainBody == 90) {
                    x_body += 0.03;
                }

                // Move Right Leg Forward
                leg1[0] += 1;
                knee[0] -= 1;

                // Move Left Leg Backward
                if (leg1[1] > -30)
                {
                    leg1[1] -= 1;
                    knee[1] -= 1;
                }

            } else {
                leg_state = 2;
            }
            glutTimerFunc(10, walkForward, 0);
            break;

        case 2:
            if (leg1[0] > 0) {
                // Move Body
                // Check on movement's direction
                if (mainBody == 0) {
                    z_body += 0.03;
                } else if (mainBody == -90 || mainBody == 270) {
                    x_body -= 0.03;
                } else if (mainBody == -180 || mainBody == 180) {
                    z_body -= 0.03;
                } else if (mainBody == -270 || mainBody == 90) {
                    x_body += 0.03;
                }

                // Move Right Leg
                leg1[0] -= 1;
                knee[0] += 1;

                // Return Left Leg
                if (leg1[1] < 0)
                {
                    leg1[1] += 1;
                    knee[1] += 1;
                }
            }
            else {
                leg_state = 0;
            }
            glutTimerFunc(10, walkForward, 0);
            break;
        case 0:
            leg_state = 1;
            break;

        default:
            break;

    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    { //  Jump over
    case 'N':
            jumpOver(4);
            break;
    case 'n':
            walkForward(0);
            break;
        // Whole Body rotation
    case 'm':
            rotateBodyRight( mainBody - 90);
           
            break;
    case 'M':
            rotateBodyLeft(mainBody + 90);
            
            break;
        //RIGHT HIP MOVEMENT //
    case 'h':
        if (leg1[0] < 45) {
            leg1[0] = (leg1[0] + 5);
            glutPostRedisplay();
        }
        break;
    case 'H':
        if (leg1[0] > -100) {
            leg1[0] = (leg1[0] - 5);
            glutPostRedisplay();
        }
        break;
        //RIGHT KNEE MOVEMENT //
    case 'k':
        if (knee[0] < 0) {
            knee[0] = (knee[0] + 5);
            glutPostRedisplay();
        }
        break;
    case 'K':
        if (knee[0] > -120) {
            knee[0] = (knee[0] - 5);
            glutPostRedisplay();
        }

        break;
        //LEFT HIP MOVEMENT //
    case 'l':
        if (leg1[1] < 45) {
            leg1[1] = (leg1[1] + 5) % 360;
            glutPostRedisplay();
        }
        break;
    case 'L':
        if (leg1[1] > -100) {
            leg1[1] = (leg1[1] - 5) % 360;
            glutPostRedisplay();
        }
        break;
        //LEFT KNEE MOVEMENT//
    case 'g':
        if (knee[1] < 0) {
            knee[1] = (knee[1] + 5) % 360;
            glutPostRedisplay();
        }
        break;

    case 'G':
        if (knee[1] > -120) {
            knee[1] = (knee[1] - 5);
            glutPostRedisplay();
        }
        break;
        //ANOTHER RIGHT HIP MOVEMENT//
    case 'j':
        if (leg11[0] < 90) {
            leg11[0] = (leg11[0] + 5);
            glutPostRedisplay();
        }
        break;
    case 'J':
        if (leg11[0] > 0) {
            leg11[0] = (leg11[0] - 5);
            glutPostRedisplay();
        }
        break;
        //ANOTHER LEFT HIP MOVEMENT//
    case 'w':
        if (leg11[1] < 90) {
            leg11[1] = (leg11[1] + 5);
            glutPostRedisplay();
        }
        break;
    case 'W':
        if (leg11[1] > 0) {
            leg11[1] = (leg11[1] - 5);
            glutPostRedisplay();
        }
        break;
        //RIGHT SHOULDER MOVEMENT//
    case 'y':
        if (arm1[0] < 90) {
            arm1[0] = (arm1[0] + 5);
            glutPostRedisplay();
        }
        break;
    case 'Y':
        if (arm1[0] > -90) {
            arm1[0] = (arm1[0] - 5);
            glutPostRedisplay();
        }
        break;
        //LEFT SHOULDER MOVEMENT//
    case 'T':
        if (arm1[1] < 90) {
            arm1[1] = (arm1[1] + 5);
            glutPostRedisplay();
        }
        break;
    case 't':
        if (arm1[1] > -90) {
            arm1[1] = (arm1[1] - 5);
            glutPostRedisplay();
        }
        break;
        //RIGHT ELBOW MOVEMENT//
    case 'r':
        if (elbow[0] < 150) {
            elbow[0] = (elbow[0] + 5);
            glutPostRedisplay();
        }
        break;
    case 'R':
        if (elbow[0] > 0) {
            elbow[0] = (elbow[0] - 5);
            glutPostRedisplay();
        }
        break;
        //LEFT ELBOW MOVEMENT//
    case 'e':
        if (elbow[1] < 150) {
            elbow[1] = (elbow[1] + 5);
            glutPostRedisplay();
        }
        break;
    case 'E':
        if (elbow[1] > 0) {
            elbow[1] = (elbow[1] - 5);
            glutPostRedisplay();
        }
        break;
        //Another RIGHT SHOULDER MOVEMENT//
    case 'q':
        if (arm11[0] < 0) {
            arm11[0] = (arm11[0] + 5);
            glutPostRedisplay();
        }
        break;
    case 'Q':
        if (arm11[0] > -120) {
            arm11[0] = (arm11[0] - 5);
            glutPostRedisplay();
        }
        break;
        //ANOTHER LEFT SHOULDER MOVEMENT//
    case 'o':
        if (arm11[1] < 0) {
            arm11[1] = (arm11[1] + 5);
            glutPostRedisplay();
        }
        break;
    case 'O':
        if (arm11[1] > -120) {
            arm11[1] = (arm11[1] - 5);
            glutPostRedisplay();
        }
        break;
        //Fingers RIGHT ARM MOVEMENT//
    case 'd':
        if (finger[0] < 60) {
            finger[0] = (finger[0] + 5);
            glutPostRedisplay();

        }
        break;
    case 'D':
        if (finger[0] > -90) {
            finger[0] = (finger[0] - 5);
            glutPostRedisplay();
        }
        break;
        //Fingers LEFT ARM MOVEMENT//
    case 'F':
        if (finger[1] < 90) {
            finger[1] = (finger[1] + 5);
            glutPostRedisplay();
        }
        break;
    case 'f':
        if (finger[1] > -60) {
            finger[1] = (finger[1] - 5);
            glutPostRedisplay();
        }
        break;
        //Flangs RIGHT ARM MOVEMENT//
    case 'a':
        if (flang[0] < 0) {
            flang[0] = (flang[0] + 5);
            glutPostRedisplay();
        }
        break;
    case 'A':
        if (flang[0] > -90)
            flang[0] = (flang[0] - 5);
        glutPostRedisplay();
        break;
        //Flangs LEFT ARM MOVEMENT//
    case 's':
        if (flang[1] < 0) {
            flang[1] = (flang[1] + 5) % 360;
            glutPostRedisplay();
        }
        break;
    case 'S':
        if (flang[1] > -90) {
            flang[1] = (flang[1] - 5) % 360;
            glutPostRedisplay();
        }
        break;
    case '+':

        MoveForward();
        glutPostRedisplay();
        break;
    case '-':
        MoveBackward();
        glutPostRedisplay();
        break;

    case 'b':
        reset();
        glutPostRedisplay();
        break;


    case 27:
        exit(0);
        break;
    default:
        break;
    }
}

static void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            moving = 1;
            startx = x;
            

        }
        if (state == GLUT_UP) {
            moving = 0;

        }
    }
}

void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT: LEFT(); break;
    case GLUT_KEY_RIGHT: RIGHT(); break;
    case GLUT_KEY_UP:    UP(); break;
    case GLUT_KEY_DOWN:  DOWN(); break;
    }

    glutPostRedisplay();
}


static void motion(int x, int y)
{
    if (moving) {
        angle = angle + (x - startx);
        angle2 = angle2 + (y - starty);
        startx = x;
        starty = y;
        glutPostRedisplay();
    }
}

void Choose_texture(int id)
{
    /*
     
        This function is responsible for changing the texture of the floor
        using a drop menu by the right click action on the mouse.
     */
    switch (id)
    {
    case 1:
        initRendering("Floor1.bmp", textureId);
        break;

    case 2:
        initRendering("Floor2.bmp", textureId);
        break;

    case 3:
        initRendering("Floor3.bmp", textureId);
        break;

    case 4:
        initRendering("Floor4.bmp", textureId);
        break;

    default:
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    // The Main Floor Texture for the first view
    initRendering("Floor3.bmp", textureId);

    glutCreateMenu(Choose_texture);
    glutAddMenuEntry("Floor 1", 1);
    glutAddMenuEntry("Floor 2", 2);
    glutAddMenuEntry("Floor 3", 3);
    glutAddMenuEntry("Floor 4", 4);
    
 

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    init();
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    return 0;
}