/**
 * ReaderFactory.c
 * (c) 2004 - 2006 SkyeTek, Inc. All Rights Reserved.
 *
 * Implementation of the ReaderFactory.
 */
#include "../SkyeTekAPI.h"
#include "Reader.h"
#include "ReaderFactory.h"
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

static LPREADER_FACTORY ReaderFactories[] = {
  &SkyetekReaderFactory
};

#define READERFACTORIES_COUNT sizeof(ReaderFactories)/sizeof(LPREADER_FACTORY)

unsigned int 
ReaderFactory_GetCount(void)
{
  return READERFACTORIES_COUNT; 
}

LPREADER_FACTORY 
ReaderFactory_GetFactory(
  unsigned int    i
  )
{
  if(i > READERFACTORIES_COUNT)
    return NULL;
  else
    return ReaderFactories[i];
}

SKYETEK_STATUS 
CreateReaderImpl(
  LPSKYETEK_DEVICE    device, 
  LPSKYETEK_READER    *reader
  )
{
  LPREADER_FACTORY pReaderFactory;
  unsigned int ix;
  
  for(ix = 0; ix < ReaderFactory_GetCount(); ix++) 
    {
      pReaderFactory = ReaderFactory_GetFactory(ix);
    
      if(pReaderFactory->CreateReader(device, reader) == SKYETEK_SUCCESS)
        return SKYETEK_SUCCESS;
    
    }
  
  return SKYETEK_FAILURE;
}

unsigned int 
DiscoverReadersImpl(
  LPSKYETEK_DEVICE      *mDevices, 
  unsigned int          deviceCount, 
  LPSKYETEK_READER      **mReaders
  )
{
  LPREADER_FACTORY pReaderFactory;
  unsigned int ix, readerCount, localCount;
  LPSKYETEK_READER* localReaders;
  
  readerCount = 0;
  
  for(ix = 0; ix < ReaderFactory_GetCount(); ix++) 
  {
    pReaderFactory = ReaderFactory_GetFactory(ix);
    localReaders = NULL;
    localCount = pReaderFactory->DiscoverReaders(mDevices, deviceCount, &localReaders);
  
    if(localCount > 0) 
    {
      *mReaders = (LPSKYETEK_READER*)realloc(*mReaders, (readerCount + localCount) * sizeof(LPSKYETEK_READER));
      memcpy(((*mReaders) + readerCount), localReaders, localCount*sizeof(LPSKYETEK_READER));
      free(localReaders);
      readerCount += localCount;
    }
  }

  return readerCount;
}

void 
FreeReadersImpl(
  LPSKYETEK_READER    *mReaders, 
  unsigned int        count
  )
{
  LPREADER_FACTORY pReaderFactory;
  unsigned int ix;
  for(ix = 0; ix < ReaderFactory_GetCount(); ix++) 
  {
    pReaderFactory = ReaderFactory_GetFactory(ix);
    pReaderFactory->FreeReaders(mReaders,count);
  }
}

void 
FreeReaderImpl(
  LPSKYETEK_READER lpReader
  )
{
  LPREADER_FACTORY pReaderFactory;
  unsigned int ix;
  for(ix = 0; ix < ReaderFactory_GetCount(); ix++) 
  {
    pReaderFactory = ReaderFactory_GetFactory(ix);
    if( pReaderFactory->FreeReader(lpReader) )
      break;
  }
}

