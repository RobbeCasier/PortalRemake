#include "stdafx.h"
#include "MovableBox.h"
#include "PhysxManager.h"
#include "..\..\Materials\ColorMaterial.h"
#include "..\..\Materials\UberMaterial.h"
#include <ContentManager.h>
#include "..\..\Materials\Shadow\DiffuseMaterial_Shadow.h"

MovableBox::MovableBox(DirectX::XMFLOAT3 startLocation)
	: m_StartLocation{startLocation}
{
}

void MovableBox::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto pBouncyMaterial = physX->createMaterial(0.3f, 0.8f, 0.05f);
	ModelComponent* pModelComponent = new ModelComponent(L"./Resources/Meshes/CompanionBlock.ovm");
	AddComponent(pModelComponent);

	pModelComponent->SetMaterial(2);

	auto rigidBody = new RigidBodyComponent();
	AddComponent(rigidBody);
	rigidBody->SetCollisionGroup(CollisionGroupFlag::Group2);


	std::shared_ptr<physx::PxGeometry> boxGeometry(std::make_shared<physx::PxConvexMeshGeometry>(ContentManager::Load<physx::PxConvexMesh>(L"./Resources/Meshes/CompanionBlock.ovpc"), physx::PxMeshScale(2.5f)));
	AddComponent(new ColliderComponent(boxGeometry, *pBouncyMaterial));
}

void MovableBox::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	GetTransform()->Scale(2.5f, 2.5f, 2.5f);
	GetTransform()->Translate(m_StartLocation);
}

void MovableBox::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void MovableBox::SetStartLocation(const float& x, const float& y, const float& z)
{
	GetTransform()->Translate(x, y, z);
}
