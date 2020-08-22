#define TIMER_32_0_IRQn 18          // for Timer setup
#include "timer_11u6x.h"
#include "clock_11u6x.h"

const char* folderName = "/music/joegoespop/";

const char* songnames[] = {
"yummy.raw",
};

File musicFile;    // Handle of open audio file
File sfxFile;    // Handle of open audio file

bool playBGM = true;
bool playSFX = true;
int LISTENINGSAMPLERATE = 22050;

void playRandomTune();

int currentSongNumber = 0;


// enableDAC() from Pokitto MiniLib
inline void enableDAC() {
    volatile unsigned int *PIO1 = (volatile unsigned int *) 0x40044060;
    volatile unsigned int *PIO2 = (volatile unsigned int *) 0x400440F0;
    volatile unsigned int *DIR1 = (volatile unsigned int *) 0xA0002004;
    volatile unsigned int *DIR2 = (volatile unsigned int *) 0xA0002008;
    PIO1[28] = PIO1[29] = PIO1[30] = PIO1[31] = 1<<7;
    PIO2[20] = PIO2[21] = PIO2[22] = PIO2[23] = 1<<7;
    *DIR1 |= (1<<28) | (1<<29) | (1<<30) | (1<<31);
    *DIR2 |= (1<<20) | (1<<21) | (1<<22) | (1<<23);

    // DIR1 is already declared above
    volatile unsigned int* SET1 = (unsigned int*)(0xa0002204);
    *DIR1 |= 1 << 17;
    *SET1 = 1 << 17;
}

// writeDAC() from Pokitto MiniLib
inline void writeDAC(unsigned char out) {
    volatile unsigned char* P1 = (unsigned char*)(0xa0000020);
    volatile unsigned char* P2 = (unsigned char*)(0xa0000040);
    P1[28] = out & 1; out >>= 1;
    P1[29] = out & 1; out >>= 1;
    P1[30] = out & 1; out >>= 1;
    P1[31] = out & 1; out >>= 1;
    P2[20] = out & 1; out >>= 1;
    P2[21] = out & 1; out >>= 1;
    P2[22] = out & 1; out >>= 1;
    P2[23] = out;
}

typedef struct{
    bool playSample;
    int soundPoint;
    const uint8_t *currentSound;
    uint32_t currentSoundSize;
    int volume;
    int speed;
    int repeat;
}sampletype;

sampletype snd[4]; // up to 4 sounds at once?

#define audioBufferSize 512
unsigned char *audioBuffer = (unsigned char *) 0x20000000;
int bufferOffset[4]={ audioBufferSize*3,  0, audioBufferSize, audioBufferSize*2 };

uint8_t currentBuffer = 0;
uint8_t completeBuffer = 0;         // Which section is full
long int audioOffset=0;

// messing with sound
bool playingMusic1 = true;
bool oldPlayingMusic1 = true;
int globalVolume = 16;


// new playsound function
uint8_t playSound(int channel, const unsigned char *sound, int volume = 255, int speed=255, int offset=0){
    
    if(playSFX == false) return 0;
    
    int dataOffset = 14 + offset;

    // get sound length
    uint32_t soundSize = 0;
    for(int t=0; t<4; t++){
        soundSize <<= 8;
        soundSize |= sound[t] & 0xFF;
    }

    int sampleRate = 0;
    for(int t=0; t<2; t++){
        sampleRate <<= 8;
        sampleRate |= sound[t+4] & 0xFF;
    }

    // get repeat start
    int repeatStart = 0;
    for(int t=0; t<4; t++){
        repeatStart <<= 8;
        repeatStart |= sound[t+6] & 0xFF;
    }

    // get repeat end
    int repeatEnd = 0;
    for(int t=0; t<4; t++){
        repeatEnd <<= 8;
        repeatEnd |= sound[t+10] & 0xFF;
    }

    // cheat, if there is a looping sample, make the size the length of the loop
    if(repeatEnd != 0){
        soundSize = repeatEnd;
        // also cheat for start loop
        if(repeatStart == 0){
           repeatStart = 1;
        }
    }

    // play sound at correct speed, no matter what the current playback rate is.
    float spd = ((float)LISTENINGSAMPLERATE / (float)sampleRate);

    snd[channel].currentSound = &sound[dataOffset];          // sound to play
    snd[channel].volume = volume;               // volume
    snd[channel].speed = (speed/spd);           // recalculated above
    snd[channel].currentSoundSize = soundSize;  // length of sound array
    snd[channel].soundPoint = 0;                // where the current sound is upto
    snd[channel].repeat = repeatStart * 255 / snd[channel].speed;          // repeat point
    snd[channel].playSample = 1;                // trigger sample playing

    return channel;
}


