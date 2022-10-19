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

#ifndef __ND_SAVE_LOAD_SYSTEM_H__
#define __ND_SAVE_LOAD_SYSTEM_H__

#include "ndCoreStdafx.h"
#include "ndTypes.h"
#include "ndTree.h"
#include "ndClassAlloc.h"

class ndBody;
class ndShape;
class ndModel;
class ndBodyKinematic;
class ndShapeInstance;
class ndCharacterNode;
class ndCharacterNode;
class ndJointBilateralConstraint;

class ndShapeLoaderCache : public ndTree<const ndShapeInstance, ndInt32>
{
};

class ndBodyLoaderCache: public ndTree<const ndBody*, ndInt32>
{
};

class ndJointLoaderCache: public ndTree<const ndJointBilateralConstraint*, ndInt32>
{
};

class ndModelLoaderCache: public ndTree<const ndModel*, ndInt32>
{
};

class ndLoadSaveBase: public ndClassAlloc
{
	public:
	class ndLoadDescriptor
	{
		public:
		ndLoadDescriptor()
			:m_assetPath(nullptr)
			,m_rootNode(nullptr)
			,m_bodyMap(nullptr)
			,m_modelMap(nullptr)
			,m_shapeMap(nullptr)
			,m_jointMap(nullptr)
			,m_parentModelNode(nullptr)
		{
		}

		ndLoadDescriptor(const ndLoadDescriptor& desc)
			:m_assetPath(desc.m_assetPath)
			,m_rootNode(desc.m_rootNode->FirstChild())
			,m_bodyMap(desc.m_bodyMap)
			,m_modelMap(desc.m_modelMap)
			,m_shapeMap(desc.m_shapeMap)
			,m_jointMap(desc.m_jointMap)
			,m_parentModelNode(desc.m_parentModelNode)
		{
		}

		const char* m_assetPath;
		const nd::TiXmlNode* m_rootNode;
		const ndBodyLoaderCache* m_bodyMap;
		const ndModelLoaderCache* m_modelMap;
		const ndShapeLoaderCache* m_shapeMap;
		const ndJointLoaderCache* m_jointMap;
		const ndCharacterNode* m_parentModelNode;
	};

	class ndSaveDescriptor
	{
		public:
		ndSaveDescriptor()
			:m_assetPath(nullptr)
			,m_assetName(nullptr)
			,m_rootNode(nullptr)
			,m_nodeNodeHash(0)
			,m_shapeNodeHash(0)
			,m_body0NodeHash(0)
			,m_body1NodeHash(0)
			,m_assetIndex(0)
			,m_shapeMap(nullptr)
			,m_bodyMap(nullptr)
			,m_jointMap(nullptr)
		{
		}

		ndSaveDescriptor(const ndSaveDescriptor& desc, nd::TiXmlNode* const rootNode)
			:m_assetPath(desc.m_assetPath)
			,m_assetName(desc.m_assetName)
			,m_rootNode(rootNode)
			,m_nodeNodeHash(desc.m_nodeNodeHash)
			,m_shapeNodeHash(desc.m_shapeNodeHash)
			,m_body0NodeHash(desc.m_body0NodeHash)
			,m_body1NodeHash(desc.m_body1NodeHash)
			,m_assetIndex(desc.m_assetIndex)
			,m_shapeMap(desc.m_shapeMap)
			,m_bodyMap(desc.m_bodyMap)
			,m_jointMap(desc.m_jointMap)
		{
		}

		const char* m_assetPath;
		const char* m_assetName;
		nd::TiXmlNode* m_rootNode;
		ndInt32 m_nodeNodeHash;
		ndInt32 m_shapeNodeHash;
		ndInt32 m_body0NodeHash;
		ndInt32 m_body1NodeHash;
		mutable ndInt32 m_assetIndex;
		ndTree<ndInt32, const ndShape*>* m_shapeMap;
		ndTree<ndInt32, const ndBodyKinematic*>* m_bodyMap;
		ndTree<ndInt32, const ndJointBilateralConstraint*>* m_jointMap;
	};

	virtual ~ndLoadSaveBase()
	{
	}

	virtual void* CreateClass(const ndLoadDescriptor&)
	{
		ndAssert(0);
		return nullptr;
	}
};

D_CORE_API void* LoadClass(const char* const className, const ndLoadSaveBase::ndLoadDescriptor& desc);
D_CORE_API void RegisterLoaderClass(const char* const className, ndLoadSaveBase* const loaderClass);

template<class T>
class ndLoadSaveClass: public ndLoadSaveBase
{
	public:
	ndLoadSaveClass<T>(const char* const className)
	{
		RegisterLoaderClass(className, this);
	}

	virtual void* CreateClass(const ndLoadDescriptor& desc)
	{
		return new T(desc);
	}
};

#define D_CLASS_REFLECTION(Class)								\
	const virtual char* SubClassName() {return #Class;}			\
	static const char* ClassName() {return #Class;}				\
	static ndLoadSaveClass<Class> __classLoader__;

#define D_CLASS_REFLECTION_IMPLEMENT_LOADER(Class) \
	ndLoadSaveClass<Class> Class::__classLoader__(#Class);

#define D_CLASS_REFLECTION_LOAD_NODE(castType,className,desc) \
	(castType*)LoadClass(className, desc);

#endif

