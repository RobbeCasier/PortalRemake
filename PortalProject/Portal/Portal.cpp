#include "stdafx.h"
#include "Portal.h"
#include "PhysxManager.h"
#include "CubePrefab.h"
#include <ContentManager.h>
#include <FixedCamera.h>
#include <GameScene.h>
#include <Cast.h>
#include "PhysxProxy.h"

Portal::Portal(const DirectX::XMVECTOR& location, const DirectX::XMVECTOR& normal, PortalColor color)
	: m_Location{location}
	, m_Normal{normal}
	, m_Color{color}
{
}

void Portal::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto pBouncyMaterial = physX->createMaterial(0.0f, 0.0f, 0.0f);
	m_pModel = new GameObject();
	auto model = new ModelComponent(L"./Resources/Meshes/Portal.ovm");
	m_pModel->AddComponent(model);
	this->GetScene()->AddChild(m_pModel);

	auto rigidBody = new RigidBodyComponent();
	AddComponent(rigidBody);
	rigidBody->SetKinematic(true);

	std::shared_ptr<physx::PxGeometry> boxGeometry(std::make_shared<physx::PxConvexMeshGeometry>(ContentManager::Load<physx::PxConvexMesh>(L"./Resources/Meshes/PortalFrame.ovpc"), physx::PxMeshScale(0.5f)));
	m_pCollider = new ColliderComponent(boxGeometry, *pBouncyMaterial);
	AddComponent(m_pCollider);

	m_pCamera = new FixedCamera();
	this->GetScene()->AddChild(m_pCamera);
	if (m_Color == PortalColor::blue)
	{
		model->SetMaterial(10);
		SetTag(L"portalBlue");
		m_pModel->SetTag(L"portalBlue");
		m_pPortalMapper = gameContext.pPortalMappers[1];
	}
	else
	{
		model->SetMaterial(11);
		SetTag(L"portalOrange");
		m_pModel->SetTag(L"portalOrange");
		m_pPortalMapper = gameContext.pPortalMappers[0];
	}
}

void Portal::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto newLocation = DirectX::XMVectorAdd(m_Location, DirectX::XMVectorMultiply(m_Normal, { 0.4f, 0.4f, 0.4f }));
	auto newLocationMesh = DirectX::XMVectorAdd(m_Location, DirectX::XMVectorMultiply(m_Normal, { 0.1f, 0.1f, 0.1f }));
	GetTransform()->Translate(newLocation);
	m_pModel->GetTransform()->Translate(newLocationMesh);
	Offset();
	
	//Rotate towards the normal
	DirectX::XMFLOAT2 newNormal(m_Normal.m128_f32[0], m_Normal.m128_f32[2]);
	DirectX::XMFLOAT2 oldNormal = { GetTransform()->GetForward().x, GetTransform()->GetForward().z };
	auto angle = DirectX::XMVector2AngleBetweenNormals(DirectX::XMLoadFloat2(&oldNormal), DirectX::XMLoadFloat2(&newNormal));

	auto cross = DirectX::XMVector2Cross(DirectX::XMLoadFloat2(&newNormal), DirectX::XMLoadFloat2(&oldNormal));
	float sign = 0.f;
	if (cross.m128_f32[0] < 0.f)
		sign = -1.f;
	else if (cross.m128_f32[0] > 0.f)
		sign = 1.f;
	else
		sign = newNormal.y;

	GetTransform()->Rotate(0, sign * angle.m128_f32[0] / DirectX::XM_PI * 180, 0);

	//Rotate plane, to show front
	if (m_Normal.m128_f32[2] < 0 || m_Normal.m128_f32[0] > 0)
		m_pModel->GetTransform()->Rotate(0, sign * angle.m128_f32[0] / DirectX::XM_PI * 180 - 180, 0);
	else
		m_pModel->GetTransform()->Rotate(0, sign * angle.m128_f32[0] / DirectX::XM_PI * 180, 0);

	//set the trigger function
	m_pCollider->EnableTrigger(true);
	PhysicsCallback function = std::bind(&Portal::BeginOverlap, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	SetOnTriggerCallBack(function);
}

