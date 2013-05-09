#include "aiescort.hpp"
#include <iostream>
#include <cmath>

#include "character.hpp"

#include "../mwworld/class.hpp"
#include "../mwbase/world.hpp"
#include "../mwworld/timestamp.hpp"
#include "../mwbase/environment.hpp"
#include "../mwworld/player.hpp"
#include "movement.hpp"

#include <boost/graph/astar_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "boost/tuple/tuple.hpp"

namespace
{
    float sgn(float a)
    {
        if(a>0) return 1.;
        else return -1.;
    }

    /*

    MWWorld::Ptr InterpreterContext::getReference (
        const std::string& id, bool activeOnly)
    {
        if (!id.empty())
        {
            return MWBase::Environment::get().getWorld()->getPtr (id, activeOnly);
        }
        else
        {
            if (mReference.isEmpty())
                throw std::runtime_error ("no implicit reference");

            return mReference;
        }
    }

    float InterpreterContext::getDistance (const std::string& name, const std::string& id) const
    {
        // TODO handle exterior cells (when ref and ref2 are located in different cells)
        /const MWWorld::Ptr ref2 = MWBase::Environment::get().getWorld()->getPtr(id, false);

        double diff[3];

        const float* const pos1 = actor.getRefData().getPosition().pos;
        const float* const pos2 = ref2.getRefData().getPosition().pos;

        for (int i=0; i<3; ++i)
            diff[i] = pos1[i] - pos2[i];

        return std::sqrt (diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2]);
    }
    */

}

// TODO: Fix all commenting!
// TODO: Test vanilla behavior on passing x0, y0, and z0 with duration of anything including 0.
// TODO: Different behavior for AIEscort a d x y z and AIEscortCell a c d x y z.
// Necessary?     MWWorld::Ptr follower = MWBase::Environment::get().getWorld()->getPtr(mActorId, true);

MWMechanics::AiEscort::AiEscort(const std::string &actorId,int duration, float x, float y, float z)
: mActorId(actorId), mX(x), mY(y), mZ(z), mDuration(duration)
{
    //\ < The CS Help File states that if a duration is givin, the AI package will run for that long
    // BUT if a location is givin, it "trumps" the duration so it will simply escort to that location.
    if(mX != 0 || mY != 0 || mZ != 0)
        mDuration = 0;

    else
    {
        MWWorld::TimeStamp startTime = MWBase::Environment::get().getWorld()->getTimeStamp();
        mStartingSecond = ((startTime.getHour() - int(startTime.getHour())) * 100);
        std::cout << "AiEscort Started at: " << mStartingSecond << " For: "
            << duration << "Started At: " << mStartingSecond << std::endl;
    }
}

MWMechanics::AiEscort::AiEscort(const std::string &actorId,const std::string &cellId,int duration, float x, float y, float z)
: mActorId(actorId), mCellId(cellId), mX(x), mY(y), mZ(z), mDuration(duration)
{
    //\ < The CS Help File states that if a duration is givin, the AI package will run for that long
    // BUT if a location is givin, it "trumps" the duration so it will simply escort to that location.
    if(mX != 0 || mY != 0 || mZ != 0)
        mDuration = 0;

    else
    {
        MWWorld::TimeStamp startTime = MWBase::Environment::get().getWorld()->getTimeStamp();
        mStartingSecond = ((startTime.getHour() - int(startTime.getHour())) * 100);
        std::cout << "AiEscort Started at: " << mStartingSecond << " For: "
            << duration << "Started At: " << mStartingSecond << std::endl;
    }
}


MWMechanics::AiEscort *MWMechanics::AiEscort::clone() const
{
    return new AiEscort(*this);
}

