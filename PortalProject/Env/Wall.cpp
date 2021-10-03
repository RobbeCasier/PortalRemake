#include "stdafx.h"
#include "Wall.h"
#include "PhysxManager.h"
#include <ContentManager.h>
#include "..\..\Materials\Shadow\DiffuseMaterial_Shadow.h"

Wall::Wall(const DirectX::XMFLOAT3& position)
	: m_StartLocation{position}
{
}

void Wall::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto pBouncyMaterial = physX->createMaterial(0.9f, 0.9f, 0.1f);
	ModelComponent* pModelComponent = new ModelComponent(L"./Resources/Meshes/PortalWall.ovm");
	AddComponent(pModelComponent);

	pModelComponent->SetMaterial(3);

	GetTransform()->Translate(m_StartLocation);
	auto rigidBody = new RigidBodyComponent(true);
	AddComponent(rigidBody);
	rigidBody->SetCollisionGroup(CollisionGroupFlag::Group1);

	std::shared_ptr<physx::PxGeometry> boxGeometry(std::make_shared<physx::PxConvexMeshGeometry>(ContentManager::Load<physx::PxConvexMesh>(L"./Resources/Meshes/PortalWall.ovpc")));
	auto collider = new ColliderComponent(boxGeometry, *pBouncyMaterial);
	AddComponent(collider);

	SetTag(L"Wall");
}

void Wall::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void Wall::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
