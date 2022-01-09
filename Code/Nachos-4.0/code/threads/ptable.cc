#include "ptable.h"
#include "main.h"

#define For(i,a,b) for (int i = (a); i < b; ++i)

PTable::PTable(int size) 
{

    if (size < 0)
        return;

    psize = size;
    bm = new Bitmap(size);
    bmsem = new Semaphore("bmsem",1);

    For(i,0,MAX_PROCESS){
		pcb[i] = 0;
    }

	bm->Mark(0);

	pcb[0] = new PCB(0);
	pcb[0]->SetFileName("./test/scheduler");
	pcb[0]->parentID = -1;
}

PTable::~PTable()
{
    if( bm != 0 ){
		delete bm;
	}
	
    
    For(i,0,psize){
		if(pcb[i] != 0)
			delete pcb[i];
    }
		
	if( bmsem != 0){
		delete bmsem;
	}
}

int PTable::ExecUpdate(char* name)
{
    //Gọi mutex->P(); để giúp tránh tình trạng nạp 2 tiến trình cùng 1 lúc.
	bmsem->P();
	// Kiểm tra sự tồn tại của chương trình “name”.
	if(name == NULL)
	{
		printf("\nPTable::Exec : Can't not execute name is NULL.\n");
		bmsem->V();
		return -1;
	}
	// So sánh tên chương trình và tên của currentThread để chắc chắn rằng chương trình này không gọi thực thi chính nó.
	if( strcmp(name,"./test/scheduler") == 0 || strcmp(name,kernel->currentThread->getName()) == 0 )
	{
		printf("\nPTable::Exec : Can't not execute itself.\n");		
		bmsem->V();
		return -1;
	}
	// Tìm slot trống trong bảng Ptable.
	int index = this->GetFreeSlot();
    // Kiểm tra còn slot không
	if(index < 0)
	{
		printf("\nPTable::Exec :There is no free slot.\n");
		bmsem->V();
		return -1;
	}
	//Có slot riêng thì tạo PCB 
	pcb[index] = new PCB(index);
	pcb[index]->SetFileName(name);
	// parrentID là processID của currentThread
    pcb[index]->parentID = kernel->currentThread->processID;
	// Gọi phương thức Exec của lớp PCB.
	int pid = pcb[index]->Exec(name,index);
	printf("\npid: %d\n",pid);
	// Gọi bmsem->V()
	bmsem->V(); 
	// Trả về kết quả thực thi của PCB->Exec.
	return pid;
}

int PTable::JoinUpdate(int id)
{
	// Ta kiểm tra tính hợp lệ của processID id 
	if(id < 0)
	{
		printf("\nPTable::JoinUpdate : id = %d", id);
		return -1;
	}
	// Kiểm tra tiến trình đang chạy là tiến trình cha
	if(kernel->currentThread->processID != pcb[id]->parentID)
	{
		printf("\nPTable::JoinUpdate Khong the join tien trinh nay vi la tien trinh cha.\n");
		return -1;
	}
	// Tăng numWait
	pcb[pcb[id]->parentID]->IncNumWait();

	pcb[id]->JoinWait();

	// Xử lý exitcode.	
	int ec = pcb[id]->GetExitCode();
	// ExitRelease() để cho phép tiến trình con thoát.
	pcb[id]->ExitRelease();

	return ec;
}

PCB * PTable::GetPCB(int id){
	return pcb[id];
}

int PTable::ExitUpdate(int exitcode)
{              
    // Trường hợp là main process
	int id = kernel->currentThread->processID;
	if(id == 0)
	{
		
		kernel->currentThread->FreeSpace();		
		kernel->interrupt->Halt();
		return 0;
	}
    
        if(IsExist(id) == false)
	{
		printf("\nPTable::ExitUpdate: Id %d khong ton tai", id);
		return -1;
	}
	// Ngược lại gọi SetExitCode để đặt exitcode cho tiến trình gọi.
	pcb[id]->SetExitCode(exitcode);
	pcb[pcb[id]->parentID]->DecNumWait();
    // Gọi JoinRelease để giải phóng tiến trình cha đang đợi nó(nếu có) và ExitWait() để xin tiến trình cha
    // cho phép thoát.
	pcb[id]->JoinRelease();
    // 
	pcb[id]->ExitWait();
	
	Remove(id);
	return exitcode;
}

int PTable::GetFreeSlot()
{
	return bm->FindAndSet();
}

bool PTable::IsExist(int pid)
{
	return bm->Test(pid);
}

void PTable::Remove(int pid)
{
	bm->Clear(pid);
	if(pcb[pid] != 0)
		delete pcb[pid];
}

char* PTable::GetFileName(int id)
{
	return (pcb[id]->GetFileName());
}
 