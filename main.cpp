
#include <Pokitto.h>
#include <Tilemap.hpp>
#include <File>
#include "sprites.h"
#include "font.h"
#include "background.h"
#include "buttonhandling.h"
#include "tables.h"
//#include "image.h"
bool songFound = 0;
#include "sound.h"

#include "FixedPoints.h"
#include "FixedPointsCommon.h"

#define LEFT_SIDE 56
int TOP_SIDE = 8;
#define BALL_WIDTH 14
#define HALFBALL BALL_WIDTH/2
#define BALL_HEIGHT 12
#define tileWidth 16
#define tileHeight 16
#define HALFTILE 8
#define HALFTALL 6
#define MOVEANGLE 2
#define WALKFRAMES 36
#define WALKDIVIDE (WALKFRAMES/4)

#define EXPLODE_START 18
#define EXPLODE_END 25
#define SPRITE_OFFSET 5
#define MAXSPRITES 64
#define MAXCOLOURS 6

//auto sdfs = new SDFileSystem(P0_9, P0_8, P0_6, P0_7, "sd", NC, SDFileSystem::SWITCH_NONE, 24000000 );
int levelNumber = 0;
const uint8_t *current_bg_Tiles = bg_Tiles;
const uint8_t *current_bg_map = titleMap;// levelMap;



//int dirtMask[176];
int totalBalls=0;
long int myDelay;
long int tempTime;
long int frameCount;
uint8_t fpsCount=0;
long int fpsCounter;
long int lastMillis;

uint32_t frameSkip = 0;
long int levelTime;
int score = 0;
int tempScore=0;
int offsetAngle = 0;
uint8_t queueOffset = 0;
bool moveQueue = false;
#define queueLength 6

#define titleScreenMode 0
#define playLevelMode 1
int gameMode = titleScreenMode;
bool updatePlayfield = true;
int wipeDown=0;

uint8_t cannonFrame=0;

DigitalOut myPin(P0_21);

int screenMask = 0;
inline void fillMask(){
    screenMask = 0b1111'11111111'11111111;
}
inline void clearMask(){
    screenMask = 0;
}
inline void markDirty(int y){
//    dirtMask[y/8]=1;
//    int bit = y/8;
//    screenMask |= 1<<((bit)-2);
//    screenMask |= 1<<((bit)-1);
//    screenMask |= 1<<bit;
//    screenMask |= 1<<((bit)+1);
//    screenMask |= 1<<((bit)+2);
}


bool usingTarget = false;
// Create tile map
Tilemap myTilemap;

uint8_t nextBall[queueLength+1]; // next few balls

struct BALL_DATA {
    float x;                // x postition
    float y;                // y position
    float vx;               // x vector
    float vy;               // y vector
    uint8_t colour;         // tile number
    float angle = 45.0;     // current angle of ball
    float distance;         // current distance of ball
    float speed;            // speed of ball
    uint8_t moveRepeat; 
    int radius = 7.5;         // radius of the ball
    bool moving = false;    // is the ball moving
} ball, target;

struct ARROD_DATA {
    int srcW = balls[50][0];
	int srcH = balls[50][1];
    int fAngle = 0;
    int vx;
    int vy;
    int distance=16;
} arrowSprite;

struct grid {
    int colour;
    bool processed = false;
    bool toProcess = false;
    bool toRemove = false;
}  levelGrid[8][12];

bool dirGrid[8][12]; // what direction for the balls to face on the bg

struct ANIM_DATA {
    bool inUse;
    uint8_t x;
    uint8_t y;
    uint8_t frame;
    int bitmap;
    uint8_t frameSpeed;
    uint8_t lastFrame;
    int frameCounter;
    uint8_t type; // 0 for stay in place, 1 for fall
    uint8_t maskFrame;
    float speed;
} mySprite[MAXSPRITES];

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

/*
struct TARGET_LINE {
    int x;
    int y;
} tgt[128];
int numtgt;
*/

int processCount = 0;

// print text
void myPrint(char x, char y, const char* text) {
  uint8_t numChars = strlen(text);
  uint8_t x1 = 0;//2+x+28*y;
  for (uint8_t t = 0; t < numChars; t++) {
    uint8_t character = text[t] - 32;
    Pokitto::Display::drawSprite(x+((x1++)*8), y, font88[character]);
  }
}

void setFPS(int fps){
  myDelay = 1000 / fps;
}

inline float radToDeg(float angle) {
    return angle * (180 / PI);
}
inline float degToRad(float angle) {
    return angle * (PI / 180);
}

using Fract = SQ15x16;

//Fract sine[361];
//Fract cosine[361];

Fract angleTable[64];

constexpr const float floatPi = 3.141592654f;
constexpr const double doublePi = 3.141592654;

void generateAngleTable()
{
	const double factor = doublePi / 128.0;
	for(unsigned int a = 0; a < 64; ++a)
	{
		angleTable[a] = static_cast<Fract>(std::sin(static_cast<double>(a) * factor));
	}
}

Fract lookupAngle(std::uint_fast8_t index)
{
    return (index == 64) ? 1 : angleTable[(index & (0x3F))];
}

Fract Sin(std::uint8_t brads)
{
	const std::uint_fast8_t fastBrads = brads;
	const std::uint_fast8_t quarter = ((fastBrads >> 6) & 0x03);
	const std::uint_fast8_t index = ((fastBrads >> 0) & 0x3F);
	switch (quarter)
	{
		case 0: return lookupAngle(index);
		case 1: return lookupAngle(64 - index);
		case 2: return -lookupAngle(index);
		case 3: return -lookupAngle(64 - index);
		default: return 0;
	}
}

