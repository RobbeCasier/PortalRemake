#include "stdafx.h"
#include "MainMenu.h"

#include <ContentManager.h>
#include <SpriteComponent.h>
#include <TransformComponent.h>
#include "../UI/StartButton.h"
#include "../UI/ExitButton.h"
#include <InputManager.h>
#include <SceneManager.h>
#include "../Scenes/PortalScene.h"

MainMenu::MainMenu()
	: GameScene(L"MainMenu")
{
}

MainMenu::~MainMenu()
{
}

void MainMenu::Initialize()
{
	const auto& gameContext = GetGameContext();

	//setup audio
	FMOD_RESULT result;
	result = FMOD::System_Create(&m_pAudioSystem);
	if (result != FMOD_OK)
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	result = m_pAudioSystem->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	//setup sounds
	m_pAudioSystem->createSound("./Sound/Procedural Jiggle Bone.wav", FMOD_DEFAULT, 0, &m_pAmbientMenu);
	m_pAmbientMenu->setLoopCount(-1);

	//Main Menu
	//background
	auto background = new GameObject();
	background->AddComponent(new SpriteComponent(L"./Resources/Textures/MainMenu.png", DirectX::XMFLOAT2(0.f, 0.f), true));
	AddChild(background);
	background->SetTag(L"BackGround");
	background->GetTransform()->Translate(0.f, 0.f, 0.9f);

	//start button
	auto startButton = new StartButton();
	AddChild(startButton);
	startButton->GetTransform()->Translate(640.f, 270.f, 0.8f);
	m_Buttons.push_back(startButton);
	startButton->SetTag(L"Start");

	//exit button
	auto exitButton = new ExitButton();
	AddChild(exitButton);
	exitButton->GetTransform()->Translate(640.f, 380.f, 0.8f);
	m_Buttons.push_back(exitButton);

	//input
	auto input = InputAction((int)MenuInput::LeftClick, InputTriggerState::Released, -1, VK_LBUTTON);
	gameContext.pInput->AddInputAction(input);
}

void MainMenu::Update()
{
	const auto& gameContext = GetGameContext();
	auto mousePos = gameContext.pInput->GetMousePosition();
	
	for (auto button: m_Buttons)
	{
		if (button->MouseInShape(mousePos))
		{
			if (gameContext.pInput->IsActionTriggered((int)MenuInput::LeftClick))
			{
				if (button->GetTag()._Equal(L"Start"))
				{
					m_pAmbientChannel->stop();
					if (!m_pPortalScene)
					{
						m_pPortalScene = new PortalScene();
						SceneManager::GetInstance()->AddGameScene(m_pPortalScene);
					}
					SceneManager::GetInstance()->SetActiveGameScene(L"PortalScene");
				}
				button->Click(gameContext);
			}

		}
	}
}

void MainMenu::Draw()
{
}

void MainMenu::SceneActivated()
{
	m_pAudioSystem->playSound(m_pAmbientMenu, 0, false, &m_pAmbientChannel);
		
}
