#include "stdafx.h"
#include "ToMainButton.h"
#include <GameState.h>
#include <SceneManager.h>

void ToMainButton::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_pSprite = new SpriteComponent(L"./Resources/Textures/ToMainButton.png", DirectX::XMFLOAT2(0.5f, 0.5f), false);
	AddComponent(m_pSprite);
}

void ToMainButton::Click(const GameContext& gameContext)
{
	gameContext.pGameState->ChangeGameState(GameStates::MainMenu);
	SceneManager::GetInstance()->SetActiveGameScene(L"MainMenu");
}

bool ToMainButton::MouseInShape(const POINT& mousePos)
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
