#include "syscall.h"

int main(){
    int success,FileID,processID,id,tmp,water=10;
    FileID=Open("output.txt",0);
    processID=CurrentThreadId();
    while (water--){
        Wait("voinuoc");
        id='0'+processID;
        while(1){
            if (Read(&tmp,1,FileID)<1){
                break;
            }
        }
        Write(&id,1,FileID);
        Write(" ",1,FileID);
        Signal("voinuoc");
    }
    Close(FileID);
}
