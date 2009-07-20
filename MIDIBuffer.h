/* -*- Mode: C; -*-
 *
 * ___ MIDIBuffer.h _________________________________________
 * 
 * $URL: https://mm1.local/svn/private/trunk/lib/c/AF_MIDIBuffer/MIDIBuffer.h $
 * $Id: MIDIBuffer.h 211 2009-07-20 06:26:24Z greg $
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

#ifndef __MIDIBuffer_h__
#define __MIDIBuffer_h__

#include <CoreMIDI/CoreMIDI.h>

struct MIDIBufferPool;

struct MIDIBuffer {
  struct MIDIBuffer *next;
  struct MIDIBufferPool *pool;
  ByteCount bufferSize;
  void *srcRefCon;
  UInt16 numPackets;
  MIDIPacket packet[1];
};

typedef struct MIDIBuffer MIDIBuffer;
typedef MIDIBuffer* MIDIBufferRef;

inline unsigned long sizeof_MIDIPacket(unsigned int size);
inline unsigned long sizeof_MIDIBuffer(unsigned int size);

MIDIBuffer *MIDIBufferCreate(unsigned int size);

void MIDIBufferRelease(MIDIBuffer *buffer);

MIDIPacket *MIDIBufferPacketPointer(MIDIBuffer *buffer);

unsigned long MIDIBufferUnsignedRefCon(MIDIBuffer *buffer);
 
void MIDIBufferDump(const MIDIBuffer *buffer);

#endif