Fract Cos(std::uint8_t brads)
{
	const std::uint_fast8_t fastBrads = brads;
	const std::uint_fast8_t quarter = ((fastBrads >> 6) & 0x03);
	const std::uint_fast8_t index = ((fastBrads >> 0) & 0x3F);
	switch (quarter)
	{
		case 0: return lookupAngle(64 - index);
		case 1: return -lookupAngle(index);
		case 2: return -lookupAngle(64 - index);
		case 3: return lookupAngle(index);
		default: return 0;
	}
}


void rotateSprite(unsigned int fAngle){

	unsigned int dstW = arrow_sprite[0];
	unsigned int dstH = arrow_sprite[1];
	unsigned int fDstCX = dstW/2;
	unsigned int fDstCY = dstH/2;

	unsigned int srcW = cannon[cannonFrame][0];
	unsigned int srcH = cannon[cannonFrame][1];
	unsigned int fSrcCX = srcW/2;
	unsigned int fSrcCY = srcH/2;

	const Fract inverseScale = 1.0 / 0.95;

    fAngle = fAngle*256/360;

	Fract s = Sin(-fAngle);//sine[fAngle];  // pre-calculated look-up
	Fract c = Cos(-fAngle);//cosine[fAngle];

	Fract duCol = s * inverseScale;
	Fract dvCol = c * inverseScale;
	Fract duRow = dvCol;
	Fract dvRow = -duCol;

	Fract startingu = fSrcCX - (fDstCX * dvCol + fDstCY * duCol);
	Fract startingv = fSrcCY - (fDstCX * dvRow + fDstCY * duRow);

	Fract rowu = startingu;
	Fract rowv = startingv;

    int s1=0;
    for(int y=0; y<dstH; y++){
        Fract u = rowu;
		Fract v = rowv;
		for(int x=0; x<dstW; x++){
            if ( u >=0 && u < arrow_sprite[0] && v >=0 && v < arrow_sprite[1] ){
                
                const int16_t iv = static_cast<int16_t>(v);
				const int16_t iu = static_cast<int16_t>(u);
				if(iv >0 && iv <cannon[cannonFrame][1] && iu >0 && iu < cannon[cannonFrame][0]){
				    unsigned int i = iv * srcW + iu ;
                    arrow_sprite[2+s1] = cannon[cannonFrame][i+2];
                    arrow_spriteb[2+s1] = cannonb[cannonFrame][i+2];
				}else{
                    arrow_sprite[2+s1] = 0;
                    arrow_spriteb[2+s1] = 0;
				}
            }
            s1++;
            u += duRow;
            v += dvRow;
        }
        rowu += duCol;
        rowv += dvCol;
    }
}

inline int getTile(int x, int y){
    return myMap[((x/8)+myMap[0]*(y/8))+2];
}

inline int circleCollision(int x1, int y1, int r1, int x2, int y2, int r2){
    int dx = x1-x2;
    int dy = y1-y2;
    int len = sqrt(dx*dx+dy*dy);
    if(len < r1+r2) return 1;
    return 0;
}

float angle = 0;

void nxtBall(){
    for(int t=0; t<queueLength; t++){
        nextBall[t] = nextBall[t+1];
    }
    int temp = nextBall[queueLength];
    while(nextBall[queueLength] == temp){
        nextBall[queueLength] = random(maxBall[levelNumber])+1;
    }
    ball.colour = nextBall[0];
}

void newBall(){
    
    moveQueue = true;
    ball.moving=false;
    ball.x = arrowSprite.vx;//LEFT_SIDE+(112/2);
    ball.y = arrowSprite.vy+7;//176-tileHeight/2;
    ball.speed = 0.5;
    ball.moveRepeat = 5/ball.speed;
    ball.vx = ball.speed * cos((int)ball.angle);
    ball.vy = ball.speed * -1*sin((int)ball.angle);
}


void loadLevel(const uint8_t *buffer){
    memcpy(&myMap[0], &buffer[0], sizeof(myMap));

    for(int num=0; num<MAXSPRITES; num++){
        mySprite[num].inUse = false;
    }

    TOP_SIDE = 0;
    for(int y=0; y<11; y++){
        for(int x=0; x<8; x++){
            levelGrid[x][y].colour = levelData[levelNumber][x+8*y];
        }
    }
}

void newGame(){
    // reload the bg gfx into the map
    
    loadLevel(&levelMap[0]);
    levelTime = Pokitto::Core::getTime()+1000;
    current_bg_map = levelMap;

    score=0;
    for(int t=0; t<queueLength+1; t++){
        nextBall[t] = random(maxBall[levelNumber])+1; 
    }
    newBall();
    for(int t=0; t<queueLength; t++){
        nxtBall();
    }

}
void clearToProcess(){
    for(int y=0; y<11; y++){
        for(int x=0; x<8; x++){
            levelGrid[x][y].toProcess = false;
        }
    }
}

void clearProcessed(){
    for(int y=0; y<11; y++){
        for(int x=0; x<8; x++){
            if(levelGrid[x][y].toProcess == true){
                levelGrid[x][y].toProcess = false;
                levelGrid[x][y].colour = 0;
                levelGrid[x][y].toRemove = false;
            }
        }
    }
}

