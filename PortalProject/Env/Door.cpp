#include "stdafx.h"
#include "Door.h"
#include <PhysxManager.h>
#include <ContentManager.h>
#include <GameScene.h>
#include "../Interactive/Button.h"

Door::Door(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation)
	: m_Position{position}
	, m_Rotation{rotation}
{
}

void Door::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//setup audio
	FMOD_RESULT result;
	result = FMOD::System_Create(&m_pAudioSystem);
	if (result != FMOD_OK)
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	result = m_pAudioSystem->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	//setup sounds
	m_pAudioSystem->createSound("./Sound/DoorOpen.wav", FMOD_DEFAULT, 0, &m_pOpen);
	m_pAudioSystem->createSound("./Sound/DoorClose.wav", FMOD_DEFAULT, 0, &m_pClose);

	GetTransform()->Translate(m_Position);
	GetTransform()->Rotate(m_Rotation);
	GetTransform()->Scale({ 0.15f, 0.15f, 0.15f });

	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto pBouncyMaterial = physX->createMaterial(0.0f, 0.0f, 0.0f);
	//Door Frame
	ModelComponent* pModel = new ModelComponent(L"./Resources/Meshes/DoorFrame.ovm");
	pModel->SetMaterial(7);
	AddComponent(pModel);

	//Left Door
	m_pLeftDoor = new GameObject();
	pModel = new ModelComponent(L"./Resources/Meshes/LeftDoor.ovm");
	pModel->SetMaterial(7);
	m_pLeftDoor->AddComponent(pModel);

	auto rigidBody = new RigidBodyComponent();
	rigidBody->SetKinematic(true);
	m_pLeftDoor->AddComponent(rigidBody);
	rigidBody->SetCollisionGroup(CollisionGroupFlag::Group2);

	std::shared_ptr<physx::PxGeometry> doorGeometry(std::make_shared<physx::PxConvexMeshGeometry>(ContentManager::Load<physx::PxConvexMesh>(L"./Resources/Meshes/LeftDoor.ovpc"), physx::PxMeshScale(0.15f)));
	auto collider = new ColliderComponent(doorGeometry, *pBouncyMaterial);
	m_pLeftDoor->AddComponent(collider);
	this->GetScene()->AddChild(m_pLeftDoor);
	m_pLeftDoor->GetTransform()->Scale({ 0.15f, 0.15f, 0.15f });

	//Right Door
	m_pRightDoor = new GameObject();
	pModel = new ModelComponent(L"./Resources/Meshes/RightDoor.ovm");
	pModel->SetMaterial(7);
	m_pRightDoor->AddComponent(pModel);

	rigidBody = new RigidBodyComponent();
	rigidBody->SetKinematic(true);
	m_pRightDoor->AddComponent(rigidBody);
	rigidBody->SetCollisionGroup(CollisionGroupFlag::Group2);

	doorGeometry = std::make_shared<physx::PxConvexMeshGeometry>(ContentManager::Load<physx::PxConvexMesh>(L"./Resources/Meshes/RightDoor.ovpc"), physx::PxMeshScale(0.15f));
	collider = new ColliderComponent(doorGeometry, *pBouncyMaterial);
	m_pRightDoor->AddComponent(collider);
	this->GetScene()->AddChild(m_pRightDoor);
	m_pRightDoor->GetTransform()->Scale({ 0.15f, 0.15f, 0.15f });

}

void Door::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_pLeftDoor->GetComponent<RigidBodyComponent>()->GetTransform()->Translate(m_Position);
	m_pLeftDoor->GetComponent<RigidBodyComponent>()->GetTransform()->Rotate(90,180,0);
	m_pRightDoor->GetComponent<RigidBodyComponent>()->GetTransform()->Translate(m_Position);
	m_pRightDoor->GetComponent<RigidBodyComponent>()->GetTransform()->Rotate(90, 180, 0);

	m_EndPositionRight = ToPxVec3(GetTransform()->GetPosition()) + (ToPxVec3(GetTransform()->GetRight()) * m_DisplacementDoor);
	m_EndPositionLeft = ToPxVec3(GetTransform()->GetPosition()) - (ToPxVec3(GetTransform()->GetRight()) * m_DisplacementDoor);
}

