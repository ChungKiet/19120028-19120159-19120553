#include "stable.h"

// Constructor
STable::STable()
{
	this->bm = new Bitmap(MAX_SEMAPHORE);

	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		this->semTab[i] = NULL;
	}
}

// Destructor
STable::~STable()
{
	if (this->bm)
	{
		delete this->bm;
		this->bm = NULL;
	}
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if (this->semTab[i])
		{
			delete this->semTab[i];
			this->semTab[i] = NULL;
		}
	}
}

int STable::FindFreeSlot()
{
	return this->bm->FindAndSet();
}

int STable::Create(char *name, int init)
{
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if (bm->Test(i))
		{
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				return -1;
			}
		}
	}
	int id = this->FindFreeSlot();
	if (id < 0)
	{
		return -1;
	}
	this->semTab[id] = new Sem(name, init);
	return 0;
}

int STable::Wait(char *name)
{
	for (int i = 0; i < 10; i++)
	{
	
		if (bm->Test(i))
		{
			// So sanh name voi name cua semaphore trong semTab
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				// Neu ton tai thi down()
				semTab[i]->wait();
				return 0;
			}
		}
	}
	printf("Khong ton tai!");
	return -1;
}

int STable::Signal(char *name)
{
	for (int i = 0; i < 10; i++)
	{
		// Kiem tra o thu i da duoc nap semaphore chua
		if (bm->Test(i))
		{
			// Neu co thi tien hanh so sanh name voi name cua semaphore trong semTab
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				// Cho semaphore up() khi ton tai
				semTab[i]->signal();
				return 0;
			}
		}
	}
	printf("Khong ton tai!");
	return -1;
}