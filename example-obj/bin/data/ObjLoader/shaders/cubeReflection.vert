#version 120

varying vec4 vPos;
varying vec3 vNormal;
varying vec3 vEye;
varying vec2 vTexCoord;

void main(){
	vec3 CameraPosition = vec4(gl_ModelViewMatrixInverse * normalize(vec4(1.))).xyz;

    vec4 ecPosition  = gl_Vertex;
    vec3 ecPosition3 = ecPosition.xyz / ecPosition.w;

    vEye = normalize(ecPosition3-CameraPosition);
    vNormal = gl_Normal;

    vPos =  vec4( gl_Vertex.xyz, 1.0 );
    vTexCoord = gl_MultiTexCoord0.xy;

    gl_Position    = ftransform();
    gl_FrontColor = gl_Color;
}