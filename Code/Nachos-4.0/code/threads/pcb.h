#ifndef PCB_H
#define PCB_H

#define MAX_FILE 10
#define CONSOLE_INP 0
#define CONSOLE_OUT 1
#include "thread.h"
#include "synch.h"

class Thread; // forward declare to ignore bug
class Semaphore; // forward declare to ignore bug


class PCB
{
private:
    Semaphore* joinsem;         // semaphore cho quá trình join
    Semaphore* exitsem;         // semaphore cho quá trình exit
    Semaphore* multex;          // semaphore cho quá trình truy xuất đọc quyền  

    int exitcode;		
    int numwait;                // số tiến trình đã join

    char FileName[32];          // Ten cua tien trinh

    Thread* thread;             // Tien trinh cua chuong trinh
    Bitmap* bmfile;             
    int pid;
public:
    int parentID;               // ID cua tien trinh parent da chay truoc do
    
    OpenFile** fileTable;       //fileTable đe luu cac file dang duoc mo
    
    
    PCB(int = 0);               // Contructor
    ~PCB();                     // Destructor

    int Exec(char*,int);        // Tao mot thread moi
    int GetID();                // Trả về ProcessID của tiến trình gọi thực hiện
    int GetNumWait();           // Trả về số lượng tiến trình chờ


    int FindFreeSlot();      // Tim slot trong fileTable

    void JoinWait();            // Tiến trình cha đợi tiến trình con kết thúc
                        
    void ExitWait();             //Tiến trình con kết thúc

    void JoinRelease();         // 2. Báo cho tiến trình cha thực thi tiếp
    void ExitRelease();         // 3. Cho phép tiến trình con kết thúc

    void IncNumWait();          // Tăng số tiến trình chờ
    void DecNumWait();          // Giảm số tiến trình chờ

    void SetExitCode(int);      // Đặt exitcode của tiến trình
    int GetExitCode();          // Trả về exitcode

    void SetFileName(char*);    // Set ten tien trinh
    char* GetFileName();        // Tra ve ten tien trinh
    bool IsExist(int);

    OpenFile *Open(char *name);	
    OpenFile *Open(char *name, int type);
    bool Remove(char *name);
    
};

#endif // PCB_H