// find the next availabe sprite, give it some data
void newSprite(int x, int y, int startFrame, int endFrame,int speed, int type=0){
    for(int num=0; num<MAXSPRITES; num++){
        if( mySprite[num].inUse == false ){
            mySprite[num].x = x;
            mySprite[num].y = y;
            mySprite[num].frame = startFrame;
            mySprite[num].lastFrame = endFrame;
            mySprite[num].frameSpeed = speed;
            mySprite[num].frameCounter = 0;
            mySprite[num].inUse = true;
            mySprite[num].type = type;
            mySprite[num].speed = 0;
            break;
        }
    }
}

void animProcessed(){
    // explode joining balls
    for(int y=0; y<11; y++){
        for(int x=0; x<8; x++){
            // collection to remove
            if(levelGrid[x][y].toProcess == true){
/*
                newSprite((BALL_WIDTH/2*(y%2))+LEFT_SIDE+x*BALL_WIDTH, TOP_SIDE+y*BALL_HEIGHT,42,49,4);
                levelGrid[x][y].toProcess = false;
                levelGrid[x][y].colour = 0;
                levelGrid[x][y].toRemove = false;
*/
                levelGrid[x][y].toRemove = false;
                newSprite((BALL_WIDTH/2*(y%2))+LEFT_SIDE+x*BALL_WIDTH, TOP_SIDE+y*BALL_HEIGHT, levelGrid[x][y].colour+SPRITE_OFFSET, levelGrid[x][y].colour+SPRITE_OFFSET,2,1);
                levelGrid[x][y].colour = 0;

            }
        }
    }
}

void animRemoved(){
    // make balls fall down
    tempScore=2;
    for(int y=0; y<11; y++){
        for(int x=0; x<8; x++){
            // collection floating, needs to fall
            if(levelGrid[x][y].toRemove == true){
                levelGrid[x][y].toRemove = false;
                newSprite((BALL_WIDTH/2*(y%2))+LEFT_SIDE+x*BALL_WIDTH, TOP_SIDE+y*BALL_HEIGHT, levelGrid[x][y].colour+SPRITE_OFFSET, levelGrid[x][y].colour+SPRITE_OFFSET,2,1);
                levelGrid[x][y].colour = 0;
                score+=tempScore;
                tempScore++;
            }
        }
    }
}

void setRemove(){
    for(int y=0; y<11; y++){
        for(int x=0; x<8; x++){
            levelGrid[x][y].toProcess = false;
            if(levelGrid[x][y].colour != 0){
                levelGrid[x][y].toRemove = true;
            }
        }
    }
}

void findSame(int x,int y, int colour){
    if(x<0 || x>8-(y%2) || y<0 || y>14) return;  // out of bounds
    if(levelGrid[x][y].colour != colour) return; // different colour
    if(levelGrid[x][y].toProcess == true) return; // already checked

    levelGrid[x][y].toProcess = true; // mark checked
    processCount++;
    
    for(int t=0; t<6; t++){
        if(y%2==0){
            findSame(x+evenlist[t][0], y+evenlist[t][1], colour);
        }else{
            findSame(x+oddlist[t][0], y+oddlist[t][1], colour);
        }
    }
    return;
}

void findRemain(int x,int y){
    using PD=Pokitto::Display;
    if(x<0 || x>8-(y%2) || y<0 || y>14) return;  // out of bounds
    if(levelGrid[x][y].colour == 0) return; // 0 = no ball/empty
    if(levelGrid[x][y].toProcess == true) return; // already checked

    levelGrid[x][y].toProcess = true; // mark checked
    levelGrid[x][y].toRemove = false; // mark checked

    for(int t=0; t<6; t++){
        if(y%2==0){
            findRemain(x+evenlist[t][0], y+evenlist[t][1]);
        }else{
            findRemain(x+oddlist[t][0], y+oddlist[t][1]);
        }
    }
    return;
}




/*
     ______                 __                 _______                              
    |   __ \.-----.-----.--|  |.-----.----.   |     __|.----.----.-----.-----.-----.
    |      <|  -__|     |  _  ||  -__|   _|   |__     ||  __|   _|  -__|  -__|     |
    |___|__||_____|__|__|_____||_____|__|     |_______||____|__| |_____|_____|__|__|
*/

uint32_t lineTime=0;

inline void myBGFiller2(std::uint8_t* line, std::uint32_t y, bool skip){

    // my Background filer, doesn't take into account scrolling or anything at all
    using PD=Pokitto::Display;

    if(skip){
        memset(&line[0],0,220);
        return;
    }


    // tile map
    #define bgTileSizeH 8
    #define bgTileSizeW 8
    #define tbt 64 // 8*8
    
    uint32_t x=0;
    uint32_t tileIndex = (y/bgTileSizeH) * levelMap[0];
    //uint16_t jStart = (y %bgTileSizeH) * bgTileSizeW; // current line in current tile
    uint32_t jStart = y <<29>>26;//ymod8[y]; // current line in current tile

    uint32_t tileStart;
    uint32_t lineOffset;

    #define bgTileLine2()\
        tileStart = levelMap[2+tileIndex++]*tbt;\
        lineOffset = tileStart + jStart;\
        line[x++] = bg_Tiles[lineOffset++];\
        line[x++] = bg_Tiles[lineOffset++];\
        line[x++] = bg_Tiles[lineOffset++];\
        line[x++] = bg_Tiles[lineOffset++];\
        line[x++] = bg_Tiles[lineOffset++];\
        line[x++] = bg_Tiles[lineOffset++];\
        line[x++] = bg_Tiles[lineOffset++];\
        line[x++] = bg_Tiles[lineOffset++];

    // half tile for the last column
    #define bgHalfTileLine2()\
        tileStart = levelMap[2+tileIndex++]*tbt;\
        lineOffset = tileStart + jStart;\
        line[x++] = bg_Tiles[lineOffset++];\
        line[x++] = bg_Tiles[lineOffset++];\
        line[x++] = bg_Tiles[lineOffset++];\
        line[x++] = bg_Tiles[lineOffset++];

    for(uint32_t c=0; c<25; c++){
        bgTileLine2();
    }
    
    bgTileLine2();
    bgTileLine2();
    bgHalfTileLine2();

}

