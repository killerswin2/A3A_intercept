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

#ifndef __ND_WORLD_H__
#define __ND_WORLD_H__

#include "ndNewtonStdafx.h"
#include "ndJointList.h"
#include "ndModelList.h"
#include "ndSkeletonList.h"
#include "ndBodyParticleSetList.h"

class ndWorld;
class ndModel;
class ndBodyDynamic;
class ndRayCastNotify;
class ndDynamicsUpdate;
class ndConvexCastNotify;
class ndBodiesInAabbNotify;
class ndJointBilateralConstraint;

#define D_NEWTON_ENGINE_MAJOR_VERSION 4
#define D_NEWTON_ENGINE_MINOR_VERSION 00

#define D_SLEEP_ENTRIES			8

D_MSV_NEWTON_ALIGN_32
class ndWorld: public ndClassAlloc
{
	public:
	enum ndSolverModes
	{	
		ndStandardSolver,
		ndSimdSoaSolver,
		ndSimdAvx2Solver,
		ndCudaSolver,
		ndOpenclSolver1,
		ndOpenclSolver2,
	};

	D_NEWTON_API ndWorld();
	D_NEWTON_API virtual ~ndWorld();

	D_NEWTON_API virtual void CleanUp();

	ndInt32 GetEngineVersion() const;

	void Sync() const;
	void Update(ndFloat32 timestep);
	void CollisionUpdate(ndFloat32 timestep);

	virtual void OnPostUpdate(ndFloat32 timestep);

	ndInt32 GetThreadCount() const;
	void SetThreadCount(ndInt32 count);

	ndInt32 GetSubSteps() const;
	void SetSubSteps(ndInt32 subSteps);

	ndSolverModes GetSelectedSolver() const;
	D_NEWTON_API void SelectSolver(ndSolverModes solverMode);

	D_NEWTON_API bool IsGPU() const;
	D_NEWTON_API const char* GetSolverString() const;

	D_NEWTON_API virtual bool AddBody(ndBody* const body);
	D_NEWTON_API virtual void RemoveBody(ndBody* const body);
	D_NEWTON_API virtual void DeleteBody(ndBody* const body);

	D_NEWTON_API virtual void AddJoint(ndJointBilateralConstraint* const joint);
	D_NEWTON_API virtual void RemoveJoint(ndJointBilateralConstraint* const joint);

	D_NEWTON_API virtual void AddModel(ndModel* const model);
	D_NEWTON_API virtual void RemoveModel(ndModel* const model);

	const ndBodyList& GetBodyList() const;
	const ndJointList& GetJointList() const;
	const ndModelList& GetModelList() const;
	const ndContactArray& GetContactList() const;
	const ndSkeletonList& GetSkeletonList() const;
	const ndBodyParticleSetList& GetParticleList() const;

	ndBodyKinematic* GetSentinelBody() const;

	ndInt32 GetSolverIterations() const;
	void SetSolverIterations(ndInt32 iterations);

	ndScene* GetScene() const;

	ndFloat32 GetUpdateTime() const;
	ndUnsigned32 GetFrameNumber() const;
	ndUnsigned32 GetSubFrameNumber() const;
	ndFloat32 GetAverageUpdateTime() const;
	ndFloat32 GetExtensionAverageUpdateTime() const;

	ndContactNotify* GetContactNotify() const;
	void SetContactNotify(ndContactNotify* const notify);

	void DebugScene(ndSceneTreeNotiFy* const notify);
	void SendBackgroundTask(ndBackgroundTask* const job);

	D_NEWTON_API void ClearCache();
	D_NEWTON_API void BodiesInAabb(ndBodiesInAabbNotify& callback) const;
	D_NEWTON_API bool RayCast(ndRayCastNotify& callback, const ndVector& globalOrigin, const ndVector& globalDest) const;
	D_NEWTON_API bool ConvexCast(ndConvexCastNotify& callback, const ndShapeInstance& convexShape, const ndMatrix& globalOrigin, const ndVector& globalDest) const;

	private:
	void ThreadFunction();
	void PostUpdate(ndFloat32 timestep);
	
	protected:
	D_NEWTON_API virtual void UpdateSkeletons();
	D_NEWTON_API virtual void UpdateTransforms();
	D_NEWTON_API virtual void PostModelTransform();

	private:
	class dgSolverProgressiveSleepEntry
	{
		public:
		ndFloat32 m_maxAccel;
		ndFloat32 m_maxVeloc;
		ndInt32 m_steps;
	};

	class ndIslandMember
	{
		public:
		ndBodyKinematic* m_root;
		ndBodyKinematic* m_body;
	};

	void ModelUpdate();
	void ModelPostUpdate();
	void CalculateAverageUpdateTime();
	void SubStepUpdate(ndFloat32 timestep);
	void ParticleUpdate(ndFloat32 timestep);

	bool SkeletonJointTest(ndJointBilateralConstraint* const jointA) const;
	static ndInt32 CompareJointByInvMass(const ndJointBilateralConstraint* const jointA, const ndJointBilateralConstraint* const jointB, void* notUsed);

