//
//  UIText.cpp
//  Thesis
//
//  Created by Patricio Gonzalez Vivo on 9/26/13.
//
//

#include "UIText.h"

UIText::UIText(){
    shape = NULL;
    fontDir = "none";
    bEnable = true;
    
}

void UIText::setupUI(){
    
//    gui->addTextInput("content", "_content_", OFX_UI_FONT_SMALL);
//    gui->addSlider("width",10,ofGetWindowWidth(),&width);
//    gui->addSlider("height", 10, ofGetWindowHeight(), &height);
    
    UIRectangle::setupUI();
    
    gui->addSlider("font_size",4.0,80.0,&fontSize);
    gui->addSlider("font_scale",0.0,2.0,&fontScale);
    
    ofDirectory dir;
    dir.open( fontDir );
    if (dir.listDir()){
        fonts.clear();
        for(int i = 0; i < dir.getFiles().size(); i++ ){
            fonts.push_back( dir.getFiles()[i].getFileName() );
        }
        gui->addDropDownList("FONTS", fonts );
    }
    
    addUIColor("font_color", &fontColor);
    gui->addSlider("font_alpha",0.0,1.0, &fontColor.alpha);
    
    vector<string> horizontal;
    horizontal.push_back("LEFT");
    horizontal.push_back("RIGHT");
    horizontal.push_back("JUSTIFIED");
    horizontal.push_back("CENTER");
    
    gui->addDropDownList("HORIZONTAL_ALIGN", horizontal);
    
    vector<string> vertical;
    vertical.push_back("TOP");
    vertical.push_back("BOTTOM");
    vertical.push_back("MIDDLE");
    
    gui->addDropDownList("VERTICAL_ALIGN", vertical);
    
    vector<string> type;
    type.push_back("ARC");
    type.push_back("BLOCK");
    gui->addDropDownList("TYPE", type);
    gui->addSlider("arc_angle", -PI, PI, &arcAngle);
}

void UIText::setText(string _text){
    text =_text;
    if(shape!=NULL){
        shape->setText(text);
    }
}

string UIText::getText(){
    return text;
}

void UIText::guiEvent(ofxUIEventArgs &e){
    if (gui != NULL || shape != NULL){
        string name = e.widget->getName();
        
        if( name == "width" || name == "height" ){
            ofPoint center = getCenter();
            setFromCenter(center, width, height);
            
            if ( shape != NULL ){
                delete shape;
            }
            
            shape = new TextBlock();
            ((TextBlock*)(shape))->setWrapping(false);
            shape->set(*this);
            shape->loadFont( fontDir+"/"+fontName, fontSize );
            shape->setScale( fontScale );
            shape->setAlignment( textAlignH , textAlignV );
            shape->setText(text);
            
            ofxUIWidget *angWidget =  gui->getWidget("arc_angle");
            angWidget->setVisible(false);
        
        } else if( name == "BLOCK" ){
            shapeType = TEXT_SHAPE_BLOCK;
            
            if ( shape != NULL ){
                delete shape;
            }
            
            shape = new TextBlock();
            ((TextBlock*)(shape))->setWrapping(false);
            shape->set(*this);
            shape->loadFont( fontDir+"/"+fontName, fontSize );
            shape->setScale( fontScale );
            shape->setAlignment( textAlignH , textAlignV );
            shape->setText(text);
            
            ofxUIWidget *angWidget =  gui->getWidget("arc_angle");
            angWidget->setVisible(false);
            
        } else if( name == "ARC"){
            shapeType = TEXT_SHAPE_ARC;
            
            if ( shape != NULL ){
                delete shape;
            }
            
            shape = new TextArc();
            shape->set(*this);
            shape->loadFont( fontDir+"/"+fontName, fontSize );
            shape->setScale( fontScale );
            shape->setAlignment( textAlignH , textAlignV );
            shape->setText(text);
            ((TextArc*)(shape))->setRadius(arcAngle);
            
            ofxUIWidget *angWidget =  gui->getWidget("arc_angle");
            angWidget->setVisible(true);
            
        } else if( name == "font_size" || name == "font_scale"){
            shape->loadFont( fontDir+"/"+fontName, fontSize );
            shape->setScale( fontScale );
        } else if ( name == "content"){
            ofxUITextInput *widget = (ofxUITextInput *)e.widget;
            text = widget->getTextString();
            
            shape->setText(text);
            widget->setTextString(text);
        } else if ( name == "LEFT" ){
            textAlignH = TEXT_ALIGN_LEFT;
            shape->setAlignment( textAlignH , textAlignV );
        } else if ( name == "RIGHT" ){
            textAlignH = TEXT_ALIGN_RIGHT;
            shape->setAlignment( textAlignH , textAlignV );
        } else if ( name == "JUSTIFIED" ){
            textAlignH = TEXT_ALIGN_JUSTIFIED;
            shape->setAlignment( textAlignH , textAlignV );
        } else if ( name == "CENTER" ){
            textAlignH = TEXT_ALIGN_CENTER;
            shape->setAlignment( textAlignH , textAlignV );
        } else if ( name == "TOP" ){
            textAlignV = TEXT_ALIGN_TOP;
            shape->setAlignment( textAlignH , textAlignV );
        } else if ( name == "BOTTOM" ){
            textAlignV = TEXT_ALIGN_BOTTOM;
            shape->setAlignment( textAlignH , textAlignV );
        } else if ( name == "MIDDLE" ){
            textAlignV = TEXT_ALIGN_MIDDLE;
            shape->setAlignment( textAlignH , textAlignV );
        } else if ( name == "ENABLE" || name == "FONTS" || name == "VERTICAL_ALIGN" || name == "HORIZONTAL_ALIGN"){
            
        } else if ( name == "arc_angle"){
            if (shapeType == TEXT_SHAPE_ARC) {
                ((TextArc*)(shape))->setAngle(arcAngle);
            }
        } else if ( name == "EDIT" ) {
            UIRectangle::guiEvent(e);
        } else{
            for(int i = 0; i < fonts.size(); i++ ){
                if (name == fonts[i]){
                    fontName = name;
                    cout << "Load Font " << fontDir+fontName << endl;
                    shape->loadFont( fontDir+fontName, fontSize );
                    break;
                }
            }
        }
    }
    bUpdate = true;
    gui->autoSizeToFitWidgets();
}

void UIText::setFontsDir( string _dir ){
    fontDir = _dir;
    if ( shape != NULL ){
        
        ofDirectory dir;
        dir.open( fontDir );
        if (dir.listDir()){
            fonts.clear();
            for(int i = 0; i < dir.getFiles().size(); i++ ){
                fonts.push_back( dir.getFiles()[i].getFileName() );
            }
            
            shape->loadFont( fontDir+"/"+fontName, fontSize );
        }
    }
}

void UIText::draw(){
    if (bEnable){
        
        if(bEdit){
            UIRectangle::draw();
        }
        
        if ( shape != NULL ){
            fontColor.update();
            
            ofPushStyle();
            ofSetColor(fontColor);
            shape->draw();
            ofPopStyle();
        }
    }
}
