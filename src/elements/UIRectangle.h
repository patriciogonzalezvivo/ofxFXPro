//
//  UIRectangle.h
//  Monster
//
//  Created by Patricio Gonzalez Vivo on 7/10/14.
//
//

#pragma once

#include "UIClass.h"

class UIRectangle : public UIClass, public ofRectangle{
public:
    
    UIRectangle();
    virtual ~UIRectangle();
    
    virtual string getClassName(){return name;};
    
    void    draw();
    
    string  name;
    bool    bEdit;
    
protected:
    virtual void    setupUI( );
    virtual void    guiEvent(ofxUIEventArgs &e);
    virtual void    _mouseDragged(ofMouseEventArgs &e);
    
    void drawBrakets(ofRectangle _rect, float size, float margin);
    
    bool        bUpdate;
    
};
