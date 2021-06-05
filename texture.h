#pragma once


#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "imageloader.h"
#include <GL/glut.h>

GLuint loadTexture(Image* image);  // Loads A given Texture using Image Object and returns it`s id
void drawFloorTexture(GLuint textID);  // Draw a floor from a bitmap image

#endif _TEXTURE_H
