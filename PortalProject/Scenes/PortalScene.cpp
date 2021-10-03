#include "stdafx.h"
#include "PortalScene.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"
#include "Components.h"
#include "FixedCamera.h"
#include "../Character.h"
#include "OverlordGame.h"
#include "../Interactive/MovableBox.h"
#include "../Env/Wall.h"
#include "../Env/Floor.h"
#include "../Portal/Portal.h"
#include "ModelAnimator.h"
#include "../Env/FullWall.h"
#include "../Env/Door.h"
#include "../Interactive\Button.h"
#include "../..\Materials\Shadow\DiffuseMaterial_Shadow.h"
#include "../..\Materials\DiffuseMaterial.h"
#include "../..\Materials\Shadow\SkinnedDiffuseMaterial_Shadow.h"
#include <Cast.h>
#include "../UI/ExitButton.h"
#include "../UI/RestartButton.h"
#include "../UI/ToMainButton.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include <GameState.h>
#include "../Portal/PortalBall.h"
#include <SceneManager.h>
#include <SpriteComponent.h>
#include "../PostProccesing/PostVignette.h"

PortalScene::PortalScene()
	: GameScene(L"PortalScene")
{
}

PortalScene::~PortalScene()
{
}

void PortalScene::Initialize()
{
	const auto& gameContext = GetGameContext();

	gameContext.pShadowMapper->SetLight({ -500.f,500,-500.f }, { 0.707129888f, -0.797205281f, 0.809117377f });
	gameContext.pGameTime->ForceElapsedUpperbound(true);

	//setup audio
	FMOD_RESULT result;
	result = FMOD::System_Create(&m_pAudioSystem);
	if (result != FMOD_OK)
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	result = m_pAudioSystem->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	//setup sounds
	m_pAudioSystem->createSound("./Sound/PortalGun.wav", FMOD_DEFAULT, 0, &m_pPortalGunSound);
	m_pAudioSystem->createSound("./Sound/Taste of Blood.wav", FMOD_DEFAULT, 0, &m_pAmbient);


	auto skinnedMat = new SkinnedDiffuseMaterial_Shadow();
	skinnedMat->SetDiffuseTexture(L"./Resources/Textures/chell_body.png");
	skinnedMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(skinnedMat, 666);


	//Walls
	DiffuseMaterial_Shadow* pDiff = new DiffuseMaterial_Shadow();
	pDiff->SetDiffuseTexture(L"./Resources/Textures/concrete_modular_wall5.png");
	pDiff->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());

	gameContext.pMaterialManager->AddMaterial(pDiff, 3);

	DiffuseMaterial_Shadow* pDiffFloor = new DiffuseMaterial_Shadow();
	pDiffFloor->SetDiffuseTexture(L"./Resources/Textures/concrete_modular_floor2.png");
	pDiffFloor->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pDiffFloor, 4);

	pDiff = new DiffuseMaterial_Shadow();
	pDiff->SetDiffuseTexture(L"./Resources/Textures/DefaultMaterial_albedo.jpg");
	pDiff->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());

	gameContext.pMaterialManager->AddMaterial(pDiff, 2);

	DiffuseMaterial* pDiffMat = new DiffuseMaterial();
	pDiffMat->SetDiffuseTexture(L"./Resources/Textures/Blueportal.png");
	gameContext.pMaterialManager->AddMaterial(pDiffMat, 10);
	pDiffMat = new DiffuseMaterial();
	pDiffMat->SetDiffuseTexture(L"./Resources/Textures/Orangeportal.png");
	gameContext.pMaterialManager->AddMaterial(pDiffMat, 11);

	pDiff = new DiffuseMaterial_Shadow();
	pDiff->SetDiffuseTexture(L"./Resources/Textures/portal_button_01.png");
	pDiff->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pDiff, 5);

	pDiff = new DiffuseMaterial_Shadow();
	pDiff->SetDiffuseTexture(L"./Resources/Textures/portal_button_02.png");
	pDiff->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pDiff, 6);

	pDiff = new DiffuseMaterial_Shadow();
	pDiff->SetDiffuseTexture(L"./Resources/Textures/portal_door_a.png");
	pDiff->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pDiff, 7);

	pDiff = new DiffuseMaterial_Shadow();
	pDiff->SetDiffuseTexture(L"./Resources/Textures/v_portalgun.png");
	pDiff->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pDiff, 8);

	//**room1**//
