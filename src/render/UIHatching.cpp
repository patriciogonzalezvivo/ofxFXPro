//
//  UIHatching.cpp
//
//  Created by Patricio Gonzalez Vivo on 7/4/14.
//
//

#include "UIHatching.h"

#include "UIHatching.h"

UIHatching::UIHatching(){
    
    vertexShader = "#version 120\n\n\
uniform float repeat;\n\
\n\
varying vec3 vNormal;\n\
varying vec2 vUv;\n\
varying vec3 vPosition;\n\
\n\
varying vec3 pos;\n\
\n\
void main() {\n\
    vec4 mvPosition = gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1.0 );\n\
    vPosition = (vec3(mvPosition)) / mvPosition.w;\n\
    vNormal = gl_NormalMatrix * gl_Normal;\n\
    vUv = gl_MultiTexCoord0.xy * repeat * 100.0;\n\
\n\
    gl_Position = gl_ProjectionMatrix * mvPosition;\n\
    gl_FrontColor = gl_Color;\n\
\n\
    pos = gl_Position.xyz;\n\
}";
    
    fragmentShader = "#version 120\n\n\
uniform sampler2D hatch1;\n\
uniform sampler2D hatch2;\n\
uniform sampler2D hatch3;\n\
uniform sampler2D hatch4;\n\
uniform sampler2D hatch5;\n\
uniform sampler2D hatch6;\n\
\n\
uniform float repeat;\n\
\n\
uniform float useRim;\n\
uniform float claroOscuro;\n\
\n\
uniform float dephtScale;\n\
uniform float dephtExp;\n\
\n\
uniform float border;\n\
\n\
uniform vec3 _inkColor;\n\
\n\
varying vec2 vUv;\n\
varying vec3 vNormal;\n\
varying vec3 vPosition;\n\
varying vec3 pos;\n\
\n\
vec4 hatch(float shading){\n\
    vec4 c = vec4(0.);\n\
    float step = 1. / 6.;\n\
    if( shading <= step ){\n\
        c = mix( texture2D( hatch6, vUv ), texture2D( hatch5, vUv ), 6. * shading );\n\
    } else if( shading > step && shading <= 2. * step ){\n\
        c = mix( texture2D( hatch5, vUv ), texture2D( hatch4, vUv) , 6. * ( shading - step ) );\n\
    } else if( shading > 2. * step && shading <= 3. * step ){\n\
        c = mix( texture2D( hatch4, vUv ), texture2D( hatch3, vUv ), 6. * ( shading - 2. * step ) );\n\
    } else if( shading > 3. * step && shading <= 4. * step ){\n\
        c = mix( texture2D( hatch3, vUv ), texture2D( hatch2, vUv ), 6. * ( shading - 3. * step ) );\n\
    } else if( shading > 4. * step && shading <= 5. * step ){\n\
        c = mix( texture2D( hatch2, vUv ), texture2D( hatch1, vUv ), 6. * ( shading - 4. * step ) );\n\
    } else if( shading > 5. * step ){\n\
        vec4 lightHatch = texture2D( hatch1, vUv );\n\
        c = mix( lightHatch, vec4( vec3(1.), min(claroOscuro,lightHatch.a) ), 6. * ( shading - 5. * step ) );\n\
    }\n\
    return c;\n\
}\n\
\n\
int lightsNumber = 8;\n\
void DirectionalLight(in int i, in vec3 normal,inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){\n\
    float nDotVP,nDotHV,pf;\n\
\n\
    nDotVP = max(0.0, dot(normal, normalize(vec3(gl_LightSource[i].position))));\n\
    nDotHV = max(0.0, dot(normal, vec3(gl_LightSource[i].halfVector)));\n\
\n\
    if (nDotVP == 0.0)\n\
        pf = 0.0;\n\
    else\n\
        pf = pow(nDotHV, gl_FrontMaterial.shininess);\n\
\n\
    ambient  += gl_LightSource[i].ambient;\n\
    diffuse  += gl_LightSource[i].diffuse * nDotVP;\n\
    specular += gl_LightSource[i].specular * pf;\n\
}\n\
void PointLight(in int i, in vec3 eye, in vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){\n\
    float nDotVP,nDotHV,pf,attenuation,d;\n\
    vec3 VP,halfVector;\n\
\n\
    VP = vec3(gl_LightSource[i].position) - ecPosition3;\n\
\n\
    d = length(VP);\n\
\n\
    VP = normalize(VP);\n\
\n\
    attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +\n\
                         gl_LightSource[i].linearAttenuation * d +\n\
                         gl_LightSource[i].quadraticAttenuation * d * d);\n\
\n\
    halfVector = normalize(VP + eye);\n\
\n\
    nDotVP = max(0.0, dot(normal, VP));\n\
    nDotHV = max(0.0, dot(normal, halfVector));\n\
\n\
    if (nDotVP == 0.0)\n\
        pf = 0.0;\n\
    else\n\
        pf = pow(nDotHV, gl_FrontMaterial.shininess);\n\
\n\
    ambient += gl_LightSource[i].ambient * attenuation;\n\
    diffuse += gl_LightSource[i].diffuse * nDotVP * attenuation;\n\
    specular += gl_LightSource[i].specular * pf * attenuation;\n\
}\n\
\n\
void SpotLight(in int i, in vec3 eye, vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){\n\
    float nDotVP,nDotHV,pf,spotDot,spotAttenuation,attenuation,d;\n\
    vec3 VP, halfVector;\n\
\n\
    VP = vec3(gl_LightSource[i].position) - ecPosition3;\n\
\n\
    d = length(VP);\n\
\n\
    VP = normalize(VP);\n\
\n\
    attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +\n\
                         gl_LightSource[i].linearAttenuation * d +\n\
                         gl_LightSource[i].quadraticAttenuation * d * d);\n\
\n\
    spotDot = dot(-VP, normalize(gl_LightSource[i].spotDirection));\n\
\n\
    if (spotDot < gl_LightSource[i].spotCosCutoff)\n\
        spotAttenuation = 0.0;\n\
    else\n\
        spotAttenuation = pow(spotDot, gl_LightSource[i].spotExponent);\n\
\n\
    attenuation *= spotAttenuation;\n\
\n\
    halfVector = normalize(VP + eye);\n\
\n\
    nDotVP = max(0.0, dot(normal, VP));\n\
    nDotHV = max(0.0, dot(normal, halfVector));\n\
\n\
    if (nDotVP == 0.0)\n\
        pf = 0.0;\n\
    else\n\
        pf = pow(nDotHV, gl_FrontMaterial.shininess);\n\
\n\
    ambient  += gl_LightSource[i].ambient * attenuation;\n\
    diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;\n\
    specular += gl_LightSource[i].specular * pf * attenuation;\n\
}\n\
\n\
vec4 calc_lighting_shade(in vec3 _ecPosition, in vec3 _normal) {\n\
    vec3 eye = vec3(0.0, 0.0, 1.0);\n\
\n\
    vec4 amb  = vec4(0.0);\n\
    vec4 diff = vec4(0.0);\n\
    vec4 spec = vec4(0.0);\n\
\n\
    for (int i = 0; i < lightsNumber; i++){\n\
        if (gl_LightSource[i].position.w == 0.0)\n\
            DirectionalLight(i, normalize(_normal), amb, diff, spec);\n\
        else if (gl_LightSource[i].spotCutoff == 180.0)\n\
            PointLight(i, eye, _ecPosition, normalize(_normal), amb, diff, spec);\n\
        else\n\
            SpotLight(i, eye, _ecPosition, normalize(_normal), amb, diff, spec);\n\
    }\n\
\n\
    float rim = max( 0., abs( dot( normalize( vNormal ), normalize( vPosition ) ) ) );\n\
\n\
    vec4 color = gl_FrontLightModelProduct.sceneColor +\n\
                amb * gl_FrontMaterial.ambient +\n\
                diff * gl_FrontMaterial.diffuse +\n\
                spec * gl_FrontMaterial.specular +\n\
                ( 1. - smoothstep( 0.0, 1., rim ) ) * useRim;\n\
\n\
    return hatch(length(color.rgb));\n\
}\n\
\n\
void main() {\n\
\n\
    float depth = clamp(pow( length( pos ) / (10.0+dephtScale*1000.), dephtExp*10. ),0., 1. );\n\
    vec4 color = vec4(0.);\n\
\n\
    if(!gl_FrontFacing){\n\
        color = calc_lighting_shade(vPosition,vNormal);\n\
\n\
        if(dephtScale>0. && dephtExp>0.){\n\
            color.a *= 1.0-depth;\n\
        }\n\
    }\n\
\n\
    float cosTheta = abs( dot( normalize(vPosition), vNormal) );\n\
    float fresnel = pow(1.0 - cosTheta, 30.);\n\
    if(border>0.){\n\
        if(fresnel>depth*(1.0-border )){\n\
            color = vec4(0.,0.,0.,1.);\n\
        }\n\
    }\n\
\n\
    if(gl_FrontFacing)\n\
        gl_FragColor = vec4(0.);\n\
    else\n\
        gl_FragColor = color;\n\
}";
    
    setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    
    linkProgram();
    
    extractUniforms(fragmentShader);
    
    ofDisableArbTex();
    for (int i = 0; i < 6; i++) {
        ofLoadImage(textures[i],"GUI/hatching/hatch_"+ofToString(i)+".png");
        textures[i].setTextureWrap(GL_REPEAT, GL_REPEAT);
    }
    ofEnableArbTex();
    
    inkColor.set(0);
}

string UIHatching::getClassName(){
    if(fragFile.isFile()){
        return fragFile.getBaseName();
    } else {
        return "Hatching";
    }
}

void UIHatching::setupUI(){
    
    //addUIColor("InkColor",inkColor);
    
    UIShader::setupUI();
}

void UIHatching::begin(){
    if (bEnable) {
        inkColor.update();
        
        UIShader::begin();
        setUniform3f("_inkColor",inkColor.r,inkColor.g,inkColor.b);
        for (int i = 0; i < 6; i++) {
            if(textures[i].isAllocated()){
                setUniformTexture("hatch"+ofToString(i+1), textures[i], i);
            }
        }
    }
}

void UIHatching::end(){
    if (bEnable) {
        UIShader::end();
    }
}