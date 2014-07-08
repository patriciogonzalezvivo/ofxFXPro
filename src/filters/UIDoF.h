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
    
    void    begin();
    
    bool    bDebug;
    
protected:
    PingPong    pingpong;
    
    float radius;
	float focalRange;
	float focalDistance;
};