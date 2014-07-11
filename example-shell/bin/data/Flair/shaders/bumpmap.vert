#version 120

varying vec4 vPos;

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 vEye;

void main() {

    vPos =  vec4( gl_Vertex.xyz, 1.0 );
    vTexCoord = gl_MultiTexCoord0.xy;

    vNormal = normalize(gl_NormalMatrix * gl_Normal);

    vec4 eyeSpaceVertexPos = gl_ModelViewMatrix * gl_Vertex;
    vEye = (vec3(eyeSpaceVertexPos)) / eyeSpaceVertexPos.w;

    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1.0 );
    gl_FrontColor = gl_Color;
}