#pragma region room1
	//floors
	auto floor = new Floor({ 48,0,0 });
	AddChild(floor);
	floor = new Floor({ 80,0,0 });
	AddChild(floor);
	floor = new Floor({ 80,0,32 });
	AddChild(floor);

	auto wall = new FullWall();
	//back
	wall->AddWallPiece(new Wall({ 24, 0, -8 }));
	wall->AddWallPiece(new Wall({ 24, 16, -8 }));
	wall->AddWallPiece(new Wall({ 24, 32, -8 }));
	wall->AddWallPiece(new Wall({ 24, 0, 8 }));
	wall->AddWallPiece(new Wall({ 24, 16, 8 }));
	wall->AddWallPiece(new Wall({ 24, 32, 8 }));

	wall->AddWallPiece(new Wall({ 56, 0, 40 }));
	wall->AddWallPiece(new Wall({ 56, 16, 40 }));
	wall->AddWallPiece(new Wall({ 56, 32, 40 }));

	//left
	wall->AddWallPiece(new Wall({ 40, 0, 24 }));
	wall->AddWallPiece(new Wall({ 40, 16, 24 }));
	wall->AddWallPiece(new Wall({ 40, 32, 24 }));
	wall->AddWallPiece(new Wall({ 56, 0, 24 }));
	wall->AddWallPiece(new Wall({ 56, 16, 24 }));
	wall->AddWallPiece(new Wall({ 56, 32, 24 }));

	wall->AddWallPiece(new Wall({ 72, 0, 56 }));
	wall->AddWallPiece(new Wall({ 72, 16, 56 }));
	wall->AddWallPiece(new Wall({ 72, 32, 56 }));
	wall->AddWallPiece(new Wall({ 88, 0, 56 }));
	wall->AddWallPiece(new Wall({ 88, 16, 56 }));
	wall->AddWallPiece(new Wall({ 88, 32, 56 }));

	//right
	wall->AddWallPiece(new Wall({ 40, 0, -24 }));
	wall->AddWallPiece(new Wall({ 40, 16, -24 }));
	wall->AddWallPiece(new Wall({ 40, 32, -24 }));
	wall->AddWallPiece(new Wall({ 56, 0, -24 }));
	wall->AddWallPiece(new Wall({ 56, 16, -24 }));
	wall->AddWallPiece(new Wall({ 56, 32, -24 }));
	wall->AddWallPiece(new Wall({ 72, 0, -24 }));
	wall->AddWallPiece(new Wall({ 72, 16, -24 }));
	wall->AddWallPiece(new Wall({ 72, 32, -24 }));
	wall->AddWallPiece(new Wall({ 88, 0, -24 }));
	wall->AddWallPiece(new Wall({ 88, 16, -24 }));
	wall->AddWallPiece(new Wall({ 88, 32, -24 }));

	//front
	wall->AddWallPiece(new Wall({ 104, 0, 64 }));
	wall->AddWallPiece(new Wall({ 104, 16, 64 }));
	wall->AddWallPiece(new Wall({ 104, 32, 64 }));
	wall->AddWallPiece(new Wall({ 104, 0, 48 }));
	wall->AddWallPiece(new Wall({ 104, 16, 48 }));
	wall->AddWallPiece(new Wall({ 104, 32, 48 }));
	wall->AddWallPiece(new Wall({ 104, 0, 32 }));
	wall->AddWallPiece(new Wall({ 104, 16, 32 }));
	wall->AddWallPiece(new Wall({ 104, 32, 32 }));
	wall->AddWallPiece(new Wall({ 104, 0, 16 }));
	wall->AddWallPiece(new Wall({ 104, 16, 16 }));
	wall->AddWallPiece(new Wall({ 104, 32, 16 }));
	wall->AddWallPiece(new Wall({ 104, 16, 0 }));
	wall->AddWallPiece(new Wall({ 104, 32, 0 }));
	wall->AddWallPiece(new Wall({ 104, 0, -16 }));
	wall->AddWallPiece(new Wall({ 104, 16, -16 }));
	wall->AddWallPiece(new Wall({ 104, 32, -16 }));
	AddChild(wall);

	//door
	auto door = new Door({ 96.6f, 0.f, 0.f }, { 90, 90, 0 });
	AddChild(door);

	//block
	auto block = new MovableBox({ 48,5,10 });
	block->SetTag(L"Box");
	m_Boxs.push_back(block);
	AddChild(block);

	//button
	auto button = new Button({ 80, 0, 32 });
	AddChild(button);
	door->AddButton(button);

	//character
	m_pCharacter = new Character();
	m_pCharacter->GetTransform()->Translate(XMFLOAT3{ 0,3.5f,0 });
	m_pCharacter->SetStartPosition(XMFLOAT3{ 48,3.5f,0 });
	AddChild(m_pCharacter);
