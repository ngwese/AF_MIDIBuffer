/* -*- Mode: C; -*-
 *
 * ___ MIDIBuffer.c _________________________________________
 * 
 * $URL: https://mm1.local/svn/private/trunk/lib/c/AF_MIDIBuffer/MIDIBuffer.c $
 * $Id: MIDIBuffer.c 211 2009-07-20 06:26:24Z greg $
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

#include <stdio.h>

#include "MIDIBuffer.h"
#include "MIDIPacketUtils.h"

inline unsigned long sizeof_MIDIPacket(unsigned int size)
{
  return sizeof(MIDITimeStamp)
  + sizeof(UInt16)
  + size;
}

inline unsigned long sizeof_MIDIBuffer(unsigned int size)
{
  return sizeof(struct MIDIBuffer *)
  + sizeof(struct MIDIBufferPool *)
  + sizeof(ByteCount)
  + sizeof(void *)
  + sizeof_MIDIPacket(size);
}

MIDIBuffer *MIDIBufferCreate(unsigned int size)
{
  MIDIBuffer *buffer;
  
  /* calculate buffer size then allocate */
  buffer = (MIDIBuffer *)malloc(sizeof_MIDIBuffer(size));
  
  buffer->next = NULL;
  buffer->pool = NULL;
  buffer->srcRefCon = NULL;
  buffer->bufferSize = sizeof_MIDIPacket(size);
  buffer->numPackets = 0;
  
  return buffer;
}

void MIDIBufferRelease(MIDIBuffer *buffer)
{
  free(buffer);
}

MIDIPacket *MIDIBufferPacketPointer(MIDIBuffer *buffer)
{
  return &buffer->packet[0];
}

unsigned long MIDIBufferUnsignedRefCon(MIDIBuffer *buffer)
{
  return (unsigned long)buffer->srcRefCon;
}
 
void MIDIBufferDump(const MIDIBuffer *buffer)
{
  fprintf(stderr, ">> MIDIBufferDump() for 0x%x:\n", (unsigned int)buffer);
  MIDIPacketListDump(&buffer->packet[0], buffer->numPackets);
}


