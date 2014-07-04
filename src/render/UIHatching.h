//
//  UIHatching.h
//
//  Created by Patricio Gonzalez Vivo on 7/4/14.
//
//

#pragma once

#include "UIShader.h"

class UIHatching : public UIShader {
public:
    UIHatching();
    
    void    setupUI();
    
    string  getClassName();
    
    void    begin();
    void    end();
    
    aColor  inkColor;
protected:
//    void    guiEvent(ofxUIEventArgs &e);
    ofTexture   textures[6];
};