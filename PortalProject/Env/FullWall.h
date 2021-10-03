#pragma once
#include <GameObject.h>
#include <RigidBodyComponent.h>

class FullWall : public GameObject
{
public:
	FullWall() = default;
	virtual ~FullWall() = default;

	void AddWallPiece(GameObject* pWallPiece)
	{
		auto it = std::find(m_Wall.begin(), m_Wall.end(), pWallPiece);
		if (it == m_Wall.end())
		{
			m_Wall.push_back(pWallPiece);
			AddChild(pWallPiece);
		}
	}
private:
	std::vector<GameObject*> m_Wall;
};