void Door::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//Check buttons
	bool allActive = false;
	for (auto button : m_Buttons)
	{
		if (button->IsActive())
			allActive = true;
		else
		{
			allActive = false;
			break;
		}
	}
	OpenDoor(allActive);

	if (m_IsOpen)
	{
		const auto& rPos = m_pRightDoor->GetTransform()->GetPosition();
		const auto& lPos = m_pLeftDoor->GetTransform()->GetPosition();
		const auto& rot = GetTransform()->GetRotation();
		physx::PxVec3 pxRPos = { rPos.x, rPos.y, rPos.z };
		physx::PxVec3 pxLPos = { lPos.x, lPos.y, lPos.z };
		physx::PxQuat pxRot = { rot.x, rot.y, rot.z, rot.w };

		const auto& dirVec = GetTransform()->GetRight();
		physx::PxVec3 pxDirVec = { dirVec.x, dirVec.y, dirVec.z };
		
		//Right Door
		auto newLocation = pxRPos + (pxDirVec * m_Speed * gameContext.pGameTime->GetElapsed());
		physx::PxTransform transform = physx::PxTransform(newLocation, pxRot);
		if ((dirVec.x > 0 && newLocation.x < m_EndPositionRight.x)
			|| (dirVec.x < 0 && newLocation.x > m_EndPositionRight.x)
			|| (dirVec.z > 0 && newLocation.z < m_EndPositionRight.z)
			|| (dirVec.z < 0 && newLocation.z > m_EndPositionRight.z))
		{
			m_pRightDoor->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->isRigidDynamic()->setKinematicTarget(transform);
		}
		
		newLocation = pxLPos - (pxDirVec * m_Speed * gameContext.pGameTime->GetElapsed());
		if ((-dirVec.x > 0 && newLocation.x < m_EndPositionLeft.x)
			|| (-dirVec.x < 0 && newLocation.x > m_EndPositionLeft.x)
			|| (-dirVec.z > 0 && newLocation.z < m_EndPositionLeft.z)
			|| (-dirVec.z < 0 && newLocation.z > m_EndPositionLeft.z))
		{
			transform = physx::PxTransform(newLocation, pxRot);
			m_pLeftDoor->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->isRigidDynamic()->setKinematicTarget(transform);
		}
	}
	else
	{
		const auto& pos = GetTransform()->GetPosition();
		const auto& rot = GetTransform()->GetRotation();
		physx::PxVec3 pxPos = { pos.x, pos.y, pos.z };
		physx::PxQuat pxRot = { rot.x, rot.y, rot.z, rot.w };

		const auto& rPos = m_pRightDoor->GetTransform()->GetPosition();
		const auto& lPos = m_pLeftDoor->GetTransform()->GetPosition();
		physx::PxVec3 pxRPos = { rPos.x, rPos.y, rPos.z };
		physx::PxVec3 pxLPos = { lPos.x, lPos.y, lPos.z };

		const auto& dirVec = GetTransform()->GetRight();
		physx::PxVec3 pxDirVec = { dirVec.x, dirVec.y, dirVec.z };

		//Right Door
		auto newLocation = pxRPos - (pxDirVec * m_Speed * gameContext.pGameTime->GetElapsed());
		physx::PxTransform transform = physx::PxTransform(newLocation, pxRot);
		if ((-dirVec.x > 0 && newLocation.x < pxPos.x)
			|| (-dirVec.x < 0 && newLocation.x > pxPos.x)
			|| (-dirVec.z > 0 && newLocation.z < pxPos.z)
			|| (-dirVec.z < 0 && newLocation.z > pxPos.z))
		{
			m_pRightDoor->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->isRigidDynamic()->setKinematicTarget(transform);
		}
		else
		{
			transform = physx::PxTransform(pxPos, pxRot);
			m_pRightDoor->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->isRigidDynamic()->setKinematicTarget(transform);
		}
		newLocation = pxLPos + (pxDirVec * m_Speed * gameContext.pGameTime->GetElapsed());
		if ((dirVec.x > 0 && newLocation.x < pxPos.x)
			|| (dirVec.x < 0 && newLocation.x > pxPos.x)
			|| (dirVec.z > 0 && newLocation.z < pxPos.z)
			|| (dirVec.z < 0 && newLocation.z > pxPos.z))
		{
			transform = physx::PxTransform(newLocation, pxRot);
			m_pLeftDoor->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->isRigidDynamic()->setKinematicTarget(transform);
		}
		else
		{
			transform = physx::PxTransform(pxPos, pxRot);
			m_pLeftDoor->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->isRigidDynamic()->setKinematicTarget(transform);
		}
	}
}

void Door::AddButton(Button* pButton)
{
	auto it = std::find(m_Buttons.begin(), m_Buttons.end(), pButton);

	if (it == m_Buttons.end())
		m_Buttons.push_back(pButton);
}

void Door::OpenDoor(bool open)
{
	if (m_IsOpen && !open)
		m_pAudioSystem->playSound(m_pClose, 0, false, 0);
	else if (!m_IsOpen && open)
		m_pAudioSystem->playSound(m_pOpen, 0, false, 0);

	m_IsOpen = open;
}
