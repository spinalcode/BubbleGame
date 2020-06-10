#include <Pokitto.h>
#include <miloslav.h>
#include <Tilemap.hpp>
#include "sprites.h"
#include "background.h"
#include "buttonhandling.h"

#define LEFT_SIDE 56
#define TOP_SIDE 8
#define BALL_WIDTH 14
#define BALL_HEIGHT 12
#define tileWidth 14
#define tileHeight 12
#define MOVEANGLE 1

long int myDelay;
long int tempTime;

struct BALL_DATA {
    float x;  // x postition
    float y;  // y position
    float vx;  // x vector
    float vy;  // y vector
    uint8_t colour; // tile number
    float angle = 45.0; // current angle of ball
    float distance; // current distance of ball
    float speed;
    float radius = 7;
    bool moving = false;
} ball, target;

struct grid {
    int colour;
    bool processed = false;
    bool toProcess = false;
    bool toRemove = false;
}  levelGrid[8][12];

// order to check tiles
int oddlist[][2]={
	     { 0,-1}, { 1,-1},
	{-1, 0},          { 1, 0},
	     { 0, 1}, { 1, 1},
};
int evenlist[][2]={
	     {-1,-1}, { 0,-1},
	{-1, 0},          { 1, 0},
	     {-1, 1}, { 0, 1},
};

struct TARGET_LINE {
    int x;
    int y;
} tgt[200];
int numtgt;

int procressCount = 0;


void setFPS(int fps){
  myDelay = 1000 / fps;
}


float radToDeg(float angle) {
    return angle * (180 / PI);
}
float degToRad(float angle) {
    return angle * (PI / 180);
}

int getTile(int x, int y){
    return levelMap[((x/8)+levelMap[0]*(y/8))+2];
}

int circleCollision(int x1, int y1, int r1, int x2, int y2, int r2){
    int dx = x1-x2;
    int dy = y1-y2;
    int len = sqrt(dx*dx+dy*dy);
    if(len < r1+r2) return 1;
    return 0;
}

float angle = 0;

void newBall(){
    ball.x = 110;
    ball.y = 176-tileHeight;
    ball.colour = random(4)+2;
    ball.speed = 5;
    ball.vx = ball.speed * cos(degToRad(ball.angle));
    ball.vy = ball.speed * -1*sin(degToRad(ball.angle));
    ball.moving=false;
}

void newGame(){
    for(int y=0; y<11; y++){
        for(int x=0; x<8; x++){
            levelGrid[x][y].colour = 0;
            levelGrid[x][y].toProcess = false;
            levelGrid[x][y].toRemove = false;
        }
    }
}
void clearToProcess(){
    for(int y=0; y<11; y++){
        for(int x=0; x<8; x++){
            levelGrid[x][y].toProcess = false;
            levelGrid[x][y].toRemove = false;
        }
    }
}



void floodFill(int x,int y, int colour){

    if(x<0 || x>8-(y%2) || y<0 || y>14) return;
    if(levelGrid[x][y].colour != colour) return;
    levelGrid[x][y].toProcess = true;

    procressCount++;
    
    for(int t=0; t<6; t++){
        if(y%2==0){
            if(x+evenlist[t][0] >=0 && x+evenlist[t][1]>=0 && x+evenlist[t][1]<8-(y%2)){
                floodFill(x+evenlist[t][0], y+evenlist[t][1], colour);
            }
        }else{
            if(x+oddlist[t][0] >=0 && x+oddlist[t][1]>=0 && x+oddlist[t][1]<8-(y%2)){
                floodFill(x+oddlist[t][0], y+oddlist[t][1], colour);
            }
        }
    }

    return;
}

void renderLevel(){
    using PD=Pokitto::Display;

    // background
    for(int y=0; y<22; y++){
        for(int x=0; x<28; x++){
            PD::drawBitmap(x*8, y*8, bg_Tiles[levelMap[2+x+28*y]]);
        }
    }

    for(int y=0; y<11; y++){
        for(int x=0; x<8; x++){
            PD::color=1;
            if(levelGrid[x][y].colour != 0){
                if(y&1){
                    PD::drawBitmap((balls[0][0]/2)+LEFT_SIDE+x*BALL_WIDTH, TOP_SIDE+y*BALL_HEIGHT, balls[levelGrid[x][y].colour]);
                }else{
                    PD::drawBitmap(LEFT_SIDE+x*BALL_WIDTH, TOP_SIDE+y*BALL_HEIGHT, balls[levelGrid[x][y].colour]);
                }
            }

            if(levelGrid[x][y].toProcess ==true){
                if(y&1){
                    PD::drawBitmap((balls[0][0]/2)+LEFT_SIDE+x*BALL_WIDTH, TOP_SIDE+y*BALL_HEIGHT, balls[0]);
                }else{
                    PD::drawBitmap(LEFT_SIDE+x*BALL_WIDTH, TOP_SIDE+y*BALL_HEIGHT, balls[0]);
                }
            }
        }
    }


    // target
    for (int t=0; t<numtgt; t++){
        PD::drawBitmap(tgt[t].x-target.radius, tgt[t].y-target.radius, balls[0]);
    }
    // ball
    PD::drawBitmap(ball.x-ball.radius, ball.y-ball.radius, balls[ball.colour]);


    float tx = 110 + 20 * cos(degToRad(angle));
    float ty = (176-tileHeight) + 20 * -1*sin(degToRad(angle));
    PD::color = 2;
    PD::drawLine(110,(176-tileHeight),tx,ty);
}

