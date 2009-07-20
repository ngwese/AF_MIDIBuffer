// -*- Mode: ObjC; -*-
//
// ___ QueueInspectorController.h _________________________________________
// 
// Copyright (c) 2004-2005, Gregory C. Wuller. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or
// without modification, are permitted provided that the following
// conditions are met:
// 
//  - Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  - Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the
//    distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR 'AS IS' AND ANY EXPRESSED
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//

#import <Cocoa/Cocoa.h>

@interface QueueInspectorController : NSObject
{
    IBOutlet NSTextField *p0BufferCount;
    IBOutlet NSTextField *p0BufferRequestCount;
    IBOutlet NSTextField *p0BufferSize;
    IBOutlet NSTextField *p0DynamicAllocations;
    IBOutlet NSTextField *p0LockErrors;
    IBOutlet NSTextField *p0MinBufferCount;
  
    IBOutlet NSTextField *p1BufferCount;
    IBOutlet NSTextField *p1BufferRequestCount;
    IBOutlet NSTextField *p1BufferSize;
    IBOutlet NSTextField *p1DynamicAllocations;
    IBOutlet NSTextField *p1LockErrors;
    IBOutlet NSTextField *p1MinBufferCount;
    
    IBOutlet NSTextField *qDynamicAllocations;
    IBOutlet NSTextField *qMaxPacketListLength;
    IBOutlet NSTextField *qMaxPacketSize;
    IBOutlet NSTextField *qNotifyErrors;
    IBOutlet NSTextField *qPacketListsDropped;
}
- (IBAction)resetCounters:(id)sender;
- (IBAction)togglePool0Allocation:(id)sender;
- (IBAction)togglePool1Allocation:(id)sender;
- (IBAction)toggleQueueActive:(id)sender;
- (IBAction)toggleQueueAllocation:(id)sender;
@end
