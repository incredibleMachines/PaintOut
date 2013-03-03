#include "stroke.h"

//#include "testApp.h"
//((testApp *)ofGetAppPtr())->mynumber;



//--------------------------------------------------------------------
void stroke::addPoint(float x, float y){
	pts.push_back(ofVec2f(x,y));
}
void stroke::addColor(int r, int g,int b, int o){
	ofColor tempColor;
	tempColor.r=r;
	tempColor.g=g;
	tempColor.b=b;
	tempColor.a=o;
	colors.push_back(tempColor);
}

//--------------------------------------------------------------------
void stroke::clearStroke(){
	pts.clear();
    colors.clear();
    blurs.clear();
}

//--------------------------------------------------------------------
void stroke::draw(){
	int space = 5;
	//ofSetLineWidth(20);
	//ofSetColor(255,255,255,opacity);
	if (pts.size() > 1){
		for (int i = 0; i < pts.size()-1; i++){
			//ofSetColor(255, 255, 255,opacity);
			int dist = int(ofDist(pts[i].x, pts[i].y, pts[i+1].x, pts[i+1].y));
			//cout << dist << endl;
			ofPoint diff = pts[i];
			for (int j=0; j<dist/(strokeAmount+(size/50)); j++) {
				//diff += diff + (pts[i+1]/dist);
				//ballPos += (clickPos-ballPos)/5;
				diff += (pts[i+1]-pts[i])/(dist/(strokeAmount+(size/50)));
				//diff += (pts[i+1]-pts[i])/(dist/20);
				//ofCircle(diff.x, diff.y, size);
				ofSetColor(colors[i].r, colors[i].g, colors[i].b);
                ofEnableAlphaBlending();
				brush[int(colors[i].a)].draw(diff.x-(size/2), diff.y-(size/2), size,size);
                ofDisableAlphaBlending();
                //ofCircle(diff.x-(size/2), diff.y-(size/2), 20);
			}
//            if (dist==0) {
//                brush[int(colors[i].a)].draw(diff.x-(size/2), diff.y-(size/2), size,size);
//            }
			//cout << ofDist(pts[i].x, pts[i].y, pts[i+1].x, pts[i+1].y) << endl;
			
						
		}
	}
}

//--------------------------------------------------------------------
ofRectangle stroke::getBoundingBox(){
	
	float minx = 0;
	float miny = 0;
	float maxx = 0;
	float maxy = 0;
	
	for (int i = 0; i < pts.size(); i++){
		if (i == 0){
			minx = pts[i].x;
			miny = pts[i].y;
			maxx = pts[i].x;
			maxy = pts[i].y;
		} else {
			
			if (pts[i].x < minx){
				minx = pts[i].x;
			}
			
			if (pts[i].y < miny){
				miny = pts[i].y;
			}
			
			if (pts[i].x > maxx){
				maxx = pts[i].x;
			}
			
			if (pts[i].y > maxy){
				maxy = pts[i].y;
			}
			
		}
	}
	
	return ofRectangle(minx, miny, maxx - minx, maxy - miny);
}

//--------------------------------------------------------------------
float stroke::getLength(){
	float length = 0;
	if (pts.size() > 1){
		for (int i = 0; i < pts.size()-1; i++){
			length += (pts[i] - pts[i+1]).length();
		}
	}
	return length;
}

//--------------------------------------------------------------------
float stroke::getAverageLength(){
	
	if (pts.size() < 1) return 0;
	
	float avglength = 0;
	if (pts.size() > 1){
		for (int i = 0; i < pts.size()-1; i++){
			avglength += (pts[i] - pts[i+1]).length();
		}
	}
	return avglength / (float) pts.size();
	
}


//--------------------------------------------------------------------
float stroke::getTotalAngleChange(){
	
	float totalAngle = 0;
	
	if (pts.size() > 1){
		
		
		float prevAngle = 0;
		
		for (int i = 1; i < pts.size(); i++){
			ofVec2f linea =  pts[i-1] - pts[i];
			
			float angle = atan2(linea.y, linea.x);
		
			if (i != 1) {
				float diffAngle = angle - prevAngle;
				
				// since the difference between angles can be big or small, 
				// let's take the shorter distance.
				
				if (diffAngle > PI)		diffAngle -= TWO_PI;	
				if (diffAngle < -PI)	diffAngle += TWO_PI;
				totalAngle += fabs(diffAngle);
			}
			prevAngle = angle;
		}
		
	}
	
	return totalAngle;
}

//--------------------------------------------------------------------
void stroke::smooth(float amount){
	
	if (pts.size() < 3) return;
	
	vector <ofVec2f> newPts;						// new pts
	
	//--- store first point since we'll never resample it out of existence
	newPts.push_back(pts.front());
	for(int i = 1; i < (int)pts.size()-1; i++){
		ofVec2f smoothedPoint;
		smoothedPoint = amount * pts[i-1] + amount * pts[i+1] + (1-amount*2) * pts[i];
		newPts.push_back(smoothedPoint);
	}
	newPts.push_back(pts.back());
		
	pts = newPts;
	
}

//--------------------------------------------------------------------
void stroke::resample(int newNumPts){
	
	float interval = getLength() / (newNumPts - 1); // calc interval length
	
	float D = 0.0;									// total distance, as we walk across
	
	vector <ofVec2f> newPts;						// new pts
	
	//--- store first point since we'll never resample it out of existence
	newPts.push_back(pts.front());
	for(int i = 1; i < (int)pts.size(); i++){
		
		ofVec2f currentPoint  = pts[i];
		ofVec2f previousPoint = pts[i-1];
		float d = (previousPoint - currentPoint).length();
		if ((D + d) >= interval){
			float qx = previousPoint.x + ((interval - D) / d) * (currentPoint.x - previousPoint.x);
			float qy = previousPoint.y + ((interval - D) / d) * (currentPoint.y - previousPoint.y);
			ofVec2f point(qx, qy);
			newPts.push_back(point);
			pts.insert(pts.begin() + i, point);
			D = 0.0;
		}
		else D += d;
	}
	
	// somtimes we fall a rounding-error short of adding the last point, so add it if so
	if (newPts.size() == (newNumPts - 1)){
		newPts.push_back(pts.back());
	}
	
	pts = newPts;
	
}
