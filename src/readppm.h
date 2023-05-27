#include <iostream>
#include <stdio.h>

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

using namespace std;

GLuint * readppm(char *filename) {
    FILE *fd;
    int k, nm, n, m;
    char c;
    int i;
    float s;
    int red, green, blue;
    char b[100];
    GLuint *image;

    fd = fopen(filename, "r");
    fscanf(fd, "%[^\n] ", b);
    if(b[0]!='P'|| b[1] != '3'){
        printf("%s is not a PPM file!\n", b);
        exit(0);
    }

    fscanf(fd, "%c",&c);
    while(c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
    }
    ungetc(c,fd);

    fscanf(fd, "%d %d %d", &n, &m, &k);
    printf("%d rows %d columns max value= %d\n",n,m,k);
    nm = n*m;
    image = (GLuint *) malloc(3*sizeof(GLuint)*nm);
    for(i=nm;i>0;i--)
    {
        fscanf(fd,"%d %d %d",&red, &green, &blue );
        image[3*nm-3*i]=red;
        image[3*nm-3*i+1]=green;
        image[3*nm-3*i+2]=blue;
    }

    return image;
}