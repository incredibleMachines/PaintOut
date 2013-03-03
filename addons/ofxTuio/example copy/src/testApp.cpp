#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableSmoothing();
    
    tuioClient.start(3333);
    
    ofAddListener(tuioClient.cursorAdded,this,&testApp::tuioAdded);
	ofAddListener(tuioClient.cursorRemoved,this,&testApp::tuioRemoved);
	ofAddListener(tuioClient.cursorUpdated,this,&testApp::tuioUpdated);
    ard.connect("/dev/tty.usbserial-A600eztS", 57600);
	// listen for EInitialized notification. this indicates that
    // the arduino is ready to receive commands and it is safe to
    // call setupArduino()
    ofAddListener(ard.EInitialized, this, &testApp::setupArduino);
	bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
}

//--------------------------------------------------------------
void testApp::update(){
    tuioClient.getMessage();
    updateArduino();
}

//--------------------------------------------------------------
void testApp::draw(){
    tuioClient.drawCursors();
}

void testApp::tuioAdded(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	cout << "Point n" << tuioCursor.getSessionId() << " add at " << loc << endl;
}

void testApp::tuioUpdated(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	cout << "Point n" << tuioCursor.getSessionId() << " updated at " << loc << endl;
}

void testApp::tuioRemoved(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	cout << "Point n" << tuioCursor.getSessionId() << " remove at " << loc << endl;
}
//--------------------------------------------------------------
void testApp::setupArduino(const int & version){
	
    // remove listener because we don't need it anymore
	ofRemoveListener(ard.EInitialized, this, &testApp::setupArduino);
    
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
    ofAddListener(ard.EDigitalPinChanged, this, &testApp::digitalPinChanged);
    ofAddListener(ard.EAnalogPinChanged, this, &testApp::analogPinChanged);
	//ard.sendDigitalPinMode(6, ARD_OUTPUT);
	
	//ard.sendAnalogPinReporting(5, ARD_ANALOG);	// get pot data
}
void testApp::updateArduino(){
	
	// update the arduino, get any data or messages:
	ard.update();
//	if (bSetupArduino) {
//		int r,g,b=0;
//		float h = ((float)ard.getAnalog(2))/1024;
//		int h_int = (int) 360*h;
//		h2rgb(h,r,g,b);
//		//ard.sendDigital(10, ARD_HIGH);
//		//cout << ard.getDigital(8) << endl;
//		if(ard.getDigital(8) == 1){
//			ard.sendDigital(10, ARD_HIGH);
//			capButton=true;
//		}else {
//			ard.sendDigital(10, ARD_LOW);
//			capButton=false;
//		}
//		ard.sendDigital(2, ARD_HIGH);
//		//cout << ard.getDigital(2) << endl;
//		if(ard.getDigital(2) == 0 && canButton == false){
//			buttonTime = ofGetElapsedTimeMillis();
//			canButton =true;
//			//changeSound=true;
//		}
//        //        else if (ard.getDigital(2) == 1 && changeSound==true) {
//        //			synthType++;
//        //			changeSound=false;
//        //			canButton=false;
//        //			if (synthType>5) {
//        //				synthType=1;
//        //			}
//        //
//        //		}
//        else if (ard.getDigital(2) == 1) {
//			canButton=false;
//		}
//		if (canButton == true && (ofGetElapsedTimeMillis()-buttonTime)>= 2000) {
//			//changeSound=false;
//			for(int i=0; i<myStrokes.size(); i++)
//			{
//				myStrokes[i].clear();
//			}
//		}
//        
//		//synthType = int(ofMap(ard.getAnalog(2), 1, 5, 0, 1023));
//		ard.sendPwm(3, ofMap(red, 0, 255, 255, 0));
//		ard.sendPwm(5, ofMap(green, 0, 255, 255, 0));
//		ard.sendPwm(6, ofMap(blue, 0, 255, 255, 0));
//		sharp = ard.getAnalog(4);
//		blurNum = ofMap(sharp, 200, 730, 9, 0);
//	}
}
//--------------------------------------------------------------
void testApp::digitalPinChanged(const int & pinNum) {
    // do something with the digital input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    buttonState = "digital pin: " + ofToString(pinNum) + " = " + ofToString(ard.getDigital(pinNum));
    cout << buttonState << endl;
}

// analog pin event handler, called whenever an analog pin value has changed

//--------------------------------------------------------------
void testApp::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    potValue = "analog pin: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}