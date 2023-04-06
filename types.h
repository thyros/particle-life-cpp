#pragma once

struct Vector3d {
    float x { 0 };
    float y { 0 };

    void mul(float d) {
        x *= d;
        y *= d;
    }
};

struct Particle {
    Vector3d position;
    Vector3d velocity;
    int type;
};

struct PhysicsSettings {
    float containerSize = 0.13f;
    int particlesCount = 1000;
    int matrixSize;
    int preferredNumberOfThreads = 10;
    float defaultFriction = 0.1f;
};