inline void myBGFiller(std::uint8_t* line, std::uint32_t y, bool skip){

    // my Background filer, doesn't take into account scrolling or anything at all

    uint32_t tileStart;
    uint32_t lineOffset;
    uint32_t thisTile;

    uint32_t x=0;
    uint32_t tileIndex = 2+(y/bgTileSizeH) * levelMap[0];
    uint32_t jStart = y <<29>>26;//ymod8[y]; // current line in current tile


    #define bgTileLine()\
        thisTile = current_bg_map[tileIndex++];\
        tileStart = thisTile*tbt;\
        lineOffset = tileStart + jStart;\
        for(uint32_t b=0; b<bgTileSizeW; b++){\
            line[x++] = current_bg_Tiles[lineOffset++];\
        }
        
    bgTileLine(); bgTileLine(); bgTileLine(); bgTileLine();
    bgTileLine(); bgTileLine(); bgTileLine(); bgTileLine();
    bgTileLine(); bgTileLine(); bgTileLine(); bgTileLine();
    bgTileLine(); bgTileLine(); bgTileLine(); bgTileLine();
    bgTileLine(); bgTileLine(); bgTileLine(); bgTileLine();
    bgTileLine(); bgTileLine(); bgTileLine(); bgTileLine();
    bgTileLine(); bgTileLine(); bgTileLine(); bgTileLine();
    bgTileLine(); 

}

inline void wiggleFill(std::uint8_t* line, std::uint32_t y, bool skip){

    if(skip){
        memset(&line[0],0,220);
        return;
    }

    int t=wiggle[y+offsetAngle];
    if(t>=0){
        memcpy(&line[0], &line[t], 220-t);
        memset(&line[220-t], 255, t);
    }else{
        for(int p=220; p>-t; p--){
            line[p] = line[t+p];
        }
        memset(&line[0], 255, -t);
    }
    return;
}

inline void ballFiller(std::uint8_t* line, std::uint32_t y, bool skip){

    using PD=Pokitto::Display;
/*
    if(dirtMask[y/8]==0){
        //memset(&line[0],0,220);
        return;
    }
*/
    // TOP_SIDE is the top of the playfield, it moves down the screen
    uint8_t ts = y-TOP_SIDE-1;
    if(y < TOP_SIDE || y>140) return;

    // if we hit an empty line, stop rendering, there is nothing below
    uint8_t td = ts/12;
    uint8_t total = 0;
    for(uint32_t x=0; x<8; x++){ // -(td%2)
        total += levelGrid[x][td-1].colour;
    }
    if(total==0)return;

    // tile map
    #define myTileH 6
    #define myTileW 7
    #define myMapWidth 16
    uint32_t x=56;
    uint32_t tileY = ts/myTileH;
    uint32_t tileIndex = (tileY * myMapWidth);
    //uint8_t jStart = ((ts %myTileH) * myTileW) +2; // current line in current tile, first 2 bytes are w/h
    uint32_t jStart = ymod6_7[ts]+2; // current line in current tile, first 2 bytes are w/h
    uint32_t lineOffset;
    uint32_t pix1;

    #define tileLine()\
        lineOffset = jStart;\
        pix1 = ballMap[tileIndex++];\
        if(pix1){\
            pix1-=4;\
            if(bgballs2[pix1][lineOffset]) { line[x++] = bgballs2[pix1][lineOffset++]; } else { x++; lineOffset++; }\
            if(bgballs2[pix1][lineOffset]) { line[x++] = bgballs2[pix1][lineOffset++]; } else { x++; lineOffset++; }\
            if(bgballs2[pix1][lineOffset]) { line[x++] = bgballs2[pix1][lineOffset++]; } else { x++; lineOffset++; }\
            if(bgballs2[pix1][lineOffset]) { line[x++] = bgballs2[pix1][lineOffset++]; } else { x++; lineOffset++; }\
            if(bgballs2[pix1][lineOffset]) { line[x++] = bgballs2[pix1][lineOffset++]; } else { x++; lineOffset++; }\
            if(bgballs2[pix1][lineOffset]) { line[x++] = bgballs2[pix1][lineOffset++]; } else { x++; lineOffset++; }\
            if(bgballs2[pix1][lineOffset]) { line[x++] = bgballs2[pix1][lineOffset++]; } else { x++; lineOffset++; }\
        }else{\
            x+=7; lineOffset+=7;\
        }
/*
        pixelCopy(&line[x], &bgballs2[pix1][lineOffset], 7, 0);\
        x+=7; lineOffset+=7;
*/

    for(uint32_t c=0;c<16; c++){
        tileLine();
    }
    
    
    
}

