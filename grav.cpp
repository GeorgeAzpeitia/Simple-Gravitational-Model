// Programmer: Mihalis Tsoukalos
// Date: Wednesday 04 June 2014
//
// A simple OpenGL program that draws a triangle.

#include "GL/freeglut.h"
#include "GL/gl.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <string>
#include <float.h>

using namespace std;

void timer(int);
void draw();
void updatePlanets();
void read(char* inFile);
void mouse(int button, int state, int x, int y);
void addPlanet(bool);
void addPlanetRand();
void keyPress(unsigned char, int, int);
void mouseMotion(int x, int y);
void eraseCoords();
struct Planet
{
    float x;
    float y;
    float z;
    float vx;
    float vy;
    float vz;
    float color[3];
    float mass; //in kg
    int radius;
};

struct Coord
{
    float x;
    float y;
};

vector<vector<Coord>> previousXY;
static const int step = 10;
vector<Planet> planets;
vector<Planet> stars;
int mPosX, mPosY;
bool leftMouseBtn, drawCoords = false;
bool drawingLine;
Coord mDown, mUp;
//const float GRAV_CONSTANT = 0.0000000000667;
const float GRAV_CONSTANT = 0.00667;
int main(int argc, char **argv)
{
    read(argv[1]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    //set the size of the window
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("A Simple Gravitational Model");
    
    //set the coordinate type and coordinate limits
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1000.0, 1000.0, -1000.0, 1000.0, 0, 1);

    glutDisplayFunc(draw);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyPress);

    timer(0);

    glutMainLoop();
    return 0;
}
void read(char* s){
    Planet testOne;
    Planet testTwo;
    testOne.x = 0.0f;
    testOne.y = 0.0f;
    testOne.z = 0.0f;

    testOne.vx = 0.0f;
    testOne.vy = 0.0f;
    testOne.vz = 0.0f;
    testOne.mass = 5000000.0f;

    stars.push_back(testOne);

    // testTwo.x = 300.0f;
    // testTwo.y = 0.0f;
    // testTwo.z = 0.0f;

    // testTwo.vx = 10.0f;
    // testTwo.vy = 10.0f;
    // testTwo.vz = 0.0f;
    // testTwo.mass = 10000.0f;
    testTwo.color[0] = 0.5;
    testTwo.color[1] = 0.8;
    testTwo.radius = 10;
    // planets.push_back(testTwo);
    Coord c;
    // c.x = 300;
    // c.y = 0;
    // vector<Coord> v {c};
    // previousXY.push_back(v);

    testTwo.x = -300.0f;
    testTwo.y = 200.0f;
    testTwo.z = 0.0f;

    testTwo.vx = -10.0f;
    testTwo.vy = -1.0f;
    testTwo.vz = 0.00f;
    testTwo.mass = 100.0f;
    planets.push_back(testTwo);

    c.x = -300;
    c.y = 200;
    vector<Coord> v2 {c};
    previousXY.push_back(v2);

}
void updatePlanets(){
    float force, distance, unitx, unity, unitz;
    float deltaT = 1.0 / step;

    for(Planet& p : planets){
        p.x = p.x + p.vx * deltaT;
        p.y = p.y + p.vy * deltaT;
        p.z = p.z + p.vz * deltaT;
    }
    int c = 1;
    for(int c = 0; c < planets.size(); ++c){
        Planet& p = planets.at(c);
        for(int i =( c + 1); i < planets.size(); ++i){
            Planet& p2 = planets.at(i);

            distance = sqrt((p2.x - p.x) * (p2.x - p.x) 
                          + (p2.y - p.y) * (p2.y - p.y) 
                          + (p2.z - p.z) * (p2.z - p.z));

            force = GRAV_CONSTANT * p.mass * p2.mass / (distance * distance);

            unitx = (p2.x - p.x) / distance;
            unity = (p2.y - p.y) / distance;
            unitz = (p2.z - p.z) / distance;

            p.vx = p.vx + (force / p.mass) * unitx * deltaT;
            p.vy = p.vy + (force / p.mass) * unity * deltaT;
            p.vz = p.vz + (force / p.mass) * unitz * deltaT;

            p2.vx = p2.vx + (force / p2.mass) * (-unitx) * deltaT;
            p2.vy = p2.vy + (force / p2.mass) * (-unity) * deltaT;
            p2.vz = p2.vz + (force / p2.mass) * (-unitz) * deltaT;
        }
        for(Planet& s : stars){

            distance = sqrt((s.x - p.x) * (s.x - p.x) + (s.y - p.y) *
                (s.y - p.y) + (s.z - p.z) * (s.z - p.z));

            if(distance < 50){
                planets.erase(planets.begin() + c);
                previousXY.erase(previousXY.begin() + c);
                break;
            }

            force = GRAV_CONSTANT * p.mass * s.mass / (distance * distance);

            unitx = (s.x - p.x) / distance;
            unity = (s.y - p.y) / distance;
            unitz = (s.z - p.z) / distance;


            p.vx = p.vx + (force / p.mass) * unitx * deltaT;
            p.vy = p.vy + (force / p.mass) * unity * deltaT;
            p.vz = p.vz + (force / p.mass) * unitz * deltaT;

            
        }
    }
}
void timer(int){
    for(int i = 0; i < step; ++i){
        updatePlanets();
    }
    glutPostRedisplay();
    glutTimerFunc(30, timer, 0);
}