#pragma endregion

	//**room2**//
#pragma region room2
	//floors
	floor = new Floor({ 112,0,-8 });
	AddChild(floor);
	floor = new Floor({ 144,16,-8 });
	AddChild(floor);

	wall = new FullWall();
	//left
	wall->AddWallPiece(new Wall({ 120, 0, 16 }));
	wall->AddWallPiece(new Wall({ 120, 16, 16 }));
	wall->AddWallPiece(new Wall({ 120, 32, 16 }));
	wall->AddWallPiece(new Wall({ 120, 48, 16 }));
	wall->AddWallPiece(new Wall({ 120, 64, 16 }));
	wall->AddWallPiece(new Wall({ 136, 16, 16 }));
	wall->AddWallPiece(new Wall({ 136, 32, 16 }));
	wall->AddWallPiece(new Wall({ 136, 48, 16 }));
	wall->AddWallPiece(new Wall({ 136, 64, 16 }));

	//right
	wall->AddWallPiece(new Wall({ 120, 0, -16 }));
	wall->AddWallPiece(new Wall({ 120, 16, -16 }));
	wall->AddWallPiece(new Wall({ 120, 32, -16 }));
	wall->AddWallPiece(new Wall({ 120, 48, -16 }));
	wall->AddWallPiece(new Wall({ 120, 64, -16 }));

	//front
	wall->AddWallPiece(new Wall({ 136, -4, 0 }));

	wall->AddWallPiece(new Wall({ 152, 16, 0 }));
	wall->AddWallPiece(new Wall({ 152, 32, 0 }));
	wall->AddWallPiece(new Wall({ 152, 48, 0 }));
	wall->AddWallPiece(new Wall({ 152, 64, 0 }));
	wall->AddWallPiece(new Wall({ 152, 16, -16 }));
	wall->AddWallPiece(new Wall({ 152, 32, -16 }));
	wall->AddWallPiece(new Wall({ 152, 48, -16 }));
	wall->AddWallPiece(new Wall({ 152, 64, -16 }));
	AddChild(wall);
#pragma endregion

	//**room3**//
