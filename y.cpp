#include <GL/glut.h>  // Header File For The GLUT Library 
#include <GL/gl.h>	  // Header File For The OpenGL32 Library
#include <GL/glu.h>	  // Header File For The GLu32 Library
#include <math.h>
#include <unistd.h>
#include <algorithm>

#include <bits/stdc++.h>
using namespace std;

#define debug(x...) cerr << "[" << #x << "] = [", _print(x), cerr << "]" << endl
void _print() { } template<typename T, typename... U> void _print(T a, U... b) { if(sizeof... (b)){ cerr << a << ", "; _print(b...); } else cerr << a; }
#define endl '\n'

const int ESC = 27;

int window; 

struct vec2 {
    float x,y;
    vec2(float x, float y) : x(x), y(y) { }
    vec2() { }
    float length() {
        return hypot(x,y);
    }
    vec2 operator-(const vec2&o) {
        return {x-o.x,y-o.y};
    }
    vec2 operator+(const vec2&o) {
        return {x+o.x,y+o.y};
    }
    vec2 operator*(const float k) {
        return {x*k,y*k};
    }
    vec2 operator/(const float k) {
        return {x/k,y/k};
    }
};

float dot(vec2 a,vec2 b) {
    return a.x*b.x+a.y*b.y;
}
float cross(vec2 a,vec2 b) {
    return a.x*b.y-a.y*b.x;
}
float get_angle(vec2 a, vec2 b) {
    return acos(dot(a,b)/(a.length()*b.length()));
}
float to_rad(int angle) {
    return angle*2*M_PI/360.0f;
}
float to_deg(float angle) {
    return angle*360.0f/(2.0*M_PI);
}
float clamp(float val, const float&l, const float&r) {
    if(val <= l) val = l;
    if(val >= r) val = r;
    return val;
}

vec2 rotate(vec2 v, float A) {
    return vec2(v.x*cos(A)-v.y*sin(A),v.x*sin(A)+v.y*cos(A));
}

struct line {
    vec2 a, b;
    line(vec2 a, vec2 b) : a(a), b(b) { }
};

struct color {
    float r,g,b;
    color(float r, float g, float b) : r(r), g(g), b(b) { }
    color() { }
};

const color red = color(1,0,0);
const color green = color(0,1,0);
const color blue = color(0,0,1);
const color cyan = color(0,1,1);
const color magenta = color(1,0,1);
const color yellow = color(1,1,0);

vec2 target;
vec2 camera(0,0);
float zRot = 0;

void draw_pixel(vec2 pos, color cor) {
    glColor3f(cor.r, cor.g, cor.b);
    glVertex3f(pos.x, pos.y, 0.0f);
}

void draw_circle(float x, float y, float r, color cor) {
    for(int angle=0;angle<=360;angle++) {
        float rad = to_rad(angle);
        vec2 pos(x+r*cos(rad), y+r*sin(rad));
        draw_pixel(pos, cor);
    }
}

vec2 proj(vec2 v, vec2 u) {
    return u * (dot(u,v)/dot(u,u));
}

vec2 intersect(line l1, line l2) {
    vec2 l1v = l1.b-l1.a;
    vec2 l2v = l2.b-l2.a;
    float k_num = l2v.y * (l2.a.x - l1.a.x) + l2v.x * (l1.a.y - l2.a.y);
    float k_den = cross(l1v,l2v);
    return l1.a + l1v*(k_num/k_den);
}
bool in(vec2 p, line l) {
    if(abs(cross(l.b-p,l.a-p))>-1e-9) return false;
    return dot(l.a-p,l.b-p)<1e-9;
}

const double wall_sz = 7.5f;
const double wall_st = -1.0f;
void draw_line(line l, color cor) {
    glColor3f(cor.r, cor.g, cor.b);
    vec2 side(1,0);
    vec2 front(0,1);
    side = rotate(side, to_rad(zRot));
    front = rotate(front, to_rad(zRot));
    if(dot(l.a-camera,front)<=0 and dot(l.b-camera,front)<=0) return;
    if(dot(l.a-camera,front)<=0) {
        l.a = l.a - camera;
        l.b = l.b - camera;
        assert(abs(cross(l.b-l.a,side))>1e-9);
        l.a = intersect(l, line(side*-1000,side*1000));
        l.a = l.a + camera;
        l.b = l.b + camera;
    }
    if(dot(l.b-camera,front)<=0) {
        l.a = l.a - camera;
        l.b = l.b - camera;
        assert(abs(cross(l.b-l.a,side))>1e-9);
        l.b = intersect(l, line(side*-1000,side*1000));
        l.a = l.a + camera;
        l.b = l.b + camera;
    }
    float da = rotate(l.a-camera, to_rad(zRot)).length(), db = rotate(l.b-camera, to_rad(zRot)).length();
    da = 1.0f/da;
    db = 1.0f/db;
    l.a = l.a * da;
    l.b = l.b * db;
    l.a.y += wall_st;
    l.b.y += wall_st;

    glVertex3f(l.a.x, l.a.y-wall_sz*da/2, 0.0f);
    glVertex3f(l.b.x, l.b.y-wall_sz*db/2, 0.0f);
    glVertex3f(l.a.x, l.a.y+wall_sz*da/2, 0.0f);
    glVertex3f(l.b.x, l.b.y+wall_sz*db/2, 0.0f);

    glVertex3f(l.a.x, l.a.y-wall_sz*da/2, 0.0f);
    glVertex3f(l.a.x, l.a.y+wall_sz*da/2, 0.0f);
    glVertex3f(l.b.x, l.b.y-wall_sz*db/2, 0.0f);
    glVertex3f(l.b.x, l.b.y+wall_sz*db/2, 0.0f);
}


