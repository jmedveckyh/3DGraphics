#ifndef SNOWFALL_RENDERABLE_HPP
#define SNOWFALL_RENDERABLE_HPP

#include "../dynamics/DynamicSystem.hpp"
#include "../dynamics/ConstantForceField.hpp"
#include "DynamicSystemRenderable.hpp"
#include "ParticleRenderable.hpp"
#include "PuckRenderable.hpp"
#include "../HierarchicalRenderable.hpp"
#include "../dynamics/Plane.hpp"

#include <vector>
#include <glm/glm.hpp>

/**@brief Render terrain on the screen. */
class SnowFallRenderable : public HierarchicalRenderable
{
public:
    ~SnowFallRenderable();
    SnowFallRenderable(ShaderProgramPtr program, DynamicSystemPtr &system, DynamicSystemRenderablePtr &systemRenderable, ConstantForceFieldPtr &gravityForceField);

private:
    void spawnSnow();

    void do_draw();
    void do_animate(float dTime, float time);

    const float frequency = 0.01;
    float next = 0.0;
    DynamicSystemPtr system;
    DynamicSystemRenderablePtr systemRenderable;
    ShaderProgramPtr shader;
    ConstantForceFieldPtr gravityForceField;

    const float duration = 1.0;
    std::vector<float> timers;
    std::vector<PuckRenderablePtr> snowflakes;
};

typedef std::shared_ptr<SnowFallRenderable> SnowFallRenderablePtr;

#endif //PLANE_RENDERABLE_HPP
