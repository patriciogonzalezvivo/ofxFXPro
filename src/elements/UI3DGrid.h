//
//  UI3DGrid.h
//
//  Copyright (C) 2013 Patricio Gonzalez Vivo ( http://www.patriciogonzalezvivo.com )
//
//

#pragma once
#include "UIClass.h"

class UI3DGrid : public UIClass {
public:
    
    UI3DGrid();

    string  getClassName(){ return "3DGRID"; }
    
    void    draw();
    
protected:
    virtual void    setupUI();
    virtual void    guiEvent(ofxUIEventArgs &e);

    ofImage colorSampleImage;
    
	ofVbo   gridMajor;
    ofVbo   grid;

    ofShader shader;
    
    aColor  gridColor;
    aColor  gridMajorColor;
    
    float   gridDim;
    float   gridLineWidth, majorGridLineWidth;
    float   fogFalloffDistance, fogFalloffExpo, fogFalloffScale;
    float   gridScale, gridMajorScale, gridAlphaScale;
    
	int     numGridVertices, numGridMajorVertices;
};