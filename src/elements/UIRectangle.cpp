//
//  UIRectangle.cpp
//  Monster
//
//  Created by Patricio Gonzalez Vivo on 7/10/14.
//
//

#include "UIRectangle.h"

UIRectangle::UIRectangle(){
    name        = "RECT";
    
    bEdit       = false;
    bUpdate     = true;
}

UIRectangle::~UIRectangle(){
    if (bEdit){
        ofRemoveListener(ofEvents().mouseDragged, this, &UIRectangle::_mouseDragged);
    }
}

void UIRectangle::setupUI( ){
    float length = (gui->getGlobalCanvasWidth()-gui->getWidgetSpacing()*5);
    float dim = gui->getGlobalSliderHeight();
    
    gui->addMinimalSlider("x", 0, ofGetScreenWidth(), &x, length/2., dim);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addMinimalSlider("y", 0, ofGetScreenHeight(), &y, length/2., dim);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addMinimalSlider("width", 0, ofGetScreenHeight(), &width, length/2., dim);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addMinimalSlider("height", 0, ofGetScreenHeight(), &height, length/2., dim);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addToggle("EDIT", &bEdit);
}

void UIRectangle::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if(name == "EDIT"){
        if (bEdit) {
            ofAddListener(ofEvents().mouseDragged, this, &UIRectangle::_mouseDragged);
        } else {
            ofRemoveListener(ofEvents().mouseDragged, this, &UIRectangle::_mouseDragged);
        }
    } else {
        bUpdate = true;
    }
}

void UIRectangle::draw(){
    if(bEnable){
        ofPushStyle();
        ofPushMatrix();
        ofTranslate(x, y);
        ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
        ofSetColor(255,0,0,200);
        ofDrawBitmapString(name,15,-5);
        
        ofNoFill();
        ofSetColor(255,0,0,200);
        ofRect(0,0,width,height);
        
        ofFill();
        ofSetColor(255,0,0,150);
        drawBrakets(ofRectangle(width-7,height-7,14,14), 5, 2 );
        
        ofPopMatrix();
        ofPopStyle();
    }
}

void UIRectangle::drawBrakets(ofRectangle _rect, float size, float margin){
    
    ofPushStyle();
    
    ofPushMatrix();
    ofTranslate(_rect.getTopLeft() + ofPoint(-margin,-margin));
    ofLine(0, 0, size, 0);
    ofLine(0, 0, 0, size);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(_rect.getTopRight() + ofPoint(margin,-margin));
    ofLine(0, 0, -size, 0);
    ofLine(0, 0, 0, size);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(_rect.getBottomLeft() + ofPoint(-margin,margin));
    ofLine(0, 0, size, 0);
    ofLine(0, 0, 0, -size);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(_rect.getBottomRight() + ofPoint(margin,margin));
    ofLine(0, 0, -size, 0);
    ofLine(0, 0, 0, -size);
    ofPopMatrix();
    
    ofPopStyle();
}

void UIRectangle::_mouseDragged(ofMouseEventArgs &e){
    ofVec3f mouse = ofVec3f(e.x, e.y,0);
    ofVec3f mouseLast = ofVec3f(ofGetPreviousMouseX(),ofGetPreviousMouseY(),0);

    ofPoint B = ofPoint(x+width,y+height);
    if ( B.distance( mouse) < 40 ){
        width += mouse.x - x - width;
        height += mouse.y - y - height;
        this->set(x,y, width, height);
    } else if(inside(mouse)){
        ofPoint diff = mouse - mouseLast;
        x += diff.x;
        y += diff.y;
    }
}


