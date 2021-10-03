#include "stdafx.h"
#include "PortalBall.h"
#include "PhysxManager.h"
#include <GameScene.h>
#include <PhysxProxy.h>

PortalBall::PortalBall(DirectX::XMFLOAT3 startLocation, DirectX::XMFLOAT3 direction, bool isBluePortal)
	: m_IsBluePortal{ isBluePortal }
	, m_StartLocation { startLocation}
	, m_Direction{ direction }
{
}

void PortalBall::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if (m_IsBluePortal)
		m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/Sparks.png", 60);
	else
		m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/SparksOrange.png", 60);

	m_pParticleEmitter->SetVelocity(DirectX::XMFLOAT3(0, 0.0f, 0));
	m_pParticleEmitter->SetMinSize(0.5f);
	m_pParticleEmitter->SetMaxSize(1.0f);
	m_pParticleEmitter->SetMinEnergy(0.0f);
	m_pParticleEmitter->SetMaxEnergy(0.1f);
	m_pParticleEmitter->SetMinSizeGrow(3.5f);
	m_pParticleEmitter->SetMaxSizeGrow(5.5f);
	m_pParticleEmitter->SetMinEmitterRange(0.2f);
	m_pParticleEmitter->SetMaxEmitterRange(0.5f);
	m_pParticleEmitter->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.6f));
	AddComponent(m_pParticleEmitter);

}

void PortalBall::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	GetTransform()->Translate(m_StartLocation);
}

void PortalBall::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	DirectX::XMFLOAT3 curLoc = GetTransform()->GetPosition();
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&m_Direction);
	DirectX::XMVECTOR curLocVec = DirectX::XMLoadFloat3(&curLoc);
	float addedDistance = m_Speed * gameContext.pGameTime->GetElapsed();
	auto newLoc = DirectX::XMVectorAdd(curLocVec, DirectX::XMVectorScale(direction, addedDistance));
	//check if you not will overlap or pass an object
	CheckForPlaceableObject(gameContext, addedDistance);
	GetTransform()->Translate(newLoc);
}

void PortalBall::CheckForPlaceableObject(const GameContext& gameContext, const float& addedDistance)
{
	auto rayStart = ToPxVec3(GetTransform()->GetPosition());
	auto rayDir = ToPxVec3(m_Direction);
	physx::PxQueryFilterData filterData;
	filterData.data.word0 = ~physx::PxU32(CollisionGroupFlag::Group0);

	physx::PxRaycastBuffer hit;
	PhysxProxy* physxProxy = GetScene()->GetPhysxProxy();
	if (physxProxy->Raycast(rayStart, rayDir, addedDistance, hit, physx::PxHitFlag::eDEFAULT, filterData))
	{
		m_Hit = true;
		physx::PxRigidActor* selectedActor = hit.getAnyHit(0).actor;
		if (selectedActor)
		{
			auto actorData = selectedActor->userData;
			if (actorData != nullptr)
			{
				auto comp = reinterpret_cast<BaseComponent*>(actorData);
				if (comp)
				{
					auto tag = comp->GetGameObject()->GetTag();
					if (tag._Equal(L"Wall"))
					{
						m_CanPlacePortal = true;
						m_PortalLocation = ToXMFLOAT3(hit.getAnyHit(0).position);
						m_PortalDirection = ToXMFLOAT3(hit.getAnyHit(0).normal);
					}
				}
			}
		}
	}
}

