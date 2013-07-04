// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVSCollision/Body/Sensor.h"
#include "SSVSCollision/Body/Body.h"
#include "SSVSCollision/Utils/Utils.h"
#include "SSVSCollision/World/World.h"

using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvsc::Utils;

namespace ssvsc
{
	Sensor::Sensor(World& mWorld, Vector2i mPosition, Vector2i mSize) : Base(mWorld), shape{mPosition, mSize / 2} { }
	Sensor::~Sensor() { spatialInfo.destroy(); }

	/*void Sensor::update(float mFrameTime)
	{
		onPreUpdate();
		if(outOfBounds) { outOfBounds = false; return; };
		spatialInfo.preUpdate();
		for(const auto& body : spatialInfo.getBodiesToCheck())
		{
			if(!isOverlapping(shape, body->getShape())) continue;
			onDetection({*body, mFrameTime, body->getUserData(), getMinIntersection(shape, body->getShape())});
		}
		spatialInfo.postUpdate();
	}*/
	void Sensor::destroy() { world.delSensor(this); Base::destroy(); }
}
