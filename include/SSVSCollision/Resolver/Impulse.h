// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVSC_RESOLVER_IMPULSE
#define SSVSC_RESOLVER_IMPULSE

#include "SSVSCollision/Body/Body.h"
#include "SSVSCollision/Utils/Utils.h"
#include "SSVSCollision/Global/Typedefs.h"

namespace ssvsc
{
	template<typename TW> struct ImpulseInfo
	{
		public:
			using BodyType = Body<TW>;
			using ResolverType = typename TW::ResolverType;
			friend ResolverType;

		protected:
			BodyType& body;
			Vec2f velTransferMult, velTransferImpulse, stress, nextStress;
			float stressMult{1.f}, stressPropagationMult{0.1f};

		public:
			inline ImpulseInfo(BodyType& mBody) : body(mBody) { }

			inline void applyImpulse(const Vec2f& mImpulse) noexcept
			{
				body.velocity.x += body.getInvMass() * (mImpulse.x / (1.f + (stress.y * stressPropagationMult)));
				body.velocity.y += body.getInvMass() * (mImpulse.y / (1.f + (stress.x * stressPropagationMult)));
			}
			inline void applyImpulse(const BodyType& mBody, const Vec2f& mImpulse) noexcept	{ if(body.mustResolveAgainst(mBody)) applyImpulse(mImpulse); }
			inline void applyStress(const Vec2f& mStress) noexcept							{ nextStress += ssvs::getAbs(body.getInvMass() * mStress * stressMult); }
			inline void applyStress(const BodyType& mBody, const Vec2f& mStress) noexcept	{ if(body.mustResolveAgainst(mBody)) applyStress(mStress); }

			inline void setVelTransferMultX(float mValue) noexcept					{ velTransferMult.x = mValue; }
			inline void setVelTransferMultY(float mValue) noexcept					{ velTransferMult.y = mValue; }
			inline void setStressMult(float mValue) noexcept						{ stressMult = mValue; }
			inline void setStressPropagationMult(float mValue) noexcept				{ stressPropagationMult = mValue; }

			inline float getVelTransferMultX() const noexcept						{ return velTransferMult.x; }
			inline float getVelTransferMultY() const noexcept						{ return velTransferMult.y; }
			inline Vec2f& getVelTransferImpulse() noexcept							{ return velTransferImpulse; }
			inline const Vec2f& getVelTransferImpulse() const noexcept				{ return velTransferImpulse; }
			inline const Vec2f& getStress() const noexcept							{ return stress; }
			inline float getStressMult() const noexcept								{ return stressMult; }
			inline float getStressPropagationMult() const noexcept					{ return stressPropagationMult; }
	};

