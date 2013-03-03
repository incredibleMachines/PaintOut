#ifndef _STROKE_H
#define _STROKE_H

#include "ofMain.h"






class stroke {
	
	public:
		
	void	addPoint(float x, float y);
	void    addColor(int r, int g,int b, int o);
	void	clearStroke();
	void	draw();

	vector <ofVec2f> pts;
	vector <ofColor> colors;
	vector <float> blurs;

	float				getLength();
	float				getTotalAngleChange();
	float				getAverageLength();
	ofRectangle			getBoundingBox();
	

	void	resample(int newNumPts);
	void	smooth(float amount);
	int	strokeAmount;
	int size;
	//ofxBlurShader blur;
	//shaderBlur blur;
	//vector<ofImage> *brush;
    ofImage *brush;
	//int blurNum;
};

#endif
	
