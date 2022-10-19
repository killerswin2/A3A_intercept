/* Copyright (c) <2003-2022> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef __ND_CHARACTER_INVERSE_DYNAMICS_NODE_H__
#define __ND_CHARACTER_INVERSE_DYNAMICS_NODE_H__

#include "ndNewtonStdafx.h"
#include "ndCharacterNode.h"

class ndCharacterInverseDynamicNode: public ndCharacterNode 
{
	public:
	D_CLASS_REFLECTION(ndCharacterInverseDynamicNode);
	D_NEWTON_API ndCharacterInverseDynamicNode(const ndCharacterLoadDescriptor& desc);
	D_NEWTON_API ndCharacterInverseDynamicNode(const ndMatrix& matrixInGlobalSpace, ndBodyDynamic* const body, ndCharacterNode* const parent);
	D_NEWTON_API virtual ~ndCharacterInverseDynamicNode ();

	virtual ndBodyDynamic* GetBody() const;
	virtual ndJointBilateralConstraint* GetJoint() const;

	protected:
	void Save(const ndCharacterSaveDescriptor& desc) const;

	ndBodyDynamic* m_body;
	ndJointBilateralConstraint* m_joint;
};

inline ndBodyDynamic* ndCharacterInverseDynamicNode::GetBody() const
{
	return m_body;
}

inline ndJointBilateralConstraint* ndCharacterInverseDynamicNode::GetJoint() const
{
	return m_joint;
}

#endif