#pragma region room3
	floor = new Floor({ 144,16,-40 });
	AddChild(floor);
	floor = new Floor({ 144,16,-72 });
	AddChild(floor);
	floor = new Floor({ 176,16,-40 });
	AddChild(floor);
	floor = new Floor({ 176,16,-72 });
	AddChild(floor);

	floor = new Floor({ 112,-16,-48 });
	AddChild(floor);

	wall = new FullWall();
	//left
	wall->AddWallPiece(new Wall({ 104, -16, -24 }));
	wall->AddWallPiece(new Wall({ 104, 0, -24 }));
	wall->AddWallPiece(new Wall({ 104, 16, -24 }));
	wall->AddWallPiece(new Wall({ 104, 32, -24 }));
	wall->AddWallPiece(new Wall({ 104, 48, -24 }));
	wall->AddWallPiece(new Wall({ 104, 64, -24 }));
	wall->AddWallPiece(new Wall({ 120, -16, -24 }));
	wall->AddWallPiece(new Wall({ 120, 0, -24 }));
	wall->AddWallPiece(new Wall({ 120, 16, -24 }));
	wall->AddWallPiece(new Wall({ 120, 32, -24 }));
	wall->AddWallPiece(new Wall({ 120, 48, -24 }));
	wall->AddWallPiece(new Wall({ 120, 64, -24 }));

	wall->AddWallPiece(new Wall({ 168, 16, -16 }));
	wall->AddWallPiece(new Wall({ 168, 32, -16 }));
	wall->AddWallPiece(new Wall({ 168, 48, -16 }));
	wall->AddWallPiece(new Wall({ 168, 64, -16 }));
	wall->AddWallPiece(new Wall({ 184, 16, -16 }));
	wall->AddWallPiece(new Wall({ 184, 32, -16 }));
	wall->AddWallPiece(new Wall({ 184, 48, -16 }));
	wall->AddWallPiece(new Wall({ 184, 64, -16 }));

	//back
	wall->AddWallPiece(new Wall({ 88, -16, -40 }));
	wall->AddWallPiece(new Wall({ 88, 0, -40 }));
	wall->AddWallPiece(new Wall({ 88, 16, -40 }));
	wall->AddWallPiece(new Wall({ 88, 32, -40 }));
	wall->AddWallPiece(new Wall({ 88, 48, -40 }));
	wall->AddWallPiece(new Wall({ 88, 64, -40 }));
	wall->AddWallPiece(new Wall({ 88, -16, -56 }));
	wall->AddWallPiece(new Wall({ 88, 0, -56 }));
	wall->AddWallPiece(new Wall({ 88, 16, -56 }));
	wall->AddWallPiece(new Wall({ 88, 32, -56 }));
	wall->AddWallPiece(new Wall({ 88, 48, -56 }));
	wall->AddWallPiece(new Wall({ 88, 64, -56 }));

	wall->AddWallPiece(new Wall({ 120, 16, -88 }));
	wall->AddWallPiece(new Wall({ 120, 32, -88 }));
	wall->AddWallPiece(new Wall({ 120, 48, -88 }));
	wall->AddWallPiece(new Wall({ 120, 64, -88 }));

	//right
	wall->AddWallPiece(new Wall({ 104, -16, -72 }));
	wall->AddWallPiece(new Wall({ 104, 0, -72 }));
	wall->AddWallPiece(new Wall({ 104, 16, -72 }));
	wall->AddWallPiece(new Wall({ 104, 32, -72 }));
	wall->AddWallPiece(new Wall({ 104, 48, -72 }));
	wall->AddWallPiece(new Wall({ 104, 64, -72 }));
	wall->AddWallPiece(new Wall({ 120, -16, -72 }));
	wall->AddWallPiece(new Wall({ 120, 0, -72 }));
	wall->AddWallPiece(new Wall({ 120, 16, -72 }));
	wall->AddWallPiece(new Wall({ 120, 32, -72 }));
	wall->AddWallPiece(new Wall({ 120, 48, -72 }));
	wall->AddWallPiece(new Wall({ 120, 64, -72 }));

	wall->AddWallPiece(new Wall({ 128, 16, -96 }));
	wall->AddWallPiece(new Wall({ 128, 32, -96 }));
	wall->AddWallPiece(new Wall({ 128, 48, -96 }));
	wall->AddWallPiece(new Wall({ 128, 64, -96 }));
	wall->AddWallPiece(new Wall({ 144, 16, -96 }));
	wall->AddWallPiece(new Wall({ 144, 32, -96 }));
	wall->AddWallPiece(new Wall({ 144, 48, -96 }));
	wall->AddWallPiece(new Wall({ 144, 64, -96 }));
	wall->AddWallPiece(new Wall({ 160, 32, -96 }));
	wall->AddWallPiece(new Wall({ 160, 48, -96 }));
	wall->AddWallPiece(new Wall({ 160, 64, -96 }));
	wall->AddWallPiece(new Wall({ 176, 16, -96 }));
	wall->AddWallPiece(new Wall({ 176, 32, -96 }));
	wall->AddWallPiece(new Wall({ 176, 48, -96 }));
	wall->AddWallPiece(new Wall({ 176, 64, -96 }));
	wall->AddWallPiece(new Wall({ 192, 16, -96 }));
	wall->AddWallPiece(new Wall({ 192, 32, -96 }));
	wall->AddWallPiece(new Wall({ 192, 48, -96 }));
	wall->AddWallPiece(new Wall({ 192, 64, -96 }));

	//front
	wall->AddWallPiece(new Wall({ 136, -20, -40 }));
	wall->AddWallPiece(new Wall({ 136, -4, -40 }));
	wall->AddWallPiece(new Wall({ 136, -20, -56 }));
	wall->AddWallPiece(new Wall({ 136, -4, -56 }));

	wall->AddWallPiece(new Wall({ 200, 16, -32 }));
	wall->AddWallPiece(new Wall({ 200, 32, -32 }));
	wall->AddWallPiece(new Wall({ 200, 48, -32 }));
	wall->AddWallPiece(new Wall({ 200, 64, -32 }));
	wall->AddWallPiece(new Wall({ 200, 16, -48 }));
	wall->AddWallPiece(new Wall({ 200, 32, -48 }));
	wall->AddWallPiece(new Wall({ 200, 48, -48 }));
	wall->AddWallPiece(new Wall({ 200, 64, -48 }));
	wall->AddWallPiece(new Wall({ 200, 16, -64 }));
	wall->AddWallPiece(new Wall({ 200, 32, -64 }));
	wall->AddWallPiece(new Wall({ 200, 48, -64 }));
	wall->AddWallPiece(new Wall({ 200, 64, -64 }));
	wall->AddWallPiece(new Wall({ 200, 16, -80 }));
	wall->AddWallPiece(new Wall({ 200, 32, -80 }));
	wall->AddWallPiece(new Wall({ 200, 48, -80 }));
	wall->AddWallPiece(new Wall({ 200, 64, -80 }));

	//extra
	wall->AddWallPiece(new Wall({ 184, 16, -24 }));
	wall->AddWallPiece(new Wall({ 184, 32, -24 }));
	AddChild(wall);


	//companion blocks
	block = new MovableBox({ 112,-10,-48 });
	block->SetTag(L"Box");
	m_Boxs.push_back(block);
	AddChild(block);

	block = new MovableBox({ 178, 52, -28 });
	block->SetTag(L"Box");
	m_Boxs.push_back(block);
	AddChild(block);

	//door
	door = new Door({ 160.f, 16.f, -88.6f }, { 90, 180, 0 });
	AddChild(door);

	//buttons
	button = new Button({ 144, 16, -72 });
	AddChild(button);
	door->AddButton(button);

	button = new Button({ 176, 16, -72 });
	AddChild(button);
	door->AddButton(button);
