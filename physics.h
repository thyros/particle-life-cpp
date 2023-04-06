#pragma once

#include "loadDistributor.h"
#include "types.h"
#include <algorithm>
#include <atomic>
#include <functional>
#include <ranges>
#include <vector>

constexpr int containerNeighbors[9][2] {
    {-1,-1}, {0, -1}, {1, -1},
    {-1, 0}, {0, 0}, {1, 0},
    {-1, 1}, {0, 1}, {1, 1}
};

class Physics {
public:
    using TypeSetter = std::function<int ()>;
    using PositionSetter = std::function<Vector3d ()>;
    

    Physics(const PhysicsSettings& physicsSettings, TypeSetter typeSetter, PositionSetter positionSetter)
        : mParticles(physicsSettings.particlesCount, Particle {})
        , mSettings(physicsSettings)
    {
        const auto generateParticle = [&typeSetter, &positionSetter](auto& particle){ particle.type = typeSetter(); particle.position = positionSetter(); };
        std::ranges::for_each(mParticles, generateParticle);
    }

    void update() {
        makeContainers();

        std::atomic<bool> updateThreadsShouldRun {true};
        LoadDistributor loadDistributor;

        loadDistributor.distributeLoadEvenly(mParticles.size(), mSettings.preferredNumberOfThreads, [&updateThreadsShouldRun, this](int index) -> bool {
            if (!updateThreadsShouldRun) return false;
            updateVelocity(index);
            return true;
        });

        loadDistributor.distributeLoadEvenly(mParticles.size(), mSettings.preferredNumberOfThreads, [&updateThreadsShouldRun, this](int index) -> bool {
            if (!updateThreadsShouldRun) return false;
            updatePosition(index);
            return true;
        });
    }

    void updateVelocity(int index) {
         Particle& particle = mParticles[index];

        particle.velocity.mul(mSettings.defaultFriction);

        const int cx0 = (int) Math.floor((p.position.x + 1) / containerSize);
        const int cy0 = (int) Math.floor((p.position.y + 1) / containerSize);

    }

    void updatePosition(int index) {}


private:

    void makeContainers() {

        const float containerSize = mSettings.containerSize;
        const int nx = static_cast<int>(2.0 / containerSize);
        const int ny = static_cast<int>(2.0 / containerSize);

        if (mContainers.size() != static_cast<size_t>(nx * ny)) {
            mContainers = std::vector<int>(nx * ny, 0);
        } else {
            std::fill(begin(mContainers), end(mContainers), 0);
        }

        if (mParticlesBuffer.size() != mParticles.size()) {
            mParticlesBuffer = std::vector<Particle>(mParticles.size(), Particle {});
        }

        for (const Particle& p: mParticles) {
            const int ci = getContainerIndex(p.position, nx, ny, mSettings.containerSize);
            ++mContainers[ci];
        }

        int offset = 0;
        for (size_t i = 0; i < mContainers.size(); ++i) {
            const int capacity = mContainers[i];
            mContainers[i] = offset;
            offset += capacity;
        }

        for (const Particle& p: mParticles) {
            const int ci = getContainerIndex(p.position, nx, ny, mSettings.containerSize);
            const int i = mContainers[ci];
            mParticlesBuffer[i] = p;
            ++mContainers[ci];
        }

        std::swap(mParticles, mParticlesBuffer);
    }

    int getContainerIndex(const Vector3d& position, int nx, int ny, float containerSize) {
        int cx = static_cast<int>((position.x + 1) / containerSize);
        int cy = static_cast<int>((position.y + 1) / containerSize);

        // for solid borders
        if (cx == nx) {
            cx = nx - 1;
        }
        if (cy == ny) {
            cy = ny - 1;
        }

        return cx + cy * nx;

    }
    
    const PhysicsSettings& mSettings;
    std::vector<Particle> mParticles;
    std::vector<Particle> mParticlesBuffer;
    std::vector<int> mContainers;
    int mNx;
    int mNy;
    // matrix
};
