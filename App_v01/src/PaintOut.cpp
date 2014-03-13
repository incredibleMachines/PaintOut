#include "PaintOut.h"

//--------------------------------------------------------------
void PaintOut::setup(){
    ofEnableSmoothing();
    
    logo.loadImage("megalogo1.png");
    drawLogo = false;
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
    panel.setup("control", ofGetWidth()-250, 0, 250, 600);
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
    panel.hide();
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
        //cout << message << endl;
		//stroke.clear();
		float x,y;
		vector<string> striped = ofSplitString(message,"[/p]");
		//for(int i=0;i<strPoints.size();i++){
        vector<string> point = ofSplitString(striped[0],".");
        if( point.size() == 2 ){
            //cout << point[0] << " " << point[1] << endl;
            canId = point[0];
            buttonEvent = ofToString(point[1].c_str());
            if (buttonEvent == "HOLD") {
                //cout << "CLEARING" << endl;
                if(myStrokes.size()>0 && !bDrawing){
                    ofImage screenie;
                    screenie.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
                    //displayGrabber.grabImage(screenie);
                    ofxDisplay* display = ofxDisplay::getMainDisplay();
                    display->grabImage(screenie);
                    //screenie.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
                    string date = "riot/"+ofToString(ofGetUnixTime());
                    screenie.saveImage(date+".png");
                    //ofSaveViewport(date+".png");
                    for(int i=0; i<myStrokes.size(); i++)
                    {
                        myStrokes[i].clearStroke();
                        myStrokes.clear();
                        currentStroke = 0;
                    }
                    //cout << "[CAN BUTTON] CLEAR ALL" <<endl;
                }
            }
            if (buttonEvent == "CLICK") {
                if(myStrokes.size()>0 && !bDrawing){
                    
                    //undo
                    myStrokes[myStrokes.size()-1].clearStroke();
                    myStrokes.pop_back();
                    currentStroke--;
                   // cout << "[CAN BUTTON]" << endl;
                }
            }
            if (buttonEvent == "CLEAR") {
                if(myStrokes.size()>0 && !bDrawing){
                    
                    drawLogo=!drawLogo;
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
            int linear = (2914 / (sharp + 5)) - 1;
            cout << "[linear] " << linear << endl;
            int blur = ofMap(linear, 0, 25, 0, 9);
            red = ofMap(red, 0, 255, 255, 0);
            green = ofMap(green, 0, 255, 255, 0);
            blue = ofMap(blue, 0, 255, 255, 0);
            panel.setValueI("VAL_R", red);
            panel.setValueI("VAL_G", green);
            panel.setValueI("VAL_B", blue);
            panel.setValueI("VAL_SHARP", linear);
            panel.setValueI("VAL_BLUR", blur);
        }
		//}
	}
    for(int i=0; i<myStrokes.size(); i++)
	{
		myStrokes[i].strokeAmount = panel.getValueI("VAL_AMT");
		myStrokes[i].strokeSize = panel.getValueI("VAL_SIZE");
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
    if(drawLogo){
        ofEnableAlphaBlending();
        ofSetRectMode(OF_RECTMODE_CENTER);
        logo.draw(ofGetWidth()/2, ofGetHeight()/2);
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofDisableAlphaBlending();
    }
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
    if(key == 's'){
        drawLogo=!drawLogo;
    }
    if(key == 'p'){
        if(panel.hidden){
            panel.show();
        }else{
            panel.hide();
        }
    }
    
}

//--------------------------------------------------------------
void PaintOut::keyReleased(int key){
//    bDrawing = false;
//	newStroke = true;
//	//myStrokes[currentStroke].smooth(0.1f);		// smoothing for angle calculation.
//	
//	currentStroke++;

}

//--------------------------------------------------------------
void PaintOut::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void PaintOut::mouseDragged(int x, int y, int button){
    //-------
//    if(x<=10 || y<=10){
//
//    }else{
	myStrokes[currentStroke].addPoint(x,y);
	myStrokes[currentStroke].addColor(red,green,blue,panel.getValueI("VAL_BLUR"));
    //myStrokes[currentStroke].addColor(255,0,0,50);
    panel.mouseDragged(x,y,button);
    //}
}

//--------------------------------------------------------------
void PaintOut::mousePressed(int x, int y, int button){
    
//    if(x<=10 || y<=10){
//
//    }else{
    bDrawing=true;
    stroke s;
	s.brush = brushMain;
	//pngBrush b;
	//s.blurNum = panel.getValueI("VAL_BLUR");
	myStrokes.push_back( s );
	myStrokes[currentStroke].addPoint(x,y);
	myStrokes[currentStroke].addColor(red,green,blue,panel.getValueI("VAL_BLUR"));
    //myStrokes[currentStroke].addColor(255,0,0,50);
    //myStrokes[currentStroke].addPoint(x,y);
	//myStrokes[currentStroke].addColor(red,green,blue,panel.getValueI("VAL_BLUR"));
   // myStrokes[currentStroke].addColor(255,0,0,50);
	//myBrushes.push_back( b );
	newStroke = false;
    panel.mousePressed(x,y,button);
    //}
}

//--------------------------------------------------------------
void PaintOut::mouseReleased(int x, int y, int button){
	
    if(x<=10 || y<=10){
        
    }else{
    bDrawing = false;
	newStroke = true;
	//myStrokes[currentStroke].smooth(0.1f);		// smoothing for angle calculation.
	
	currentStroke++;
	panel.mouseReleased();
    }
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