void Portal::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	if (m_pOtherPortal == nullptr || m_pCharacter == nullptr)
		return;

	DirectX::XMVECTOR scal, rot, trans;

	//control for parallel and oposite normals
	auto thisNormal = DirectX::XMLoadFloat3(&GetTransform()->GetForward());
	auto otherNormal = DirectX::XMLoadFloat3(&m_pOtherPortal->GetTransform()->GetForward());
	bool rotate180x = false;
	if (((thisNormal.m128_f32[2] == 1.0f || thisNormal.m128_f32[2] == -1.0f) && (thisNormal.m128_f32[2] == otherNormal.m128_f32[2] || -thisNormal.m128_f32[2] == otherNormal.m128_f32[2]))
		|| ((thisNormal.m128_f32[0] >= 0.99f || thisNormal.m128_f32[0] <= -0.99f) && (thisNormal.m128_f32[0] == otherNormal.m128_f32[0] || -thisNormal.m128_f32[0] == otherNormal.m128_f32[0])))
		rotate180x = true;

	//transformation
	auto thisPortal = DirectX::XMLoadFloat4x4(&GetTransform()->GetWorld());
	auto thisPortalLoc = DirectX::XMLoadFloat3(&GetTransform()->GetWorldPosition());
	auto otherPortal = DirectX::XMLoadFloat4x4(&m_pOtherPortal->GetTransform()->GetWorld());
	auto player = DirectX::XMLoadFloat4x4(&m_pCharacter->GetChild<FixedCamera>()->GetTransform()->GetWorld());
	auto mat = DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, thisPortal), otherPortal);
	auto endmat = DirectX::XMMatrixMultiply(player, mat);

	//!!!--FIX FORMULA--!!!//
	auto matrix = DirectX::XMMatrixRotationRollPitchYaw(0, DirectX::XM_PI, 0);
	endmat = DirectX::XMMatrixMultiply(matrix, endmat);
	DirectX::XMMatrixDecompose(&scal, &rot, &trans, endmat);
		
	auto temp = otherPortal.r[3].m128_f32[2] - trans.m128_f32[2];
	trans.m128_f32[2] += 2 * temp;
	temp = otherPortal.r[3].m128_f32[0] - trans.m128_f32[0];
	trans.m128_f32[0] += 2 * temp;

	m_pCamera->GetTransform()->Translate(trans);

	//flip x and z
	rot.m128_f32[0] = -rot.m128_f32[0];
	rot.m128_f32[2] = -rot.m128_f32[2];

	m_pCamera->GetTransform()->Rotate(rot);

	//Clipping plane

	auto otherPortalLoc = DirectX::XMLoadFloat3(&m_pOtherPortal->GetTransform()->GetPosition());

	CameraComponent* camComp = m_pCamera->GetComponent<CameraComponent>();
	camComp->UseObliqueProjection();

	float camSpaceDst = DirectX::XMVector3Dot(otherPortalLoc, -otherNormal).m128_f32[0];

	DirectX::XMVECTOR clipPlane = { otherNormal.m128_f32[0], otherNormal.m128_f32[1], otherNormal.m128_f32[2], camSpaceDst };
	clipPlane = DirectX::XMPlaneTransform(clipPlane, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&camComp->GetViewInverse())));

	camComp->SetClipPlane(clipPlane.m128_f32[0], clipPlane.m128_f32[1], clipPlane.m128_f32[2], clipPlane.m128_f32[3]);

	//Character teleport
	if (m_IsCharacterOverlapping)
	{
		//m_pCharacter->GetComponent<ControllerComponent>()->SetCollisionIgnoreGroups(CollisionGroupFlag::Group1);
		//auto otherPortalLoc = DirectX::XMLoadFloat3(&m_pOtherPortal->GetTransform()->GetPosition());
		float radius = m_pCharacter->GetRadius();
		DirectX::XMVECTOR radiusVec = { radius, radius, radius, 1.0f };
		auto newLocation = DirectX::XMVectorAdd(otherPortalLoc, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&m_pOtherPortal->GetTransform()->GetForward()), radiusVec));
		newLocation = DirectX::XMVectorAdd(newLocation, DirectX::XMLoadFloat3(&m_pOtherPortal->GetTransform()->GetForward()));
		m_pCharacter->GetTransform()->Translate(newLocation);

		//rotation = DirectX::XMQuaternionRotationNormal({ 0,1,0,0 }, 1.5);
		DirectX::XMFLOAT2 newNormal(m_pOtherPortal->GetTransform()->GetForward().x, m_pOtherPortal->GetTransform()->GetForward().z);
		DirectX::XMFLOAT2 oldNormal = { GetTransform()->GetForward().x, GetTransform()->GetForward().z };
		auto angle = DirectX::XMVector2AngleBetweenNormals(DirectX::XMLoadFloat2(&oldNormal), DirectX::XMLoadFloat2(&newNormal));

		auto cross = DirectX::XMVector2Cross(DirectX::XMLoadFloat2(&newNormal), DirectX::XMLoadFloat2(&oldNormal));
		float sign = 0.f;
		if (cross.m128_f32[0] < 0.f)
			sign = -1.f;
		else if (cross.m128_f32[0] > 0.f)
			sign = 1.f;
		else
			sign = newNormal.y;

		m_pCharacter->Rotate(0, ((sign * angle.m128_f32[0] - DirectX::XM_PI) / DirectX::XM_PI * 180));
		m_IsCharacterOverlapping = false;
	}

}

