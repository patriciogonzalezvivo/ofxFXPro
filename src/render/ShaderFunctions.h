//
//  ShaderFunctions.h
//
//  Created by Patricio Gonzalez Vivo on 8/5/14.
//
//

#include "ofMain.h"

const string lightsFunctions = "\n\
int lightsNumber = 8;\n\
\n\
void DirectionalLight(in int i, in vec3 normal,inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){\n\
    float nDotVP;\n\
    float nDotHV;\n\
    float pf;\n\
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
\n\
void PointLight(in int i, in vec3 eye, in vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){\n\
    float nDotVP;\n\
    float nDotHV;\n\
    float pf;\n\
    float attenuation;\n\
    float d;\n\
    vec3  VP;\n\
    vec3  halfVector;\n\
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
    float nDotVP, nDotHV, pf, spotDot, spotAttenuation, attenuation, d;\n\
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
vec4 calc_lighting(in vec3 _ecPosition, in vec3 _normal, in vec4 _ambientColor, in vec4 _diffuseColor, in vec4 _specularColor) {\n\
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
    return  amb * _ambientColor +\n\
            diff * _diffuseColor +\n\
            spec * _specularColor;\n\
}\n\
\n\
vec4 calc_lighting_color(in vec3 _ecPosition, in vec3 _normal) {\n\
    return calc_lighting(_ecPosition,_normal,gl_FrontMaterial.ambient,gl_FrontMaterial.diffuse,gl_FrontMaterial.specular);\n\
}\n";

const string triplanarFunctions ="\n\
vec3 getTriPlanarBlend(vec3 _wNorm){\n\
    vec3 blending = abs( _wNorm );\n\
    blending = normalize(max(blending, 0.00001));\n\
    float b = (blending.x + blending.y + blending.z);\n\
    blending /= vec3(b, b, b);\n\
    return blending;\n\
}\n\
\n\
vec3 getTriplanarNormal(sampler2D _NormalMap, vec3 _vertexPosistion, vec3 _vertexNormal, float _repeat, float _scale){\n\
    vec3 blending = getTriPlanarBlend(_vertexNormal);\n\
    vec3 xaxis = texture2D( _NormalMap, _vertexPosistion.yz * _repeat).rgb;\n\
    vec3 yaxis = texture2D( _NormalMap, _vertexPosistion.xz * _repeat).rgb;\n\
    vec3 zaxis = texture2D( _NormalMap, _vertexPosistion.xy * _repeat).rgb;\n\
    vec3 normalTex = xaxis * blending.x + xaxis * blending.y + zaxis * blending.z;\n\
    normalTex = normalTex * 2.0 - 1.0;\n\
    normalTex.xy *= _scale*2.0;\n\
    normalTex = normalize( normalTex );\n\
    \n\
    vec3 T = vec3(0.,1.,0.);\n\
    vec3 BT = normalize( cross( _vertexNormal, T ) * 1.0 );\n\
    mat3 tsb = mat3( normalize( T ), normalize( BT ), normalize( _vertexNormal ) );\n\
    return tsb * normalTex;\n\
}\n";