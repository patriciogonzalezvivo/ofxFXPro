//
//  Shell.h
//
//  Created by Patricio Gonzalez Vivo on 10/27/13.
//
//

#pragma once
#include "UIClass.h"

class UIShell : public UIClass {
public:
    
    UIShell();
    virtual ~UIShell(){};
    
    string      getClassName(){return "SHELL";};
    
    void        feed(vector<float> &_newAverage);
    
    void        update();
    void        draw();
    
    int         buffer;
    int         resolution;
    
    float       angleA,angleB;
    ofPoint     translation;
    
private:
    void        setupUI();
    
    ofPolyline  freqArc( vector<float> &_vector, const ofPoint &_center, float _angleBegin, float _angleEnd, float _minRad , bool _bSmooth = false);

    vector< vector<float> > averages;
    vector< ofPoint >       points;
    
    ofVboMesh   mesh;
    ofPoint     offSet;
    
    int         bufferSize;
};