// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "synchconsole.h"
#include "machine.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case NoException:
		SysHalt();
		return;

	case PageFaultException:
		printf("\nPage Fault Exception.\n");
		ASSERT(false);
		break;

	case ReadOnlyException:
		printf("\nReadOnly Exception.\n");
		ASSERT(false);
		break;

	case BusErrorException:
		printf("\nBus error exception.\n");
		ASSERT(false);
		break;

	case AddressErrorException:
		printf("\nAddress error exception.\n");
		ASSERT(false);
		break;

	case OverflowException:
		printf("\nOverflow Exception.\n");
		ASSERT(false);
		break;

	case IllegalInstrException:
		printf("\nIllegal Instr Exception.\n");
		ASSERT(false);
		break;

	case NumExceptionTypes:
		printf("\nNum Exception Types.\n");
		ASSERT(false);
		break;

	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;

		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
			printf("%d + %d", kernel->machine->ReadRegister(4), kernel->machine->ReadRegister(5));
			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			increasePC();

			return;

			ASSERTNOTREACHED();

			break;

		case SC_ReadNum:
		{
			char ch = 0;
			int ret = 0;
			bool flag = true;
			bool sign = false;
			for (int i = 0; i < 255; i++)
			{
				// Doc 1 chu so
				ch = (char)kernel->synchConsoleIn->GetChar();
				//Xet truong hop so am
				if (i == 0 && ch == '-')
				{
					sign = true;
					continue;
				}
				if (ch == 10)
					break;
				if ('0' <= ch && ch <= '9')
				{
					int temp = ret;
					ret = ret * 10 + ch - '0';
					//Truong hop ret vuot qua gioi han cua int thi qua khao sat thi se bi sai lech ve gia tri
					if (temp > ret)
					{
						ExceptionHandler(OverflowException);
					}
				}
				else
					flag = false;
			}
			if (sign == true)
				ret *= -1;
			kernel->machine->WriteRegister(2, (flag) ? (int)ret : 0);
			increasePC();

			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_PrintNum:
		{
			//Doc tu tham so thu nhat
			int num = (int)kernel->machine->ReadRegister(4);
			bool sign = (num < 0);
			char a[10];
			//Neu la so 0
			if (num == 0)
				kernel->synchConsoleOut->PutChar('0');
			else
			{
				size_t len = 0;
				if (num < 0)
					num *= -1;
				while (num != 0)
				{
					a[len++] = (num % 10 + '0');
					num /= 10;
				}
				if (sign)
					kernel->synchConsoleOut->PutChar('-');
				for (int i = (int)len - 1; i >= 0; i--)
				{
					kernel->synchConsoleOut->PutChar(char(a[i]));
				}
			}
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_ReadChar:
			char ch;
			//Doc mot ky tu
			ch = (char)kernel->synchConsoleIn->GetChar();
			kernel->machine->WriteRegister(2, (int)ch);
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		case SC_PrintChar:
			char ch_print;
			ch_print = (char)kernel->machine->ReadRegister(4);
			//Xuat mot ky tu
			kernel->synchConsoleOut->PutChar(ch_print);
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
			//Day la vi du Random Number
		case SC_RandomNum:
			int r;
			//Khoi tao seed trong random
			RandomInit((unsigned)time(NULL));
			//random trong khoang cua int
			r = RandomNumber();
			DEBUG(dbgSys, "Random with number " << r << "\n");
			kernel->machine->WriteRegister(2, (int)r);
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		case SC_ReadString:
		{
			int bufferRead, lengthRead;
			//Doc dia chi cua chuoi
			bufferRead = (int)kernel->machine->ReadRegister(4);
			//Doc tham so thu hai
			lengthRead = (int)kernel->machine->ReadRegister(5);
			//Truong hop truyen length <=0
			if (lengthRead <= 0)
			{
				ExceptionHandler(AddressErrorException);
				//increasePC();
				return;
			}
			char *tempRead = new char[lengthRead + 1];
			for (int i = 0; i < lengthRead; ++i)
			{
				tempRead[i] = kernel->synchConsoleIn->GetChar();
				//Quy uoc chuoi ket thuc la \n
				if (tempRead[i] == '\n')
				{
					tempRead[i + 1] = '\0';
					break;
				}
			}
			tempRead[lengthRead] = '\0';
			//Copy tu vung nho system sang user
			System2User(bufferRead, lengthRead, tempRead);
			delete[] tempRead;
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintString:
		{
			int bufferWrite;
			char *tempWrite;
			bufferWrite = kernel->machine->ReadRegister(4);
			//Khoi tao vung nho tu user cho system
			tempWrite = User2System(bufferWrite, 255);
			int lengthWrite = 0;
			//Truong hop dung thi vi tri chuoi bang 0
			while (tempWrite[lengthWrite] != 0)
			{
				kernel->synchConsoleOut->PutChar(tempWrite[lengthWrite]);
				lengthWrite++;
				//Truong hop chuoi vuot qua 255
				if (lengthWrite == 255)
				{
					delete[] tempWrite;
					bufferWrite = bufferWrite + 255;
					tempWrite = User2System(bufferWrite, 255);
					lengthWrite = 0;
				}
			}
			delete[] tempWrite;
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_CurrentThreadId:
			SysCurrentThreadID();
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;

		case SC_Create:
			SysCreateFile(kernel->machine->ReadRegister(4));
			increasePC();
			return;
			ASSERTNOTREACHED();
			break;
		case SC_Open:
			SysOpen(kernel->machine->ReadRegister(4), kernel->machine->ReadRegister(5));
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		case SC_Close:
			SysClose(kernel->machine->ReadRegister(4));
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		case SC_Read:
			SysRead(kernel->machine->ReadRegister(4), kernel->machine->ReadRegister(5));
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		case SC_Write:
			SysWrite(kernel->machine->ReadRegister(4), kernel->machine->ReadRegister(5));
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
			
		case SC_Exec:
			SysExec(kernel->machine->ReadRegister(4));
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		case SC_Join:
			SysJoin(kernel->machine->ReadRegister(4));
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		case SC_Exit:
			SysExit(kernel->machine->ReadRegister(4));
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		case SC_CreateSemaphore:
			SysCreateSemaphore(kernel->machine->ReadRegister(4), kernel->machine->ReadRegister(5));
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		case SC_Wait:
			SysWait(kernel->machine->ReadRegister(4));
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		case SC_Signal:
			SysSignal(kernel->machine->ReadRegister(4));
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;
		case SC_Seek:
			SysSeek(kernel->machine->ReadRegister(4), kernel->machine->ReadRegister(5));
			increasePC();
			return;

			ASSERTNOTREACHED();
			break;

		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
