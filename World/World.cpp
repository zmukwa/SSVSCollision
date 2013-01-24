#include "World.h"
#include "Body/Body.h"
#include "Utils/Utils.h"

using namespace std;
using namespace ssvsc::Utils;

namespace ssvsc
{
	World::World(int mColumns, int mRows, int mCellSize, int mOffset) : grid{mColumns, mRows, mCellSize, mOffset} { bodiesToDel.set_empty_key(nullptr); }
	World::~World() { for(auto& body : bodies) delete body; }

	void World::add(Body* mBody) { bodies.push_back(mBody); }
	void World::del(Body* mBody) { bodiesToDel.insert(mBody); }
	void World::update(float mFrameTime)
	{
		for(auto& body : bodiesToDel) { eraseFromVector(bodies, body); delete body; }
		bodiesToDel.clear();

		for(auto& body : bodies) body->update(mFrameTime);
	}

	Grid& World::getGrid() { return grid; }
}
