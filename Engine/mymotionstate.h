
#include "gameobject.h"

class MyMotionState : public btMotionState
{
protected:
	btTransform mInitialPosition;

public:
	MyMotionState(const btTransform &initialPosition)
	{
		mInitialPosition = initialPosition;
	}
	virtual ~MyMotionState()
	{
	}
	btVector3 getPos() {
		return mInitialPosition.getOrigin();
	}
	void setPos(btVector3 pos){
		mInitialPosition.setOrigin(pos);
	}
	virtual void getWorldTransform(btTransform &worldTrans) const
	{
		worldTrans = mInitialPosition;
	}

	virtual void setWorldTransform(const btTransform &worldTrans)
	{
		btVector3 pos = worldTrans.getOrigin();
		mInitialPosition.setOrigin(pos);
	}
};