#define sq(x) ((x)*(x))

void InitMap() {
}


// Function to handle mouse movement
void mouseMove(int x, int y) {
    // Convert mouse coordinates (screen space) to OpenGL coordinates (-1 to 1)
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    target.x = (float)x / (float)windowWidth * 2.0f - 1.0f;   // Normalize to (-1, 1)
    target.y = -((float)y / (float)windowHeight * 2.0f - 1.0f); // Invert Y axis
}

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

    glMatrixMode(GL_MODELVIEW);

    InitMap();
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
void ReSizeGLScene(int Width, int Height) {
    if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
        Height=1;

    glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}


// Respond to arrow keys (rotate)
void SpecialKeys(int key, int x, int y){  
    switch(key) {
        case GLUT_KEY_LEFT:
            zRot += 5.0f;
            break;
        case GLUT_KEY_RIGHT:
            zRot -= 5.0f;
            break;
    }
    zRot = (GLfloat)((const int)zRot % 360);
    // Refresh the Window  
    glutPostRedisplay();  
}

/* The main drawing function. */
void DrawGLScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();				// Reset The View

	glRotatef(-zRot, 0.0f, 1.0f, 0.0f);  
    glTranslatef(-camera.x,0,-6.0f);		// Move boids 6.0 units into the screen

    glBegin(GL_LINES);
    vec2 a(-4.33,2.4), b(4.33,2.4);
    line l(a,b);
    draw_line(l,green);
    a = vec2(4.33,2.4), b = vec2(4.33,-2.4);
    l = line(a,b);
    draw_line(l,yellow);
    a = vec2(4.33,-2.4), b = vec2(-4.33,-2.4);
    l = line(a,b);
    draw_line(l,cyan);
    a = vec2(-4.33,-2.4), b = vec2(-4.33,2.4);
    l = line(a,b);
    draw_line(l,magenta);

    a = vec2(-4.33,-2.4), b = vec2(-1,0);
    l = line(a,b);
    draw_line(l,blue);
    a = vec2(-1,0), b = vec2(-4.33,2.4);
    l = line(a,b);
    draw_line(l,yellow);

    a = vec2(4.33,2.4), b = vec2(1,0);
    l = line(a,b);
    draw_line(l,magenta);
    a = vec2(1,0), b = vec2(4.33,-2.4);
    l = line(a,b);
    draw_line(l,green);
    glEnd();


    // swap buffers to display, since we're double buffered.
    glutSwapBuffers();
    usleep(20000);
}

/* The function called whenever a key is pressed. */
void keyPressed(unsigned char key, int x, int y) {
    if (key == ESC) { 
        glutDestroyWindow(window); 

        exit(0);                   
    }
    vec2 move(0,0);
    switch(key) {
        case 's':
            move.y -= 0.1f;
            break;
        case 'w':
            move.y += 0.1f;
            break;
        case 'a':
            move.x -= 0.1f;
            break;
        case 'd':
            move.x += 0.1f;
            break;
    }
    //rodar move com base em zRot
    move = rotate(move,to_rad(zRot));
    camera.x += move.x;
    camera.y += move.y;
    // Refresh the Window  
    glutPostRedisplay();  

}

int main(int argc, char **argv) {  
    glutInit(&argc, argv);  
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(640, 480);  
    glutInitWindowPosition(0, 0);  
    window = glutCreateWindow("TF");  
    glutDisplayFunc(&DrawGLScene);  
    glutFullScreen();
    glutSpecialFunc(&SpecialKeys);  
    glutIdleFunc(&DrawGLScene);
    glutReshapeFunc(&ReSizeGLScene);
    glutKeyboardFunc(&keyPressed);

    glutPassiveMotionFunc(&mouseMove);
    InitGL(640, 480);
    glutMainLoop();  

    return 0;
}


