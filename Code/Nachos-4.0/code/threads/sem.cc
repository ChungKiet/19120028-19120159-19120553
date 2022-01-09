#include "sem.h"

Sem::Sem(char *na, int i)
{
	strcpy(this->name, na);
	sem = new Semaphore(this->name, i);
}
Sem::~Sem()
{
	if (sem){
		delete sem;
	}
}
void Sem::wait()
{
	sem->P(); 
}
void Sem::signal()
{
	sem->V(); 
}
char *Sem::GetName()
{
	return this->name;
}