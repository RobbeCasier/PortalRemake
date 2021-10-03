#include "stdafx.h"
#include "StartButton.h"
#include <PhysxProxy.h>
#include <GameScene.h>
#include <SceneManager.h>
#include <GameState.h>
#include "../scenes/PortalScene.h"

StartButton::StartButton()
{
}

void StartButton::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_pSprite = new SpriteComponent(L"./Resources/Textures/StartButton.png", DirectX::XMFLOAT2(0.5f, 0.5f), false);
	AddComponent(m_pSprite);
}

void StartButton::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void StartButton::Click(const GameContext& gameContext)
{
	gameContext.pGameState->ChangeGameState(GameStates::Controls);
	gameContext.pInput->CursorVisible(false);
	gameContext.pInput->ForceMouseToCenter(true);
}

bool StartButton::MouseInShape(const POINT& mousePos)
{
	auto dimension = m_pSprite->GetDimension();
	auto pivot = m_pSprite->GetPivot();
	auto center = GetTransform()->GetPosition();

	DirectX::XMFLOAT4 rectPoints; //x = left, y = right, z = top, w = down
	rectPoints.x = center.x - (pivot.x * dimension.x);
	rectPoints.y = rectPoints.x + dimension.x;
	rectPoints.z = center.y - (pivot.y * dimension.y);
	rectPoints.w = rectPoints.z + dimension.y;

	if (mousePos.x > rectPoints.x
		&& mousePos.x < rectPoints.y
		&& mousePos.y > rectPoints.z
		&& mousePos.y < rectPoints.w)
		return true;
	return false;
}
