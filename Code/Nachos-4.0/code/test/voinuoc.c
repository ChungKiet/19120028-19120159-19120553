#include "syscall.h"

int main(){
    int n,success,i,id;
    int PID[5];

    
    success=CreateFile("output.txt");
    success=CreateSemaphore("voinuoc",1);
    PrintString("So luong sinh vien ? (<5):");
    n=ReadNum(); 
    for (i=1;i<=n;++i)
    {
        id=Exec("../test/sinhvien");
        PID[i]=id;
    }
    for (i=1;i<=n;++i){
        Join(PID[i]);
    }

}
