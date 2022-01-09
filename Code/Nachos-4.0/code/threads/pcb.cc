#include "main.h"
#include "pcb.h"
#include "directory.h"
PCB::PCB(int id)
{
	this->joinsem = new Semaphore("joinsem", 0);
	this->exitsem = new Semaphore("exitsem", 0);
	this->multex = new Semaphore("multex", 1);

	this->numwait = this->exitcode = 0;

	this->thread = NULL;
	this->bmfile = new Bitmap(MAX_FILE);
	this->pid = id;

	if (id <= 0)
		this->parentID = -1;
	else
		this->parentID = kernel->currentThread->processID;

	this->fileTable = new OpenFile *[MAX_FILE];
	for (int i = 0; i < 10; ++i)
	{
		fileTable[i] = NULL;
	}

	kernel->fileSystem->Create("stdin",0);
	fileTable[0] = kernel->fileSystem->Open("stdin");
	fileTable[0]->type = 1; // Init for stdin file. It can't open
	bmfile->Mark(0);

	kernel->fileSystem->Create("stdout",0);
	fileTable[1] = kernel->fileSystem->Open("stdout");
	fileTable[1]->type = 0;  // Init for stdout file. It can't open
	bmfile->Mark(1);
}

PCB::~PCB()
{
	if (bmfile != NULL)
		delete this->bmfile;
	if (joinsem != NULL)
		delete this->joinsem;
	if (exitsem != NULL)
		delete this->exitsem;
	if (multex != NULL)
		delete this->multex;
	if (thread != NULL)
	{
		thread->FreeSpace();
		thread->Finish();
	}
	for (int i = 0; i < 10; ++i)
	{
		if (fileTable[i] != NULL)
			delete fileTable[i];
	}
	delete[] fileTable;
}

void StartProcess_2(void *pid)
{
	int id = *((int *)pid); // check error . It cause segmentation fault
	delete (int *)pid;
	char *fileName = pTab->GetFileName((int)id);
	AddrSpace *space;
	space = new AddrSpace(fileName); 
	if (space == NULL)
	{
		printf("\nPCB::Exec: Can't create AddSpace.");
		return;
	}

	kernel->currentThread->space = space;

	space->InitRegisters(); 
	space->RestoreState();	

	kernel->machine->Run(); 
	ASSERT(FALSE);									
}

int PCB::Exec(char *filename, int id)
{
	//Gọi mutex->P(); để giúp tránh tình trạng nạp 2 tiến trình cùng 1 lúc.
	multex->P();

	// Kiểm tra thread đã khởi tạo thành công chưa, nếu chưa thì báo lỗi là không đủ bộ nhớ, gọi mutex->V() và return.
	this->thread = new Thread(filename);

	if (this->thread == NULL)
	{
		printf("\nPCB::Exec:: Not enough memory..!\n");
		multex->V();
		return -1;
	}
	//  Đặt processID của thread này là id.
	this->thread->processID = id;

	// Đặt parrentID của thread này là processID của thread gọi thực thi Exec
	this->parentID = kernel->currentThread->processID;
	// Gọi thực thi Fork(StartProcess_2,id) => Ta cast thread thành kiểu int, sau đó khi xử ký hàm StartProcess ta cast Thread về đúng kiểu của nó.
	printf("\nThread %d is created.\n", id);
	void *pid = new int;
	*(int *)pid = id;
	this->thread->Fork(StartProcess_2, pid); // StartProcess_2 : void Func(void* Args)
	
	multex->V();
	// Trả về id.
	return id;
}

int PCB::FindFreeSlot()
{
	return bmfile->FindAndSet();
}
int PCB::GetID() { return this->pid; }
int PCB::GetNumWait() { return this->numwait; }
int PCB::GetExitCode() { return this->exitcode; }

void PCB::SetExitCode(int ec) { this->exitcode = ec; }

void PCB::JoinWait()
{
	joinsem->P();
}

void PCB::JoinRelease()
{
	joinsem->V();
}

void PCB::ExitWait()
{
	exitsem->P();
}

void PCB::ExitRelease()
{
	exitsem->V();
}

void PCB::IncNumWait()
{
	multex->P();
	++numwait;
	multex->V();
}

void PCB::DecNumWait()
{
	multex->P();
	if (numwait > 0)
		--numwait;
	multex->V();
}

void PCB::SetFileName(char *fn) { strcpy(FileName, fn); }
char *PCB::GetFileName()
{
	if (thread != NULL)
	{
		return this->FileName;
	}
	return NULL;
}

bool PCB::IsExist(int id)
{
	if (id < 0 || id >= MAX_FILE)
		return false;
	return bmfile->Test(id);
}
OpenFile *PCB::Open(char *name)
{
	int fileDescriptor = OpenForReadWrite(name, FALSE);

	if (fileDescriptor == -1)
	{
		return NULL;
	}

	return new OpenFile(fileDescriptor);
}
OpenFile* PCB::Open(char *name, int type)
{
	OpenFile* f = kernel->fileSystem->Open(name);
	if(f == NULL)
		return NULL;
	f->type = type;
	return f;
}
bool PCB::Remove(char *name)
{
	return Unlink(name) == 0;
}