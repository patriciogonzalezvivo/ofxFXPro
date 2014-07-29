//  UIText.h
//  Thesis
//
//  Created by Patricio Gonzalez Vivo on 9/26/13.
//
//

#pragma once

#include "UIRectangle.h"

#include "TextBlock.h"
#include "TextArc.h"

class UIText: public UIRectangle {
public:
    
    UIText();
    
    void    setText(string _text);
    void    setFontsDir( string _dir );
    
    string  getClassName(){ return name; }
    string  getText();
    
    void    mouseDragged(ofPoint _mouse);

    void    draw();
    
protected:
    virtual void setupUI();
    virtual void reload();
    virtual void guiEvent(ofxUIEventArgs &e);
    
    string  fontDir;
    vector<string> fonts;
    
    aColor  fontColor;
    
    string  fontName;
    float   fontSize;
    float   fontScale;
    
    string              text;
    TextAlignVertical   textAlignV;
    TextAlignHorizontal textAlignH;
    
    TextShapeType       shapeType;
    TextShape           *shape;
    
    float               arcAngle;
};