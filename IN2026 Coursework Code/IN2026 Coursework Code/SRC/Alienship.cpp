#include <stdlib.h>
#include "GameUtil.h"
#include "Alienship.h"
#include "BoundingShape.h"

Alienship::Alienship(void) : GameObject("Alienship")
{
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 7.5;
	mVelocity.y = 7.5;
	mVelocity.z = 0.0;
}

Alienship::~Alienship(void)
{
}

void Alienship::Render(void)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.6, 0.6, 0.6);
	glVertex3f(-7, -7, 0.0);
	glVertex3f(-10, 0, 0.0);
	glVertex3f(-7, 7, 0.0);
	glVertex3f(0, 10, 0.0);
	glVertex3f(7, 7, 0.0);
	glVertex3f(10, 0, 0.0);
	glVertex3f(7, -7, 0.0);
	glVertex3f(0, -10, 0.0);
	glEnd();
	glEnable(GL_LIGHTING);
}

bool Alienship::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Bullet") && o->GetType() != GameObjectType("Spaceship")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Alienship::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}
