#include "texture.h"
#include "imageloader.h"

GLuint loadTexture(Image* image) {
    GLuint textureId;
    glBindTexture(GL_TEXTURE_2D, textureId); 
    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,                // GL_TEXTURE_2D
        0,                            
        GL_RGB,                       
        image->width, image->height,  
        0,                            
        GL_RGB, 
        GL_UNSIGNED_BYTE, 
   
        image->pixels);    //The actual pixel data
    return textureId; //Returns the id of the texture
}

void drawFloorTexture(GLuint textID) {
    
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS);

    glNormal3f(0.0, -1.0, 0.0);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-7, -0.25, 7);

    glTexCoord2f(5.0f, 0.0f);

    glVertex3f(7, -0.25, 7);

    glTexCoord2f(5.0f, 20.0f);

    glVertex3f(7, -0.25, -7); 

    glTexCoord2f(0.0f, 20.0f);

    glVertex3f(-7, -0.25, -7); 

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}