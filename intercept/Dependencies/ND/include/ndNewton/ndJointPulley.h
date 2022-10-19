/* Copyright (c) <2003-2022> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#ifndef __ND_JOINT_PULLEY_H__
#define __ND_JOINT_PULLEY_H__

#include "ndNewtonStdafx.h"
#include "ndJointBilateralConstraint.h"


class ndJointPulley: public ndJointBilateralConstraint
{
	public:
	D_CLASS_REFLECTION(ndJointPulley);
	D_NEWTON_API ndJointPulley(const ndLoadSaveBase::ndLoadDescriptor& desc);
	D_NEWTON_API ndJointPulley(ndFloat32 gearRatio,
		const ndVector& body0Pin, ndBodyKinematic* const body0,
		const ndVector& body1Pin, ndBodyKinematic* const body1);
	D_NEWTON_API virtual ~ndJointPulley();

	protected:
	D_NEWTON_API void JacobianDerivative(ndConstraintDescritor& desc);
	D_NEWTON_API void Save(const ndLoadSaveBase::ndSaveDescriptor& desc) const;

	ndFloat32 m_gearRatio;
};

#endif 

