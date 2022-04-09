////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2022 sk-landry
////////////////////////////////////////////////////////////
///////////////////////////HEADERS//////////////////////////
//NERO
#include <Nero/model/WheelJoint.h>
////////////////////////////////////////////////////////////
namespace nero
{
    WheelJoint::WheelJoint():
         PhysicJoint()
        ,m_Joint(nullptr)
    {
        m_Type = PhysicJoint::Wheel_Joint;
    }

    WheelJoint::~WheelJoint()
    {
        //dtor
    }

    void WheelJoint::setJoint(b2WheelJoint* joint)
    {
        m_Joint = joint;
    }

    b2WheelJoint* WheelJoint::getJoint() const
    {
        return m_Joint;
    }

    WheelJoint::Ptr WheelJoint::Cast(PhysicJoint::Ptr joint)
    {
        if(joint->getType() != PhysicJoint::Wheel_Joint)
            return nullptr;

        return  std::static_pointer_cast<WheelJoint>(joint);
    }

    b2Joint* WheelJoint::getGenericJoint()
    {
        return (b2Joint*)m_Joint;
    }
}
