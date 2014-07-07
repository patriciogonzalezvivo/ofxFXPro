//
//  UIDoF.h
//
//  Created by Patricio Gonzalez Vivo on 7/7/14.
//
//

#pragma once

#include "UIShader.h"

#include "PingPong.h"

class UIDoF : public UIShader {
public:
    
    UIDoF();
    
    void    setupUI();
    
    string  getClassName(){return "DoF";}
    
    void operator <<(ofFbo &_target);
    
    bool    bDebug;
    
protected:
    void    guiEvent(ofxUIEventArgs &e);
    
    void  setAssistShader(bool _on);
    
    PingPong    pingpong;
    
    string tmpShader;
    
    float radius;
	float focalRange;
	float focalDistance;
};