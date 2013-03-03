#pragma once

#include "ofMain.h"
#include "ofxTuio.h"
#include "stroke.h"
#include "ofxControlPanel.h"
#include "spraycan.h"

class PaintOut : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void	tuioAdded(ofxTuioCursor & tuioCursor);
	void	tuioRemoved(ofxTuioCursor & tuioCursor);
	void	tuioUpdated(ofxTuioCursor & tuioCursor);
    void    h2rgb(float H, int& R, int& G, int& B);
    
    ofxControlPanel  panel;
    ofxXmlSettings XML;
    ofTrueTypeFont TTF;
    ofxTuioClient   tuioClient;
    
    vector<spraycan> cans;
    
    
    
    
    //Arduino
    void setupArduino(const int & version);
    void digitalPinChanged(const int & pinNum);
    void analogPinChanged(const int & pinNum);
    void updateArduino();
    
    
    
    //drawing stuff
    ofImage	brushMain[10];
    vector<stroke>		myStrokes;
    int					currentStroke;
    bool				newStroke;
    bool				bDrawing;
    bool                bButtonError;
    
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

