//
//  UIDoF.cpp
//
//  Created by Patricio Gonzalez Vivo on 7/7/14.
//
//

#include "UIDoF.h"

#define STRINGIFY(A) #A

UIDoF::UIDoF(){
    fragmentShader = "#version 120\n\n";
    fragmentShader += "\n\
uniform sampler2DRect tex;\n\
uniform sampler2DRect range;\n\
uniform float _radius;\n\
uniform float _focalDistance;\n\
uniform float _focalRange;\n\
\n\
float LinearizeDepth(float zoverw){\n\
    float n = 1.0;\n\
    float f = 20000.0;\n\
    return (2.0 * n) / (f + n - zoverw * (f - n));\n\
}\n\
\n\
float FocalValue(vec2 pos){\n\
    float depth = LinearizeDepth( texture2DRect(range, pos).r ) * 20000.;\n\
    return min( abs(depth  - _focalDistance) / _focalRange, 1.0);\n\
}\n\
\n\
void main(){\n\
    float rangeTexel = FocalValue( gl_TexCoord[0].st);\n\
\n\
    vec4 original = texture2DRect( tex, gl_TexCoord[0].st );\n\
    vec4 finalColor = vec4(0.0, 0.0,0.0,1.0);\n\
\n\
    float weight = 0.;\n\
    int rad = int(_radius);\n\
    for(int x = rad * -1 ; x < rad; x++) {\n\
        for(int y = rad * -1; y < rad; y++){\n\
            vec2 coord = gl_TexCoord[0].xy + vec2(x,y);\n\
            if(distance(coord, gl_TexCoord[0].xy) < _radius){\n\
                vec4 texel = texture2DRect(tex, coord);\n\
                float w = length(texel.rgb)+ 0.1;\n\
                weight += w;\n\
                finalColor += texel * w;\n\
            }\n\
        }\n\
    }\n\
    finalColor /=weight;\n\
\n\
    gl_FragColor = mix(original,finalColor,rangeTexel);\n\
}";
    
    tmpShader = fragmentShader;
    
    setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    linkProgram();
    
    bDebug = false;
}

void UIDoF::setAssistShader(bool _on){
    
    if(_on){
        tmpShader = fragmentShader;
        fragmentShader = "#version 120\n";
        fragmentShader += STRINGIFY(
                                    uniform sampler2DRect range;
                                    uniform float _focalDistance;
                                    uniform float _focalRange;
                                    
                                    float LinearizeDepth(float zoverw){
                                        float n = 1.0; // camera z near
                                        float f = 20000.0; // camera z far
                                        return (2.0 * n) / (f + n - zoverw * (f - n));
                                    }
                                    
                                    void main(){
                                        float depth = LinearizeDepth( texture2DRect(range, gl_TexCoord[0].st).r ) * 20000.;
                                        gl_FragColor = vec4(min( abs(depth  - _focalDistance) / _focalRange, 1.0) );
                                        gl_FragColor.a = 1.0;
                                    });
    } else {
        fragmentShader = tmpShader;
    }
    
    setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    linkProgram();
}

void UIDoF::setupUI(){
    gui->addSlider("Focal_Distance", 0, 1000, &focalDistance);
    gui->addSlider("Focal_Range", 0, 1000, &focalRange);
    gui->addToggle("Debug", &bDebug);
    gui->addSlider("Blur", 0, 8, &radius);
    UIShader::setupUI();
}

void UIDoF::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if(name == "Debug"){
        setAssistShader(bDebug);
    }
    
    UIShader::guiEvent(e);
}

void UIDoF::operator <<(ofFbo &_target){
    if (!pingpong.isAllocated() ||
        pingpong.getWidth() != _target.getWidth() ||
        pingpong.getHeight() != _target.getHeight()){
        pingpong.allocate(_target.getWidth(), _target.getHeight());
    }
    
    pingpong.src->begin();
	ofClear(0,0);
	ofPushStyle();
	ofSetColor(255);
	begin();
    setUniform1f("_radius", radius);
	setUniform1f("_focalDistance", focalDistance);
	setUniform1f("_focalRange", focalRange);
	//setUniformTexture("tex", _target.getTextureReference(), 0);
	setUniformTexture("range", _target.getDepthTexture(), 1);
	_target.draw(0,0);
	end();
    ofPopStyle();
    pingpong.src->end();
	
    pingpong.src->draw(0,0);
    
//    if(bDebug){
//        
//    } else {
//        pingpong.dst->begin();
//        ofClear(0,0);
//        ofPushStyle();
//        ofSetColor(255, 255);
//        begin();
//        setUniform2f("sampleOffset", 0, blurAmount);
////	setUniformTexture("tex", pingpong.src->getTextureReference(), 0);
//        setUniformTexture("range", _target.getDepthTexture(), 1);
//        pingpong.src->draw(0,0);
//        end();
//        ofPopStyle();
//        pingpong.dst->end();
//        
//        pingpong.dst->draw(0,0);
//    }
    
    
}

