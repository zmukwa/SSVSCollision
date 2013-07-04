// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVSC_BODY
#define SSVSC_BODY

#include <bitset>
#include <SFML/System.hpp>
#include <SSVUtils/SSVUtils.h>
#include "SSVSCollision/AABB/AABB.h"
#include "SSVSCollision/Body/CallbackInfo.h"
#include "SSVSCollision/Body/MassData.h"
#include "SSVSCollision/Body/GroupData.h"
#include "SSVSCollision/Spatial/SpatialInfoBase.h"
#include "SSVSCollision/Body/Base.h"
#include "SSVSCollision/Body/Enums.h"

namespace ssvsc
{
	class World;
	struct ResolverBase;

	class Body : public Base
	{
		private:
			ResolverBase& resolver;
			AABB shape, oldShape;
			bool _static, resolve{true};
			sf::Vector2f velocity, acceleration;
			MassData massData;
			GroupData groupData;
			void* userData{nullptr};

			void integrate(float mFrameTime);

		public:
			std::vector<Body*> bodiesToResolve;

			ssvu::Delegate<void> onPreUpdate;
			ssvu::Delegate<void> onPostUpdate;
			ssvu::Delegate<void, DetectionInfo> onDetection;
			ssvu::Delegate<void, ResolutionInfo> onResolution;
			ssvu::Delegate<void> onOutOfBounds;

			Body(World& mWorld, bool mIsStatic, sf::Vector2i mPosition, sf::Vector2i mSize);
			~Body();

			inline Type getType() override { return Type::Body; }
			void preUpdate(float mFrameTime) override;
			void postUpdate(float mFrameTime) override;
			void destroy() override;

			void applyForce(sf::Vector2f mForce);
			void applyImpulse(sf::Vector2f mImpulse);

			// Setters
			inline void setPosition(sf::Vector2i mPosition)					{ oldShape = shape; shape.setPosition(mPosition); spatialInfo.invalidate(); }
			inline void setX(int mX)										{ oldShape = shape; shape.setX(mX); spatialInfo.invalidate(); }
			inline void setY(int mY)										{ oldShape = shape; shape.setY(mY); spatialInfo.invalidate(); }
			inline void setSize(sf::Vector2i mSize)							{ shape.setSize(mSize); spatialInfo.invalidate(); }
			inline void setWidth(int mWidth)								{ shape.setWidth(mWidth); spatialInfo.invalidate(); }
			inline void setHeight(int mHeight)								{ shape.setHeight(mHeight); spatialInfo.invalidate(); }
			inline void setVelocity(sf::Vector2f mVelocity) 				{ velocity = mVelocity; }
			inline void setAcceleration(sf::Vector2f mAcceleration)			{ acceleration = mAcceleration; }
			inline void setStatic(bool mStatic) 							{ _static = mStatic; }
			inline void setUserData(void* mUserData)						{ userData = mUserData; }
			inline void setVelocityX(float mX)								{ velocity.x = mX; }
			inline void setVelocityY(float mY)								{ velocity.y = mY; }
			inline void setResolve(bool mResolve)							{ resolve = mResolve; }
			inline void setMass(float mMass)								{ massData.setMass(mMass); }

			// Getters
			inline World& getWorld()										{ return world; }
			inline AABB& getShape() override								{ return shape; }
			inline AABB& getOldShape() override								{ return oldShape; }
			inline sf::Vector2i getPosition() const							{ return shape.getPosition(); }
			inline sf::Vector2f getVelocity() const							{ return velocity; }
			inline sf::Vector2f getAcceleration() const						{ return acceleration; }
			inline sf::Vector2i getSize() const								{ return shape.getSize(); }
			inline float getMass() const									{ return _static ? 0 : massData.getMass(); }
			inline float getInvMass() const									{ return _static ? 0 : massData.getInvMass(); }
			inline int getWidth() const										{ return shape.getWidth(); }
			inline int getHeight() const									{ return shape.getHeight(); }
			inline bool isStatic() const									{ return _static; }
			inline void* getUserData() const								{ return userData; }
			inline bool hasMovedLeft() const								{ return shape.getX() < oldShape.getX(); }
			inline bool hasMovedRight() const								{ return shape.getX() > oldShape.getX(); }
			inline bool hasMovedUp() const									{ return shape.getY() < oldShape.getY(); }
			inline bool hasMovedDown() const								{ return shape.getY() > oldShape.getY(); }
			inline GroupData& getGroupData() { return groupData; }
			inline const std::bitset<64>& getGroupUidsToCheck() override { return groupData.getGroupsToCheck(); }

			inline bool mustCheckAgainst(Body& mBody)		{ return (groupData.getGroupsToCheck() & mBody.getGroupData().getGroups()).any(); }
			inline bool mustResolveAgainst(Body& mBody)		{ return !(groupData.getGroupsNoResolve() & mBody.getGroupData().getGroups()).any(); }

			inline void addToResolveAgainst(Body& mBody)	{ bodiesToResolve.push_back(&mBody); }
	};
}

#endif