#pragma endregion

	//**endroom**//
#pragma region endroom
	floor = new Floor({ 160,16,-104 });
	AddChild(floor);

	wall = new FullWall();
	//back
	wall->AddWallPiece(new Wall({ 136, 16, -112 }));
	wall->AddWallPiece(new Wall({ 136, 32, -112 }));
	wall->AddWallPiece(new Wall({ 136, 48, -112 }));
	wall->AddWallPiece(new Wall({ 136, 64, -112 }));

	//right
	wall->AddWallPiece(new Wall({ 152, 16, -128 }));
	wall->AddWallPiece(new Wall({ 152, 32, -128 }));
	wall->AddWallPiece(new Wall({ 152, 48, -128 }));
	wall->AddWallPiece(new Wall({ 152, 64, -128 }));
	wall->AddWallPiece(new Wall({ 168, 16, -128 }));
	wall->AddWallPiece(new Wall({ 168, 32, -128 }));
	wall->AddWallPiece(new Wall({ 168, 48, -128 }));
	wall->AddWallPiece(new Wall({ 168, 64, -128 }));

	//front
	wall->AddWallPiece(new Wall({ 184, 16, -112 }));
	wall->AddWallPiece(new Wall({ 184, 32, -112 }));
	wall->AddWallPiece(new Wall({ 184, 48, -112 }));
	wall->AddWallPiece(new Wall({ 184, 64, -112 }));
	AddChild(wall);
	
	//Finish
	m_pEndPoint = new GameObject();
	auto physx = PhysxManager::GetInstance()->GetPhysics();
	auto pBouncyMaterial = physx->createMaterial(0.f, 0.f, 1.f);
	auto rigidBody = new RigidBodyComponent(true);
	std::shared_ptr<physx::PxGeometry> boxgeom(new physx::PxBoxGeometry(8, 8, 8));
	auto collider = new ColliderComponent(boxgeom, *pBouncyMaterial);
	m_pEndPoint->AddComponent(rigidBody);
	m_pEndPoint->AddComponent(collider);
	m_pEndPoint->GetTransform()->Translate(160, 16, -112);
	AddChild(m_pEndPoint);

	collider->EnableTrigger(true);
	GameObject::PhysicsCallback function = std::bind(&PortalScene::BeginOverlap, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	m_pEndPoint->SetOnTriggerCallBack(function);
#pragma endregion


#pragma region UI
	//menu
	auto toMain = new ToMainButton();
	AddChild(toMain);
	toMain->SetTag(L"MenuToMain");
	toMain->GetTransform()->Translate(640.f, 280.f, 0.8f);
	m_Buttons.push_back(toMain);

	auto restartButton = new RestartButton();
	AddChild(restartButton);
	restartButton->SetTag(L"MenuRestart");
	restartButton->GetTransform()->Translate(640.f, 360.f, 0.8f);
	m_Buttons.push_back(restartButton);

	auto exitButton = new ExitButton();
	AddChild(exitButton);
	exitButton->SetTag(L"MenuExit");
	exitButton->GetTransform()->Translate(640.f, 440.f, 0.8f);
	m_Buttons.push_back(exitButton);

#pragma endregion
	//Controls
	auto image = new GameObject();
	auto controlImage = new SpriteComponent(L"./Resources/Textures/Controls.png", { 0,0 }, false);
	image->AddComponent(controlImage);
	image->SetTag(L"Controls");
	image->GetTransform()->Translate(0.f, 0.f, 0.8f);
	AddChild(image);

	//HUD
	auto hud = new GameObject();
	m_pCrossHair = new SpriteComponent(L"./Resources/Textures/CrossHairEmpty.png", { 0.5f, 0.5f });
	hud->AddComponent(m_pCrossHair);
	hud->GetTransform()->Translate(640, 360, 0.9f);
	AddChild(hud);

	//Input
	auto input = InputAction((int)PortalInput::Pause, InputTriggerState::Pressed, VK_ESCAPE);
	gameContext.pInput->AddInputAction(input);
	input = InputAction((int)PortalInput::PickUpObject, InputTriggerState::Pressed, 'E');
	gameContext.pInput->AddInputAction(input);
	input = InputAction((int)PortalInput::Portal1, InputTriggerState::Released, -1, VK_LBUTTON);
	gameContext.pInput->AddInputAction(input);
	input = InputAction((int)PortalInput::Portal2, InputTriggerState::Released, -1, VK_RBUTTON);
	gameContext.pInput->AddInputAction(input);
	input = InputAction((int)PortalInput::Continue, InputTriggerState::Released, VK_RETURN);
	gameContext.pInput->AddInputAction(input);
}

