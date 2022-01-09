#ifndef SEM_H
#define SEM_H
#include "synch.h"
class Semaphore; // forward declare to ignore bug
class Sem
{
private:
	char name[50];	
	Semaphore* sem;		
public:
	Sem(char* na, int i);
	~Sem();
	void wait();
	void signal();
	char* GetName();
};
#endif