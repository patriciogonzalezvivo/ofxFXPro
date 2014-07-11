//
//  UIEdge.cpp
//  example-shell
//
//  Created by Patricio Gonzalez Vivo on 7/9/14.
//
//

#include "UIEdge.h"

UIEdge::UIEdge(){
    fragmentShader = "#version 120\n\n\
\n\
uniform sampler2DRect tex;\n\
uniform sampler2DRect depthTexture;\n\
\n\
uniform int _depth;\n\
uniform float _focalDistance;\n\
uniform float _focalRange;\n\
uniform float _radius;\n\
\n\
uniform int _debug;\n\
\n\
uniform float pct;\n\
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
float getIntensity(vec2 u){\n\
    vec3 a = vec3(0.);\n\
    if (_depth==1.){\n\
        a = vec3(FocalValue(u));\n\
    } else {\n\
        a = texture2DRect(tex,u).xyz;\n\
    }\n\
    return (a.x+a.y+a.z)/3.0;\n\
}\n\
\n\
void main(void){\n\
    vec2 uv = gl_TexCoord[0].st;\n\
\n\
    vec2 p = vec2(_radius);\n\
\n\
    float avg = 0.0;\n\
    avg += getIntensity(uv+vec2(p.x,0.0));\n\
    avg += getIntensity(uv+vec2(-p.x,0.0));\n\
    avg += getIntensity(uv+vec2(0.0,p.y));\n\
    avg += getIntensity(uv+vec2(0.0,-p.y));\n\
    avg += getIntensity(uv+vec2(p.x,p.y));\n\
    avg += getIntensity(uv+vec2(-p.x,-p.y));\n\
    avg += getIntensity(uv+vec2(p.x,-p.y));\n\
    avg += getIntensity(uv+vec2(-p.x,p.y));\n\
    avg /= 8.0;\n\
\n\
    float result = (1.0-getIntensity(uv)) + avg;\n\
    result = (1.0 - result) * (pct*10.0);\n\
\n\
    if(_debug==1.){\n\
        float depth = FocalValue(uv);\n\
        gl_FragColor.rgb = mix(vec3(0.),vec3(1.),depth);\n\
        gl_FragColor.a = 1.;\n\
    } else {\n\
        gl_FragColor = vec4(vec3(1.0-result),result);\n\
    }\n\
}";
    
    setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    linkProgram();
    
    bDebug = false;
}

void UIEdge::setupUI(){
    
    gui->addToggle("Depth", &bDepth);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addToggle("Debug", &bDebug);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addSlider("Focal_Distance", 0, 1000, &focalDistance);
    gui->addSlider("Focal_Range", 0, 1000, &focalRange);
    gui->addSpacer();
    
    
    gui->addSlider("LineWidht", 0, 8, &radius);
    UIShader::setupUI();
}

void UIEdge::begin(){
    UIShader::begin();
    setUniform1f("_radius", radius);
	setUniform1f("_focalDistance", focalDistance);
	setUniform1f("_focalRange", focalRange);
    setUniform1i("_debug", bDebug);
    setUniform1i("_depth", bDepth);
}
