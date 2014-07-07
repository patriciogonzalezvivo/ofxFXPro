//
//  Shell.cpp
//
//  Created by Patricio Gonzalez Vivo on 11/6/13.
//
//

#include "Shell.h"


void Shell::selfSetup(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();

    hatching.load(getDataPath()+"shaders/hatching");
    matcap.load(getDataPath()+"shaders/matcap");
}

void Shell::selfSetupGuis(){
    lightAdd("DIR LIGHT", OF_LIGHT_DIRECTIONAL );
    lightAdd("SPOT LIGHT", OF_LIGHT_SPOT);
    backgroundSet(new UIMapBackground() );
    
    guiAdd(shell);
    guiAdd(matcap);
    guiAdd(bumpmap);
    guiAdd(hatching);
    
    guiAdd(grid);
    
    guiAdd(audioIn);
}

void Shell::selfUpdate(){
    
    if(ofGetFrameNum()%24&&audioIn.bEnable){
        shell.feed( audioIn.average );
    }
    
    shell.update();
}

void Shell::selfDraw(){
    
    
    ofPushMatrix();
    ofPushStyle();
    ofRotateX(90);
    grid.draw();
    ofPopStyle();
    ofPopMatrix();
    
    ofSetColor(255);
    materials["MATERIAL 1"]->begin();
    
    matcap.begin();
    bumpmap.begin();
    hatching.begin();
    
    shell.draw();
    
    hatching.end();
    bumpmap.end();
    matcap.end();
    
    materials["MATERIAL 1"]->end();
}

void Shell::selfKeyPressed(ofKeyEventArgs & args){

}

void Shell::selfKeyReleased(ofKeyEventArgs & args){
	
}

void Shell::selfMouseDragged(ofMouseEventArgs& data){

}

void Shell::selfMouseMoved(ofMouseEventArgs& data){
	
}

void Shell::selfMousePressed(ofMouseEventArgs& data){

}

void Shell::selfMouseReleased(ofMouseEventArgs& data){
	
}