inline void wipeFiller(std::uint8_t* line, std::uint32_t y, bool skip){
    int yy = (y-88)*(y-88);
    int dd = wipeDown*wipeDown/1.5;
    // wipeDown is the radius of the circle, it was initially going to be a horizontal wipe effect
    for(int x=-110; x<110; x++){
        if(x*x+yy < dd){
            line[110+x]=255; // 255 = black
        }
    }
    updateStream(); // put this here to prevent the music stuttering

}

void renderLevel(bool fullScreen = false){
    using PD=Pokitto::Display;
    /*
    // for wiggling the screen!
    offsetAngle +=1;
    if(offsetAngle>=360)offsetAngle=0;
    */
/*    
    fullScreen=false;
    if(fullScreen==true){
        clearMask();
    }else{
        fillMask();
    }
*/

    //if(frameCount % 64 == 0){fillMask();}
    myTilemap.draw(0, 0);


    // top of playfield
    PD::drawSprite(LEFT_SIDE, TOP_SIDE-7, topbar); // top bar
    PD::drawSprite(LEFT_SIDE, 140, bottombar); // top bar

    // arrow or cannon
    //PD::drawSprite((LEFT_SIDE+48)+arrowSprite.vx, 160+arrowSprite.vy, arrow_sprite);
    PD::drawSprite(arrowSprite.vx-20, arrowSprite.vy-13, arrow_spriteb);


    // ball animation sprites
    for(int num=0; num<MAXSPRITES; num++){
        if( mySprite[num].inUse == true ){
            markDirty(mySprite[num].y);
            if(mySprite[num].type==0){ // normal
                PD::drawSprite(mySprite[num].x, mySprite[num].y, balls[mySprite[num].frame]);
            }
            if(mySprite[num].type==1){ // falling
                PD::drawSprite(mySprite[num].x, mySprite[num].y, balls[mySprite[num].frame]);
                PD::drawSprite(mySprite[num].x, mySprite[num].y+1, balls[17]);
                markDirty(mySprite[num].y);
            }
            if(mySprite[num].type==2){ // walking
                int temp = mySprite[num].maskFrame/WALKDIVIDE;

                if(temp==2){
                    PD::drawSprite(mySprite[num].x, mySprite[num].y, balls[mySprite[num].frame]);
                    PD::drawSprite(mySprite[num].x, mySprite[num].y+1, balls[temp+12]);
                }else{
                    PD::drawSprite(mySprite[num].x, mySprite[num].y-1, balls[mySprite[num].frame]);
                    PD::drawSprite(mySprite[num].x, mySprite[num].y, balls[temp+12]);
                }
            }
        }
    }
/*
    // target
    for (int t=0; t<numtgt; t++){
        PD::drawSprite(tgt[t].x-HALFTILE, tgt[t].y-HALFTILE, balls[0]);
    }
*/



    // ball

    if(ball.vx>0){
        // jumping
        if(ball.moving){
            PD::drawSprite(ball.x-HALFTILE, ball.y-HALFTILE, balls[ball.colour+SPRITE_OFFSET]);
            PD::drawSprite(ball.x-HALFTILE, ball.y-HALFTILE+1, balls[16]);
            markDirty(ball.y-HALFTILE);
        }
    }else{
        if(ball.moving){
            PD::drawSprite(ball.x-HALFTILE, ball.y-HALFTILE, balls[ball.colour+SPRITE_OFFSET]);
            PD::drawSprite(ball.x-HALFTILE, ball.y-HALFTILE+1, balls[16],0,1);
            markDirty(ball.y-HALFTILE);
        }
    }


    if(moveQueue==true) queueOffset++;
    if(queueOffset==32){
        queueOffset=0;
        moveQueue=false;
        nxtBall();
    }
    PD::drawSprite(27, 156, doorb);
    for(int t=1; t<queueLength+1; t++){
        PD::drawSprite(((queueLength-t)*16)+(queueOffset/2)+6, 159, balls[nextBall[t]+SPRITE_OFFSET]);
        PD::drawSprite(((queueLength-t)*16)+(queueOffset/2)+6, 160, balls[((queueOffset/2)/4)+12],0,0);
    }
    PD::drawSprite(0, 156, door);


    // cannon
    PD::drawSprite(arrowSprite.vx-20, arrowSprite.vy-13, arrow_sprite);
    if(moveQueue==false){
        if(!ball.moving){
            // standing ready to jump
            PD::drawSprite(arrowSprite.vx-7, arrowSprite.vy-1, balls[ball.colour+SPRITE_OFFSET]);
            if(ball.vx>0){
                PD::drawSprite(ball.x-HALFTILE, ball.y-HALFTILE+1, balls[16]);
            }else{
                PD::drawSprite(ball.x-HALFTILE+1, ball.y-HALFTILE+1, balls[16],0,1);
            }
            markDirty(ball.y-HALFTILE);
        }
    }

    // front of cannon and stand
    //PD::drawSprite(arrowSprite.vx-7, arrowSprite.vy-1, balls[4]);
    PD::drawSprite(arrowSprite.vx-19, arrowSprite.vy, stand);


    markDirty(144+arrowSprite.vy);
    
    char tempText[10];    
    sprintf(tempText,"%05d",score);
    myPrint(176,0,"SCORE");
    myPrint(176,8,tempText);

    
    // mask out parts of the screen
    PD::setTASRowMask( screenMask );
/*    
    for(int t=0; t<22; t++){
        dirtMask[t]=0;
    }
*/
}


