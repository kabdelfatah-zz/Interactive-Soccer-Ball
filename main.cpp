#include <GL/glut.h>
#include <iostream>
#include <time.h>
#include <cstring>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

//============================================================
struct Points {
   double x, y,z;  // Coordinates of the point.
   void assignValues(double x,double y, double z){
       this->x = x;
       this->y = y;
       this->z = z;
   }
 };
struct PolorPoint{
   double phi, theta,r;  // Coordinates of the point.
   void assignValues(double phi,double theta, double r){
       this->phi = phi;
       this->theta = theta;
       this->r = r;
   }
   double getX(){return this->r*cos(this->theta)*sin(this->phi);}
   double getY(){return this->r*cos(this->phi);}
   double getZ(){return this->r*sin(this->theta)*sin(this->phi);}
 };
struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image;
#define checkImageWidth 64
#define checkImageHeight 64
//=============================================================
static const double INITIAL_BALL_LOCATION_X=-3;
static const double INITIAL_BALL_LOCATION_Y=-0.4;
static const double INITIAL_BALL_LOCATION_Z=5;
static const double INITIAL_DECREASING_STEP_STRIGHT=0.0009;
static const double INITIAL_DECREASING_STEP_PARABOLA=0.02;
static const double INITIAL_STARTING_MOTION_STRIGHT=1;
static const double INITIAL_STARTING_RADIUS_PARABOLA=4;
static const double PI = 3.14159265;
float currWidth = 1000;
float currHeight = 800;
float rotatAngleX = 0;
float rotatAngleY = 0;
float rotatAngleZ = 0;
double tX = INITIAL_BALL_LOCATION_X;
double tY = INITIAL_BALL_LOCATION_Y;
double tZ = INITIAL_BALL_LOCATION_Z;
double angleStep;
double angleDecreasingTheStep;
double motionStepStraight;
double motionDecreasingTheStepStraight;
double parabolaRadius;
//double motionDecreasingTheStepParabola;
double parabolaDestinationZ;
double parabolaCenterZ;
double parabolaStepZ;
double angleParabola;
double angleParabolaStep;
float camerPositionX=2.2;
float camerPositionY=14;
float camerPositionZ=5;
float camerLookingX=-2;
float camerLookingY=-5.9;
float camerLookingZ=3.1;
int startAnimationInStraight = 0;
int startAnimationInParabola = 0;

float maxY=0,minY=2;
int tZDir = 1;//it should be 1 or -1
float l = -3.0, r = 3.0, b = -3.0, t = 3.0, n = 1.5, f = 20.0;
GLubyte checkImage[checkImageWidth][checkImageHeight][3];
GLuint texture[3];
//=============================================================
void drawTraingle(PolorPoint *p1,PolorPoint *p2,PolorPoint *p3){
    glBegin(GL_TRIANGLES);
        glVertex3f(p1->getX(),p1->getY(),p1->getZ());
        glVertex3f(p2->getX(),p2->getY(),p2->getZ());
        glVertex3f(p3->getX(),p3->getY(),p3->getZ());
    glEnd();
}

