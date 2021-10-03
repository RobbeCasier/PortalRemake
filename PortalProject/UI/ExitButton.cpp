#include "stdafx.h"
#include "ExitButton.h"

void ExitButton::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_pSprite = new SpriteComponent(L"./Resources/Textures/ExitButton.png", DirectX::XMFLOAT2(0.5f, 0.5f), false);
	AddComponent(m_pSprite);
}

void ExitButton::Click(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	PostQuitMessage(1);
}

bool ExitButton::MouseInShape(const POINT& mousePos)
{
	auto dimension = m_pSprite->GetDimension();
	auto pivot = m_pSprite->GetPivot();
	auto center = GetTransform()->GetPosition();

	DirectX::XMFLOAT4 rectPoints; //x = left, y = right, z = top, w = down
	rectPoints.x = center.x - (pivot.x * dimension.x)/2;
	rectPoints.y = rectPoints.x + dimension.x/2;
	rectPoints.z = center.y - (pivot.y * dimension.y)/2;
	rectPoints.w = rectPoints.z + dimension.y/2;

	if (mousePos.x > rectPoints.x
		&& mousePos.x < rectPoints.y
		&& mousePos.y > rectPoints.z
		&& mousePos.y < rectPoints.w)
		return true;
	return false;
}
