//
//  spraycan.cpp
//  PaintOut
//
//  Created by chris on 3/2/13.
//
//

#include "spraycan.h"

void spraycan::initCan(string usbport, int baudrate){
    ard.connect(usbport, baudrate);
    ofAddListener(ard.EInitialized, this, &spraycan::setupArduino);
    bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
    bDrawing = false;
	newStroke = true;
    bButtonError = false;
	currentStroke=0;

}

void spraycan::setupArduino(const int & version){
    // remove listener because we don't need it anymore
	ofRemoveListener(ard.EInitialized, this, &spraycan::setupArduino);
    
    // it is now safe to send commands to the Arduino
    bSetupArduino = true;
    
    // print firmware name and version to the console
    cout << ard.getFirmwareName() << endl;
    cout << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion() << endl;
    
	//ard.sendAnalogPinReporting(1, ARD_ANALOG);	//
	ard.sendAnalogPinReporting(2, ARD_ANALOG);	//POT VALUE
	ard.sendAnalogPinReporting(4, ARD_ANALOG);  //SHARP VALUE
	ard.sendDigitalPinMode(8, ARD_INPUT);  //Cap Button
	ard.sendDigitalPinMode(10, ARD_OUTPUT); //IR LED
	ard.sendDigitalPinMode(2, ARD_INPUT); // Can Button
	ard.sendDigitalPinMode(3, ARD_PWM);
	ard.sendDigitalPinMode(5, ARD_PWM);
	ard.sendDigitalPinMode(6, ARD_PWM);
    // Listen for changes on the digital and analog pins
    ofAddListener(ard.EDigitalPinChanged, this, &spraycan::digitalPinChanged);
    ofAddListener(ard.EAnalogPinChanged, this, &spraycan::analogPinChanged);
	//ard.sendDigitalPinMode(6, ARD_OUTPUT);
	
	//ard.sendAnalogPinReporting(5, ARD_ANALOG);	// get pot data

}
//--------------------------------------------------------------
void spraycan::digitalPinChanged(const int & pinNum) {
    // do something with the digital input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    buttonState = "digital pin: " + ofToString(pinNum) + " = " + ofToString(ard.getDigital(pinNum));
    cout << buttonState << endl;
}

// analog pin event handler, called whenever an analog pin value has changed

//--------------------------------------------------------------
void spraycan::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    potValue = "analog pin: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
}

