#ifndef STABLE_H
#define STABLE_H
#define MAX_SEMAPHORE 10
#include "sem.h"
#include "bitmap.h"

class Sem; // forward declare to ignore bug
class STable // forward declare to ignore bug
{
private:
	Bitmap* bm;	// quản lý slot trống
	Sem* semTab[MAX_SEMAPHORE];	// quản lý tối đa 10 đối tượng Sem
public:
	//khởi tạo size đối tượng Sem để quản lý 10 Semaphore. Gán giá trị ban đầu là null
	// nhớ khởi tạo bm để sử dụng
	STable();		

	~STable();	// hủy các đối tượng đã tạo
	// Kiểm tra Semaphore “name” chưa tồn tại thì tạo Semaphore mới. Ngược lại, báo lỗi.
	int Create(char *name, int init);

	// Nếu tồn tại Semaphore “name” thì gọi this->P()để thực thi. Ngược lại, báo lỗi.
	int Wait(char *name);

	// Nếu tồn tại Semaphore “name” thì gọi this->V()để thực thi. Ngược lại, báo lỗi.
	int Signal(char *name);
	
	// Tìm slot trống.
	int FindFreeSlot();

	
};

#endif // STABLE_H