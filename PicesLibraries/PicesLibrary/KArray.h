#ifndef  _KARRAY_
#define  _KARRAY_

//************************************************************************************
//*                                                                                  *
//*  Developed By:  Kurt A. Kramer                                                   *
//*  Date:          December-10th-2003                                               *
//*                                                                                  *
//************************************************************************************
//*  Meant to be a very simple dynamic array                                         *
//************************************************************************************


#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <memory>

typedef  unsigned kkint32  kkuint32;


namespace MLL
{

template <class Entry>
class  KArray
{

typedef  Entry*  EntryPtr;

private:
    kkuint32    allocSize;

    EntryPtr  block;

    Entry     initialValue;

    kkuint32    growthRate;

    kkuint32    size;

    

public:
    KArray (kkint32 _initialSize,
            Entry _initialValue);


    KArray (KArray&  k);


    ~KArray ();


    void  Swap (kkuint32 idx1,  
                kkuint32 idx2
               );


    Entry&    operator[] (kkint32 i);


    KArray&   operator= (KArray& k);

    void  Randomize ();



private:
};









template <class Entry>
KArray<Entry>::KArray (kkint32 _initialSize,
                       Entry _initialValue
                      ):
    initialValue (_initialValue),
    size         (0)
{
  if  (_initialSize == 0)
  {
    grwowthRate = 10;
  }
  else
  {
    growthRate = _initialSize;
  }

  block = new Entry[growthRate];
  allocSIze = growthRate;

  for (kkint32 i = 0; i < allocSize; i++)
  {
    block[i] = initialValue;
  }
}




template <class Entry>
KArray<Entry>::KArray (KArray&  k):
    growthRate   (k.growthRate),
    initialValue (k.initialValue),
    size         (k.size)
{
  block = new Entry[size];
  allocSize = size;

  for  (kkuint32 i = 0;  i < size;  i++)
     block[i] = k.block[i];
} 



template <class Entry>
KArray<Entry>::~KArray () 
{
  delete  block;
  block = NULL;
}       




template <class Entry>
void  KArray<Entry>::Swap (kkuint32 idx1,  
                           kkuint32 idx2
                          )
{
  Entry  temp = block[idx1];
  block[idx1] = block[idx2];
  block[idx2] = temp;
}




template <class Entry>
Entry&   KArray<Entry>::operator[] (kkint32 i)
{
  if  (i > size)
  {
    if  (i > allocSize)
    {
      kkuint32  newAllocSize = i + growthRate;
      Entry*  newBlock = new Entry[newAllocSize];

      kkuint32  i = 0;
      while  (i < size)
      {
        newBlock[i] = block[i];
        i++;
      }

      while  (i < newAllocSize)
      {
        newBlock[i] = initialVAlue;
        i++
      }

      delete block;
      block = newBlock;
      allocSize = newAllocSize;
    }

    size = i;
  }

  return  block[i];
}



template <class Entry>
KArray<Entry>&   KArray<Entry>::operator= (KArray& k)
{
  delete  block;

  size = k.size;
  block = new Entry[size];
  allocSize = size;
  growthRate = k.growthRate;

  for  (kkuint32 i = 0;  i < size;  i++)
     block[i] = k.block[i];

  return *this;
}




void  KArray<Entry>::Randomize ()
{
  kkuint32 i;
  kkuint32 j;

  srand (time (0));
  for  (i = 0;  i < numEntries;  i++)
  {
    j =  rand () *  (size - 1) / (RAND_MAX - 1);
    SwapIndexes  (i, j);
  }
}  /* RandomizeOrder */


}  /* namespace MML */