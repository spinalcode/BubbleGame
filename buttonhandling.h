// 
//  I like to use this button handling routine, it tells me if
//  a button had just been pressed, held or released
// 
//------------------------[ Button handling, very accurate ]------------------------
#define HELD 0
#define NEW 1
#define RELEASED 2
uint8_t CompletePad, ExPad, TempPad, myPad;
bool _A[3], _B[3], _C[3], _Up[3], _Down[3], _Left[3], _Right[3];

void UPDATEPAD(int pad, int var) {
        _C[pad] =      (var)&1;
        _B[pad] = (var >> 1)&1;
        _A[pad] = (var >> 2)&1;
     _Down[pad] = (var >> 3)&1;
     _Left[pad] = (var >> 4)&1;
    _Right[pad] = (var >> 5)&1;
       _Up[pad] = (var >> 6)&1;
}

void UpdatePad(int joy_code){
    ExPad = CompletePad;
    CompletePad = joy_code;
    UPDATEPAD(HELD, CompletePad); // held
    UPDATEPAD(RELEASED, (ExPad & (~CompletePad))); // released
    UPDATEPAD(NEW, (CompletePad & (~ExPad))); // newpress
}

uint8_t updateButtons(){
    Pokitto::Buttons::update(); // update the current button states
    uint8_t var = 0;
    if (    Pokitto::Buttons::cBtn()) var |= 1;
    if (    Pokitto::Buttons::bBtn()) var |= (1<<1);
    if (    Pokitto::Buttons::aBtn()) var |= (1<<2);
    if ( Pokitto::Buttons::downBtn()) var |= (1<<3);
    if ( Pokitto::Buttons::leftBtn()) var |= (1<<4);
    if (Pokitto::Buttons::rightBtn()) var |= (1<<5);
    if (   Pokitto::Buttons::upBtn()) var |= (1<<6);
    UpdatePad(var);
    return var;
}
