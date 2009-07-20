/* -*- Mode: C; -*-
 *
 * ___ MIDIBufferPool.c _________________________________________
 * 
 * $URL: https://mm1.local/svn/private/trunk/lib/c/AF_MIDIBuffer/MIDIBufferPool.c $
 * $Id: MIDIBufferPool.c 211 2009-07-20 06:26:24Z greg $
 *
 * Copyright (c) 2004-2005, Gregory C. Wuller. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 * 
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR 'AS IS' AND ANY EXPRESSED
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <CoreServices/CoreServices.h>
#include <stdio.h>

#include "AF_MIDIBuffer_Globals.h"
#include "MIDIBufferPool.h"
#include "MIDIBufferPool_Private.h"

void MIDIBufferPoolResetCounters(MIDIBufferPool *pool)
{
  pool->minBufferCount = pool->bufferCount;
  pool->lockErrors = 0;
  pool->releaseLockErrors = 0;
  pool->dynamicAllocations = 0;
  pool->bufferRequests = 0;
}  

unsigned int MIDIBufferPoolGetBufferSize(MIDIBufferPool *pool)
{
  return pool->bufferSize;
}

unsigned int MIDIBufferPoolGetBufferCount(MIDIBufferPool *pool)
{
  return pool->bufferCount;
}

unsigned long MIDIBufferPoolGetDynamicAllocationCount(MIDIBufferPool *pool)
{
  return pool->dynamicAllocations;
}

unsigned int MIDIBufferPoolGetMinBufferCount(MIDIBufferPool *pool)
{
  return pool->minBufferCount;
}

unsigned long MIDIBufferPoolGetBufferRequestCount(MIDIBufferPool *pool)
{
  return pool->bufferRequests;
}

unsigned long MIDIBufferPoolGetLockErrorCount(MIDIBufferPool *pool)
{
  return pool->lockErrors;
}

unsigned long MIDIBufferPoolGetReleaseLockErrorCount(MIDIBufferPool *pool)
{
  return pool->releaseLockErrors;
}

int MIDIBufferPoolSetAllocateFlag(MIDIBufferPool *pool, int flag)
{
  /* probably don't need to lock */
  return pool->allocateIfNeeded = flag;
}

int MIDIBufferPoolGetAllocateFlag(MIDIBufferPool *pool)
{
  return pool->allocateIfNeeded;
}

MIDIBufferPool *MIDIBufferPoolCreate(unsigned int count, unsigned int size)
{
  MIDIBufferPool *pool;
  MIDIBuffer *buffer;
  unsigned int i;

  pool = (MIDIBufferPool *)malloc(sizeof(MIDIBufferPool));

  if (pool != NULL) {
    pthread_mutex_init(&(pool->mutex), NULL);
    pthread_mutex_lock(&(pool->mutex));

    pool->allocateIfNeeded = 1;
    pool->bufferSize = size;
    pool->bufferTotalSize = sizeof_MIDIPacket(size);
    pool->bufferCount = count;
    pool->next = NULL;
    
    for (i = 0; i < count; i++) {
      buffer = MIDIBufferCreate(size); /* should verify allocation */
      buffer->next = pool->next; 
      pool->next = buffer;
    }

    MIDIBufferPoolResetCounters(pool);

    pthread_mutex_unlock(&(pool->mutex));
  }

  return pool;
}

void MIDIBufferPoolRelease(MIDIBufferPool *pool)
{
  MIDIBuffer *first, *next;

  pthread_mutex_lock(&(pool->mutex));

  first = pool->next;
  while (first != NULL) {
    next = first->next;
    free(first);
    first = next;
  }

  pthread_mutex_unlock(&(pool->mutex));
  
  free(pool);
}

MIDIBuffer *MIDIBufferNextFromPool(MIDIBufferPool *pool, Duration timeout)
{
  MIDIBuffer *buffer = NULL;

  pthread_mutex_lock(&(pool->mutex));
  pool->bufferRequests++;
  buffer = pool->next;

  if (buffer != NULL) {
    pool->next = buffer->next;
    pool->bufferCount--;
    buffer->next = NULL; /* unchain */
    buffer->pool = pool;
  }

  #ifdef __LOG__
  if (gLog) {
    if (pool->minBufferCount > pool->bufferCount) {
      pool->minBufferCount = pool->bufferCount;
    }
  }
  #endif
    
  pthread_mutex_unlock(&(pool->mutex));

  if (buffer == NULL && pool->allocateIfNeeded) {
    buffer = MIDIBufferCreate(pool->bufferSize);
    buffer->pool = pool;
    pool->dynamicAllocations++;
  }

  return buffer;
}

void MIDIBufferReleaseToPool(MIDIBuffer *buffer)
{
  MIDIBufferPool *pool;
  
  if (buffer != NULL) {
    pool = buffer->pool;

    if (buffer->pool != NULL) {
      pthread_mutex_lock(&(pool->mutex));

      buffer->next = pool->next;
      pool->next = buffer;
      pool->bufferCount++;

      pthread_mutex_unlock(&(pool->mutex));
    }
    else {
      MIDIBufferRelease(buffer);
    }
  }
}
