//
//  UIShell.cpp
//  PrintMaking
//
//  Created by Patricio Gonzalez Vivo on 10/27/13.
//
//

#include "UIShell.h"

UIShell::UIShell(){
    buffer = 100;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    bEnable = true;
}

void UIShell::setupUI(){
    gui->addIntSlider("Buffer", 10, 1000, &buffer);
    gui->addIntSlider("Resolution", 3, 100, &resolution);
    
    gui->setGlobalCanvasWidth(gui->getGlobalCanvasWidth()*0.5);
    gui->addRotarySlider("Angle_A", 0, TWO_PI, &angleA);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addRotarySlider("Angle_B", 0, TWO_PI, &angleB);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->setGlobalCanvasWidth(gui->getGlobalCanvasWidth()*2.0);
    
    
    gui->addSlider("TransX", -2.0, 2.0, &translation.x);
    gui->addSlider("TransY", -2.0, 2.0, &translation.y);
}

void UIShell::feed(vector<float> &_newAverage){

    bufferSize = _newAverage.size();
    averages.push_back( _newAverage );
    
    while(averages.size()> buffer){
        averages.erase(averages.begin());
    }
}

void UIShell::update(){
    if(bEnable){
        
        float PHI = (1.0+sqrtf(5.0))/2.0;
        
        float sides = resolution;
        float step = buffer/sides;
        float rotation = 360/sides;
        float growStep = PHI/sides;
        
        ofPoint grow = ofPoint(0.1,0.1);

        offSet = ofPoint(0,0);
        
        int width;
        points.clear();
        
        //  Add new Values to the point structure
        //
        ofMatrix4x4 matrix;
        for (int i = 0; i < averages.size(); i++){
            grow += (grow*(PHI/sides));
            offSet = translation*grow;
            
            ofPolyline line = freqArc( averages[i], offSet, angleA,angleA+angleB, grow.x );
            width = line.size();
            
            matrix.rotate( rotation , 1.0 , 0.0, 0.0);
            for (int j = 0; j < width; j++){
                points.push_back( matrix*line[j] );
            }
        }
        
        //  Create Skin based on points
        //
        if (points.size()>=width && width > 0){
            int height = points.size() / width;
            
            mesh.clear();
            for(int y = 0; y < height - 1; y ++) {
                for(int x = 0; x < width - 1; x ++) {
                    
                    ofVec3f nw = points[ x + y * width];
                    ofVec2f nwT = ofVec2f( (float)(y)/(float)(height), (float)(x)/(float)(width) );
                    
                    ofVec3f ne = points[ (x+1) + y * width];
                    ofVec2f neT = ofVec2f( (float)(y)/(float)(height), (float)(x+1)/(float)(width) );
                    
                    ofVec3f sw = points[ x + (y+1) * width];
                    ofVec2f swT = ofVec2f( (float)(y+1)/(float)(height), (float)(x)/(float)(width) );
                    
                    ofVec3f se = points[ (x+1) + (y+1) * width];
                    ofVec2f seT = ofVec2f( (float)(y+1)/(float)(height), (float)(x+1)/(float)(width) );
                    
                    ofVec3f normal = ((ne - nw).cross(se - nw)).normalize();
                    mesh.addNormal(normal);
                    mesh.addVertex(nw);
                    mesh.addTexCoord(nwT);
                    
                    mesh.addNormal(normal);
                    mesh.addVertex(ne);
                    mesh.addTexCoord(neT);
                    
                    mesh.addNormal(normal);
                    mesh.addVertex(se);
                    mesh.addTexCoord(seT);
                    
                    normal = ((se - nw).cross(sw - nw)).normalize();
                    mesh.addNormal(normal);
                    mesh.addVertex(nw);
                    mesh.addTexCoord(nwT);
                    
                    mesh.addNormal(normal);
                    mesh.addVertex(se);
                    mesh.addTexCoord(seT);
                    
                    mesh.addNormal(normal);
                    mesh.addVertex(sw);
                    mesh.addTexCoord(swT);
                }
            }
        }
    }
}

void UIShell::draw(){
    if(bEnable){
        ofPushMatrix();
        ofTranslate(0,offSet.x*0.7);
        ofRotate(-90, 0, 0, 1);
        mesh.draw();
        ofPopMatrix();
    }
}

//------------------------------------------------ HELPERS
ofPolyline UIShell::freqArc( vector<float> &_vector, const ofPoint &_center, float _angleBegin, float _angleEnd, float _minRad,  bool _bSmooth){
    
    ofPolyline line;
    
    float angle = _angleBegin;//ofWrapRadians(_angleBegin);
    float step = (_angleEnd - angle)/((float)_vector.size());//(ofWrapRadians(_angleEnd) - angle)/((float)_vector.size());
    float scale = 1.0f;
    
    ofPoint start = ofPoint(_center.x + _minRad * cos(angle),
                            _center.y + _minRad * sin(angle));
    ofPoint end = ofPoint(_center.x + _minRad * cos(_angleEnd),
                          _center.y + _minRad * sin(_angleEnd));
    
    line.addVertex( start );
    for (int i = 0; i < _vector.size(); i++){
        
        float value = ofMap(_vector[i]*scale, 0.0, 60.0f, _minRad, _minRad*2);
        ofPoint p = _center;
        p.x += value * cos(angle);
        p.y += value * sin(angle);
        angle += step;
        
        if (_bSmooth){
            line.curveTo( p );
        } else {
            line.addVertex(p);
        }
    }
    line.addVertex( end );
    
    return line;
}
