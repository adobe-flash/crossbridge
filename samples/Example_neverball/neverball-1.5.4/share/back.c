/*
 * Copyright (C) 2003 Robert Kooima
 *
 * NEVERBALL is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#include <SDL.h>
#include <math.h>
#include <string.h>

#include "glext.h"
#include "vec3.h"
#include "back.h"
#include "image.h"

/*---------------------------------------------------------------------------*/

#define PI 3.1415926535897932

static GLuint back_list;
static GLuint back_text;

/*---------------------------------------------------------------------------*/

void back_init(const char *s, int b)
{
    int i, slices = b ? 32 : 16;
    int j, stacks = b ? 16 :  8;

    back_free();
    back_text = make_image_from_file(s);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    back_list = glGenLists(1);

    glNewList(back_list, GL_COMPILE);
    {
        glBindTexture(GL_TEXTURE_2D, back_text);

        glColor3f(1.0f, 1.0f, 1.0f);

        for (i = 0; i < stacks; i++)
        {
            float k0 = (float)  i      / stacks;
            float k1 = (float) (i + 1) / stacks;

            float s0 = fsinf(V_PI * (k0 - 0.5));
            float c0 = fcosf(V_PI * (k0 - 0.5));
            float s1 = fsinf(V_PI * (k1 - 0.5));
            float c1 = fcosf(V_PI * (k1 - 0.5));

            glBegin(GL_QUAD_STRIP);
            {
                for (j = 0; j <= slices; j++)
                {
                    float k = (float) j / slices;
                    float s = fsinf(V_PI * k * 2.0);
                    float c = fcosf(V_PI * k * 2.0);

                    glTexCoord2f(k, 1.0f - k1);
                    glVertex3f(s * c1, c * c1, s1);

                    glTexCoord2f(k, 1.0f - k0);
                    glVertex3f(s * c0, c * c0, s0);
                }
            }
            glEnd();
        }
    }
    glEndList();
}

void back_free(void)
{
    if (glIsList(back_list))
        glDeleteLists(back_list, 1);

    if (glIsTexture(back_text))
        glDeleteTextures(1, &back_text);

    back_list = 0;
    back_text = 0;
}

void back_draw(float t)
{
    glPushMatrix();
    {
        GLfloat dx =  60.f * fsinf(t / 10.f) + 90.f;
        GLfloat dz = 180.f * fsinf(t / 12.f);

        glDisable(GL_LIGHTING);
        glDepthMask(GL_FALSE);
        {
            glScalef(BACK_DIST, BACK_DIST, BACK_DIST);
            glRotatef(dz, 0.f, 0.f, 1.f);
            glRotatef(dx, 1.f, 0.f, 0.f);

            glCallList(back_list);
        }
        glDepthMask(GL_TRUE);
        glEnable(GL_LIGHTING);
    }
    glPopMatrix();
}

/*---------------------------------------------------------------------------*/
