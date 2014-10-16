#ifndef _STROKE_H
#define _STROKE_H

#include "ofMain.h"






class stroke {
	
	public:
		
	void	addPoint(int x, int y);
	void    addColor(int r, int g,int b, int o);
	void	clearStroke();
	void	draw();

	vector <ofPoint> pts;
	vector <ofColor> colors;
	vector <float> blurs;

	float				getLength();
	float				getTotalAngleChange();
	float				getAverageLength();
	ofRectangle			getBoundingBox();
	

	void	resample(int newNumPts);
	void	smooth(float amount);
	int	strokeAmount;
	int strokeSize;
	//ofxBlurShader blur;
	//shaderBlur blur;
	//vector<ofImage> *brush;
    ofImage *brush;
	//int blurNum;
};

#endif
	
