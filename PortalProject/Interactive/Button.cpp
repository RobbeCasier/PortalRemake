#include "stdafx.h"
#include "Button.h"
#include "PhysxManager.h"
#include <ContentManager.h>

Button::Button(const DirectX::XMFLOAT3& position)
	: m_Position{position}
{
}

void Button::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//audio
	FMOD_RESULT result;
	result = FMOD::System_Create(&m_pAudioSystem);
	if (result != FMOD_OK)
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	result = m_pAudioSystem->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	m_pAudioSystem->createSound("./Sound/ButtonPress.wav", FMOD_DEFAULT, 0, &m_pButtonPressed);

	m_Position.y += 0.25f;
	GetTransform()->Translate(m_Position);
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto pBouncyMaterial = physX->createMaterial(0.0f, 0.0f, 0.0f);
	m_pModel = new ModelComponent(L"./Resources/Meshes/Button.ovm");
	m_pModel->SetMaterial(5);
	AddComponent(m_pModel);
	m_pModel->GetTransform()->Scale({ 0.2f, 0.2f, 0.2f });

	auto rigidBody = new RigidBodyComponent(true);
	AddComponent(rigidBody);
	rigidBody->SetCollisionGroup(CollisionGroupFlag::Group2);

	std::shared_ptr<physx::PxGeometry> buttonGeometry(std::make_shared<physx::PxConvexMeshGeometry>(ContentManager::Load<physx::PxConvexMesh>(L"./Resources/Meshes/Button.ovpc"), physx::PxMeshScale(0.2f)));
	auto collider = new ColliderComponent(buttonGeometry, *pBouncyMaterial);
	AddComponent(collider);

	std::shared_ptr<physx::PxGeometry> geom(std::make_shared<physx::PxBoxGeometry>(physx::PxBoxGeometry(4, 4, 4)));
	m_pTrigger = new ColliderComponent(geom, *pBouncyMaterial);
	AddComponent(m_pTrigger);
}

void Button::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_pTrigger->EnableTrigger(true);
	PhysicsCallback beginOverlap = std::bind(&Button::Overlap, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	SetOnTriggerCallBack(beginOverlap);
}

void Button::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void Button::Overlap(GameObject* triggerObject, GameObject* otherObject, TriggerAction action)
{
	UNREFERENCED_PARAMETER(triggerObject);
	UNREFERENCED_PARAMETER(otherObject);
	if (action != TriggerAction::ENTER)
	{
		--m_ActiveCount;
		if (m_ActiveCount == 0)
		{
			m_IsActive = false;
			m_pModel->SetMaterial(5);
		}
	}
	else
	{
		if (m_ActiveCount == 0)
			m_pAudioSystem->playSound(m_pButtonPressed, 0, false, 0);

		++m_ActiveCount;
		m_IsActive = true;
		m_pModel->SetMaterial(6);
		
	}
}
