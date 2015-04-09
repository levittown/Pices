/* Blob.cpp -- Works with Raster class to track individual connected component in Raster.
 * Copyright (C) 1994-2011 Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKB.h
 */

#include  "FirstIncludes.h"
#include  <stdlib.h>
#include  <iostream>
#include  <map>
#include  <vector>

#include  "MemoryDebug.h"
using namespace std;

#include "BitString.h"
#include "Blob.h"
using namespace KKB;




Blob::Blob (kkint32  _id):
  colLeft    (0),
  colRight   (0),
  id         (_id),
  pixelCount (0),
  rowBot     (0),
  rowTop     (0)
 
{
}



Blob::~Blob ()
{
}














BlobPtr  BlobList::LookUpByBlobId (kkint32  blobId)
{
  map<kkint32,BlobPtr>::iterator  idx;
  idx = blobIndex.find (blobId);
  if  (idx == blobIndex.end ())
    return NULL;
  return idx->second;
}  /* LookUpByBlobId */



BlobList::BlobList (bool  _owner):

  KKQueue<Blob> (_owner),
  nextBlobId (0)
{
}


BlobList::~BlobList ()
{
}


BlobPtr  BlobList::NewBlob (kkuint32  rowTop,
                            kkuint32  colLeft
                           )
{
  BlobPtr blob = new Blob (nextBlobId);
  blob->rowTop  = rowTop;
  blob->rowBot  = rowTop;

  blob->colLeft  = colLeft;
  blob->colRight = colLeft; 

  nextBlobId++;
  PushOnBack  (blob);
  return  blob;
}



void  BlobList::MergeBlobIds (BlobPtr  blob,
                              kkint32  blobId,
                              kkint32**  blobIds
                             )
{                               
  kkint32  newId = blob->Id ();
  
  map<kkint32,BlobPtr>::iterator  idx;
  idx = blobIndex.find (blobId);
  if  (idx == blobIndex.end ())
    return;
  BlobPtr  blobToMerge = idx->second;

  kkint32  col;
  kkint32  row;

  kkint32  rowBot   = blobToMerge->rowBot;
  kkint32  colRight = blobToMerge->colRight;

  for  (row = blobToMerge->rowTop; row <= rowBot; row++)
  {
    for  (col = blobToMerge->colLeft; col <= colRight; col++)
    {
      if  (blobIds[row][col] == blobId)
        blobIds[row][col] = newId;
    }
  }

  blob->rowTop   = Min (blob->rowTop,   blobToMerge->rowTop);
  blob->rowBot   = Max (blob->rowBot,   blobToMerge->rowBot);
  blob->colLeft  = Min (blob->colLeft,  blobToMerge->colLeft);
  blob->colRight = Max (blob->colRight, blobToMerge->colRight);
  blob->pixelCount = blob->pixelCount + blobToMerge->pixelCount;

  blobIndex.erase (idx);
  DeleteEntry (blobToMerge);
  delete  blobToMerge;
}  /* MergeBlobIds */




BlobPtr  BlobList::MergeIntoSingleBlob (BlobPtr  blob1,
                                        kkint32  blob2Id,
                                        kkint32**  blobIds
                                       )
{                               
  kkint32  blob1Id = blob1->Id ();
  map<kkint32,BlobPtr>::iterator  idx;
  idx = blobIndex.find (blob2Id);
  if  (idx == blobIndex.end ())
    return  blob1;
  BlobPtr  blob2 = idx->second;

  if  ((blob1Id == blob2Id)  ||  (blob1 == blob2))
  {
    return blob1;
  }

  BlobPtr  srcBlob  = NULL;
  BlobPtr  destBlob = NULL;

  if  (blob1->PixelCount () > blob2->PixelCount ())
  {
    srcBlob  = blob2;
    destBlob = blob1;
  }
  else
  {
    srcBlob = blob1;
    destBlob = blob2;
  }

  kkint32  destBlobId = destBlob->Id ();
  kkint32  srcBlobId  = srcBlob->Id ();

  kkint32  col = 0;
  kkint32  row = 0;

  kkint32  rowBot   = srcBlob->rowBot;
  kkint32  colRight = srcBlob->colRight;

  for  (row = srcBlob->rowTop;  row <= rowBot;  ++row)
  {
    for  (col = srcBlob->colLeft;  col <= colRight;  ++col)
    {
      if  (blobIds[row][col] == srcBlobId)
        blobIds[row][col] = destBlobId;
    }
  }

  destBlob->rowTop   = Min (destBlob->rowTop,   srcBlob->rowTop);
  destBlob->rowBot   = Max (destBlob->rowBot,   srcBlob->rowBot);
  destBlob->colLeft  = Min (destBlob->colLeft,  srcBlob->colLeft);
  destBlob->colRight = Max (destBlob->colRight, srcBlob->colRight);
  destBlob->pixelCount = destBlob->pixelCount + srcBlob->pixelCount;

  idx = blobIndex.find (srcBlobId);
  if  (idx != blobIndex.end ())
    blobIndex.erase (idx);
  DeleteEntry (srcBlob);
  delete  srcBlob;  srcBlob = NULL;
  return  destBlob;
}  /* MergeIntoSingleBlob */





BlobPtr  BlobList::LocateLargestBlob ()
{
  kkint32   idx;
  kkint32   qSize = QueueSize ();
  BlobPtr   blob     = NULL;
  BlobPtr   tempBlob = NULL;
  kkint32   largestSize = 0;

  for  (idx = 0; idx < qSize; idx++)
  {
    tempBlob = IdxToPtr (idx);
    if  (tempBlob->pixelCount > largestSize)
    {
      largestSize = tempBlob->pixelCount;
      blob = tempBlob;
    }
  }
  return  blob;
} /* LocateLargestBlob */



BlobPtr  BlobList::LocateMostComplete ()
{
  kkint32   idx;
  kkint32   qSize = QueueSize ();
  BlobPtr   blob     = NULL;
  BlobPtr   tempBlob = NULL;
  kkint32   largestSize = 0;

  for  (idx = 0; idx < qSize; idx++)
  {
    tempBlob = IdxToPtr (idx);

    kkint32  size = tempBlob->Height () * tempBlob->Width ();
    if  (size > largestSize)
    {
      largestSize = size;
      blob = tempBlob;
    }
  }
  return  blob;
} /* LocateMostComplete */






void  BlobList::PushOnBack  (BlobPtr  blob)
{
  blobIndex.insert (pair<kkint32, BlobPtr> (blob->Id (), blob));
  KKQueue<Blob>::PushOnBack  (blob);
}  /* PushOnBack */



void  BlobList::PushOnFront (BlobPtr  blob)
{
  blobIndex.insert (pair<kkint32, BlobPtr> (blob->Id (), blob));
  KKQueue<Blob>::PushOnFront  (blob);
}

