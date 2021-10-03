#include "stdafx.h"
#include "RestartButton.h"
#include <GameState.h>


void RestartButton::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_pSprite = new SpriteComponent(L"./Resources/Textures/RestartButton.png", DirectX::XMFLOAT2(0.5f, 0.5f), false);
	AddComponent(m_pSprite);
}

void RestartButton::Click(const GameContext& gameContext)
{
	gameContext.pGameState->ChangeGameState(GameStates::Controls);
	gameContext.pInput->CursorVisible(false);
	gameContext.pInput->ForceMouseToCenter(true);
}

bool RestartButton::MouseInShape(const POINT& mousePos)
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
