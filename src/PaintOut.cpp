#include "PaintOut.h"

//--------------------------------------------------------------
void PaintOut::setup(){
    ofEnableSmoothing();
    
    tuioClient.start(3333);
    
    ofAddListener(tuioClient.cursorAdded,this,&PaintOut::tuioAdded);
	ofAddListener(tuioClient.cursorRemoved,this,&PaintOut::tuioRemoved);
	ofAddListener(tuioClient.cursorUpdated,this,&PaintOut::tuioUpdated);
    
    //create the socket and bind to port 11999
	udpConnection.Create();
	udpConnection.Bind(11999);
	udpConnection.SetNonBlocking(true);
    
//    ard.connect("/dev/tty.usbserial-A600eztS", 57600);
//	// listen for EInitialized notification. this indicates that
//    // the arduino is ready to receive commands and it is safe to
//    // call setupArduino()
//    ofAddListener(ard.EInitialized, this, &PaintOut::setupArduino);
//	bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
    for (int i = 0; i < 10; i++){
		//int b = i+1;
        brushMain[i].loadImage("brush-0"+ ofToString(i+1) + ".png");
		//brushMain.push_back(temp);
	}
    
    ofBackground(255,255,255);
	bDrawing = false;
	newStroke = true;
    bButtonError = false;
	currentStroke=0;
	ofSetFrameRate(60);
    
    
    //SETUP PANEL
    panel.setup("control", 1030, 0, 250, 600);
	panel.addPanel("paint settings", 1, false);
	panel.setWhichPanel("paint settings");
	
	panel.addSlider("amount", "VAL_AMT", 1,1,20, true);
	panel.addSlider("size", "VAL_SIZE", 1,1,200, true);
	panel.addSlider("opacity", "VAL_OP", 100,1,100, false);
	panel.addSlider("red", "VAL_R", 255,1,255, true);
	panel.addSlider("green", "VAL_G", 255,1,255, true);
	panel.addSlider("blue", "VAL_B", 255,1,255, true);
	panel.addSlider("Blur", "VAL_BLUR", 1,0,9, true);
	panel.addSlider("sharp", "VAL_SHARP", 1,0,1024, true);
	panel.addToggle("clear", "VAL_CLEAR", false);
	panel.addToggle("can button", "VAL_BUTT", false);
	panel.addToggle("cap button", "VAL_CAP", false);

	panel.loadSettings("settings.xml");
    TTF.loadFont("mono.ttf", 7);
}

//--------------------------------------------------------------
void PaintOut::update(){
    tuioClient.getMessage();
    panel.update();
    //updateArduino();
    char udpMessage[100000];
	udpConnection.Receive(udpMessage,100000);
	string message=udpMessage;
	if(message!=""){
        ofBackground(0,0,0);
        cout << message << endl;
		//stroke.clear();
		float x,y;
		vector<string> striped = ofSplitString(message,"[/p]");
		//for(int i=0;i<strPoints.size();i++){
        vector<string> point = ofSplitString(striped[0],".");
        if( point.size() == 2 ){
            cout << point[0] << " " << point[1] << endl;
            canId = point[0];
            buttonEvent = ofToString(point[1].c_str());
            if (buttonEvent == "CLEAR") {
                cout << "CLEARING" << endl;
                if(myStrokes.size()>0 && !bDrawing){
                    for(int i=0; i<myStrokes.size(); i++)
                    {
                        myStrokes[i].clearStroke();
                        myStrokes.clear();
                        currentStroke = 0;
                    }
                    cout << "[CAN BUTTON] CLEAR ALL" <<endl;
                }
            }
            //x=atof(point[0].c_str());
            //y=atof(point[1].c_str());
            //stroke.push_back(ofPoint(x,y));
        }
        if (point.size() == 3) {
            cout << point[0] << " " << point[1] << " " << point[2] << endl;
            canId = point[0];
            vector<string> rgb = ofSplitString(point[1],",");
            red = atof(rgb[0].c_str());
            green = atof(rgb[1].c_str());
            blue = atof(rgb[2].c_str());
            sharp = atof(point[2].c_str());
            red = ofMap(red, 0, 255, 255, 0);
            green = ofMap(green, 0, 255, 255, 0);
            blue = ofMap(blue, 0, 255, 255, 0);
            panel.setValueI("VAL_R", red);
            panel.setValueI("VAL_G", green);
            panel.setValueI("VAL_B", blue);
            panel.setValueI("VAL_SHARP", sharp);
        }
		//}
	}
    for(int i=0; i<myStrokes.size(); i++)
	{
		myStrokes[i].strokeAmount = panel.getValueI("VAL_AMT");
		myStrokes[i].size = panel.getValueI("VAL_SIZE");
		//myStrokes[i].blurNum = panel.getValueI("VAL_BLUR");
	}


}

//--------------------------------------------------------------
void PaintOut::draw(){
    //tuioClient.drawCursors();
    for(int i=0; i<myStrokes.size(); i++)
	{
		myStrokes[i].draw();
	}
    ofSetColor(240, 240, 240);
    panel.draw();
}

void PaintOut::tuioAdded(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	//cout << "Point n" << tuioCursor.getSessionId() << " add at " << loc << endl;
    mousePressed(tuioCursor.getX()*ofGetWidth(), tuioCursor.getY()*ofGetHeight(), 0);
}

