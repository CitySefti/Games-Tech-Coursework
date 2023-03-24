#ifndef __EXTRALIVES_H__
#define __EXTRALIVES_H__

#include "Item.h"

class ExtraLives : public Item, public GameObject
{
public:
	ExtraLives();
	~ExtraLives();
	void Render(void);
};
#endif