	template<typename TW> struct Impulse
	{
		using BodyType = Body<TW>;
		using ResolverInfoType = ImpulseInfo<TW>;

		inline void resolve(BodyType& mBody, std::vector<BodyType*>& mToResolve) const
		{
			AABB& shape(mBody.shape);
			const AABB& oldShape(mBody.oldShape);

			ssvu::sort(mToResolve, [&](BodyType* mA, BodyType* mB){ return Utils::getOverlapArea(shape, mA->shape) > Utils::getOverlapArea(shape, mB->shape); });
			int resXNeg{0}, resXPos{0}, resYNeg{0}, resYPos{0};
			constexpr int tolerance{20};

			for(const auto& b : mToResolve)
			{
				int iX{Utils::getMinIntersectionX(shape, b->shape)}, iY{Utils::getMinIntersectionY(shape, b->shape)};

				if(std::abs(iX) < std::abs(iY))
				{
					resXNeg = std::min(resXNeg, iX);
					resXPos = std::max(resXPos, iX);
				}
				else
				{
					resYNeg = std::min(resYNeg, iY);
					resYPos = std::max(resYPos, iY);
				}
			}

			for(const auto& b : mToResolve)
			{
				const AABB& s(b->shape);
				if(!shape.isOverlapping(s)) continue;

				int iX{Utils::getMinIntersectionX(shape, s)}, iY{Utils::getMinIntersectionY(shape, s)};
				bool noResolvePosition{false}, noResolveVelocity{false};
				Vec2i resolution{std::abs(iX) < std::abs(iY) ? Vec2i{iX, 0} : Vec2i{0, iY}};

				mBody.onResolution({*b, b->getUserData(), resolution, noResolvePosition, noResolveVelocity});

				if(!noResolvePosition) mBody.resolvePosition(resolution);
				if(noResolveVelocity) continue;

				bool oldShapeLeftOfS{oldShape.isLeftOf(s)}, oldShapeRightOfS{oldShape.isRightOf(s)};
				bool oldShapeAboveS{oldShape.isAbove(s)}, oldShapeBelowS{oldShape.isBelow(s)};
				bool oldHOverlap{!(oldShapeLeftOfS || oldShapeRightOfS)}, oldVOverlap{!(oldShapeAboveS || oldShapeBelowS)};

				const auto& velocity(mBody.velocity);
				const AABB& os(b->oldShape);
				float desiredX{velocity.x}, desiredY{velocity.y};

				Vec2f normal;
				if(resolution.y < 0 && velocity.y > 0 && (oldShapeAboveS || (os.isBelow(shape) && oldHOverlap)))
				{
					if(std::abs(iY - resYNeg) < tolerance) normal.y = 1.f;
					desiredY *= mBody.getRestitutionY();
				}
				else if(resolution.y > 0 && velocity.y < 0 && (oldShapeBelowS || (os.isAbove(shape) && oldHOverlap)))
				{
					if(std::abs(iY - resYPos) < tolerance) normal.y = -1.f;
					desiredY *= mBody.getRestitutionY();
				}

				if(resolution.x < 0 && velocity.x > 0 && (oldShapeLeftOfS || (os.isRightOf(shape) && oldVOverlap)))
				{
					if(std::abs(iX - resXNeg) < tolerance) normal.x = 1.f;
					desiredX *= mBody.getRestitutionX();
				}
				else if(resolution.x > 0 && velocity.x < 0 && (oldShapeRightOfS || (os.isLeftOf(shape) && oldVOverlap)))
				{
					if(std::abs(iX - resXPos) < tolerance) normal.x = -1.f;
					desiredX *= mBody.getRestitutionX();
				}

				Vec2f velDiff{b->velocity - mBody.velocity};
				float velAlongNormal{ssvs::getDotProduct(velDiff, normal)};
				if(velAlongNormal > 0) continue;
				float invMassSum{mBody.getInvMass() + b->getInvMass()};
				float computedVel{velAlongNormal / invMassSum};
				Vec2f impulse{-(1.f + mBody.getRestitutionX()) * computedVel * normal.x , -(1.f + mBody.getRestitutionY()) * computedVel * normal.y};

				if(normal.y != 0)
				{
					float velTransferX{b->velocity.x - mBody.velocity.x};
					velTransferX /= invMassSum;
					if(b->velTransferMult.x != 0) velTransferX *= std::sqrt(mBody.velTransferMult.x * b->velTransferMult.x); else velTransferX *= 0;
					mBody.velTransferImpulse.x += velTransferX;
				}
				if(normal.x != 0)
				{
					float velTransferY{b->velocity.y - mBody.velocity.y};
					velTransferY /= invMassSum;
					if(b->velTransferMult.y != 0) velTransferY *= std::sqrt(mBody.velTransferMult.y * b->velTransferMult.y); else velTransferY *= 0;
					mBody.velTransferImpulse.y += velTransferY;
				}

				mBody.applyImpulse(*b, -impulse);
				b->applyImpulse(mBody, impulse);
				b->applyStress(mBody, (mBody.stress + impulse) * mBody.getMass());

				mBody.velocity.x = std::abs(desiredX) * ssvu::getSign(mBody.velocity.x);
				mBody.velocity.y = std::abs(desiredY) * ssvu::getSign(mBody.velocity.y);
			}
		}
		inline void postUpdate(TW& mWorld) const
		{
			for(const auto& b : mWorld.getBodies())
			{
				b->stress = ssvs::getCClamped(b->nextStress, std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
				ssvs::nullify(b->nextStress);

				b->applyImpulse(b->velTransferImpulse);
				ssvs::nullify(b->velTransferImpulse);
			}
		}
	};
}

#endif