bool MWMechanics::AiEscort::execute (const MWWorld::Ptr& actor)
{
    // If AiEscort has ran for as long or longer then the duration specified
    // and the duration is not infinite, the package is complete.
    if(mDuration != 0)
    {
        MWWorld::TimeStamp current = MWBase::Environment::get().getWorld()->getTimeStamp();
        unsigned int currentSecond = ((current.getHour() - int(current.getHour())) * 100);
        std::cout << "AiEscort: " << currentSecond << " time: " << currentSecond - mStartingSecond << std::endl;
        if(currentSecond - mStartingSecond >= mDuration)
        {
            std::cout << "AiEscort Has Run It's Duration: " << currentSecond - mStartingSecond
                << " >= " << mDuration << std::endl;
            return true;
        }
    }

    ESM::Position pos = actor.getRefData().getPosition();
    bool cellChange = actor.getCell()->mCell->mData.mX != cellX || actor.getCell()->mCell->mData.mY != cellY;
    MWWorld::Ptr player = MWBase::Environment::get().getWorld()->getPlayer().getPlayer();
    const ESM::Pathgrid *pathgrid =
        MWBase::Environment::get().getWorld()->getStore().get<ESM::Pathgrid>().search(*actor.getCell()->mCell);


        if(actor.getCell()->mCell->mData.mX != player.getCell()->mCell->mData.mX)
        {
            int sideX = sgn(actor.getCell()->mCell->mData.mX - player.getCell()->mCell->mData.mX);
            // Check if actor is near the border of an inactive cell. If so, disable AiEscort.
            // FIXME: This *should* pause the AiEscort package instead of terminating it.
            if(sideX*(pos.pos[0] - actor.getCell()->mCell->mData.mX * ESM::Land::REAL_SIZE) > sideX*(ESM::Land::REAL_SIZE/2. - 200)) 
            {
                MWWorld::Class::get(actor).getMovementSettings(actor).mPosition[1] = 0;
                return true;
            }
        }
        if(actor.getCell()->mCell->mData.mY != player.getCell()->mCell->mData.mY)
        {
            int sideY = sgn(actor.getCell()->mCell->mData.mY - player.getCell()->mCell->mData.mY);
            // Check if actor is near the border of an inactive cell. If so, disable AiEscort.
            // FIXME: This *should* pause the AiEscort package instead of terminating it.
            if(sideY*(pos.pos[1] - actor.getCell()->mCell->mData.mY * ESM::Land::REAL_SIZE) > sideY*(ESM::Land::REAL_SIZE/2. - 200)) 
            {
                MWWorld::Class::get(actor).getMovementSettings(actor).mPosition[1] = 0;
                return true;
            }
        }


    if(!mPathFinder.isPathConstructed() || cellChange)
    {
        cellX = actor.getCell()->mCell->mData.mX;
        cellY = actor.getCell()->mCell->mData.mY;
        float xCell = 0;
        float yCell = 0;
        if (actor.getCell()->mCell->isExterior())
        {
            xCell = actor.getCell()->mCell->mData.mX * ESM::Land::REAL_SIZE;
            yCell = actor.getCell()->mCell->mData.mY * ESM::Land::REAL_SIZE;
        }

        ESM::Pathgrid::Point dest;
        dest.mX = mX;
        dest.mY = mY;
        dest.mZ = mZ;

        ESM::Pathgrid::Point start;
        start.mX = pos.pos[0];
        start.mY = pos.pos[1];
        start.mZ = pos.pos[2];

        mPathFinder.buildPath(start,dest,pathgrid,xCell,yCell);
    }

    if(mPathFinder.checkIfNextPointReached(pos.pos[0],pos.pos[1],pos.pos[2]))
    {
        MWWorld::Class::get(actor).getMovementSettings(actor).mPosition[1] = 0;
        return true;
    }

    const MWWorld::Ptr follower = MWBase::Environment::get().getWorld()->getPtr(mActorId, false);
    const float* const leaderPos = actor.getRefData().getPosition().pos;
    const float* const followerPos = follower.getRefData().getPosition().pos;
    double differenceBetween[3];

    for (short i = 0; i < 3; ++i)
        differenceBetween[i] = (leaderPos[i] - followerPos[i]);

    float distanceBetweenResult =
        std::sqrt((differenceBetween[0] * differenceBetween[0]) + (differenceBetween[1] * differenceBetween[1]) + (differenceBetween[2] * differenceBetween[2]));

    if(distanceBetweenResult <= 500)
    {
        float zAngle = mPathFinder.getZAngleToNext(pos.pos[0],pos.pos[1],pos.pos[2]);
        MWBase::Environment::get().getWorld()->rotateObject(actor,0,0,zAngle,false);
        MWWorld::Class::get(actor).getMovementSettings(actor).mPosition[1] = 1;
    }
    // Stop moving if the player is to far away
    else
    {
        // TODO: Set the actor to do the equivilent of AIWander 0 0 0 playing the "look back" idle animation.
        MWWorld::Class::get(actor).getMovementSettings(actor).mPosition[1] = 0;
    }

    return false;
}

int MWMechanics::AiEscort::getTypeId() const
{
    return 2;
}

