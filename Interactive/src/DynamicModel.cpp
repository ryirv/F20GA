#include "DynamicModel.hpp"
#include <iostream>

#define PI 3.1415926f
#define HALF_PI 1.5707963f


DynamicModel::DynamicModel(int i, vec3 p, vec3 r, vec3 s, string filename, string tp) {
    id = i;
    pos = p;
    rot = r;
    scl = s;
    content.LoadGLTF(filename, tp);
    testLineLine();
}

bool DynamicModel::lineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
  // calculate the direction of the lines
  float uA = ( (x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3) ) / ( (y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1) );
  float uB = ( (x2 - x1)*(y1 - y3) - (y2 - y1)*(x1 - x3)) / ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));

  // if uA and uB are between 0-1, lines are colliding
  return (uA >= 0.f && uA <= 1.f && uB >= 0.f && uB <= 1.f);
}

void DynamicModel::testLineLine() {
  // 2 lines that intersect each other
  float x1 = 0.f;
  float y1 = 0.f;
  float x2 = 10.f;
  float y2 = 10.f;
  float x3 = 0.f;
  float y3 = 10.f;
  float x4 = 10.f;
  float y4 = 0.f;

  bool result = lineLine(x1, y1, x2, y2, x3, y3, x4, y4);
  // if they intersect, test passed
  if (result) {
    std::cout << "Test passed" << std::endl;
  } else {
    std::cout << "Test failed" << std::endl;
    while(true) {
      
    }
  }
}

void DynamicModel::calculateCloseness() {
    float xx = DynamicModel::cameraX-pos.x;
    float zz = DynamicModel::cameraZ-pos.z;
    closeness = xx*xx + zz*zz + wi*0.5f;
}

bool DynamicModel::isHovering() {
    return DynamicModel::closestObject == this;
}


void DynamicModel::checkHovering() {
    bool yawAlign = false;
    bool pitchAlign = false;
    // YAW
    {
      const float H_PI = PI/2.f;
      float d_sin = sin(cameraYaw-PI+H_PI)*(wi/2.f);
      float d_cos = cos(cameraYaw-PI+H_PI)*(wi/2.f);
      float x1 = pos.x + d_sin;
      float z1 = pos.z + d_cos;
      float x2 = pos.x - d_sin;
      float z2 = pos.z - d_cos;
      // cout << "--------------" << endl;
      // cout << "x1: " << x1 << endl;
      // cout << "z1: " << z1 << endl;
      // cout << "x2: " << x2 << endl;
      // cout << "z2: " << z2 << endl;


      const float SELECT_FAR = 500.f;

      float beamX1 = DynamicModel::cameraX;
      float beamZ1 = DynamicModel::cameraZ;

      float beamX2 = DynamicModel::cameraX+sin(DynamicModel::cameraYaw)*SELECT_FAR;
      float beamZ2 = DynamicModel::cameraZ+cos(DynamicModel::cameraYaw)*SELECT_FAR;

      // cout << beamX1 << endl;
      // cout << beamZ1 << endl;
      // cout << beamX2 << endl;
      // cout << beamZ2 << endl;

      
      yawAlign = lineLine(x1, z1, x2, z2, beamX1, beamZ1, beamX2, beamZ2);
    }
    
    // Pitch
    {
      float d_sin = sin(PI-DynamicModel::cameraPitch)*(hi/2.f);
      float d_cos = cos(PI-DynamicModel::cameraPitch)*(hi/2.f);
      
      float ourx = sqrt(pow(pos.x-DynamicModel::cameraX, 2.f) + pow(pos.z-DynamicModel::cameraZ, 2.f));
      float oury = pos.y-DynamicModel::cameraY;
      
      
      // x lies on the same coordinate as closeness to the player
      float x1 = ourx - d_sin;
      // y lies on the same coordinate as how up the player is looking
      float y1 = oury - d_cos;
      float x2 = ourx + d_sin;
      float y2 = oury + d_cos;
      

      const float SELECT_FAR = 500.f;

      float beamX1 = 0.f;
      float beamY1 = 0.f;

      float beamX2 = sin(PI-(DynamicModel::cameraPitch-HALF_PI+PI))*SELECT_FAR;
      float beamY2 = cos(PI-(DynamicModel::cameraPitch-HALF_PI+PI))*SELECT_FAR;
      
      pitchAlign = lineLine(x1, y1, x2, y2, beamX1, beamY1, beamX2, beamY2);
    }

    static int counter = 0;
    counter++;

    if (yawAlign) {
      std::cout << "YAW" << counter << std::endl;
    }

    if (pitchAlign && yawAlign) {
        std::cout << "HOVER" << counter << std::endl;
      if (closeness < DynamicModel::closestVal) {
        DynamicModel::closestVal = closeness;
        DynamicModel::closestObject = this;
      }
    }
}