void PortalScene::Update()
{
	UpdateGameState();
	const auto gameContext = GetGameContext();

	if (gameContext.pGameState->GetGameState() == GameStates::Controls)
		if (gameContext.pInput->IsActionTriggered((int)PortalInput::Continue))
			gameContext.pGameState->ChangeGameState(GameStates::Game);
	if (gameContext.pGameState->GetGameState() == GameStates::Game)
	{
		UpdateGame();
	}
	else
	{
		auto mousePos = gameContext.pInput->GetMousePosition();

		for (auto button : m_Buttons)
		{
			if (button->MouseInShape(mousePos))
			{
				if (gameContext.pInput->IsActionTriggered((int)PortalInput::Portal1))
				{
					if (button->GetTag().find(L"Restart") < button->GetTag().size())
						ResetScene();
					else if (button->GetTag().find(L"ToMain") < button->GetTag().size())
						m_pAmbientChannel->stop();

					button->Click(gameContext);
				}
			}
		}
	}

	if (m_GoToMain && !m_IsResetting)
	{
		m_pAmbientChannel->stop();
		gameContext.pInput->CursorVisible(true);
		gameContext.pInput->ForceMouseToCenter(false);
		gameContext.pGameState->ChangeGameState(GameStates::MainMenu);
		SceneManager::GetInstance()->SetActiveGameScene(L"MainMenu");
	}
	if (m_IsResetting)
	{
		if (m_ResetCount > 1)
		{
			m_IsResetting = false;
			m_ResetCount = 0;
		}
		else
			++m_ResetCount;
		
	}
}

