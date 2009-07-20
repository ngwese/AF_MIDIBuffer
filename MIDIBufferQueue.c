/* -*- Mode: C; -*-
 *
 * ___ MIDIBufferQueue.c _________________________________________
 * 
 * $URL: https://mm1.local/svn/private/trunk/lib/c/AF_MIDIBuffer/MIDIBufferQueue.c $
 * $Id: MIDIBufferQueue.c 211 2009-07-20 06:26:24Z greg $
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

#include "AF_MIDIBuffer_Globals.h"
#include "MIDIBufferQueue.h"
#include "MIDIBufferQueue_Private.h"
#include "MIDIBufferPool_Private.h"

MIDIBufferQueue *MIDIBufferQueueCreate(unsigned int queueSize,
				       MIDIBufferPool *firstPool,
				       MIDIBufferPool *secondPool)
{
  MIDIBufferQueue *queue;
  OSStatus status;
  
  queue = malloc(sizeof(MIDIBufferQueue));
  if (queue == NULL) {
    return NULL;
  }
  
  status = MPCreateQueue(&(queue->queue));
  if (status != noErr) {
    free(queue);
    return NULL;
  }
  
  status = MPSetQueueReserve(queue->queue, queueSize);
  if (status != noErr) {
    free(queue);
    return NULL;
  }
  
  queue->isActive = 0;
  queue->packetsDropped = 0;
  queue->notifyErrors = 0;
  queue->maxPacketSize = 0;
  queue->maxPacketListLength = 0;
  queue->allocateIfNeeded = 0;
  queue->dynamicAllocations = 0;
  
  queue->pool[0] = firstPool;
  queue->pool[1] = secondPool;
  queue->pool[2] = NULL;
  
  return queue;
}

void MIDIBufferQueueResetCounters(MIDIBufferQueue *queue)
{
  queue->packetsDropped = 0;
  queue->notifyErrors = 0;
  queue->maxPacketSize = 0;
  queue->maxPacketListLength = 0;
  queue->dynamicAllocations = 0;
  
}  

MIDIBufferPool *MIDIBufferQueueGetPool(MIDIBufferQueue *queue, unsigned int index)
{
  if (index < 3)
    return queue->pool[index];
  else
    return NULL;
}

MIDIBufferPool *MIDIBufferQueueSetPool(MIDIBufferQueue *queue, unsigned int index,
                                           MIDIBufferPool *pool)
{
  if (index < 3)
    return queue->pool[index] = pool;
  else
    return NULL;
}

unsigned long MIDIBufferQueueGetPacketsDropped(MIDIBufferQueue *queue)
{
  return queue->packetsDropped;
}

unsigned long MIDIBufferQueueGetNotifyErrors(MIDIBufferQueue *queue)
{
  return queue->notifyErrors;
}

unsigned long MIDIBufferQueueGetMaxPacketSize(MIDIBufferQueue *queue)
{
  return queue->maxPacketSize;
}

unsigned long MIDIBufferQueueGetMaxPacketListLength(MIDIBufferQueue *queue)
{
  return queue->maxPacketListLength;
}

unsigned long MIDIBufferQueueGetDynamicAllocationCount(MIDIBufferQueue *queue)
{
  return queue->dynamicAllocations;
}

int MIDIBufferQueueSetAllocateFlag(MIDIBufferQueue *queue, int flag)
{
  /* probably don't need to lock */
  return queue->allocateIfNeeded = flag;
}

int MIDIBufferQueueGetAllocateFlag(MIDIBufferQueue *queue)
{
  return queue->allocateIfNeeded;
}

int MIDIBufferQueueSetActive(MIDIBufferQueue *queue, int state)
{
  /* probably don't need to lock */
  return queue->isActive = state;
}

int MIDIBufferQueueGetActive(MIDIBufferQueue *queue)
{
  return queue->isActive;
}

int MIDIBufferQueueRelease(MIDIBufferQueue *queue)
{
  OSStatus status;
  
  if (queue != NULL) {
    status = MPDeleteQueue(queue->queue);
    free(queue);
  }
  
  return 0;
}

OSStatus MIDIBufferQueueInsertNullBuffer(MIDIBufferQueue *queue)
{
  return MPNotifyQueue(queue->queue, NULL, NULL, NULL);
}

MIDIBuffer *MIDIBufferNextFromQueue(MIDIBufferQueue *queue, Duration timeout)
{
  OSStatus status;
  MIDIBuffer *buffer = NULL;
  
  status = MPWaitOnQueue(queue->queue, (void **)&buffer, NULL, NULL, timeout);
  
  return buffer;
}

/* MAINT: how to pul this one off? */
int MIDIBufferQueueNextWillBlock(MIDIBufferQueue *queue)
{
  OSStatus status;

  return 1;
}

