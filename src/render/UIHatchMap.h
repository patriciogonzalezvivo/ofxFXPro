//
//  UIHatchMap.h
//
//  Created by Patricio Gonzalez Vivo on 7/4/14.
//
//

#pragma once

#include "UIShader.h"

class UIHatchMap : public UIShader {
public:
    UIHatchMap();
    
    void    setupUI();
    
    string  getClassName();
    
    void    begin();
    void    end();
    
    aColor  inkColor;
    
protected:
    ofTexture   textures[6];
};