void Portal::SetPortalLocationRotation(const DirectX::XMVECTOR& location, const DirectX::XMVECTOR& rotation, const DirectX::XMVECTOR& playerForward)
{
	UNREFERENCED_PARAMETER(playerForward);
	m_Location = location;
	m_Normal = rotation;
	auto newLocation = DirectX::XMVectorAdd(m_Location, DirectX::XMVectorMultiply(m_Normal, { 0.4f, 0.4f, 0.4f }));
	auto newLocationMesh = DirectX::XMVectorAdd(m_Location, DirectX::XMVectorMultiply(m_Normal, { 0.1f, 0.1f, 0.1f }));
	GetTransform()->Translate(newLocation);
	m_pModel->GetTransform()->Translate(newLocationMesh);
	Offset();

	DirectX::XMFLOAT2 newNormal(m_Normal.m128_f32[0], m_Normal.m128_f32[2]);
	DirectX::XMFLOAT2 oldNormal = { 0, 1 };
	auto angle = DirectX::XMVector2AngleBetweenNormals(DirectX::XMLoadFloat2(&oldNormal), DirectX::XMLoadFloat2(&newNormal));

	auto cross = DirectX::XMVector2Cross(DirectX::XMLoadFloat2(&newNormal), DirectX::XMLoadFloat2(&oldNormal));
	float sign = 0.f;
	if (cross.m128_f32[0] < 0.f)
		sign = -1.f;
	else if (cross.m128_f32[0] > 0.f)
		sign = 1.f;
	else
		sign = newNormal.y;

	GetTransform()->Rotate(0, sign * angle.m128_f32[0] / DirectX::XM_PI * 180, 0);

	//Rotate plane, to show front
	if (m_Normal.m128_f32[2] < 0 || m_Normal.m128_f32[0] > 0)
		m_pModel->GetTransform()->Rotate(0, sign * angle.m128_f32[0] / DirectX::XM_PI * 180 + 180, 0);
	else
		m_pModel->GetTransform()->Rotate(0, sign * angle.m128_f32[0] / DirectX::XM_PI * 180, 0);
}

void Portal::SetOtherPortal(Portal* pPortal)
{
	m_pOtherPortal = pPortal;
	m_IsActive = true;
	m_pPortalMapper->SetCamera(m_pCamera);
}

void Portal::SetCharacter(Character* pCharacter)
{
	m_pCharacter = pCharacter;
}

void Portal::Reset()
{
	GetScene()->RemoveChild(m_pCamera);
	m_pCamera = nullptr;
	GetScene()->RemoveChild(m_pModel);
	m_pModel = nullptr;
	m_pPortalMapper->Deactivate();
}

void Portal::BeginOverlap(GameObject* triggerObject, GameObject* otherObject, TriggerAction action)
{
	if (action == TriggerAction::LEAVE)
		return;

	Character* pPlayer = nullptr;
	if (Cast<GameObject, Character>(otherObject, &pPlayer))
	{
		if (m_IsCharacterOverlapping)
			return;
		m_IsCharacterOverlapping = true;
	}
}

