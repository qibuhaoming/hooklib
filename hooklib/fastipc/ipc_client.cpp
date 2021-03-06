/*
 *  [ BSD License: http://opensource.org/licenses/bsd-license.php ]
 *  ===========================================================================
 *  Copyright (c) 2015, Lakutin Ivan
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ipc_client.h"

BOOL
IpcClientCallIpcServer(LPCWSTR ChannelName,
                       LPVOID MessageBuffer,
                       DWORD MessageSize,
                       LPVOID AnswerBuffer,
                       DWORD AnswerSize,
                       DWORD Timeout)
{
    NAMED_PIPE Pipe;
    IPC_CHANNEL_HEADER Header;
    BOOL Accepted;

    TRY(BOOL, TRUE)
    {
        if (!IpcOpenNamedPipe(ChannelName,
                              TRUE,
                              &Pipe))
        {
            LEAVE(FALSE);
        }

        Header.HasMessage = MessageSize && MessageBuffer;
        Header.HasAnswer = AnswerSize && AnswerBuffer;
        Header.MessageSize = MessageSize;
        Header.AnswerSize = AnswerSize;

        if (!IpcWriteToNamedPipe(&Pipe,
                                 &Header,
                                 sizeof(IPC_CHANNEL_HEADER),
                                 FALSE,
                                 NULL,
                                 NULL))
        {
            LEAVE(FALSE);
        }

        if (!IpcReadFromNamedPipe(&Pipe,
                                  &Accepted,
                                  sizeof(BOOL),
                                  FALSE,
                                  NULL,
                                  NULL))
        {
            LEAVE(FALSE);
        }

        if (!IpcWriteToNamedPipe(&Pipe,
                                 MessageBuffer,
                                 MessageSize,
                                 FALSE,
                                 NULL,
                                 NULL))
        {
            LEAVE(FALSE);
        }

        if (Header.HasAnswer && !IpcReadFromNamedPipe(&Pipe,
                                                      AnswerBuffer,
                                                      AnswerSize,
                                                      FALSE,
                                                      NULL,
                                                      NULL))
        {
            LEAVE(FALSE);
        }
    }
    FINALLY
    {
        IpcCloseNamedPipe(&Pipe);
    }

    return TRY_VALUE;
}