	ndScene* m_scene;
	ndDynamicsUpdate* m_solver;
	ndJointList m_jointList;
	ndModelList m_modelList;
	ndSkeletonList m_skeletonList;
	ndBodyParticleSetList m_particleSetList;
	ndArray<ndSkeletonContainer*> m_activeSkeletons;
	ndFloat32 m_timestep;
	ndFloat32 m_freezeAccel2;
	ndFloat32 m_freezeSpeed2;
	ndFloat32 m_averageUpdateTime;
	ndFloat32 m_averageTimestepAcc;
	ndFloat32 m_averageFramesCount;
	ndFloat32 m_lastExecutionTime;
	ndFloat32 m_extensionAverageUpdateTime;
	ndFloat32 m_extensionAverageTimestepAcc;

	dgSolverProgressiveSleepEntry m_sleepTable[D_SLEEP_ENTRIES];

	ndInt32 m_subSteps;
	ndSolverModes m_solverMode;
	ndInt32 m_solverIterations;
	bool m_inUpdate;
	bool m_collisionUpdate;

	friend class ndScene;
	friend class ndWorldScene;
	friend class ndBodyDynamic;
	friend class ndDynamicsUpdate;
	friend class ndSkeletonContainer;
	friend class ndDynamicsUpdateSoa;
	friend class ndDynamicsUpdateAvx2;
	friend class ndDynamicsUpdateCuda;
	friend class ndDynamicsUpdateOpencl;
} D_GCC_NEWTON_ALIGN_32;

inline void ndWorld::Sync() const
{
	m_scene->Sync();
}

inline ndInt32 ndWorld::GetThreadCount() const
{
	return m_scene->GetThreadCount();
}

inline void ndWorld::SetThreadCount(ndInt32 count)
{
	m_scene->SetThreadCount(count);
	m_scene->m_backgroundThread.SetThreadCount(count);
}

inline ndInt32 ndWorld::GetSubSteps() const
{
	return m_subSteps;
}

inline void ndWorld::SetSubSteps(ndInt32 subSteps)
{
	m_subSteps = ndClamp(subSteps, 1, 16);
}

inline ndScene* ndWorld::GetScene() const
{
	return m_scene;
}

inline ndInt32 ndWorld::GetSolverIterations() const
{
	return m_solverIterations;
}

inline void ndWorld::SetSolverIterations(ndInt32 iterations)
{
	m_solverIterations = ndInt32(ndMax(4, iterations));
}

inline ndContactNotify* ndWorld::GetContactNotify() const
{
	return m_scene->GetContactNotify();
}

inline void ndWorld::SetContactNotify(ndContactNotify* const notify)
{
	m_scene->SetContactNotify(notify);
}

inline ndBodyKinematic* ndWorld::GetSentinelBody() const
{
	return m_scene->GetSentinelBody();
}

inline const ndBodyList& ndWorld::GetBodyList() const
{
	return m_scene->GetBodyList();
}

inline const ndJointList& ndWorld::GetJointList() const
{
	return m_jointList;
}

inline const ndContactArray& ndWorld::GetContactList() const
{
	return m_scene->GetContactArray();
}

inline const ndSkeletonList& ndWorld::GetSkeletonList() const
{
	return m_skeletonList;
}

inline const ndBodyParticleSetList& ndWorld::GetParticleList() const
{
	return m_particleSetList;
}

inline const ndModelList& ndWorld::GetModelList() const
{
	return m_modelList;
}

inline ndFloat32 ndWorld::GetUpdateTime() const
{
	return m_lastExecutionTime;
}

inline ndFloat32 ndWorld::GetAverageUpdateTime() const
{
	return m_averageUpdateTime;
}

inline ndFloat32 ndWorld::GetExtensionAverageUpdateTime() const
{
	return m_extensionAverageUpdateTime;
}

inline ndUnsigned32 ndWorld::GetFrameNumber() const
{
	return m_scene->m_frameNumber;
}

inline ndUnsigned32 ndWorld::GetSubFrameNumber() const
{
	return m_scene->m_subStepNumber;
}

inline void ndWorld::OnPostUpdate(ndFloat32)
{
}

inline void ndWorld::DebugScene(ndSceneTreeNotiFy* const notify)
{
	m_scene->DebugScene(notify);
}

inline void ndWorld::CollisionUpdate(ndFloat32 timestep)
{
	// wait until previous update complete.
	Sync();
	m_timestep = timestep;

	// update the next frame asynchronous 
	m_collisionUpdate = true;
	m_scene->TickOne();
}

inline void ndWorld::Update(ndFloat32 timestep)
{
	// wait until previous update complete.
	Sync();

	// save time state for use by the update callback
	m_timestep = timestep;

	// update the next frame asynchronous 
	m_collisionUpdate = false;
	m_scene->TickOne();
}

inline ndWorld::ndSolverModes ndWorld::GetSelectedSolver() const
{
	return m_solverMode;
}

inline ndInt32 ndWorld::GetEngineVersion() const
{
	return D_NEWTON_ENGINE_MAJOR_VERSION * 100 + D_NEWTON_ENGINE_MINOR_VERSION;
}

inline void ndWorld::SendBackgroundTask(ndBackgroundTask* const job)
{
	m_scene->SendBackgroundTask(job);
}
#endif
