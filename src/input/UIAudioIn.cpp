//
//  UIAudioIn.cpp
//  Thesis
//
//  Created by Patricio Gonzalez Vivo on 11/19/13.
//
//

#include "UIAudioIn.h"

UIAudioIn::UIAudioIn(){
    bufferSize = 512;
    sampleRate = 44100;
    
    FFTanalyzer.peakHoldTime        = 15;       // hold longer
	FFTanalyzer.peakDecayRate       = 0.95f;    // decay slower
	FFTanalyzer.linearEQIntercept   = 0.9f;     // reduced gain at lowest frequency
	FFTanalyzer.linearEQSlope       = 0.01f;    // increasing gain at higher frequencies
    FFTanalyzer.setup(sampleRate, bufferSize/2, 2);
    
    audioIn = new float[bufferSize];
	
    magnitude   = new float[bufferSize];
    phase       = new float[bufferSize];
    power       = new float[bufferSize];
    freq        = new float[bufferSize/2];
    
    pixels.allocate(17,1,3);
    for (int i = 0; i < 17; i++){
        pixels.setColor(i, 0, ofFloatColor(0.0));
    }
    averageImg.allocate(17,1,OF_IMAGE_COLOR);
    
    start();
}

void UIAudioIn::setup(int _sampleRate, int _bufferSize){
    sampleRate = _sampleRate;
    bufferSize = _bufferSize;
}

UIAudioIn::~UIAudioIn(){
    stop();
    
    delete []audioIn;
    delete []magnitude;
    delete []phase;
    delete []power;
    delete []freq;
}

void UIAudioIn::setupUI(){
    gui->addWaveform("Buffer", audioIn, bufferSize);
    gui->addIntSlider("Peak_Hold_Time", 0, 30, &FFTanalyzer.peakHoldTime);
    gui->addSlider("Peak_Decay_Rate", 0, 1., &FFTanalyzer.peakDecayRate);
    gui->addSlider("LinearEQIntercept", 0, 1., &FFTanalyzer.linearEQIntercept);
    gui->addSlider("linearEQSlope", 0, 0.1, &FFTanalyzer.linearEQSlope);
    gui->addSpectrum("FFT", FFTanalyzer.averages, 17);
    gui->addImage("Image", &averageImg, gui->getPaddingRect()->getWidth()-12,10 );
    
}

void UIAudioIn::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if(name == "ENABLE"){
        if(bEnable){
            start();
        } else {
            stop();
        }
    }
}

void UIAudioIn::start(){
    stream.setup(0, 1, sampleRate, bufferSize, 4);
    stream.setInput(this);
    
    ofAddListener(ofEvents().update,this,&UIAudioIn::update);
}

void UIAudioIn::stop(){
    stream.stop();
    ofRemoveListener(ofEvents().draw, this, &UIAudioIn::update);
}

void UIAudioIn::audioReceived(float * input, int bufferSize, int nChannels ){
    
    for(int i = 0; i < bufferSize; i++) {
        audioIn[i] = input[i];
	}
    
    soundMutex.lock();
    
    //  Do the FFT
    //
    float avg_power = 0.0f;
	myFft.powerSpectrum(0,(int)bufferSize/2, audioIn, bufferSize, magnitude, phase, power, &avg_power);
	for (int i = 0; i < (int)(bufferSize/2); i++){
		freq[i] = magnitude[i];
	}
    FFTanalyzer.calculate(freq);
    
    average.clear();
    average.assign(FFTanalyzer.averages,FFTanalyzer.averages+18);
    
	soundMutex.unlock();
}

void UIAudioIn::update(ofEventArgs & args){
    for (int i = 0; i < average.size(); i++){
        ofFloatColor color = ofFloatColor(average[i]);
        
//        color.setBrightness(1.0);
//        color.setSaturation(1.0);
//        color.setHue(middleBins[i]);
        
        pixels.setColor(i, 0, color);
    }
    averageImg.setFromPixels(pixels);
}