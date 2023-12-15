#ifndef JOYSTICK_H
#define JOYSTICK_H

struct Joystick
{
    float leftAileron{ 0.0f }, rightAileron{ 0.0f }, elevator{0.0f}, rudder{0.0f}, throttle{0.0f};
};

#endif