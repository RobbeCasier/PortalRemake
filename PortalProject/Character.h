#pragma once
#include "GameObject.h"

class ControllerComponent;
class CameraComponent;
class ModelComponent;

using namespace DirectX;
class Character : public GameObject
{
public:
	enum CharacterMovement : UINT
	{
		LEFT = 0,
		RIGHT,
		FORWARD,
		BACKWARD,
		JUMP,
		STOPANIMATIONF,
		STOPANIMATIONB,
		COUNT = 7
	};

	Character(float radius = 2, float height = 5, float moveSpeed = 100);
	virtual ~Character() = default;

	Character(const Character& other) = delete;
	Character(Character&& other) noexcept = delete;
	Character& operator=(const Character& other) = delete;
	Character& operator=(Character&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

	CameraComponent* GetCamera() const { return m_pCamera; }

	float GetRadius() const { return m_Radius; }

	void Rotate(const float& pitch, const float& yaw) {
		m_TotalYaw += yaw;
		m_TotalPitch += pitch;
	}

	//Functons written by Casier Robbe
	void UpdateCharacterTransformation(const GameContext& gameContext);

	bool HasPickedUpObject() const;
	void PickUpObject(GameObject* pObject);
	void DropObject();
	void SetStartPosition(DirectX::XMFLOAT3 startPos) { m_StartPosition = startPos; };
	void Reset();


protected:
	CameraComponent* m_pCamera;
	ControllerComponent* m_pController;

	DirectX::XMFLOAT3 m_StartPosition;

	float m_TotalPitch, m_TotalYaw;
	float m_MoveSpeed, m_RotationSpeed;
	float m_Radius, m_Height;

	//Running
	float m_MaxRunVelocity, 
		m_TerminalVelocity,
		m_Gravity, 
		m_RunAccelerationTime, 
		m_JumpAccelerationTime, 
		m_RunAcceleration, 
		m_JumpAcceleration, 
		m_RunVelocity, 
		m_JumpVelocity;

	DirectX::XMFLOAT3 m_Velocity;

	bool m_ObjectPicked = false;
	GameObject* m_PickedUpObject = nullptr;
	float m_DistancePickedObject = 0.f;

	ModelComponent* m_pModel = nullptr;
	ModelComponent* m_pGun = nullptr;

};


