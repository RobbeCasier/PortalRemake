#include "stdafx.h"
#include "Character.h"
#include "Components.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "OverlordGame.h"
#include <ModelComponent.h>
#include "ModelAnimator.h"
#include <GameState.h>


Character::Character(float radius, float height, float moveSpeed) : 
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pCamera(nullptr),
	m_pController(nullptr),
	m_TotalPitch(0), 
	m_TotalYaw(0),
	m_RotationSpeed(120.f),
	//Running
	m_MaxRunVelocity(30.0f), 
	m_TerminalVelocity(5),
	m_Gravity(9.81f), 
	m_RunAccelerationTime(0.2f), 
	m_JumpAccelerationTime(2.f), 
	m_RunAcceleration(m_MaxRunVelocity/m_RunAccelerationTime), 
	m_JumpAcceleration(m_Gravity/m_JumpAccelerationTime), 
	m_RunVelocity(0), 
	m_JumpVelocity(0),
	m_Velocity(0,0,0)
{}

void Character::Initialize(const GameContext& gameContext)
{
	//UNREFERENCED_PARAMETER(gameContext);
	SetTag(L"Character");
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto pMat = physX->createMaterial(0, 0, 1);

	//TODO: Create controller
	m_pController = new ControllerComponent(pMat, m_Radius, m_Height);
	GetTransform()->Translate(XMFLOAT3{ 0, m_Height / 2 + m_Radius, 0 });
	this->AddComponent(m_pController);

	//TODO: Add a fixed camera as child
	FixedCamera* fCam = new FixedCamera();
	fCam->SetTag(L"Character");
	this->AddChild(fCam);

	auto obj = new GameObject();
	m_pGun = new ModelComponent(L"./Resources/Meshes/PortalGun.ovm");
	m_pGun->SetMaterial(8);
	obj->AddComponent(m_pGun);
	fCam->AddChild(obj);
	obj->GetTransform()->Scale(0.5f, 0.5f, 0.5f);
	obj->GetTransform()->Translate(XMFLOAT3{ 1.0f, -1.f, 2.5f });
	obj->GetTransform()->Rotate(0, 180, 0);

	m_pModel = new ModelComponent(L"./Resources/Meshes/Chell.ovm");
	m_pModel->SetMaterial(666);
	obj = new GameObject();
	obj->AddComponent(m_pModel);
	AddChild(obj);
	obj->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	obj->GetTransform()->Translate(XMFLOAT3{ 0, -(m_Height / 2 + m_Radius), 0 });
	obj->GetTransform()->Rotate(0, 180, 0);

	//TODO: Register all Input Actions
	// backward
	auto inputAction = InputAction((int)CharacterMovement::BACKWARD, InputTriggerState::Down, 'S');
	gameContext.pInput->AddInputAction(inputAction);

	// forward
	inputAction = InputAction((int)CharacterMovement::FORWARD, InputTriggerState::Down, 'W');
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction((int)CharacterMovement::FORWARD, InputTriggerState::Pressed, 'W');
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction((int)CharacterMovement::BACKWARD, InputTriggerState::Pressed, 'S');
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction((int)CharacterMovement::STOPANIMATIONF, InputTriggerState::Released, 'W');
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction((int)CharacterMovement::STOPANIMATIONB, InputTriggerState::Released, 'S');
	gameContext.pInput->AddInputAction(inputAction);

	// right
	inputAction = InputAction((int)CharacterMovement::RIGHT, InputTriggerState::Down, 'D');
	gameContext.pInput->AddInputAction(inputAction);

	// left
	inputAction = InputAction((int)CharacterMovement::LEFT, InputTriggerState::Down, 'A');
	gameContext.pInput->AddInputAction(inputAction);

	//jump
	inputAction = InputAction((int)CharacterMovement::JUMP, InputTriggerState::Pressed, VK_SPACE);
	gameContext.pInput->AddInputAction(inputAction);
}

void Character::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: Set the camera as active
	// We need to do this in the PostInitialize because child game objects only get initialized after the Initialize of the current object finishes
	m_pCamera = this->GetChild<FixedCamera>()->GetComponent<CameraComponent>();
	m_pCamera->GetTransform()->Translate(DirectX::XMFLOAT3{ 0,3,0 });
	m_pCamera->SetActive();
}

void Character::Update(const GameContext& gameContext)
{
	if (gameContext.pGameState->GetGameState() == GameStates::Game) UpdateCharacterTransformation(gameContext);

	if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::FORWARD))
	{
		m_pModel->GetAnimator()->SetAnimationSpeed(1.5f);
		m_pModel->GetAnimator()->SetPlayReversed(false);
		m_pModel->GetAnimator()->Play();
	}
	else if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::BACKWARD))
	{
		m_pModel->GetAnimator()->SetAnimationSpeed(1.5f);
		m_pModel->GetAnimator()->SetPlayReversed(true);
		m_pModel->GetAnimator()->Play();
	}
	else if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::STOPANIMATIONF) || gameContext.pInput->IsActionTriggered((int)CharacterMovement::STOPANIMATIONB))
	{
		m_pModel->GetAnimator()->Pause();
		m_pModel->GetAnimator()->Reset();
	}


}

