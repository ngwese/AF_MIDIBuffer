/* -*- Mode: C; -*-
 *
 * ___ MIDIBufferQueue.h _________________________________________
 * 
 * $URL: https://mm1.local/svn/private/trunk/lib/c/AF_MIDIBuffer/MIDIBufferQueue.h $
 * $Id: MIDIBufferQueue.h 211 2009-07-20 06:26:24Z greg $
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

#ifndef __MIDIBufferQueue_h__
#define __MIDIBufferQueue_h__

#include "MIDIBuffer.h"
#include "MIDIBufferPool.h"

struct MIDIBufferQueue;
typedef struct MIDIBufferQueue MIDIBufferQueue;
typedef MIDIBufferQueue *MIDIBufferQueueRef;

MIDIBufferQueue *MIDIBufferQueueCreate(unsigned int queueSize,
				       MIDIBufferPool *firstPool,
				       MIDIBufferPool *secondPool);
void MIDIBufferQueueResetCounters(MIDIBufferQueue *queue);

MIDIBufferPool *MIDIBufferQueueGetPool(MIDIBufferQueue *queue, unsigned int index);
MIDIBufferPool *MIDIBufferQueueSetPool(MIDIBufferQueue *queue,
				       unsigned int index,
				       MIDIBufferPool *pool);

unsigned long MIDIBufferQueueGetPacketsDropped(MIDIBufferQueue *queue);
unsigned long MIDIBufferQueueGetNotifyErrors(MIDIBufferQueue *queue);
unsigned long MIDIBufferQueueGetMaxPacketSize(MIDIBufferQueue *queue);
unsigned long MIDIBufferQueueGetMaxPacketListLength(MIDIBufferQueue *queue);
unsigned long MIDIBufferQueueGetDynamicAllocationCount(MIDIBufferQueue *queue);

int MIDIBufferQueueSetAllocateFlag(MIDIBufferQueue *queue, int flag);
int MIDIBufferQueueGetAllocateFlag(MIDIBufferQueue *queue);

int MIDIBufferQueueSetActive(MIDIBufferQueue *queue, int state);
int MIDIBufferQueueGetActive(MIDIBufferQueue *queue);

int MIDIBufferQueueRelease(MIDIBufferQueue *queue);

OSStatus MIDIBufferQueueInsertNullBuffer(MIDIBufferQueue *queue);

MIDIBuffer *MIDIBufferNextFromQueue(MIDIBufferQueue *queue, Duration timeout);

void MIDIBufferQueueReadProc(const MIDIPacketList *packetList,
			     void *refCon,
			     void *connRefCon);

#endif