/*
 * _____ MIDIBufferQueueReadProc ___________________________________________
 *
 */

void MIDIBufferQueueReadProc(const MIDIPacketList *packetList, void *refCon, void *connRefCon)
{
  MIDIBufferQueue *queue = (MIDIBufferQueue *)refCon;
  MIDIBufferPool **pool;
  MIDIBuffer *buffer = NULL;
  const MIDIPacket *packet;
  
  void *source;
  unsigned int i, sourceSize;
  
  OSStatus status;
  
#ifdef __DEBUG__
  if (gDebug) {
    fprintf(stderr, ">> MIDIQueueReadProc() called, queue = 0x%x, refcon = 0x%x\n",
            (unsigned int)queue,
            (unsigned int)connRefCon);
    MIDIPacketListDump(&packetList->packet[0], packetList->numPackets);
  }
#endif
  
  if (queue && queue->isActive) {
    
    /* track largest packet list length */
#ifdef __LOG__
    if (queue->maxPacketListLength < packetList->numPackets)
      queue->maxPacketListLength = packetList->numPackets;
#endif
    
    /* get pointer to first packet */
    packet = &packetList->packet[0];
    
    /* save addr of first packet to copy */
    source = (void *)packet;
    sourceSize = 0;
    
    /* find the size of the entire packetlist */
    for (i = 0; i < packetList->numPackets; i++) {
      sourceSize += sizeof_MIDIPacket(packet->length);
      
#ifdef __DEBUG__
      if (gDebug)
        fprintf(stderr, ">> MIDIPacket[%d] length = %d, size = %d\n", i, packet->length, sizeof_MIDIPacket(packet->length));
#endif
      
#ifdef __LOG__
      if (queue->maxPacketSize < packet->length)
        queue->maxPacketSize = packet->length;
#endif
      
      packet = MIDIPacketNext(packet);
    }	
    
#ifdef __DEBUG__
    if (gDebug)
      fprintf(stderr, ">> MIDIPacketList size = %d\n",  sourceSize);
#endif
    
    /* find a buffer of appropriate size */
    i = 0;
    pool = queue->pool;
    while (pool[i] != NULL) {
      if (pool[i]->bufferTotalSize >= sourceSize) { 
        /* wait 1 millisecond max */
        buffer = MIDIBufferNextFromPool(pool[i], kDurationMillisecond); 
        if (buffer != NULL) {
          
          break;
        }
        else {
          pool[i]->lockErrors++;
        }
      }
      i++;
    }
    
    if (buffer == NULL && queue->allocateIfNeeded) {
      /* allocate something, problematic priority inversion possible */
      buffer = MIDIBufferCreate(sourceSize);
      queue->dynamicAllocations++;
    }
    
    if (buffer != NULL) {
      /* copy packets to buffer */
      memcpy(&(buffer->packet[0]), source, sourceSize);
      buffer->numPackets = packetList->numPackets;
      
      /* record the connection constant */
      buffer->srcRefCon = connRefCon;
      
#ifdef __DEBUG__
      if (gDebug) {
        fprintf(stderr, ">> finished copy to buffer.\n");
        MIDIBufferDump(buffer);
      }
#endif
      
      /* post buffer to queue */
      status = MPNotifyQueue(queue->queue, (void *)buffer, NULL, NULL);
      if (status != noErr) {
        MIDIBufferReleaseToPool(buffer);
        queue->notifyErrors++;
        
#ifdef __LOG__
        fprintf(stderr,
                "MIDIQueueReadProc(): Failed adding buffer to queue = 0x%x, error = %d\n",
                queue, (int)status);
#endif
      }
    }
    else {
      queue->packetsDropped++;
#ifdef __LOG__
      fprintf(stderr, "MIDIQueueReadProc(): buffer == NULL; dropping packets = 0x%x\n",
              (unsigned int)packetList);
#endif
    }
  }
  
#ifdef __DEBUG__
  if (gDebug)
    fprintf(stderr, ">> MIDIQueueReadProc() returning\n");
#endif
}

/*
 * _____ MIDIBufferQueueSysexCompletionProc ___________________________________________
 *
 */

void MIDIBufferQueueSysexCompletionProc(MIDISysexSendRequest *request)
{
  void *sendBuffer = request->completionRefCon;
  
  
#ifdef __DEBUG__
  if (gDebug) {
    fprintf(stderr, ">> MIDIBufferQueueSysexCompletionProc() called, request = 0x%x, refcon = 0x%x\n",
            (unsigned int)request,
            (unsigned int)sendBuffer);
  }
#endif
  
  free(request);
  if (sendBuffer != NULL) 
    free(sendBuffer);
}