void Character::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if (gameContext.pCamera->GetGameObject()->GetTag() == L"Character")
		m_pModel->SetVisibility(false);
	else
		m_pModel->SetVisibility();
}

void Character::UpdateCharacterTransformation(const GameContext& gameContext)
{
	//ROTATION
	auto look = XMFLOAT2(0, 0);
	POINT mousePos = InputManager::GetMousePosition();
	const auto mouseMove = InputManager::GetMouseMovement();
	look.x = static_cast<float>(mouseMove.x);
	look.y = static_cast<float>(mouseMove.y);

	float yaw = look.x * m_RotationSpeed * gameContext.pGameTime->GetElapsed();
	m_TotalYaw += yaw;
	float newPitch = m_TotalPitch + (look.y * m_RotationSpeed * gameContext.pGameTime->GetElapsed());
	if (newPitch < 75.f && newPitch > -75.f)
		m_TotalPitch = newPitch;


	//MOVEMENT
	//Plane movement
	auto movementVector = DirectX::XMFLOAT2(0, 0);
	movementVector.y = gameContext.pInput->IsActionTriggered((int)CharacterMovement::FORWARD) ? 1.0f : 0.f;
	if (movementVector.y == 0.f) movementVector.y = -((gameContext.pInput->IsActionTriggered((int)CharacterMovement::BACKWARD)) ? 1.0f : 0.f);

	movementVector.x = gameContext.pInput->IsActionTriggered((int)CharacterMovement::RIGHT) ? 1.0f : 0.f;
	if (movementVector.x == 0.f) movementVector.x = -((gameContext.pInput->IsActionTriggered((int)CharacterMovement::LEFT)) ? 1.0f : 0.f);

	const auto forward = DirectX::XMLoadFloat3(&GetTransform()->GetForward());
	const auto right = DirectX::XMLoadFloat3(&GetTransform()->GetRight());
	auto currentPos = DirectX::XMLoadFloat3(&GetTransform()->GetPosition());

	auto direction = (forward * movementVector.y) + (right * movementVector.x);
	direction = XMVector3Normalize(direction);

	if (movementVector.y != 0 || movementVector.x != 0)
	{
		m_RunVelocity += m_RunAcceleration * gameContext.pGameTime->GetElapsed();
		Clamp(m_RunVelocity, m_MaxRunVelocity, 0.f);
		const auto yComponent = m_Velocity.y;

		XMStoreFloat3(&m_Velocity, (direction * m_RunVelocity));

		m_Velocity.y = yComponent;
	}

	if (movementVector.y == 0 && movementVector.x == 0)
		m_Velocity.x = m_Velocity.z = 0;

	//UP MOVEMENT
	if (!m_pController->GetCollisionFlags().isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		m_JumpVelocity += m_JumpAcceleration * gameContext.pGameTime->GetElapsed();
		Clamp(m_JumpVelocity, m_TerminalVelocity, -m_TerminalVelocity);
	}
	else if (gameContext.pInput->IsActionTriggered((int)CharacterMovement::JUMP))
	{
		m_JumpVelocity = 0;
		m_Velocity.y = 50;
	}
	else
	{
		m_Velocity.y = 0;
	}

	m_Velocity.y -= m_JumpVelocity;

	DirectX::XMFLOAT3 newVelocity;
	XMStoreFloat3(&newVelocity, (XMLoadFloat3(&m_Velocity) * gameContext.pGameTime->GetElapsed()));
	m_pController->Move(newVelocity);

	GetTransform()->Rotate(0, m_TotalYaw, 0);
	m_pCamera->GetTransform()->Rotate(m_TotalPitch, 0, 0);

	if (m_ObjectPicked)
	{
		TransformComponent* tComponent = m_pCamera->GetTransform();
		auto dir = DirectX::XMLoadFloat3(&tComponent->GetWorldPosition()) + DirectX::XMLoadFloat3(&tComponent->GetForward()) * m_DistancePickedObject;
		m_PickedUpObject->GetTransform()->Translate(dir);
		m_PickedUpObject->GetTransform()->Rotate(0, m_TotalYaw, 0);
	}
}

bool Character::HasPickedUpObject() const
{
	return m_ObjectPicked;
}

void Character::PickUpObject(GameObject* pObject)
{
	m_ObjectPicked = true;
	m_PickedUpObject = pObject;

	auto objectPos = DirectX::XMLoadFloat3(&pObject->GetTransform()->GetPosition());
	auto characterCameraPos = DirectX::XMLoadFloat3(&GetTransform()->GetPosition());
	m_DistancePickedObject = DirectX::XMVector3Length(objectPos - characterCameraPos).m128_f32[0];
}

void Character::DropObject()
{
	m_ObjectPicked = false;
	auto body = m_PickedUpObject->GetComponent<RigidBodyComponent>()->GetPxRigidBody();
	body->setLinearVelocity({ 0,0,0 });
	body->setAngularVelocity({ 0,0,0 });
}

void Character::Reset()
{
	GetTransform()->Translate(m_StartPosition);
}