inline uint8_t myMixSound()
{
/*
    uint8_t divide = 0;
    signed int number=0;
    
    if(playBGM == true && playingMusic1 == true) {number = (audioBuffer[audioOffset]-128); divide++;}
    int currentPos;
    // check the 2 snd channels for playing audio
    for(uint8_t s=0; s<2; s++){
        if(snd[s].playSample == 1){
            snd[s].soundPoint++;
            currentPos = ((snd[s].soundPoint*snd[s].speed)>>8);
            if( currentPos >= snd[s].currentSoundSize){
                if(snd[s].repeat >0){
                    snd[s].soundPoint = snd[s].repeat;
                }else{
                    snd[s].playSample=0;
                    snd[s].soundPoint=0;
                }
            }
            divide++;
            number += (snd[s].currentSound[currentPos]-128);
        }
    }

    if(number>0 && number <1)number=1;
    if(number<=0 && number >-1)number=-1;

    if(divide == 0){ // divide is how many samples have been added together
        number=0;
    }else{
        number *= Pokitto::Core::sound.getVolume();
        //number = number >> (divide+7);
        // be double sure of keeping sign bit, thanks @FManga
        //number = int32_t(number + (uint32_t(number)>>31)) >> (divide+7);
        
        // apparently dividing the result by the sqrt of the number of samples is a good way to mix...
        float sqt[]={0.0,1.0,1.41421356237,1.73205080757,2.0};
        number = number/sqt[divide];
        number = number >> 8;
        if(number <-127)number=-127;
        if(number >127)number=127;
        
    }
    
    return (uint8_t)(number+128)&255;
*/

    return audioBuffer[audioOffset];
}

void clearAudioBuffer(){
    for(int t=0; t<audioBufferSize*4; t++){
        audioBuffer[t] = 127;
    }
}

inline void updateStream(){
    // Update music playing
    if( currentBuffer != completeBuffer){
        completeBuffer = currentBuffer;
        if(playingMusic1){
            if(!musicFile.read(&audioBuffer[bufferOffset[completeBuffer]], audioBufferSize)){
                clearAudioBuffer();
                playingMusic1 = false;
                playRandomTune();
            }
        }
    }
}

inline void audioTimer(void){
	if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 1)) {
        writeDAC( myMixSound() );
        if(++audioOffset == audioBufferSize*4){
            audioOffset = 0;
        }
        currentBuffer = audioOffset/audioBufferSize;
 	    Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
    }
}


// timer init stolen directly from Pokittolib
void initTimer(uint32_t sampleRate){
    /* Initialize 32-bit timer 0 clock */
	Chip_TIMER_Init(LPC_TIMER32_0);
    /* Timer rate is system clock rate */
	uint32_t timerFreq = Chip_Clock_GetSystemClockRate();
	/* Timer setup for match and interrupt at TICKRATE_HZ */
	Chip_TIMER_Reset(LPC_TIMER32_0);
	/* Enable both timers to generate interrupts when time matches */
	Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 1);
    /* Setup 32-bit timer's duration (32-bit match time) */
	Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, (timerFreq / sampleRate));
	/* Setup both timers to restart when match occurs */
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_0, 1);
	/* Start both timers */
	Chip_TIMER_Enable(LPC_TIMER32_0);
	/* Clear both timers of any pending interrupts */
	NVIC_ClearPendingIRQ((IRQn_Type)TIMER_32_0_IRQn);
    /* Redirect IRQ vector - Jonne*/
    NVIC_SetVector((IRQn_Type)TIMER_32_0_IRQn, (uint32_t)&audioTimer);
	/* Enable both timer interrupts */
	NVIC_EnableIRQ((IRQn_Type)TIMER_32_0_IRQn);
}


void startSong(const char* filename){

    if(musicFile.openRO(filename)){
        playingMusic1 = true;
        clearAudioBuffer();
        initTimer(LISTENINGSAMPLERATE);
        printf("%s found!\r\n",filename);
    }else{
        playingMusic1 = false;
        clearAudioBuffer();
        printf("%s not found!\r\n",filename);
    }
}


void playRandomTune(){
    char fullSongName[256];
    
    currentSongNumber = random( (sizeof(songnames)/sizeof(songnames[0])) );
    snprintf(fullSongName,sizeof(fullSongName), "%s%s", folderName, songnames[currentSongNumber]);
    //printf("%s%s\n", folderName, songnames[currentSongNumber]);
    startSong(fullSongName);
}


