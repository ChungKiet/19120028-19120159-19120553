// main.h 
//	This file defines the Nachos global variables
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef MAIN_H
#define MAIN_H

#include "ptable.h"
#include "copyright.h"
#include "debug.h"
#include "kernel.h"
#include "bitmap.h"
#include "stable.h"

class PTable; // forward declare to ignore bug
class STable; // forward declare to ignore bug
class Semaphore; // forward declare to ignore bug
extern Kernel *kernel;
extern Debug *debug;
extern Bitmap *gPhysPageBitMap;
extern Semaphore* addrLock;
extern PTable* pTab;
extern STable *semTab;
#endif // MAIN_H
