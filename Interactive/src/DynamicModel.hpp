#include <glm/glm.hpp>
#include "Content.hpp"

#pragma once

using namespace glm;

class DynamicModel
{
    public:
        
        vec3 pos;
        vec3 rot;
        vec3 scl;
        Content content;
        int id;

        // Make sure to update these before calling any of the model functions
        inline static float cameraX;
        inline static float cameraY;
        inline static float cameraZ;

        // Make sure to call these before calling any of the model functions
        inline static float cameraYaw;
        inline static float cameraPitch;
        inline static float flatSinDirectionYaw;
        inline static float flatCosDirectionYaw;
        inline static float flatSinDirectionPitch;
        inline static float flatCosDirectionPitch;

        inline static float closestVal;
        inline static DynamicModel* closestObject;
        
        // Just make em constants for now
        float wi = 2.5;
        float hi = 2.5;

        DynamicModel(int i, vec3 p, vec3 r, vec3 s, string filename, string tp);
        void calculateCloseness();
        void checkHovering();
        void setPosition(vec3 newPos);
        // NOTE TO ANYONE LOOKING HERE!
        // USE THIS!!!
        // THIS TELLS YOU WHEN THE USER IS OVERING OVER THAT ONE SPECIFIC
        // MODEL
        // AND IT DEFINITELY 100% WORKS
        bool isHovering();



    private:
        float closeness = 0.;
        bool lineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        void testLineLine();

};
