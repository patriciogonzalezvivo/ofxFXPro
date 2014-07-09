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
    bumpmap.load(getDataPath()+"shaders/bumpmap");
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
    
    guiAdd(dof);
    
    //guiAdd(audioIn);
}

void Shell::selfUpdate(){
    
//    if(ofGetFrameNum()%24&&audioIn.bEnable){
//        shell.feed( audioIn.average );
//    }
    
    vector<float> values;
    for (int i = 0; i < 50; i++) {
        values.push_back(ofNoise(ofGetElapsedTimef()*0.1,i*0.1)*10.0);
    }
    shell.feed(values);

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

void Shell::selfPostDraw(){
    
    dof.begin();
    dof.setUniformTexture("depthTexture", getRenderTarget().getDepthTexture(), 1);
    getRenderTarget().draw(0,0);
    dof.end();
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