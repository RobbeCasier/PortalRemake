#include "stdafx.h"
#include "Floor.h"
#include "PhysxManager.h"
#include <ContentManager.h>

Floor::Floor(const DirectX::XMFLOAT3& position)
	: m_StartLocation{ position }
{
}

void Floor::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto pBouncyMaterial = physX->createMaterial(0.5f, 0.5f, 1.f);
	ModelComponent* pModelComponent = new ModelComponent(L"./Resources/Meshes/PortalPlane.ovm");
	AddComponent(pModelComponent);

	pModelComponent->SetMaterial(4);

	GetTransform()->Translate(m_StartLocation);
	auto rigidBody = new RigidBodyComponent(true);
	AddComponent(rigidBody);
	rigidBody->SetCollisionGroup(CollisionGroupFlag::Group1);
	rigidBody->SetCollisionIgnoreGroups(CollisionGroupFlag::Group1);

	std::shared_ptr<physx::PxGeometry> boxGeometry(std::make_shared<physx::PxConvexMeshGeometry>(ContentManager::Load<physx::PxConvexMesh>(L"./Resources/Meshes/PortalPlane.ovpc")));
	AddComponent(new ColliderComponent(boxGeometry, *pBouncyMaterial));
}

void Floor::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void Floor::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