//--------------------------------------------------------------
void spraycan::updateArduino(){
    ard.update();
	if (bSetupArduino) {
		int r,g,b=0;
		float h = ((float)ard.getAnalog(2))/1024;
		int h_int = (int) 360*h;
		h2rgb(h,r,g,b);
		//ard.sendDigital(10, ARD_HIGH);
		//cout << ard.getDigital(8) << endl;
		if(ard.getDigital(8) == 1){
			ard.sendDigital(10, ARD_HIGH);
			capButton=true;
		}else {
			ard.sendDigital(10, ARD_LOW);
			capButton=false;
		}
		ard.sendDigital(2, ARD_HIGH);
		//cout << ard.getDigital(2) << endl;
		if(ard.getDigital(2) == 0 && canButton == false){
			buttonTime = ofGetElapsedTimeMillis();
			canButton =true;
            
            if(strokes.size()>0 && !bDrawing){
                
                //undo
                strokes[strokes.size()-1].clearStroke();
                strokes.pop_back();
                currentStroke--;
                cout << "[CAN BUTTON]" << endl;
            }
            
        }
        else if (ard.getDigital(2) == 1) {
			canButton=false;
            if(bButtonError)bButtonError=false;
		}
        
		if (canButton == true && (ofGetElapsedTimeMillis()-buttonTime)>= 2000) {
			//changeSound=false;
            
            ard.sendPwm(3, 255);
            ard.sendPwm(5, 255);
            ard.sendPwm(6, 255);
            
            bButtonError = true;
            
            if(strokes.size()>0 && !bDrawing){
                for(int i=0; i<strokes.size(); i++)
                {
                    strokes[i].clearStroke();
                    strokes.clear();
                    currentStroke = 0;
                }
                cout << "[CAN BUTTON] CLEAR ALL" <<endl;
            }
		}
        
        
        if(!bButtonError){
            //synthType = int(ofMap(ard.getAnalog(2), 1, 5, 0, 1023));
            ard.sendPwm(3, ofMap(red, 0, 255, 255, 0));
            ard.sendPwm(5, ofMap(green, 0, 255, 255, 0));
            ard.sendPwm(6, ofMap(blue, 0, 255, 255, 0));
            sharp = ard.getAnalog(4);
            blurNum = ofMap(sharp, 300, 730, 9, 0);
//            panel.setValueI("VAL_R", red);
//            panel.setValueI("VAL_G", green);
//            panel.setValueI("VAL_B", blue);
//            panel.setValueI("VAL_BLUR", blurNum);
//            panel.setValueI("VAL_SHARP", sharp);
//            panel.setValueB("VAL_BUTT", canButton);
//            panel.setValueB("VAL_CAP", capButton);
        }
	}


}
void spraycan::update(int size, int amount){
    updateArduino();
    for(int i=0; i<strokes.size(); i++)
	{
		strokes[i].strokeAmount = amount;
		strokes[i].size = size;
	}

    
}
void spraycan::draw(){
    for(int i=0; i<strokes.size(); i++)
	{
		strokes[i].draw();
	}
}
void spraycan::canPressed(int x, int y, int blobID){
    bDrawing=true;
    stroke s;
	s.brush = brush;
	strokes.push_back( s );
	strokes[currentStroke].addPoint(x,y);
	strokes[currentStroke].addColor(red,green,blue,blurNum);
    strokes[currentStroke].addPoint(x,y);
	strokes[currentStroke].addColor(red,green,blue,blurNum);
	newStroke = false;
    canBlobID = blobID;
    blobLost = false;
}
void spraycan::canDragged(int x, int y, int blobID){
    strokes[currentStroke].addPoint(x,y);
	strokes[currentStroke].addColor(red,green,blue,blurNum);
}
void spraycan::canReleased(int blobID){
    
    //if blobLost and !canButton then new stroke
    if(!canButton){
        bDrawing = false;
        newStroke = true;
        //myStrokes[currentStroke].smooth(0.1f);		// smoothing for angle calculation.
        
        currentStroke++;
    }else{
        blobLost;
    }
    

}






void spraycan::h2rgb(float H, int& R, int& G, int& B) {
	int var_i;
	float S=1, V=1, var_1, var_2, var_3, var_h, var_r, var_g, var_b;
	
	if ( S == 0 )                       //HSV values = 0 ÷ 1
	{
		R = V * 255;
		G = V * 255;
		B = V * 255;
	}
	else
	{
		var_h = H * 6;
		if ( var_h == 6 ) var_h = 0;      //H must be < 1
		var_i = int( var_h ) ;            //Or ... var_i = floor( var_h )
		var_1 = V * ( 1 - S );
		var_2 = V * ( 1 - S * ( var_h - var_i ) );
		var_3 = V * ( 1 - S * ( 1 - ( var_h - var_i ) ) );
		
		if      ( var_i == 0 ) {
			var_r = V     ;
			var_g = var_3 ;
			var_b = var_1 ;
		}
		else if ( var_i == 1 ) {
			var_r = var_2 ;
			var_g = V     ;
			var_b = var_1 ;
		}
		else if ( var_i == 2 ) {
			var_r = var_1 ;
			var_g = V     ;
			var_b = var_3 ;
		}
		else if ( var_i == 3 ) {
			var_r = var_1 ;
			var_g = var_2 ;
			var_b = V     ;
		}
		else if ( var_i == 4 ) {
			var_r = var_3 ;
			var_g = var_1 ;
			var_b = V     ;
		}
		else                   {
			var_r = V     ;
			var_g = var_1 ;
			var_b = var_2 ;
		}
		
		green = (1-var_r) * 255;                  //RGB results = 0 ÷ 255
		red = (1-var_g) * 255;
		blue = (1-var_b) * 255;
		//analogWrite(3, R);
        //		analogWrite(5, G);
        //		analogWrite(6, B);
	}
}