void PortalScene::Draw()
{
}

void PortalScene::SceneActivated()
{
	ResetScene();
	m_pAudioSystem->playSound(m_pAmbient, 0, false, &m_pAmbientChannel);
	m_GoToMain = false;
}

void PortalScene::UpdateGame()
{
	const auto gameContext = GetGameContext();
	//picking up
	if (gameContext.pInput->IsActionTriggered((int)PortalInput::PickUpObject) && !m_pCharacter->HasPickedUpObject())
	{
		GameObject* pickedObject = gameContext.pCamera->Pick(gameContext, CollisionGroupFlag::Group0);
		if (!pickedObject)
			return;
		if (pickedObject->GetTag()._Equal(L"Box"))
		{
			m_pCharacter->PickUpObject(pickedObject);
		}
	}
	else if (gameContext.pInput->IsActionTriggered((int)PortalInput::PickUpObject) && m_pCharacter->HasPickedUpObject())
	{
		m_pCharacter->DropObject();
	}

	//Spawn Portal balls
	if (gameContext.pInput->IsActionTriggered((int)PortalInput::Portal1))
	{
		if (m_pBlueBall)
		{
			RemoveChild(m_pBlueBall);
			m_pBlueBall = nullptr;
		}

		auto playerCameraTransform = m_pCharacter->GetCamera()->GetTransform();
		m_pBlueBall = new PortalBall(playerCameraTransform->GetWorldPosition(), playerCameraTransform->GetForward(), true);
		AddChild(m_pBlueBall);

		m_pAudioSystem->playSound(m_pPortalGunSound, 0, false, 0);
	}
	if (gameContext.pInput->IsActionTriggered((int)PortalInput::Portal2))
	{
		if (m_pOrangeBall)
		{
			RemoveChild(m_pOrangeBall);
			m_pOrangeBall = nullptr;
		}
		auto playerCameraTransform = m_pCharacter->GetCamera()->GetTransform();
		m_pOrangeBall = new PortalBall(playerCameraTransform->GetWorldPosition(), playerCameraTransform->GetForward(), false);
		AddChild(m_pOrangeBall);

		m_pAudioSystem->playSound(m_pPortalGunSound, 0, false, 0);
	}

	//place portals
	if (m_pBlueBall
		&& m_pBlueBall->HasHit())
	{
		if (m_pBlueBall->CanPlacePortal())
			PlacePortal(&m_pBluePortal, &m_pOrangePortal, m_pBlueBall);
		RemoveChild(m_pBlueBall);
		m_pBlueBall = nullptr;
	}
	if (m_pOrangeBall
		&& m_pOrangeBall->HasHit())
	{
		if (m_pOrangeBall->CanPlacePortal())
			PlacePortal(&m_pOrangePortal, &m_pBluePortal, m_pOrangeBall);
		RemoveChild(m_pOrangeBall);
		m_pOrangeBall = nullptr;
	}
}

