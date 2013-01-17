/*
 * OpenMW - The completely unofficial reimplementation of Morrowind
 *
 * This file (character.cpp) is part of the OpenMW package.
 *
 * OpenMW is distributed as free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 3 along with this program. If not, see
 * http://www.gnu.org/licenses/ .
 */

#include "character.hpp"

#include "../mwrender/animation.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/soundmanager.hpp"

#include "../mwworld/class.hpp"

namespace MWMechanics
{

CharacterController::CharacterController(const MWWorld::Ptr &ptr, MWRender::Animation *anim, CharacterState state)
  : mPtr(ptr), mAnimation(anim), mState(state), mSkipAnim(false)
{
    if(mAnimation)
        mAnimNames = mAnimation->getAnimationNames();
    if(mAnimNames.size() == 0)
    {
        mAnimation = NULL;
        return;
    }

    mAnimation->setController(this);
    switch(mState)
    {
        case CharState_Idle:
            mCurrentGroup = "idle";
            mAnimation->playGroup(mCurrentGroup, 1, 1);
            break;
        case CharState_Dead:
            mCurrentGroup = "death1";
            mAnimation->playGroup(mCurrentGroup, 1, 1);
            break;
    }
}

CharacterController::CharacterController(const CharacterController &rhs)
  : mPtr(rhs.mPtr), mAnimation(rhs.mAnimation), mAnimNames(rhs.mAnimNames)
  , mCurrentGroup(rhs.mCurrentGroup), mState(rhs.mState), mSkipAnim(rhs.mSkipAnim)
{
    if(mAnimNames.size() == 0)
        return;
    /* We've been copied. Update the animation with the new controller. */
    mAnimation->setController(this);
}


void CharacterController::markerEvent(const std::string &evt)
{
    if(evt.compare(0, 7, "sound: ") == 0)
    {
        MWBase::SoundManager *sndMgr = MWBase::Environment::get().getSoundManager();
        sndMgr->playSound3D(mPtr, evt.substr(7), 1.0f, 1.0f);
        return;
    }
    if(evt.compare(0, 10, "soundgen: ") == 0)
    {
        // FIXME: Lookup the SoundGen (SNDG) for the specified sound that corresponds
        // to this actor type
        return;
    }
    if(evt.length() <= mCurrentGroup.length()+2 || evt.compare(0, mCurrentGroup.length(), mCurrentGroup) != 0 ||
       evt.compare(mCurrentGroup.length(), 2, ": ") != 0)
    {
        std::cerr<< "Event \""<<evt<<"\" does not belong to group \""<<mCurrentGroup<<"\"" <<std::endl;
        return;
    }
}


Ogre::Vector3 CharacterController::update(float duration)
{
    if(mAnimation && !mSkipAnim)
        mAnimation->runAnimation(duration);
    mSkipAnim = false;
    return MWWorld::Class::get(mPtr).getMovementVector(mPtr);
}


void CharacterController::playGroup(const std::string &groupname, int mode, int count)
{
    // set mState = CharState_Idle?
    if(std::find(mAnimNames.begin(), mAnimNames.end(), groupname) != mAnimNames.end())
        mAnimation->playGroup(groupname, mode, count);
}

void CharacterController::skipAnim()
{
    mSkipAnim = true;
}


void CharacterController::setState(CharacterState state)
{
    mState = state;

    if(mAnimNames.size() == 0)
        return;
    switch(mState)
    {
        case CharState_Idle:
            mCurrentGroup = "idle";
            mAnimation->playGroup(mCurrentGroup, 1, 1);
            break;
        case CharState_Dead:
            mCurrentGroup = "death1";
            mAnimation->playGroup(mCurrentGroup, 1, 1);
            break;
    }
}

}
