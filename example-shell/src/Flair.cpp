//
//  Flair.cpp
//
//  Created by Patricio Gonzalez Vivo on 11/6/13.
//
//

#include "Flair.h"


void Flair::selfSetup(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();

    hatching.load(getDataPath()+"shaders/hatching");
    matcap.load(getDataPath()+"shaders/matcap");
    bumpmap.load(getDataPath()+"shaders/bumpmap");
    
    //  Light Flair
    //
    scattering.load(getDataPath()+"shaders/lightScattering");
    thresholdShader.load(getDataPath()+"shaders/threshold");
    flareShader.load(getDataPath()+"shaders/flare");
    modulateTextureShader.load(getDataPath()+"shaders/modulateTexture");
    
    sunImage.loadImage(getDataPath()+"sun.png");
    dirtImage.loadImage(getDataPath()+"lensdirt_highc.tga");
    
    setupNumViewports(2);
    
    ofFbo::Settings settings;
    settings.width = ofGetWidth();
    settings.height = ofGetHeight();
    settings.internalformat = GL_RGBA;
    lightRays.allocate(settings);
    thresholdFbo.allocate(settings);
    lensFlareFbo.allocate(ofGetWidth(),ofGetHeight());
    
}

void Flair::selfSetupGuis(){
    lightAdd("DIR LIGHT", OF_LIGHT_DIRECTIONAL );
    lightAdd("SPOT LIGHT", OF_LIGHT_SPOT);
    backgroundSet(new UIMapBackground() );
    
    guiAdd(shell);
    guiAdd(matcap);
    guiAdd(bumpmap);
    guiAdd(hatching);
    
    guiAdd(scattering);
    guiAdd(thresholdShader);
    guiAdd(flareShader);
    guiAdd(modulateTextureShader);
}

void Flair::selfSetupSystemGui(){
    
}

void Flair::selfUpdate(){
    
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

void Flair::selfDraw(){
    
    if(currentViewPort == 0){
        
        //  First Pass
        //
        ofPushStyle();
        
        ofPushMatrix();
        ofTranslate(-2500, -4000, -5000);
        ofSetColor(255);
        sunImage.draw(0, 0, 2048, 2048);
        ofPopMatrix();
        
        ofSetColor(255);
        materials["MATERIAL 1"]->begin();
        
        matcap.begin();
        bumpmap.begin();
        hatching.begin();
        
        ofPushMatrix();
        ofRotate(180, 1, 0, 0);
        shell.draw();
        ofPopMatrix();
        
        hatching.end();
        bumpmap.end();
        matcap.end();
        
        materials["MATERIAL 1"]->end();

        
        ofPopStyle();
        
        
    } else {
        
        //  Oclutions
        //
        ofPushStyle();
        ofClear(0,0);
        
        ofPushMatrix();
        ofTranslate(-2500, -4000, -5000);
        ofSetColor(255);
        sunImage.draw(0, 0, 2048, 2048);
        ofPopMatrix();
        
        ofSetColor(0);
        ofPushMatrix();
        ofRotate(180, 1, 0, 0);
        shell.draw();
        ofPopMatrix();
        
        ofPopStyle();
    }
}

void Flair::selfPostDraw(){

    
    int width = ofGetWidth();
    int height = ofGetHeight();
    
    //  LIGHTS
    //
    lightRays.begin();
    ofClear(0,0);
    scattering.begin();
    scattering.setUniformTexture("firstPass", getRenderTarget(1).getTextureReference(), 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(width, 0); glVertex3f(width, 0, 0);
    glTexCoord2f(width, height); glVertex3f(width, height, 0);
    glTexCoord2f(0,height);  glVertex3f(0,height, 0);
    glEnd();
    scattering.end();
    lightRays.end();
    
    //  Threshold
    //
    thresholdFbo.begin();
    ofClear(0,0);
    thresholdShader.begin();
    thresholdShader.setUniformTexture("tex0", getRenderTarget(1).getTextureReference(), 0);
    thresholdShader.setUniform1i("flipXY", 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(width, 0); glVertex3f(width, 0, 0);
    glTexCoord2f(width, height); glVertex3f(width, height, 0);
    glTexCoord2f(0,height);  glVertex3f(0,height, 0);
    glEnd();
    thresholdShader.end();
    thresholdFbo.end();
    
    //  FLARE
    //
    lensFlareFbo.src->begin();
    ofClear(0,0);
    flareShader.begin();
    flareShader.setUniformTexture("tex0", thresholdFbo, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(width, 0); glVertex3f(width, 0, 0);
    glTexCoord2f(width, height); glVertex3f(width, height, 0);
    glTexCoord2f(0,height);  glVertex3f(0,height, 0);
    glEnd();
    flareShader.end();
    lensFlareFbo.src->end();

    lensFlareFbo.dst->begin();
    ofClear(0,0);
    modulateTextureShader.begin();
    modulateTextureShader.setUniformTexture("firstPass", getRenderTarget(0).getTextureReference(), 0);
    modulateTextureShader.setUniformTexture("lightRays", lightRays.getTextureReference(), 1);
    modulateTextureShader.setUniformTexture("lensFlare", lensFlareFbo.src->getTextureReference(), 2);
    modulateTextureShader.setUniformTexture("dirtImage", dirtImage.getTextureReference(), 3);
    modulateTextureShader.setUniform2f("dirtImageRes", dirtImage.getWidth(), dirtImage.getHeight());
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(width, 0); glVertex3f(width, 0, 0);
    glTexCoord2f(width, height); glVertex3f(width, height, 0);
    glTexCoord2f(0,height);  glVertex3f(0,height, 0);
    glEnd();
    modulateTextureShader.end();
    lensFlareFbo.dst->end();
    
    getRenderTarget(0).draw(0, 0);
    
    lensFlareFbo.dst->draw(0, 0);
    
    if(bDebug){
        ofPushMatrix();
        ofScale(0.25,0.25);
        lensFlareFbo.src->draw(0, 0);
        lensFlareFbo.dst->draw(lensFlareFbo.getWidth(), 0);
        lightRays.draw(0, lensFlareFbo.getHeight());

        ofPopMatrix();
    }
}


void Flair::selfWindowResized(ofResizeEventArgs & args){
    ofFbo::Settings settings;
    settings.width = ofGetWidth();
    settings.height = ofGetHeight();
    settings.internalformat = GL_RGBA;
    lightRays.allocate(settings);
    thresholdFbo.allocate(settings);
    lensFlareFbo.allocate(ofGetWidth(),ofGetHeight());
}