void drawDeeperTrinagles(PolorPoint *p1,PolorPoint *p2,PolorPoint *p3,int TrainglesDepth ){
    PolorPoint centeriod;
    centeriod.phi = (p1->phi+p2->phi+p3->phi)/3;
    centeriod.theta = (p1->theta+p2->theta+p3->theta)/3;
    centeriod.r = (p1->r+p2->r+p3->r)/3;
    if(TrainglesDepth <= 1){
        drawTraingle(&centeriod,p1,p2);
        drawTraingle(&centeriod,p3,p2);
        drawTraingle(&centeriod,p3,p1);
    }else{
        drawDeeperTrinagles(&centeriod,p1,p2,TrainglesDepth-1);
        drawDeeperTrinagles(&centeriod,p3,p2,TrainglesDepth-1);
        drawDeeperTrinagles(&centeriod,p3,p1,TrainglesDepth-1);
    }
}
void drawLine(double r,double startAngle,double endAngle,double fixedAngle,int PhiDir){
    double step=0,s,e;
    double absDiff=0;
    if(startAngle <= endAngle){
        s=startAngle; e=endAngle;
    }
    else {
        e=startAngle; s=endAngle;
    }
    absDiff = e - s;
    step = absDiff/20;

    if(PhiDir==1){
        glBegin(GL_LINES);
            for(double p=s; p<e;p+=step){
                glVertex3f(r*sin(p)*cos(fixedAngle),r*cos(p),r*sin(p)*sin(fixedAngle));
                glVertex3f(r*sin(p+step)*cos(fixedAngle),r*cos(p+step),r*sin(p+step)*sin(fixedAngle));
            }
        glEnd();
    }else{
        glBegin(GL_LINES);
            for(double p=s; p<e;p+=step){
                glVertex3f(r*sin(fixedAngle)*cos(p),r*cos(fixedAngle),r*sin(fixedAngle)*sin(p));
                glVertex3f(r*sin(fixedAngle)*cos(p+step),r*cos(fixedAngle),r*sin(fixedAngle)*sin(p+step));
            }
        glEnd();
    }
}
void drawLineInBothDir(double r, double sp,double ep,double st,double et){
    double stepPhi,stepTheta;
    double diffPhi = ep - sp;
    double diffTheta = et - st;
    stepPhi = diffPhi/20;
    stepTheta = diffTheta/20;
    double cp=0,ct=0;
    glBegin(GL_LINES);
        for(double p=0; p < 20 ;p++){
            glVertex3f(r*sin(sp+cp)*cos(st+ct),r*cos(sp+cp),r*sin(sp+cp)*sin(st+ct));
            glVertex3f(r*sin(sp+cp+stepPhi)*cos(st+ct+stepTheta),r*cos(sp+cp+stepPhi),r*sin(sp+cp+stepPhi)*sin(st+ct+stepTheta));
            cp += stepPhi;
            ct +=stepTheta;
        }
    glEnd();
}

