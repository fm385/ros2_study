#ifndef SPIN_MOTION_CONTROLLER_HPP
#define SPIN_MOTION_CONTROLLER_HPP

#include "motion_control_interface.hpp"

namespace motion_control_system
{
    class SpinMotionController : public MotionController
    {
    public:
        SpinMotionController() = default;
        virtual ~SpinMotionController() = default;

        virtual void start() override;
        virtual void stop() override;
    };

} // namespace motion_control_system

#endif