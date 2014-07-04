//
//  UIBumpMap.h
//
//  Created by Patricio Gonzalez Vivo on 7/3/14.
//
//

#pragma once

#include "UIShader.h"

class UIBumpMap : public UIShader {
public:
    UIBumpMap();
    
    void    setupUI();
    
    void    loadNormal(string _path);
    
    string  getClassName();
    
    void    begin();
    void    end();
    
    ofTexture   normal;
    ofTexture   texture;
    
protected:
    void    guiEvent(ofxUIEventArgs &e);
};