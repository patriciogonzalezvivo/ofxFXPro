//
//  UIEdge.h
//
//  Created by Patricio Gonzalez Vivo on 7/9/14.
//
//

#pragma once

#include "UIShader.h"

#include "PingPong.h"

class UIEdge : public UIShader {
public:
    
    UIEdge();
    
    void    setupUI();
    
    string  getClassName(){return "Edge";}
    
    void    begin();
    
    bool    bDepth;
    bool    bDebug;
    
protected:
    float radius;
	float focalRange;
	float focalDistance;
};