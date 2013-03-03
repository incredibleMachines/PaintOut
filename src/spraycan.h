//
//  spraycan.h
//  PaintOut
//
//  Created by chris on 3/2/13.
//
//

#ifndef PaintOut_spraycan_h
#define PaintOut_spraycan_h

#include "ofMain.h"
#include "stroke.h"
class spraycan{
public:
//Arduino
void initCan(string usbport, int baudrate);
void setupArduino(const int & version);
void digitalPinChanged(const int & pinNum);
void analogPinChanged(const int & pinNum);
void updateArduino();
void draw();
void update(int size, int amount);
void canPressed(int x, int y, int blobID);
void canDragged(int x, int y, int blobID);
void canReleased(int blobID);
void h2rgb(float H, int& R, int& G, int& B);
void remove();

    
    

//drawing stuff
ofImage	brush[10];
vector<stroke>		strokes;
int					currentStroke;
bool				newStroke;
bool				bDrawing;
bool                bButtonError;
bool                checkRemove;
int                 canBlobID;
bool                blobLost;

int red;
int green;
int blue;

//arduino stuff
ofArduino	ard;
bool		bSetupArduino;			// flag variable for setting up arduino once
string buttonState;
string potValue;
int sharp;
int blurNum;
bool canButton;
bool capButton;
int buttonTime;
    
};


#endif
