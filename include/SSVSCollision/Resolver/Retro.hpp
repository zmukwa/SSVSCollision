// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVSC_RESOLVER_RETRO
#define SSVSC_RESOLVER_RETRO

#include "SSVSCollision/Body/Body.hpp"
#include "SSVSCollision/Utils/Utils.hpp"
#include "SSVSCollision/Global/Typedefs.hpp"

namespace ssvsc
{
	template<typename TW> struct RetroInfo
	{
		public:
			using BodyType = Body<TW>;
			using ResolverType = typename TW::ResolverType;
			friend ResolverType;

		protected:
			BodyType& body;

		public:
			inline RetroInfo(BodyType& mBody) : body(mBody) { }
	};

	template<typename TW> struct Retro
	{
		using BodyType = Body<TW>;
		using ResolverInfoType = RetroInfo<TW>;

		inline void resolve(BodyType& mBody, std::vector<BodyType*>& mToResolve) const
		{
			AABB& shape(mBody.shape);
			const AABB& oldShape(mBody.oldShape);
			ssvu::sort(mToResolve, [&](BodyType* mA, BodyType* mB){ return Utils::getOverlapArea(shape, mA->shape) > Utils::getOverlapArea(shape, mB->shape); });

			for(const auto& b : mToResolve)
			{
				const AABB& s(b->shape);
				if(!shape.isOverlapping(s)) continue;

				int iX{Utils::getMinIntersectionX(shape, s)}, iY{Utils::getMinIntersectionY(shape, s)};
				Vec2i resolution{std::abs(iX) < std::abs(iY) ? Vec2i{iX, 0} : Vec2i{0, iY}};
				bool noResolvePosition{false}, noResolveVelocity{false};
				mBody.onResolution({*b, b->getUserData(), resolution, noResolvePosition, noResolveVelocity});

				if(!noResolvePosition) mBody.resolvePosition(resolution);
				if(noResolveVelocity) continue;

				// Remember that shape has moved now
				bool oldShapeLeftOfS{oldShape.isLeftOf(s)}, oldShapeRightOfS{oldShape.isRightOf(s)};
				bool oldShapeAboveS{oldShape.isAbove(s)}, oldShapeBelowS{oldShape.isBelow(s)};
				bool oldHOverlap{!(oldShapeLeftOfS || oldShapeRightOfS)}, oldVOverlap{!(oldShapeAboveS || oldShapeBelowS)};

				// TODO: consider when two different bodies with two different rest. collide
				const auto& velocity(mBody.velocity);
				const AABB& os(b->oldShape);

				if	((resolution.y < 0 && velocity.y > 0 && (oldShapeAboveS || (os.isBelow(shape) && oldHOverlap))) ||
					(resolution.y > 0 && velocity.y < 0 && (oldShapeBelowS || (os.isAbove(shape) && oldHOverlap))))
						mBody.velocity.y *= -mBody.getRestitutionY();

				if	((resolution.x < 0 && velocity.x > 0 && (oldShapeLeftOfS || (os.isRightOf(shape) && oldVOverlap))) ||
					(resolution.x > 0 && velocity.x < 0 && (oldShapeRightOfS || (os.isLeftOf(shape) && oldVOverlap))))
						mBody.velocity.x *= -mBody.getRestitutionX();
			}
		}
		inline void postUpdate(TW&) const noexcept { }
	};
}

#endif