int main(){
    using PC=Pokitto::Core;
    using PD=Pokitto::Display;
    using PB=Pokitto::Buttons;

    PC::begin();
    PD::load565Palette(balls_pal);
    PD::invisiblecolor = 0;

    newBall();

    setFPS(50);

    angle = ball.angle;

    while( PC::isRunning() ){
        if( !PC::update() ) 
            continue;
        
        renderLevel();

        updateButtons();


        if(ball.moving){
            ball.x += ball.vx;
            ball.y += ball.vy;
            if(_B[NEW])newBall();
        }else{
            float oldAngle= ball.angle;
            if(_C[NEW])newGame();
            if(_Up[HELD]){
                if(angle > 90)angle-=MOVEANGLE;
                if(angle < 90)angle+=MOVEANGLE;
            }
            if(_Down[HELD]){
                if(angle > 90)angle+=MOVEANGLE;
                if(angle < 90)angle-=MOVEANGLE;
            }
            if(_Left[HELD])angle+=MOVEANGLE;    
            if(_Right[HELD])angle-=MOVEANGLE;
            if(angle>=180)angle = 180;
            if(angle<=0)angle = 0;
            ball.angle = angle;
            ball.vx = ball.speed * cos(degToRad(ball.angle));
            ball.vy = ball.speed * -1*sin(degToRad(ball.angle));
            if(ball.angle != oldAngle){
                target.vx = ball.vx;
                target.vy = ball.vy;
                target.x = ball.x;
                target.y = ball.y;
                target.angle = ball.angle;
                target.moving=true;
                numtgt=0;
            }
            if(_A[NEW])ball.moving = true;
        }
        
        // Left/Right walls
        if(getTile(ball.x+ball.radius+1, ball.y)==6 || getTile(ball.x-ball.radius-1, ball.y)==6){
            ball.vx = -ball.vx;
            //ball.angle = 180-ball.angle;
        }

        // top wall
        if(getTile(ball.x, ball.y-ball.radius)==4){
            int cx = int (ball.x-LEFT_SIDE)/BALL_WIDTH;
            int cy = int (ball.y-TOP_SIDE)/BALL_HEIGHT;
            levelGrid[cx][cy].colour=ball.colour;
            clearToProcess();
            procressCount = 0;
            floodFill(cx,cy,ball.colour);
            newBall();
        }
    
        // other balls
        for(int y=0; y<11; y++){
            for(int x=0; x<8; x++){
                if(levelGrid[x][y].colour != 0){

                    int x2 = int ((y&1)*(BALL_WIDTH/2))+LEFT_SIDE+x*BALL_WIDTH+ball.radius;
                    int y2 = int (TOP_SIDE+y*BALL_HEIGHT)+ball.radius;

                    if(circleCollision(ball.x+ball.vx, ball.y+ball.vy, ball.radius-1, x2, y2, ball.radius-1)){
                        
                        int cx = int(ball.x-LEFT_SIDE);
                        int cy = int(ball.y-TOP_SIDE)/BALL_HEIGHT;
                        if(cy&1)cx-=(BALL_WIDTH/2);
                        cx/=BALL_WIDTH;
    
                        levelGrid[cx][cy].colour=ball.colour;
                        clearToProcess();
                        procressCount = 0;
                        floodFill(cx,cy,ball.colour);
                        newBall();
                    }

                }
            }
        }

        int x2 = int(ball.x-LEFT_SIDE);
        int y2 = int(ball.y-TOP_SIDE)/BALL_HEIGHT;
        if(y2&1)x2-=(BALL_WIDTH/2);
        x2/=BALL_WIDTH;

        while(target.moving){
            if(numtgt<200){
                numtgt++;
            }else{
                target.moving=false;
            }
            tgt[numtgt].x = target.x;
            tgt[numtgt].y = target.y;
            target.x += target.vx;
            target.y += target.vy;

            // Left/Right walls
            if(getTile(target.x+ball.radius+1, target.y)==6 || getTile(target.x-ball.radius-1, target.y)==6){
                target.vx = -target.vx;
            }
    
            // top wall
            if(getTile(target.x, target.y-ball.radius)==4){
                int cx = int (target.x-LEFT_SIDE)/BALL_WIDTH;
                int cy = int (target.y-TOP_SIDE)/BALL_HEIGHT;
                target.moving=false;
            }

            // other balls
            for(int y=0; y<11; y++){
                for(int x=0; x<8; x++){
                    if(levelGrid[x][y].colour != 0){
    
                        int x2 = int ((y&1)*(BALL_WIDTH/2))+LEFT_SIDE+x*BALL_WIDTH+ball.radius;
                        int y2 = int (TOP_SIDE+y*BALL_HEIGHT)+ball.radius;
    
                        if(circleCollision(target.x+target.vx, target.y+target.vy, ball.radius-1, x2, y2, ball.radius-1)){
                            target.moving=false;
                        }
    
                    }
                }
            }



        }

        PD::printf("T: %d\n",numtgt);


        // fps delay and display frame
        while(PC::getTime()-tempTime < myDelay){
        }
        tempTime = PC::getTime();

    }
    
    return 0;
}
