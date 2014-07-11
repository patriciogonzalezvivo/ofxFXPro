varying vec2 texCoord;
uniform vec2 resolution;
uniform int flipXY;

void main(){
    if(flipXY == 1){
        texCoord = resolution - gl_MultiTexCoord0.st;
    }
    else{
        texCoord = gl_MultiTexCoord0.st;
    }
    gl_FrontColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}