void Portal::Offset()
{
	//check if top has wall
	PhysxProxy* physxProxy = GetScene()->GetPhysxProxy();
	
	DirectX::XMFLOAT3 location;
	DirectX::XMStoreFloat3(&location, m_Location);
	float halfHeight = 6.f;
	location.y += halfHeight;
	physx::PxVec3 rayStart = ToPxVec3(location);
	
	DirectX::XMFLOAT3 normal;
	DirectX::XMStoreFloat3(&normal, m_Normal);
	physx::PxVec3 rayDir = -ToPxVec3(normal);
	rayDir = rayDir.getNormalized();

	physx::PxRaycastBuffer hit;

	physx::PxQueryFilterData filterData;

	//vertical offset
	//touches the top a wall
	if (physxProxy->Raycast(rayStart, rayDir, 0.5f, hit, physx::PxHitFlag::eDEFAULT, filterData))
	{
		//check if portal is partialy underground
		rayStart += -rayDir * 0.5f;
		if (physxProxy->Raycast(rayStart, { 0,-1,0 }, 12.f, hit, physx::PxHitFlag::eDEFAULT, filterData))
		{
			float heightPortal = 12.0f;
			if (hit.getAnyHit(0).distance < heightPortal)
				m_Location.m128_f32[1] += heightPortal - hit.getAnyHit(0).distance;
		}

	}
	else //no
	{
		//set the next raycast start in the direction where a wall should be
		rayStart += rayDir * 0.5f;

		//raycast to see how far down a wall is
		if (physxProxy->Raycast(rayStart, { 0,-1,0 }, FLT_MAX, hit, physx::PxHitFlag::eDEFAULT, filterData))
		{
			//set the new location to this
			m_Location.m128_f32[1] -= hit.getAnyHit(0).distance;
		}
	}

	//horizontal offset
	//left (portal view)
	DirectX::XMStoreFloat3(&location, m_Location);
	rayStart = ToPxVec3(location);
	rayStart.y -= 6.f;
	auto quat = DirectX::XMQuaternionRotationAxis({ 0,1,0,0 }, -DirectX::XM_PIDIV2);
	auto left = DirectX::XMVector3Rotate(m_Normal, quat);

	DirectX::XMStoreFloat3(&normal, left);
	rayDir = ToPxVec3(normal);
	rayDir = rayDir.getNormalized();
	DirectX::XMStoreFloat3(&normal, m_Normal);
	rayStart += (ToPxVec3(normal) * 0.5f);
	if (physxProxy->Raycast(rayStart, rayDir, 5.f, hit, physx::PxHitFlag::eDEFAULT, filterData))
	{
		if (hit.getAnyHit(0).distance < 5.f)
		{
			int idx = 0;
			if (left.m128_f32[2] > 0.1f || left.m128_f32[2] < -0.1f)
				idx = 2;
			m_Location.m128_f32[idx] -= left.m128_f32[idx] * (3.f - hit.getAnyHit(0).distance);
		}
	}
	else
	{

		quat = DirectX::XMQuaternionRotationAxis({ 0,1,0,0 }, DirectX::XM_PIDIV2);
		auto right = DirectX::XMVector3Rotate(m_Normal, quat);

		DirectX::XMStoreFloat3(&normal, right);
		rayDir = ToPxVec3(normal);
		rayDir = rayDir.getNormalized();
		if (physxProxy->Raycast(rayStart, rayDir, 5.f, hit, physx::PxHitFlag::eDEFAULT, filterData))
		{
			if (hit.getAnyHit(0).distance < 5.f)
			{
				int idx = 0;
				if (left.m128_f32[2] > 0.1f || left.m128_f32[2] < -0.1f)
					idx = 2;
				m_Location.m128_f32[idx] += left.m128_f32[idx] * (3.f - hit.getAnyHit(0).distance);
			}
		}
	}


	auto newLocation = DirectX::XMVectorAdd(m_Location, DirectX::XMVectorMultiply(m_Normal, { 0.4f, 0.4f, 0.4f }));
	auto newLocationMesh = DirectX::XMVectorAdd(m_Location, DirectX::XMVectorMultiply(m_Normal, { 0.1f, 0.1f, 0.1f }));
	GetTransform()->Translate(newLocation);
	m_pModel->GetTransform()->Translate(newLocationMesh);

}
