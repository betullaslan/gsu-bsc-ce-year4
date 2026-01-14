#ifndef LIGHTING_H
#define LIGHTING_H

#include "Camera.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Dynamic lighting system with day-night cycle
class LightingSystem {
public:
    Vec3 lightPosition;
    Vec3 lightColor;
    float ambientStrength;
    float dayNightCycle;
    float cycleSpeed;
    
    LightingSystem()
        : lightPosition(5.0f, 10.0f, 5.0f),
          lightColor(1.0f, 1.0f, 1.0f),
          ambientStrength(0.3f),
          dayNightCycle(0.0f),
          cycleSpeed(0.1f) {
    }
    
    // Update lighting based on time progression
    void update(float deltaTime) {
        dayNightCycle += deltaTime * cycleSpeed;
        if (dayNightCycle > 2.0f * static_cast<float>(M_PI)) {
            dayNightCycle -= 2.0f * static_cast<float>(M_PI);
        }
        
        // Simulate sun/moon arc movement
        float sunHeight = static_cast<float>(sin(dayNightCycle)) * 15.0f;
        float sunDistance = static_cast<float>(cos(dayNightCycle)) * 10.0f;
        
        lightPosition.x = sunDistance;
        lightPosition.y = sunHeight > 0.0f ? sunHeight : -sunHeight * 0.3f;
        lightPosition.z = 5.0f;
        
        // Adjust light color based on time of day
        float dayFactor = (static_cast<float>(sin(dayNightCycle)) + 1.0f) / 2.0f;
        
        if (dayFactor > 0.5f) {
            // Daytime - bright white light
            lightColor.x = 1.0f;
            lightColor.y = 1.0f - (dayFactor - 0.5f) * 0.3f;
            lightColor.z = 0.9f;
            ambientStrength = 0.3f + dayFactor * 0.4f;
        } else {
            if (dayFactor > 0.2f) {
                // Sunset - orange light
                lightColor.x = 1.0f;
                lightColor.y = 0.5f;
                lightColor.z = 0.3f;
                ambientStrength = 0.2f + dayFactor * 0.3f;
            } else {
                // Night - dim blue moonlight
                lightColor.x = 0.3f;
                lightColor.y = 0.3f;
                lightColor.z = 0.6f;
                ambientStrength = 0.05f + dayFactor * 0.15f;
            }
        }
    }
    
    // Calculate sky color based on time of day
    Vec3 getSkyColor() const {
        float dayFactor = (static_cast<float>(sin(dayNightCycle)) + 1.0f) / 2.0f;
        
        if (dayFactor > 0.5f) {
            return Vec3(0.5f, 0.7f, 1.0f);
        } else if (dayFactor > 0.2f) {
            float t = (dayFactor - 0.2f) / 0.3f;
            return Vec3(0.8f - t * 0.3f, 0.4f + t * 0.3f, 0.3f + t * 0.7f);
        } else {
            return Vec3(0.05f, 0.05f, 0.15f);
        }
    }
};

#endif