void draw()
{
    

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);


    glColor3f(1, 1, 1);
    if(drawCoords){
        for(int i = 0; i < planets.size() && i < previousXY.size(); ++i){
            glBegin(GL_LINES);
                for(Coord& c : previousXY.at(i)){
                    glVertex3f(c.x, c.y, 0);
                }
                
                Coord c;
                c.x = planets.at(i).x;
                c.y = planets.at(i).y;
                glVertex3f(c.x, c.y, 0);
                previousXY.at(i).push_back(c);
            glEnd();
        }
    }
    if(drawingLine){
        glBegin(GL_LINES);
            glVertex2f(mDown.x, mDown.y);
            glVertex2f(mUp.x, mUp.y);
        glEnd();
    }

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    //draw planets
    for(Planet& p : planets){
        glPointSize(p.radius);
        glBegin(GL_POINTS);
            glColor3f(p.color[0], p.color[1], p.color[2]);
            glVertex3f(p.x, p.y, 0.0);
        glEnd();
    }

    

    glPointSize(50);
    glBegin(GL_POINTS);
    for(Planet& s : stars){
        glColor3f(1.0, 1.0, 0.0);
        glVertex3f(s.x, s.y, 0.0);
    }
    glEnd();
    

    
    glFlush();
    
}

void mouse(int button, int state, int x, int y){
    mPosX = x * 2 - 1000;
    mPosY = -y * 2 + 1000;
    
    if(mUp.x != 0 && mUp.y != 0 && state == GLUT_UP){
        addPlanet(false);
        leftMouseBtn = false;
    }
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP){
        addPlanet(true);
    }
    
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        mDown.x = mPosX;
        mDown.y = mPosY;
        leftMouseBtn = true;
        drawingLine = false;
    }
    
}
void mouseMotion(int x, int y){
    mPosX = x * 2 - 1000;
    mPosY = -y * 2 + 1000;  
    drawingLine = true;
    //end of line with draging
    if(leftMouseBtn)
    {
        mUp.x = mPosX;
        mUp.y = mPosY;
    }
}
void addPlanet(bool star){
        if (star){
            Planet p;
            p.x = mPosX;
            p.y = mPosY;
            p.z = 0;
            p.vx = 0;
            p.vy = 0;
            p.vz = 0;

            p.mass = 2000000.0f;
            p.radius = 50;

            p.color[0] = 1;
            p.color[1] = 1;
            p.color[2] = 0;

            stars.push_back(p);
        } else {
            Planet p;
            p.x = mDown.x;
            p.y = mDown.y;
            p.z = 0;
            p.vx = (mDown.x - mUp.x) / 10;
            p.vy = (mDown.y - mUp.y) / 10;
            p.vz = 0;

            p.mass = rand() % 500 + 500;
            p.radius = (int) ((p.mass / 1000) * 20);

            p.color[0] = (rand() % 100) / 100.0;
            p.color[1] = (rand() % 100) / 100.0;
            p.color[2] = (rand() % 100) / 100.0;

            planets.push_back(p);

            Coord c;
            c.x = p.x;
            c.y = p.y;
            vector<Coord> v {c};
            previousXY.push_back(v);
            mDown.x = mDown.y = 0;
            mUp.x = mUp.y = 0;
    }
}
void addPlanetRand(bool planetsOnly){
    bool star = planetsOnly ? false : rand() % 2;
        if (star){
            Planet p;
            p.x = (rand() % 2000) - 1000 ;
            p.y = (rand() % 2000) - 1000;
            p.z = 0;
            p.vx = 0;
            p.vy = 0;
            p.vz = 0;

            p.mass = 5000000.0f;
            p.radius = 50;

            p.color[0] = 1;
            p.color[1] = 1;
            p.color[2] = 0;

            stars.push_back(p);
        } else {
            Planet p;
            p.x = (rand() % 2000) - 1000;
            p.y = (rand() % 2000) - 1000;
            p.z = 0;
            p.vx = (rand() % 20) - 10;
            p.vy = (rand() % 20) - 10;
            p.vz = 0;

            p.mass = rand() % 500 + 500;
            p.radius = (int) ((p.mass / 1000) * 20);

            p.color[0] = (rand() % 100) / 100.0;
            p.color[1] = (rand() % 100) / 100.0;
            p.color[2] = (rand() % 100) / 100.0;

            planets.push_back(p);

            Coord c;
            c.x = p.x;
            c.y = p.y;
            vector<Coord> v {c};
            previousXY.push_back(v);
            mDown.x = mDown.y = 0;
            mUp.x = mUp.y = 0;
    }
}

void keyPress(unsigned char key, int x, int y){
    if(key == 's'){
        for(int i = 0; i < 50; ++i){
            addPlanetRand(true);
        }
    }else if(key == 'S'){
        for(int i = 0; i < 20; ++i){
            addPlanetRand(false);
        }
    }else if(key == 'x' ){
        eraseCoords();
    }
    else if (key == 'X'){
        eraseCoords();
        drawCoords = !drawCoords;
    }
}

void eraseCoords(){
    for(int i = 0; i < planets.size() && i < previousXY.size(); ++i){
        previousXY.at(i).clear();
        Coord c;
        c.x = planets.at(i).x;
        c.y = planets.at(i).y;

        previousXY.at(i).push_back(c);
    }
}