void PaintOut::tuioUpdated(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	//cout << "Point n" << tuioCursor.getSessionId() << " updated at " << loc << endl;
    if (tuioCursor.getX()>0.0 && tuioCursor.getY()>0.0) {
		mouseDragged(tuioCursor.getX()*ofGetWidth(), tuioCursor.getY()*ofGetHeight(), 0);
	}
}

void PaintOut::tuioRemoved(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	//cout << "Point n" << tuioCursor.getSessionId() << " remove at " << loc << endl;
    mouseReleased(tuioCursor.getX()*ofGetWidth(), tuioCursor.getY()*ofGetHeight(), 0);
}
//--------------------------------------------------------------
void PaintOut::setupArduino(const int & version){
	
    // remove listener because we don't need it anymore
	ofRemoveListener(ard.EInitialized, this, &PaintOut::setupArduino);
    
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
    ofAddListener(ard.EDigitalPinChanged, this, &PaintOut::digitalPinChanged);
    ofAddListener(ard.EAnalogPinChanged, this, &PaintOut::analogPinChanged);
	//ard.sendDigitalPinMode(6, ARD_OUTPUT);
	
	//ard.sendAnalogPinReporting(5, ARD_ANALOG);	// get pot data
}
void PaintOut::updateArduino(){
	
	// update the arduino, get any data or messages:
	ard.update();
	if (bSetupArduino) {
		int r,g,b=0;
        ofBackground(0,0,0);
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
            
            if(myStrokes.size()>0 && !bDrawing){
            
            //undo
            myStrokes[myStrokes.size()-1].clearStroke();
            myStrokes.pop_back();
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
            
            if(myStrokes.size()>0 && !bDrawing){
                for(int i=0; i<myStrokes.size(); i++)
                {
                    myStrokes[i].clearStroke();
                    myStrokes.clear();
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
            panel.setValueI("VAL_R", red);
            panel.setValueI("VAL_G", green);
            panel.setValueI("VAL_B", blue);
            panel.setValueI("VAL_BLUR", blurNum);
            panel.setValueI("VAL_SHARP", sharp);
            panel.setValueB("VAL_BUTT", canButton);
            panel.setValueB("VAL_CAP", capButton);
        }
	}
}
//--------------------------------------------------------------
void PaintOut::digitalPinChanged(const int & pinNum) {
    // do something with the digital input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    buttonState = "digital pin: " + ofToString(pinNum) + " = " + ofToString(ard.getDigital(pinNum));
    cout << buttonState << endl;
}

// analog pin event handler, called whenever an analog pin value has changed

//--------------------------------------------------------------
void PaintOut::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    potValue = "analog pin: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
}

//--------------------------------------------------------------
void PaintOut::keyPressed(int key){

}

//--------------------------------------------------------------
void PaintOut::keyReleased(int key){
    bDrawing = false;
	newStroke = true;
	//myStrokes[currentStroke].smooth(0.1f);		// smoothing for angle calculation.
	
	currentStroke++;

}

//--------------------------------------------------------------
void PaintOut::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void PaintOut::mouseDragged(int x, int y, int button){
    //-------
	myStrokes[currentStroke].addPoint(x,y);
	myStrokes[currentStroke].addColor(red,green,blue,panel.getValueI("VAL_BLUR"));
    //myStrokes[currentStroke].addColor(255,0,0,50);
    panel.mouseDragged(x,y,button);
}

//--------------------------------------------------------------
void PaintOut::mousePressed(int x, int y, int button){
    bDrawing=true;
    stroke s;
	s.brush = brushMain;
	//pngBrush b;
	//s.blurNum = panel.getValueI("VAL_BLUR");
	myStrokes.push_back( s );
	myStrokes[currentStroke].addPoint(x,y);
	myStrokes[currentStroke].addColor(red,green,blue,panel.getValueI("VAL_BLUR"));
    //myStrokes[currentStroke].addColor(255,0,0,50);
    myStrokes[currentStroke].addPoint(x,y);
	myStrokes[currentStroke].addColor(red,green,blue,panel.getValueI("VAL_BLUR"));
   // myStrokes[currentStroke].addColor(255,0,0,50);
	//myBrushes.push_back( b );
	newStroke = false;
    panel.mousePressed(x,y,button);
}

//--------------------------------------------------------------
void PaintOut::mouseReleased(int x, int y, int button){
	bDrawing = false;
	newStroke = true;
	//myStrokes[currentStroke].smooth(0.1f);		// smoothing for angle calculation.
	
	currentStroke++;
	panel.mouseReleased();
}

//--------------------------------------------------------------
void PaintOut::windowResized(int w, int h){

}

//--------------------------------------------------------------
void PaintOut::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void PaintOut::dragEvent(ofDragInfo dragInfo){ 

}
void PaintOut::h2rgb(float H, int& R, int& G, int& B) {
	int var_i;
	float S=1, V=1, var_1, var_2, var_3, var_h, var_r, var_g, var_b;
	
	if ( S == 0 )                       //HSV values = 0 Ö 1
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
		
		green = (1-var_r) * 255;                  //RGB results = 0 Ö 255
		red = (1-var_g) * 255;
		blue = (1-var_b) * 255;
		//analogWrite(3, R);
        //		analogWrite(5, G);
        //		analogWrite(6, B);
	}
}