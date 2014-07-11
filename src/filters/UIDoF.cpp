//
//  UIDoF.cpp
//
//  Created by Patricio Gonzalez Vivo on 7/7/14.
//
//

#include "UIDoF.h"

#define STRINGIFY(A) #A

UIDoF::UIDoF(){
    fragmentShader = "#version 120\n\n\
\n\
uniform sampler2DRect tex;\n\
uniform sampler2DRect depthTexture;\n\
uniform float _radius;\n\
uniform float _focalDistance;\n\
uniform float _focalRange;\n\
uniform int _debug;\n\
\n\
float LinearizeDepth(float zoverw){\n\
    float n = 1.0;\n\
    float f = 20000.0;\n\
    return (2.0 * n) / (f + n - zoverw * (f - n));\n\
}\n\
\n\
float FocalValue(vec2 pos){\n\
    float depth = LinearizeDepth( texture2DRect(depthTexture, pos).r ) * 20000.;\n\
    float value = abs(depth  - _focalDistance) / _focalRange;\n\
    return min( value , 1.0);\n\
}\n\
\n\
void main(){\n\
    vec4 original = texture2DRect( tex, gl_TexCoord[0].st );\n\
    vec4 finalColor = vec4(0.0, 0.0,0.0,1.0);\n\
\n\
    float rangeTexel = FocalValue(gl_TexCoord[0].st);\n\
    float weight = 1.;\n\
    int rad = int(_radius);\n\
    for(int x = rad * -1 ; x < rad; x++) {\n\
        for(int y = rad * -1; y < rad; y++){\n\
            vec2 coord = gl_TexCoord[0].xy + vec2(x,y);\n\
            if(distance(coord, gl_TexCoord[0].xy) < _radius){\n\
                vec4 texel = texture2DRect(tex, coord);\n\
                float rFV = FocalValue(coord);\n\
\n\
                float w = length(texel.rgb) * rFV;\n\
                weight += w;\n\
                finalColor += texel * w;\n\
            }\n\
        }\n\
    }\n\
    finalColor /=weight;\n\
    if(_debug==1.){\n\
        gl_FragColor.rgb = mix(vec3(0.),vec3(1.),rangeTexel);\n\
        gl_FragColor.a = 1.;\n\
    } else{\n\
        gl_FragColor = mix(original,finalColor,rangeTexel);\n\
    }\n\
}";
    
    setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    linkProgram();
    
    bDebug = false;
}

void UIDoF::setupUI(){
    gui->addSlider("Focal_Distance", 0, 1000, &focalDistance);
    gui->addSlider("Focal_Range", 0, 1000, &focalRange);
    gui->addToggle("Debug", &bDebug);
    gui->addSlider("Blur", 0, 8, &radius);
    UIShader::setupUI();
}

void UIDoF::begin(){
    UIShader::begin();
    setUniform1f("_radius", radius);
	setUniform1f("_focalDistance", focalDistance);
	setUniform1f("_focalRange", focalRange);
    setUniform1i("_debug", bDebug);
}