void PortalScene::UpdateGameState()
{
	auto gameContext = GetGameContext();
	if (gameContext.pInput->IsActionTriggered((int)PortalInput::Pause) 
		&& (gameContext.pGameState->GetGameState() != GameStates::MainMenu
			&& gameContext.pGameState->GetGameState() != GameStates::Controls))
	{
		GameStates state = (gameContext.pGameState->GetGameState() == GameStates::Game) ? GameStates::Menu : GameStates::Game;
		gameContext.pGameState->ChangeGameState(state);
		if (gameContext.pGameState->GetGameState() == GameStates::Game)
		{
			gameContext.pInput->CursorVisible(false);
			gameContext.pInput->ForceMouseToCenter(true);
			RemovePostProcessingEffect(m_pVignette);
		}
		else
		{
			gameContext.pInput->CursorVisible(true);
			gameContext.pInput->ForceMouseToCenter(false);
			m_pVignette = new PostVignette();
			AddPostProcessingEffect(m_pVignette);
		}
	}
}

void PortalScene::PlacePortal(Portal** portalToPlace, Portal** connectionPortal, PortalBall* portalBall)
{
	const auto locationF3 = portalBall->GetPortalLocation();
	const auto location = DirectX::XMLoadFloat3(&locationF3);
	const auto normalF3 = portalBall->GetPortalDirection();
	auto normal = DirectX::XMLoadFloat3(&normalF3);
	normal = DirectX::XMVector3Normalize(normal);

	PortalColor color;
	if (portalBall->IsBlue())
		color = PortalColor::blue;
	else
		color = PortalColor::orange;

	auto portal = *portalToPlace;
	if (portal == nullptr)
	{

#pragma warning (push)
#pragma warning (disable : 4316)
		portal = new Portal(location, normal, color);
#pragma warning (pop)
		auto connection = *connectionPortal;
		portal->SetCharacter(m_pCharacter);
		AddChild(portal);
		if (connection != nullptr)
		{
			portal->SetOtherPortal(*connectionPortal);
			connection->SetOtherPortal(portal);
			m_pCrossHair->SetTexture(L"./Resources/Textures/CrossHairFull.png");
			*portalToPlace = portal;
			*connectionPortal = connection;
		}
		else
		{
			*portalToPlace = portal;
			if (portalBall->IsBlue())
				m_pCrossHair->SetTexture(L"./Resources/Textures/CrossHairBlue.png");
			else
				m_pCrossHair->SetTexture(L"./Resources/Textures/CrossHairOrange.png");
		}
	}
	else
	{
		portal->SetPortalLocationRotation(location, normal, DirectX::XMLoadFloat3(&m_pCharacter->GetTransform()->GetForward()));
		*portalToPlace = portal;
	}
}

void PortalScene::BeginOverlap(GameObject* triggerObject, GameObject* otherObject, GameObject::TriggerAction action)
{
	if (!m_GoToMain && !m_IsResetting)
		m_GoToMain = true;
}

void PortalScene::ResetScene()
{
	m_IsResetting = true;
	m_pCrossHair->SetTexture(L"./Resources/Textures/CrossHairEmpty.png");
	m_pCharacter->Reset();
	m_pCharacter->GetTransform()->Rotate(0, 0, 0);

	for (auto block : m_Boxs)
	{
		block->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setAngularVelocity({ 0,0,0 });
		block->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setLinearVelocity({ 0,0,0 });
		block->Reset();
	}

	if (m_pBlueBall)
	{
		RemoveChild(m_pBlueBall);
		m_pBlueBall = nullptr;
	}
	if (m_pOrangeBall)
	{
		RemoveChild(m_pOrangeBall);
		m_pOrangeBall = nullptr;
	}
	if (m_pBluePortal)
	{
		m_pBluePortal->Reset();
		RemoveChild(m_pBluePortal);
		m_pBluePortal = nullptr;
	}
	if (m_pOrangePortal)
	{
		m_pOrangePortal->Reset();
		RemoveChild(m_pOrangePortal);
		m_pOrangePortal = nullptr;
	}

	RemovePostProcessingEffect(m_pVignette);
}