void titleScreen(){
    using PD=Pokitto::Display;
    if(frameCount % 32 == 0){fillMask();}

    myTilemap.draw(0, 0);

    //markDirty(122);

    if(!(frameCount%128&64)){
        myPrint(84,122,"PRESS A");
    }

    if(_A[NEW]){
        gameMode = playLevelMode;
        // switch to my own bg filler
        // it's about 10fps faster because it does less (i assume)
        //PD::lineFillers[0] = myBGFiller; // map layer
        newGame();
    }

    // mask out parts of the screen
    PD::setTASRowMask( screenMask );

}



void checkCollisions(){
    

    // Left/Right walls
    if( ball.x-ball.radius+1 <= LEFT_SIDE || ball.x+ball.radius+1 >= LEFT_SIDE+112){
        ball.vx = -ball.vx;
    }
    
    
    // top wall
    if(ball.vy+ball.y-ball.radius <= TOP_SIDE){
    
        int dist = 100;
        int tx=0,ty=0;
        // ball playfield
        for(int y=0; y<11; y++){
            for(int x=0; x<8-(y%2); x++){
                if(levelGrid[x][y].colour == 0){
                    int bx = (x*BALL_WIDTH)+(BALL_WIDTH/2*(y%2))+LEFT_SIDE-1;
                    int by = (y*BALL_HEIGHT)+TOP_SIDE-1;
                    int ax = ball.x+ball.vx-8;
                    int ay = ball.y+ball.vy-8;
                    if(abs(bx-ax)+abs(by-ay) <dist){
                        dist = abs(bx-ax)+abs(by-ay);
                        tx=x;
                        ty=y;
                    }
                }
            }
        }

        clearToProcess();
        levelGrid[tx][ty].colour = ball.colour;
        dirGrid[tx][ty] = ball.vx<0 ? 0:1;
        processCount = 0;
        findSame(tx,ty,ball.colour);
        if(processCount < 3){
            clearToProcess();
        }else{
            // 3 or more, so remove
            score+=processCount;
            animProcessed();
        }
        setRemove();
        for(int x=0; x<8; x++){
            findRemain(x,0);
        }
        animRemoved();
        newBall();

    }else{ // hit top barrier

        // other balls
        for(int y=0; y<11; y++){
            for(int x=0; x<8; x++){
                if(levelGrid[x][y].colour != 0){
                    int x2 = (x*BALL_WIDTH)+(BALL_WIDTH/2*(y%2))+LEFT_SIDE-1;
                    int y2 = (y*BALL_HEIGHT)+TOP_SIDE-1;

                    if(circleCollision(ball.x+ball.vx, ball.y+ball.vy, ball.radius-1, x2+8, y2+8, ball.radius-1)){
    
                        int dist = 100;
                        int tx,ty;
                        // ball playfield
                        for(int y=0; y<11; y++){
                            for(int x=0; x<8-(y%2); x++){
                                if(levelGrid[x][y].colour == 0){
                                    int bx = (x*BALL_WIDTH)+(BALL_WIDTH/2*(y%2))+LEFT_SIDE-1;
                                    int by = (y*BALL_HEIGHT)+TOP_SIDE-1;
                                    int ax = ball.x+ball.vx-8;
                                    int ay = ball.y+ball.vy-8;
                                    if(abs(bx-ax)+abs(by-ay) <dist){
                                        dist = abs(bx-ax)+abs(by-ay);
                                        tx=x;
                                        ty=y;
                                    }
                                }
                            }
                        }
    
                        int bx = (tx*BALL_WIDTH)+(BALL_WIDTH/2*(ty%2))+LEFT_SIDE-1;
                        int by = (ty*BALL_HEIGHT)+TOP_SIDE-1;
                        
                        clearToProcess();
                        levelGrid[tx][ty].colour = ball.colour;
                        dirGrid[tx][ty] = ball.vx<0 ? 0:1;
                        
                        //stampSprite((bx/BALL_WIDTH)*BALL_WIDTH,((by/BALL_HEIGHT)*BALL_HEIGHT)-TOP_SIDE,balls[ball.colour+SPRITE_OFFSET]);
    
                        processCount = 0;
                        
                        findSame(tx,ty,ball.colour);
                        if(processCount < 3){
                            clearToProcess();
                        }else{
                            // 3 or more, so remove
                            score+=processCount;
                            animProcessed();
                        }
                        setRemove();
                        for(int x=0; x<8; x++){
                            findRemain(x,0);
                        }
                        animRemoved();
                        newBall();
                    }
                }
            }
        }

    } // hit other balls

}


