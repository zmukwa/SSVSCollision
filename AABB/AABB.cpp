#include "AABB.h"

using namespace sf;

namespace ssvsc
{
	AABB::AABB(Vector2i mPosition, Vector2i mHalfSize) : position{mPosition}, halfSize{mHalfSize} { }

	void AABB::setPosition(Vector2i mPosition) 	{ position = mPosition; }
	Vector2i AABB::getPosition() const 			{ return position; }
	int AABB::getX() const 						{ return position.x; }
	int AABB::getY() const 						{ return position.y; }
	int AABB::getLeft() const 					{ return position.x - halfSize.x; }
	int AABB::getRight() const 					{ return position.x + halfSize.x; }
	int AABB::getTop() const 					{ return position.y - halfSize.y; }
	int AABB::getBottom() const 				{ return position.y + halfSize.y; }
	int AABB::getHalfWidth() const 				{ return halfSize.x; }
	int AABB::getHalfHeight() const 			{ return halfSize.y; }
	int AABB::getWidth() const 					{ return halfSize.x * 2; }
	int AABB::getHeight() const 				{ return halfSize.y * 2; }
}
