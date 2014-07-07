//
//  UIAudioIn.h
//
//  Created by Patricio Gonzalez Vivo on 11/19/13.
//
//
#pragma once

#include "UIClass.h"

#include "fft.h"
#include "FFTOctaveAnalyzer.h"

class UIAudioIn : public UIClass, public ofBaseSoundInput {
public:
    
    UIAudioIn();
    virtual ~UIAudioIn();
    
    string  getClassName(){return "AudioIn";};
    void    setup(int _sampleRate, int _bufferSize);
    
    void    start();
    void    stop();
    
    int     getBufferSize(){return bufferSize;};

    vector<float>   average;
    ofImage         averageImg;
    
    bool    bHue;
    
protected:
    void setupUI();
    void update(ofEventArgs & args);
    void guiEvent(ofxUIEventArgs &e);
    void audioReceived( float * input, int bufferSize, int nChannels );
    
    ofSoundStream stream;
    
    ofMutex soundMutex;
    float   *audioIn;
    float   *magnitude;
    float   *phase;
    float   *power;
    float   *freq;
    
    ofFloatPixels       pixels;
    
    fft                 myFft;
    FFTOctaveAnalyzer   FFTanalyzer;
    
    int     sampleRate;
    int     bufferSize;
};