void updateBallMap(){

    memset(&ballMap[0],0,16*29);
    for(int y=0; y<11; y++){
        for(int x=0; x<8; x++){
            // as we check through the level grid, might as well update the mini map for the ball tiles
            int x2 = x*2;
            int y2 = y*2;
            if(levelGrid[x][y].colour!=0){
                if(dirGrid[x][y]){
                    ballMap[y%2+    x2+16*y2] =    (levelGrid[x][y].colour+2)*2;    // top left
                    ballMap[y%2+  1+x2+16*y2] =    (levelGrid[x][y].colour+2)*2+1;  // top right
                    ballMap[y%2+ 16+x2+16*y2] = 18+(levelGrid[x][y].colour+2)*2;  // bottom left
                    ballMap[y%2+ 17+x2+16*y2] = 18+(levelGrid[x][y].colour+2)*2+1;  // bottom right
                }else{
                    ballMap[y%2+    x2+16*y2] = 72+((levelGrid[x][y].colour+2)*2);    // top left
                    ballMap[y%2+  1+x2+16*y2] = 72+((levelGrid[x][y].colour+2)*2+1);  // top right
                    ballMap[y%2+ 16+x2+16*y2] = 90+((levelGrid[x][y].colour+2)*2);  // bottom left
                    ballMap[y%2+ 17+x2+16*y2] = 90+((levelGrid[x][y].colour+2)*2+1);  // bottom right
                }
            }
        }
    }

#define FIRSTBALL 7
#define LASTBALL 40
#define NUMBALLS 36
#define SECONDBALL 16*4
#define SECONDLASTBALL SECONDBALL+(13*4)

    // remap the balls
    for(int y1 = 0; y1<26; y1+=2){
        for(int x1=0; x1<16; x1++){
            // is current tile empty
            if(ballMap[x1+16*y1] <= 3){
                // is tile above ball?
                if(y1>1){
                    int theBall = ballMap[x1+16*(y1-1)];
                    if((theBall > FIRSTBALL && theBall < LASTBALL) || (theBall >= SECONDBALL && theBall < SECONDLASTBALL)){
                        if(tileMask[x1+16*y1] == 0) ballMap[x1+16*y1] = LASTBALL;
                        if(tileMask[x1+16*y1] == 1) ballMap[x1+16*y1] = LASTBALL+1;
                    }
                }
                // is below ball?
                int theBall = ballMap[x1+16*(y1+2)];
                if((theBall > FIRSTBALL && theBall < LASTBALL) || (theBall >= SECONDBALL && theBall < SECONDLASTBALL)){
                    if(tileMask[x1+16*(y1+1)] == 2) ballMap[x1+16*(y1+1)] = LASTBALL+18;
                    if(tileMask[x1+16*(y1+1)] == 3) ballMap[x1+16*(y1+1)] = LASTBALL+19;
                }

            }else if(ballMap[x1+16*y1] < LASTBALL || (ballMap[x1+16*y1] >= SECONDBALL && ballMap[x1+16*y1] < SECONDLASTBALL)){
                // is below ball?
                if(ballMap[x1+16*(y1+2)] > FIRSTBALL){
                    ballMap[x1+16*(y1+1)] += NUMBALLS;
                }
                // is above ball?
                int theBall = ballMap[x1+16*(y1-1)];
                if(y1>1 && theBall > FIRSTBALL && (theBall < LASTBALL || theBall > 64)){
                    ballMap[x1+16*y1] += NUMBALLS;
                }
            }
        }
    }
    
}

void playLevel(){
    //memset(&ballMap[0],0,16*29);
    
    if(!ball.moving && totalBalls >0){
        float oldAngle= ball.angle;
        //if(_C[NEW])newGame();
        if(_Up[HELD]){
            if(angle > 90)angle-=MOVEANGLE;
            if(angle < 90)angle+=MOVEANGLE;
            //if(TOP_SIDE > 8)TOP_SIDE--;
        }
        if(_Down[HELD]){
            //if(angle > 90)angle+=MOVEANGLE;
            //if(angle < 90)angle-=MOVEANGLE;
            //TOP_SIDE++;
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
            //numtgt=0;
            rotateSprite(ball.angle);
            //arrowSprite.vx = arrowSprite.distance * cos(degToRad(ball.angle));
            //arrowSprite.vy = arrowSprite.distance * -1*sin(degToRad(ball.angle));
            frameSkip=4;
        }
        if(_A[NEW]){
            cannonFrame=1;
            //moveQueue = true;
            //ball.moving = true;
        }
    }
    
    //if(_B[NEW]){usingTarget=!usingTarget;}
    
    for(int p=0; p<ball.moveRepeat; p++){
        if(ball.moving){
            ball.x += ball.vx;  
            ball.y += ball.vy;
            checkCollisions();
        }
    }    

    updateBallMap();


/*
    if(usingTarget){
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
            if( target.x-ball.radius+1 <= LEFT_SIDE || target.x+ball.radius+1 >= LEFT_SIDE+112){
                target.vx = -target.vx;
            }
    
            // top wall
            if(target.y-ball.radius <= TOP_SIDE){
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
    } // using target
*/

    // animate the sprites
    for(int num=0; num<MAXSPRITES; num++){
        if( mySprite[num].inUse == true ){
            // not falling?
            if(mySprite[num].type==0){
                if(mySprite[num].type==0){
                    mySprite[num].frameCounter++;
                    if(mySprite[num].frameCounter == mySprite[num].frameSpeed){
                        mySprite[num].frameCounter=0;
                        mySprite[num].frame++;
                        if(mySprite[num].frame==mySprite[num].lastFrame){
                            mySprite[num].inUse = false;
                        }
                    }
                }
            }

            // if falling down
            if(mySprite[num].type==1){
                if(mySprite[num].speed < 8)mySprite[num].speed += .5;
                mySprite[num].y += mySprite[num].speed;
                if(mySprite[num].y >= 160){
                    mySprite[num].y = 160;
                    mySprite[num].type=2;
                    mySprite[num].maskFrame = random(WALKFRAMES);
                }
            }

            // if walking along the ground
            if(mySprite[num].type==2){
                if(++mySprite[num].maskFrame==WALKFRAMES)mySprite[num].maskFrame=0;
                mySprite[num].x +=1;
                if(mySprite[num].x >= 220){
                    mySprite[num].type=3;
                    mySprite[num].inUse = false;
                }
            }

        }
    }

    if( levelTime <= Pokitto::Core::getTime()){
        levelTime = Pokitto::Core::getTime()+5000;
        TOP_SIDE++;
//        markDirty(TOP_SIDE);
    }

    if(cannonFrame!=0 && frameCount%5==0){
        cannonFrame++;
        rotateSprite(ball.angle);

        if(cannonFrame==4){
            cannonFrame=0;
            rotateSprite(ball.angle);
            ball.moving = true;
        }
    }

    //if(++frameSkip>=4){
    //    frameSkip = 0;
//        renderLevel(true);
    //    Pokitto::Core::update();
    //}
    
    
    
    
    totalBalls = 0;
    for(uint32_t y=0; y<11; y++){
        for(uint32_t x=0; x<8; x++){ // -(td%2)
            if(levelGrid[x][y].colour) totalBalls++;
        }
    }
    //if(total==0)exit; // level complete
    
    if(wipeDown > 0){
        Pokitto::Display::lineFillers[3] = wipeFiller; // overlay effects
    }else{
        Pokitto::Display::lineFillers[3] = NULL; // overlay effects
    }

    
    if(totalBalls==0){
        if(wipeDown==176){
            levelNumber++;
            loadLevel(&levelMap[0]);
            levelTime = Pokitto::Core::getTime()+1000;
            current_bg_map = levelMap;
        }

        if(wipeDown<=176){
            wipeDown+=2;
        }
    }else{
        if(wipeDown>0){
            wipeDown-=2;
        }
    }
}


