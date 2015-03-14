/* ImageExtractionThread.cpp -- Manages the threads that perform the image extraction process.
 * Copyright (C) 2012 Kurt Kramer
 * For conditions of distribution and use, see copyright notice in ImageExtractionManager.h
 */
#include "FirstIncludes.h"
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "MsgQueue.h"
using namespace KKB;


#include "ImageExtractionThread.h"
#include "ExtractionManager.h"
using namespace ImageExtractionManager;






ImageExtractionThread::ImageExtractionThread (ExtractionParms&      _parms,
                                              ExtractionManagerPtr  _extractionManager,
                                              const KKStr&          _threadName,
                                              MsgQueuePtr           _msgQueue
                                             ):
   KKThread (_threadName, NULL, _msgQueue),
   extractionManager (_extractionManager),
   parms             (_parms)
{
}



ImageExtractionThread::~ImageExtractionThread ()
{
}







