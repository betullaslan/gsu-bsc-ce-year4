#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 3D vector structure for position and direction
struct Vec3 {
    float x, y, z;
    
    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
    
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    
    Vec3 cross(const Vec3& v) const {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    
    Vec3 normalize() const {
        float len = sqrtf(x * x + y * y + z * z);
        if (len > 0.0f) return Vec3(x / len, y / len, z / len);
        return Vec3(0.0f, 0.0f, 0.0f);
    }
};

// 4x4 matrix for transformations
struct Mat4 {
    float m[16];
    
    Mat4() {
        for (int i = 0; i < 16; i++) m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }
    
    static Mat4 perspective(float fov, float aspect, float nearPlane, float farPlane) {
        Mat4 result;
        float tanHalfFov = tan(fov / 2.0f);
        
        result.m[0] = 1.0f / (aspect * tanHalfFov);
        result.m[5] = 1.0f / tanHalfFov;
        result.m[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result.m[11] = -1.0f;
        result.m[14] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        result.m[15] = 0.0f;
        
        return result;
    }
    
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalize();
        Vec3 s = f.cross(up).normalize();
        Vec3 u = s.cross(f);
        
        Mat4 result;
        result.m[0] = s.x;
        result.m[4] = s.y;
        result.m[8] = s.z;
        result.m[1] = u.x;
        result.m[5] = u.y;
        result.m[9] = u.z;
        result.m[2] = -f.x;
        result.m[6] = -f.y;
        result.m[10] = -f.z;
        result.m[12] = -(s.x * eye.x + s.y * eye.y + s.z * eye.z);
        result.m[13] = -(u.x * eye.x + u.y * eye.y + u.z * eye.z);
        result.m[14] = f.x * eye.x + f.y * eye.y + f.z * eye.z;
        
        return result;
    }
};

// Camera class for 3D scene navigation
class Camera {
public:
    Vec3 position;
    Vec3 front;
    Vec3 up;
    Vec3 right;
    Vec3 worldUp;
    
    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    
    Camera(Vec3 pos = Vec3(0.0f, 2.0f, 5.0f), 
           Vec3 up = Vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, 
           float pitch = 0.0f)
        : position(pos), worldUp(up), yaw(yaw), pitch(pitch),
          movementSpeed(2.5f), mouseSensitivity(0.1f), zoom(45.0f) {
        updateCameraVectors();
    }
    
    Mat4 getViewMatrix() const {
        return Mat4::lookAt(position, position + front, up);
    }
    
    // Process keyboard input for camera movement
    void processKeyboard(int direction, float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        
        if (direction == 0) position = position + front * velocity;
        if (direction == 1) position = position - front * velocity;
        if (direction == 2) position = position - right * velocity;
        if (direction == 3) position = position + right * velocity;
        if (direction == 4) position = position + worldUp * velocity;
        if (direction == 5) position = position - worldUp * velocity;
    }
    
    // Process mouse movement for camera rotation
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;
        
        yaw += xoffset;
        pitch += yoffset;
        
        if (constrainPitch) {
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }
        
        updateCameraVectors();
    }
    
    // Process mouse scroll for zoom
    void processMouseScroll(float yoffset) {
        zoom -= yoffset;
        if (zoom < 1.0f) zoom = 1.0f;
        if (zoom > 45.0f) zoom = 45.0f;
    }
    
private:
    // Update camera direction vectors based on yaw and pitch
    void updateCameraVectors() {
        Vec3 newFront;
        newFront.x = static_cast<float>(cos(yaw * M_PI / 180.0) * cos(pitch * M_PI / 180.0));
        newFront.y = static_cast<float>(sin(pitch * M_PI / 180.0));
        newFront.z = static_cast<float>(sin(yaw * M_PI / 180.0) * cos(pitch * M_PI / 180.0));
        front = newFront.normalize();
        
        right = front.cross(worldUp).normalize();
        up = right.cross(front).normalize();
    }
};

#endif