void buildSoccerBall(){

    double deg_rad = PI/180;
    double a = 20.07675128*deg_rad;
    double b = 63.434948823*deg_rad;
    double r = 0.6;
    double phi1 = b-(a*sin(18*deg_rad));
    double phi2 = b+(a*sin(54*deg_rad));
    double theta1 = 22.749223595*deg_rad;
    double theta2 = theta1*sin(36*deg_rad)/sin(72*deg_rad);
    PolorPoint *p1,*p2,*p3;
    p1 = new PolorPoint;
    p2 = new PolorPoint;
    p3 = new PolorPoint;
    int trianglesDepth = 5;
//============UPPER PART==================================
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexCoord2f(0.5, 0.5);
    //Top upper pentagon
    for(int i=0; i<5;i++)
    {
        p1->assignValues(0,0,r);
        p2->assignValues(a,i*72*deg_rad,r);
        p3->assignValues(a,(i+1)*72*deg_rad,r);
        drawDeeperTrinagles(p1,p2,p3,2);
    }
    //upper 6 Pentagons
    for(int i=0; i<5;i++){
        double t1 = theta1 + i*72*deg_rad;
        double t11 = -theta1 + i*72*deg_rad;
        double t2 = theta2 + i*72*deg_rad;
        double t22 = -theta2 + i*72*deg_rad;
        p1->assignValues(b,(i*72*deg_rad),r);
        p2->assignValues(phi1,t1,r);
        p3->assignValues((b-a),(i*72*deg_rad),r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(b,(i*72*deg_rad),r);
        p2->assignValues((b-a),(i*72*deg_rad),r);
        p3->assignValues(phi1,t11,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(b,(i*72*deg_rad),r);
        p2->assignValues(phi1,t11,r);
        p3->assignValues(phi2,t22,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);


        p1->assignValues(b,(i*72*deg_rad),r);
        p2->assignValues(phi2,t22,r);
        p3->assignValues(phi2,t2,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(b,(i*72*deg_rad),r);
        p2->assignValues(phi2,t2,r);
        p3->assignValues(phi1,t1,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

    }
    //Connect upper pentagons with lines
    glColor3f(0.0f,0.0f,0.0f);
    for(int i=0; i<5;i++){
        drawLine(r,a,b-a,(i*72*deg_rad),1);
        double t1 = theta1 + i*72*deg_rad;
        double t11 = -theta1 + (i+1)*72*deg_rad;
        drawLine(r,t11,t1,phi1,0);
    }
    //Fill the upper hexagon
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexCoord2f(0.5, 0.5);
    for(int i=0; i<6;i++){
        double ct = (36+i*72)*deg_rad;
        double t1 = theta1 + i*72*deg_rad;
        double t11 = -theta1 + (i+1)*72*deg_rad;
        p1->assignValues((b-a),ct,r);
        p2->assignValues((b-a),(i*72*deg_rad),r);
        p3->assignValues(a,(i*72*deg_rad),r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues((b-a),ct,r);
        p2->assignValues(a,(i*72*deg_rad),r);
        p3->assignValues(a,((i+1)*72*deg_rad),r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues((b-a),ct,r);
        p2->assignValues(a,((i+1)*72*deg_rad),r);
        p3->assignValues((b-a),((i+1)*72*deg_rad),r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues((b-a),ct,r);
        p2->assignValues((b-a),((i+1)*72*deg_rad),r);
        p3->assignValues(phi1,t11,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues((b-a),ct,r);
        p2->assignValues(phi1,t11,r);
        p3->assignValues(phi1,t1,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues((b-a),ct,r);
        p2->assignValues(phi1,t1,r);
        p3->assignValues((b-a),(i*72*deg_rad),r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

    }
//============LOWER PART==================================
//glColor3f(0.0f,0.0f,0.0f);
glBindTexture(GL_TEXTURE_2D, texture[0]);
glTexCoord2f(0.5, 0.5);
    //lower pentagon
    double yPhi = 180*deg_rad;
    for(int i=0; i<5;i++){
        p1->assignValues(yPhi,0,r);
        p2->assignValues(yPhi-a,(36+i*72)*deg_rad,r);
        p3->assignValues(yPhi-a,(36+(i+1)*72)*deg_rad,r);
        drawDeeperTrinagles(p1,p2,p3,2);
    }

    //lower 6 pentagons
    for(int i=0; i<5;i++){
        double t1 = theta1 + (36+i*72)*deg_rad;
        double t11 = -theta1 + (36+i*72)*deg_rad;
        double t2 = theta2 + (36+i*72)*deg_rad;
        double t22 = -theta2 + (36+i*72)*deg_rad;

        p1->assignValues(yPhi-b,(36+i*72)*deg_rad,r);
        p2->assignValues(yPhi-(b-a),(36+i*72)*deg_rad,r);
        p3->assignValues(yPhi-phi1,t1,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-b,(36+i*72)*deg_rad,r);
        p2->assignValues(yPhi-phi1,t1,r);
        p3->assignValues(yPhi-phi2,t2,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-b,(36+i*72)*deg_rad,r);
        p2->assignValues(yPhi-phi2,t2,r);
        p3->assignValues(yPhi-phi2,t22,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-b,(36+i*72)*deg_rad,r);
        p2->assignValues(yPhi-phi2,t22,r);
        p3->assignValues(yPhi-phi1,t11,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-b,(36+i*72)*deg_rad,r);
        p2->assignValues(yPhi-phi1,t11,r);
        p3->assignValues(yPhi-(b-a),(36+i*72)*deg_rad,r);

        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

    }

    //Connect Lower pentagons with lines
    //glColor3f(0.0f,0.0f,0.0f);
    for(int i=0; i<5;i++){
        drawLine(r,yPhi- a,yPhi-(b-a),(36+i*72)*deg_rad,1);
        double t1 = theta1 + (36+i*72)*deg_rad;
        double t11 = -theta1 + (36+(i+1)*72)*deg_rad;
        drawLine(r,t11,t1,yPhi- phi1,0);
    }
    //fill the lower hexagons
    //glColor3f(0.0f,0.0f,1.0f);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexCoord2f(0.5, 0.5);
    for(int i=0; i<6;i++){
        double t1 = -theta1 + (36+i*72)*deg_rad;
        double t11 = theta1 + (36+(i-1)*72)*deg_rad;
        double ct = (i*72)*deg_rad;

        p1->assignValues(yPhi-(b-a),ct,r);
        p2->assignValues(yPhi-(b-a),((36+i*72)*deg_rad),r);
        p3->assignValues(yPhi-a,((36+i*72)*deg_rad),r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-(b-a),ct,r);
        p2->assignValues(yPhi-a,((36+i*72)*deg_rad),r);
        p3->assignValues(yPhi-a,((36+(i-1)*72)*deg_rad),r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-(b-a),ct,r);
        p2->assignValues(yPhi-a,((36+(i-1)*72)*deg_rad),r);
        p3->assignValues(yPhi-(b-a),((36+(i-1)*72)*deg_rad),r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-(b-a),ct,r);
        p2->assignValues(yPhi-(b-a),((36+(i-1)*72)*deg_rad),r);
        p3->assignValues(yPhi-phi1,t11,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-(b-a),ct,r);
        p2->assignValues(yPhi-phi1,t11,r);
        p3->assignValues(yPhi-phi1,t1,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-(b-a),ct,r);
        p2->assignValues(yPhi-phi1,t1,r);
        p3->assignValues(yPhi-(b-a),((36+i*72)*deg_rad),r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

    }
//============MIDDLE PART==================================
    //connect upper pentagons with lower pentagons with lines(middle hexagons)
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexCoord2f(0.5, 0.5);
    for(int i=0; i<5;i++){
        double t2 = theta2 + i*72*deg_rad;
        double t22 = -theta2 + i*72*deg_rad;
        double t23 = theta2 + (36+(i-1)*72)*deg_rad;
        double t24 = -theta2 + (36+(i)*72)*deg_rad;
        glColor3f(0.0f,0.0f,0.0f);
        drawLineInBothDir(r,phi2,yPhi-phi2,t22,t23);
        drawLineInBothDir(r,phi2,yPhi-phi2,t2,t24);
    }

    //fill the middle hexagons
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexCoord2f(0.5, 0.5);
    for(int i=0; i<5;i++){
        double t1 = -theta1 + (36+i*72)*deg_rad;
        double t11 = theta1 + (36+(i-1)*72)*deg_rad;
        double t2 = theta2 + i*72*deg_rad;
        double t22 = -theta2 + i*72*deg_rad;
        double ct = i*72*deg_rad;
        double t3 = -theta2 + (36+i*72)*deg_rad;
        double t31 = theta2 + (36+(i-1)*72)*deg_rad;

        p1->assignValues(yPhi-phi2,ct,r);
        p2->assignValues(yPhi-phi2,t3,r);
        p3->assignValues(phi2,t2,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-phi2,ct,r);
        p2->assignValues(phi2,t2,r);
        p3->assignValues(phi2,t22,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-phi2,ct,r);
        p2->assignValues(phi2,t22,r);
        p3->assignValues(yPhi-phi2,t31,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-phi2,ct,r);
        p2->assignValues(yPhi-phi2,t31,r);
        p3->assignValues(yPhi-phi1,t11,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-phi2,ct,r);
        p2->assignValues(yPhi-phi1,t11,r);
        p3->assignValues(yPhi-phi1,t1,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(yPhi-phi2,ct,r);
        p2->assignValues(yPhi-phi1,t1,r);
        p3->assignValues(yPhi-phi2,t3,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        //=========================
        ct = (36+i*72)*deg_rad;
        t1 = -theta1 + ((i+1)*72)*deg_rad;
        t11 = theta1 + (i*72)*deg_rad;
        t2 = -theta2 + (i+1)*72*deg_rad;
        t22 = theta2 + i*72*deg_rad;
        t3 = theta2 + (36+i*72)*deg_rad;
        t31 = -theta2 + (36+i*72)*deg_rad;
        p1->assignValues(phi2,ct,r);
        p2->assignValues(yPhi-phi2,t3,r);
        p3->assignValues(phi2,t2,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(phi2,ct,r);
        p2->assignValues(phi2,t2,r);
        p3->assignValues(phi1,t1,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(phi2,ct,r);
        p2->assignValues(phi1,t1,r);
        p3->assignValues(phi1,t11,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(phi2,ct,r);
        p2->assignValues(phi1,t11,r);
        p3->assignValues(phi2,t22,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(phi2,ct,r);
        p2->assignValues(phi2,t22,r);
        p3->assignValues(yPhi-phi2,t31,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

        p1->assignValues(phi2,ct,r);
        p2->assignValues(yPhi-phi2,t31,r);
        p3->assignValues(yPhi-phi2,t3,r);
        drawDeeperTrinagles(p1,p2,p3,trianglesDepth);

    }
}

void resetProjection() {
    glViewport(0,0,(GLsizei)currWidth,(GLsizei)currHeight);
  //glPushMatrix();
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(l, r, b, t, n, f);
   glMatrixMode(GL_MODELVIEW);
   //glLoadIdentity();
   //glPopMatrix();
}
void reshape(int w, int h) {
   currHeight = h;
   currWidth = w;
  //
   //resetProjection(l,r,b, t);
}
void bulidField(){
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexCoord2f(0.5, 0.5);
    //draw the center line of the field
    glBegin(GL_LINES);
        glVertex3f(-10,-1.0,5);
        glVertex3f(4,-1.0,5);
    glEnd();

    //draw the center circle of the field
    glBegin(GL_LINE_STRIP);
    double r=1.5;
        for(double phi=0;phi<2*PI;phi+=PI/20){
            glVertex3f(-3+r*cos(phi),-1.0,5+r*sin(phi));
        }
    glEnd();

    //draw the four border lines of the field
    glBegin(GL_LINES);
        glVertex3f(-9.5,-1.0,29);
        glVertex3f(-9.5,-1.0,-19);

        glVertex3f(-9.5,-1.0,-19);
        glVertex3f(3.5,-1.0,-19);

        glVertex3f(3.5,-1.0,-19);
        glVertex3f(3.5,-1.0,29);

        glVertex3f(3.5,-1.0,29);
        glVertex3f(-9.5,-1.0,29);
    glEnd();
    //draw the four corners
    //draw the top left corner
    glBegin(GL_LINE_STRIP);
    r=1;
        for(double phi=0;phi<0.5*PI;phi+=PI/20){
            glVertex3f(-9.5+r*cos(phi),-1.0,29-r*sin(phi));
        }
    glEnd();
    //draw the top right corner
    glBegin(GL_LINE_STRIP);
    r=1;
        for(double phi=0;phi<0.5*PI;phi+=PI/20){
            glVertex3f(-9.5+r*cos(phi),-1.0,-19+r*sin(phi));
        }
    glEnd();
    //draw the buttom right corner
    glBegin(GL_LINE_STRIP);
    r=1;
        for(double phi=0;phi<0.5*PI;phi+=PI/20){
            glVertex3f(3.5-r*cos(phi),-1.0,-19+r*sin(phi));
        }
    glEnd();
    //draw the buttom left corner
    glBegin(GL_LINE_STRIP);
    r=1;
        for(double phi=0;phi<0.5*PI;phi+=PI/20){
            glVertex3f(3.5-r*cos(phi),-1.0,29-r*sin(phi));
        }
    glEnd();
    //draw the field
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexCoord2f(0.5, 0.5);
    glBegin(GL_POLYGON);
        glVertex3f(-10,-1.0,30);
        glVertex3f(-10,-1.0,-20);
        glVertex3f(4,-1.0,-20);
        glVertex3f(4,-1.0,30);
    glEnd();



}

void moveTheBallInStraight(){
    motionStepStraight -= motionDecreasingTheStepStraight;
    angleStep -= angleDecreasingTheStep;
    if(motionStepStraight <= 0)
        startAnimationInStraight = 0;
    /*angleStep -= PI/15;*/

    if(startAnimationInStraight == 1){
        //cout<<"M: "<<motionStep<<"   DIR: "<<tZDir<<"   LOC: "<<tZ<<"\n";
        if(tZ < 28 && tZDir==1){
            //ctZ = tZ;
            tZ +=motionStepStraight;
            rotatAngleX +=angleStep;
            rotatAngleY += angleStep;
            rotatAngleZ += angleStep;
        }
        else if(tZ > -19 && tZDir==-1){
            //ctZ = tZ;
            tZ -=motionStepStraight;
            rotatAngleX -=angleStep;
            rotatAngleY -= angleStep;
            rotatAngleZ -= angleStep;

        }
        else {tZDir *=-1;}
        motionDecreasingTheStepStraight +=0.0001;
        angleDecreasingTheStep += PI/30;
        glutPostRedisplay();
    }
}
void moveTheBallInParabola(){
    angleStep -= angleDecreasingTheStep;
    if(parabolaRadius <= 0.35){
        startAnimationInParabola = 0;
        motionStepStraight = 0.2;
        motionDecreasingTheStepStraight=0.009;
        startAnimationInStraight = 1;
    }
    if(startAnimationInParabola == 1){
        if(tZ >= 28){
            tZDir = -1;
             parabolaRadius -= parabolaRadius/5;
             parabolaDestinationZ = tZ - parabolaRadius;
             parabolaStepZ = abs(tZ - parabolaDestinationZ)/20;
             parabolaCenterZ = (parabolaDestinationZ+tZ)/2;
        }
        else if(tZ <= -19){
            tZDir = 1;
             parabolaRadius -= parabolaRadius/5;
             parabolaDestinationZ = tZ + parabolaRadius;
             parabolaStepZ = abs(tZ - parabolaDestinationZ)/20;
             parabolaCenterZ = (parabolaDestinationZ+tZ)/2;
        }
//PX=7.79999--PY7.09998--PZ=3--LX=-2--LY4.59999--LZ3.1
        if(tZDir==1){
            tZ += parabolaStepZ;
            rotatAngleX +=angleStep;
            rotatAngleY += angleStep;
            rotatAngleZ += angleStep;
            double s = (tZ-parabolaCenterZ)/(parabolaRadius/2);
            if(tZ >= parabolaDestinationZ)
                tY = INITIAL_BALL_LOCATION_Y;
            else tY = INITIAL_BALL_LOCATION_Y+sqrt(1-s*s)*parabolaRadius*2;
            cout<<"3***";
        }else if(tZDir==-1){
            tZ -= parabolaStepZ;
            rotatAngleX -=angleStep;
            rotatAngleY -= angleStep;
            rotatAngleZ -= angleStep;
            double s = (tZ-parabolaCenterZ)/(parabolaRadius/2);
            if(tZ <= parabolaDestinationZ)
                tY = INITIAL_BALL_LOCATION_Y;
            else tY = INITIAL_BALL_LOCATION_Y+sqrt(1-s*s)*parabolaRadius*2;
            cout<<"4***";
        }

        if(tY <= INITIAL_BALL_LOCATION_Y)
        {
            cout<<"*******************************************";
            parabolaRadius -= parabolaRadius/3;
            //motionDecreasingTheStepParabola +=0.3;
            tY = INITIAL_BALL_LOCATION_Y;
            if(tZDir==1)
                parabolaDestinationZ = tZ + parabolaRadius;
            if(tZDir==-1)
                parabolaDestinationZ = tZ - parabolaRadius;
            parabolaStepZ = abs(tZ - parabolaDestinationZ)/20;
            parabolaCenterZ = (parabolaDestinationZ+tZ)/2;
angleDecreasingTheStep += PI/30;
        }

        cout<<"   DIR: "<<tZDir<<"  TY: "<<tY<<"  TZ:"<<tZ<<" r:"<<parabolaRadius<<"\n";
    }
    glutPostRedisplay();
}

void display() {
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLoadIdentity();
    gluLookAt (camerPositionX, camerPositionY, camerPositionZ, camerLookingX, camerLookingY, camerLookingZ, 0.0, 1.0, 0.0);

    bulidField();
    glPushMatrix();
    glTranslatef(tX,tY,tZ);
    glRotatef(rotatAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotatAngleY, 0.0f, 1.0f, 0.0f);
    //glRotatef(firstRotatAngleZ, 0.0f, 1.0f, 0.0f);
    buildSoccerBall();
    glPopMatrix();

   glutSwapBuffers();
   if(startAnimationInStraight==1){
       sleep(0.033333333);
       moveTheBallInStraight();
   }else if(startAnimationInParabola==1){
       sleep(0.033333333);
       moveTheBallInParabola();
   }
}

void keyboardPressed(unsigned char key, int x, int y) {
    if(key == 'l' || key == 'L') {
        camerPositionX=2.2;
        camerPositionY=14;
        camerPositionZ=5;
        camerLookingX=-2;
        camerLookingY=-5.9;
        camerLookingZ=3.1;
        angleStep =2*PI;
        angleDecreasingTheStep = PI/30;
        motionStepStraight = INITIAL_STARTING_MOTION_STRIGHT;
        motionDecreasingTheStepStraight=INITIAL_DECREASING_STEP_STRIGHT;
        parabolaRadius = INITIAL_STARTING_RADIUS_PARABOLA;
        //motionDecreasingTheStepParabola=INITIAL_DECREASING_STEP_PARABOLA;
        startAnimationInStraight = 0;
        startAnimationInParabola = 0;
        tX = INITIAL_BALL_LOCATION_X;
        tY = INITIAL_BALL_LOCATION_Y;
        tZ = INITIAL_BALL_LOCATION_Z;
        rotatAngleX = 0;
        rotatAngleY = 0;
        rotatAngleZ = 0;
    }
   else if(key == 'a' || key == 'q'|| key == 'w'|| key == 'e'|| key == 's'|| key == 'd'|| key == 'r'|| key == 'f'|| key == 't'|| key == 'g'|| key == 'y'|| key == 'u'){
        if(key == 'q'){
            camerPositionX += 0.1;
        }else if(key == 'a'){
            camerPositionX -= 0.1;
        }else if(key == 'w'){
            camerPositionY += 0.1;
        }else if(key == 's'){
            camerPositionY -= 0.1;
        }else if(key == 'e'){
            camerPositionZ += 0.1;
        }else if(key == 'd'){
            camerPositionZ -= 0.1;
        }else if(key == 'r'){
            camerLookingX += 0.1;
        }else if(key == 'f'){
            camerLookingX -= 0.1;
        }else if(key == 't'){
            camerLookingY += 0.1;
        }else if(key == 'g'){
            camerLookingY -= 0.1;
        }else if(key == 'y'){
            camerLookingZ += 0.1;
        }else if(key == 'u'){
            camerLookingZ -= 0.1;
        }
        cout<<"PX="<<camerPositionX<<"--PY"<<camerPositionY<<"--PZ="<<camerPositionZ;
        cout<<"--LX="<<camerLookingX<<"--LY"<<camerLookingY<<"--LZ"<<camerLookingZ<<"\n";
    } else if(key == 'h'){
        tY = INITIAL_BALL_LOCATION_Y;
        angleStep =2*PI;
        angleDecreasingTheStep = PI/30;
        motionStepStraight = INITIAL_STARTING_MOTION_STRIGHT;
        motionDecreasingTheStepStraight=INITIAL_DECREASING_STEP_STRIGHT;
        startAnimationInStraight = 1;

    } else if(key == 'v'){

        angleStep =2*PI;
        angleDecreasingTheStep = PI/30;
        parabolaRadius= INITIAL_STARTING_RADIUS_PARABOLA;
        if(tZDir == -1){
            parabolaDestinationZ = tZ - parabolaRadius;
        }
        else if(tZDir == 1){
            parabolaDestinationZ = tZ + parabolaRadius;
        }
        parabolaStepZ = abs(tZ - parabolaDestinationZ)/20;
        parabolaCenterZ = (parabolaDestinationZ+tZ)/2;
        angleParabolaStep=PI/30;
        startAnimationInParabola = 1;
        camerPositionX = 9.0;
        camerPositionY = 7.09998;
        camerPositionZ = 2.7;
        camerLookingX = -2;
        camerLookingY = 4.5;
        camerLookingZ = 3.1;
    }
   else if (key == 27)
      exit(0);
    glutPostRedisplay();
}
int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size; // size of the image in bytes.
    unsigned long i; // standard counter.
    unsigned short int planes; // number of planes in image (must be 1)
    unsigned short int bpp; // number of bits per pixel (must be 24)
    char temp; // temporary color storage for bgr-rgb conversion.
    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL){
        printf("File Not Found : %s\n",filename);
        return 0;
    }
    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);
    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
        printf("Error reading width from %s.\n", filename);
        return 0;
    }
    //printf("Width of %s: %lu\n", filename, image->sizeX);
    // read the height
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
        printf("Error reading height from %s.\n", filename);
        return 0;
    }
    //printf("Height of %s: %lu\n", filename, image->sizeY);
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;
    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
        printf("Error reading planes from %s.\n", filename);
        return 0;
    }
    if (planes != 1) {
        printf("Planes from %s is not 1: %u\n", filename, planes);
        return 0;
    }
    // read the bitsperpixel
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
        printf("Error reading bpp from %s.\n", filename);
        return 0;
    }
    if (bpp != 24) {
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return 0;
    }
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);
    // read the data.
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
        printf("Error allocating memory for color-corrected image data");
        return 0;
    }
    if ((i = fread(image->data, size, 1, file)) != 1) {
        printf("Error reading image data from %s.\n", filename);
        return 0;
    }
    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
        temp = image->data[i];
        image->data[i] = image->data[i+2];
        image->data[i+2] = temp;
    }
    // we're done.
    return 1;
}

Image * loadTexture(char *filename){
    Image *image1;
    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
        printf("Error allocating space for image");
        exit(0);
    }
    if (!ImageLoad(filename, image1)) {
        exit(1);
    }
    return image1;
}
void makeCheckImage(void){
    int i, j, c;
    for (i = 0; i < checkImageWidth; i++) {
        for (j = 0; j < checkImageHeight; j++) {
            c = ((((i&0x8)==0)^((j&0x8)==0)))*255;
            checkImage[i][j][0] = (GLubyte) c;
            checkImage[i][j][1] = (GLubyte) c;
            checkImage[i][j][2] = (GLubyte) c;
        }
    }
}
void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    gluLookAt (camerPositionX, camerPositionY, camerPositionZ, camerLookingX, camerLookingY, camerLookingZ, 0.0, 1.0, 0.0);
    resetProjection();

    Image *image1 = loadTexture("black.bmp");
    Image *image2 = loadTexture("white.bmp");
    Image *image3 = loadTexture("field.bmp");
    if(image1 == NULL){
        printf("Black Image was not returned from loadTexture\n");
        exit(0);
    }
    if(image2 == NULL){
        printf("White Image was not returned from loadTexture\n");
        exit(0);
    }
    if(image3 == NULL){
        printf("Field Image was not returned from loadTexture\n");
        exit(0);
    }
    //makeCheckImage();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Create Texture
    glGenTextures(3, texture);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image1->sizeX, image1->sizeY, 0,GL_RGB, GL_UNSIGNED_BYTE, image1->data);

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, checkImageWidth,checkImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,&checkImage[0][0][0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image2->sizeX, image2->sizeY, 0,GL_RGB, GL_UNSIGNED_BYTE, image2->data);

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image3->sizeX, image3->sizeY, 0,GL_RGB, GL_UNSIGNED_BYTE, image3->data);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

}
int main(int argc, char **argv) {

   glutInit(&argc,argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
       glEnable(GL_DEPTH_TEST);

   glutInitWindowSize(currWidth,currHeight);
   glutInitWindowPosition(0,0);
   glutCreateWindow("Project5: Socccer Ball");


   glutDisplayFunc(display);
   glutReshapeFunc(reshape);

   glutKeyboardFunc(keyboardPressed);
init();
//   GLfloat amb[]={1.0, 1.0, 1.0, 1.0};
//   GLfloat dif[]={1.0, 1.0, 1.0, 1.0};
//   GLfloat spc[]={1.0, 1.0, 1.0, 1.0};
//   GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
//   GLfloat local_view[] = {0.0};
//   glEnable(GL_LIGHTING);
//   glEnable(GL_LIGHT0);
//      glEnable(GL_DEPTH_TEST);
//   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);


//   glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
//   glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
//   glLightfv(GL_LIGHT0, GL_SPECULAR, spc);
//   glEnable(GL_NORMALIZE);

//   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
//   glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

   glutMainLoop();
   //delete MengerCube;
}