int main(){
    using PC=Pokitto::Core;
    using PD=Pokitto::Display;
    using PB=Pokitto::Buttons;

    PC::begin();
    PD::load565Palette(balls_pal);
    PD::invisiblecolor = 0;
    PD::persistence = true;
    PD::adjustCharStep = 0;
    PD::adjustLineStep = 0;
    
    pokInitSD();
    
    myPin = 0;

    generateAngleTable();


    setFPS(60);

    angle = ball.angle;
    frameCount=0;

    // create tile map
    myTilemap.set(myMap[0], myMap[1], myMap+2);
    for(int i=0; i<sizeof(bg_Tiles)/(POK_TILE_W*POK_TILE_H); i++)
        myTilemap.setTile(i, POK_TILE_W, POK_TILE_H, bg_Tiles+i*POK_TILE_W*POK_TILE_H);


    loadLevel(&titleMap[0]);
//    loadLevel(&levelMap[0]);
//      myPrint(0,0,"Hello World 12345!");


    rotateSprite(ball.angle);
    //arrowSprite.vx = arrowSprite.distance * cos(degToRad(ball.angle));
    //arrowSprite.vy = arrowSprite.distance * -1*sin(degToRad(ball.angle));

    arrowSprite.vx = 110;//arrowSprite.distance * cos(degToRad(ball.angle));
    arrowSprite.vy = 155;//arrowSprite.distance * -1*sin(degToRad(ball.angle));
    //newBall();


    // line filler test
    // 0 = tile layer
    // 1 = sprite layer
    // 2 = next layer
//    PD::lineFillers[3] = wipeFiller; // overlay effects
    PD::lineFillers[2] = PD::lineFillers[1]; // move sprite layer to front
    PD::lineFillers[1] = ballFiller; // replace mid-layer with my stuff
    PD::lineFillers[0] = myBGFiller; // map layer
//    PD::lineFillers[0] = NULL;//TAS::BGTileFiller; // original TAS bg filler
    //PD::lineFillers[3] = wiggleFill; // replace mid-layer with my stuff


   //Pokitto::Sound::playMusicStream("music/joegoespop/yummy.raw",0);

/*    
    char tempText[20];
    for(int y=0; y<180; y+=5){
        rotateSprite((float)y);
        sprintf(tempText,"/sd/_dump/%03d.bmp",y);
        saveBMP(tempText,arrow_sprite);
    }
*/

    //startSong("yummy.raw");
    bool nextFrame = 0;
    bool logicFrame=0;


//    enableDAC();

    playRandomTune();


    while( PC::isRunning() ){
        
        updateStream();

        if( !PC::update() ) 
            continue;

        //updateButtons();
        //PC::sound.updateStream();
  
        switch(gameMode){
            case 0:
                updateButtons();
                titleScreen();
                break;
            case 1:
                if(logicFrame==1){
                    logicFrame=0;
                    updateButtons();
                    playLevel();
                }
                renderLevel(true);
                break;
        }

        
        char tempText[10];
        sprintf(tempText,"FPS:%d",fpsCount);
        myPrint(0,0,tempText);
        
        
        
        
        sprintf(tempText,"S:%03d,%03d,%03d,%03d,%03d",audioBuffer[0],audioBuffer[1],audioBuffer[2],audioBuffer[3],audioBuffer[4]);
        myPrint(0,8,tempText);

/*
        // fps delay and display frame
        while(PC::getTime()-tempTime < myDelay){
        }
        tempTime = PC::getTime();
*/

        if(PC::getTime()-tempTime > myDelay){
            logicFrame=1;
            tempTime = PC::getTime();
        }

        frameCount++;

        fpsCounter++;
        if(PC::getTime() >= lastMillis+1000){
            lastMillis = PC::getTime();
            fpsCount = fpsCounter;
            fpsCounter = 0;
        }


    }
    
    return 0;
}

/*
-------
 Notes
-------
https://en.wikipedia.org/wiki/Puzzle_Bobble
Scoring - 10 points per popped bubble
falling bubbles = double per bubble starting at 20



*/