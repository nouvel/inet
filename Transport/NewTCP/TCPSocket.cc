//
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include <omnetpp.h>
#include "TCPSocket.h"
#include "TCPCommand_m.h"



int TCPSocket::nextConnId = 0;


TCPSocket::TCPSocket()
{
    // don't allow user-specified connIds because they may conflict with
    // automatically assigned ones.
    connId = ++nextConnId;
    isBound = false;

    cb = NULL;
    yourPtr = NULL;

}

void TCPSocket::sendToTCP(cMessage *msg)
{
    check_and_cast<cSimpleModule *>(simulation.contextModule())->send(msg,"tcpOut");
}

void TCPSocket::bind(int lPort)
{
    localPort = lPort;
    isBound = true;
}

void TCPSocket::bind(IPAddress lAddr, int lPort)
{
    localAddr = lAddr;
    localPort = lPort;
    isBound = true;
}

void TCPSocket::accept()
{
    if (!isBound)
        opp_error("TCPSocket: must call bind() before accept()");

    cMessage *msg = new cMessage("PassiveOPEN", TCP_C_OPEN_PASSIVE);

    TCPOpenCommand *openCmd = new TCPOpenCommand();
    openCmd->setLocalAddr(localAddr);
    openCmd->setLocalPort(localPort);
    openCmd->setConnId(connId);

    msg->setControlInfo(openCmd);
    sendToTCP(msg);
}

void TCPSocket::connect(IPAddress remoteAddr, int remotePort)
{
    if (!isBound)
        opp_error("TCPSocket: must call bind() before connect()");

    cMessage *msg = new cMessage("ActiveOPEN", TCP_C_OPEN_ACTIVE);

    TCPOpenCommand *openCmd = new TCPOpenCommand();
    openCmd->setConnId(connId);
    openCmd->setLocalAddr(localAddr);
    openCmd->setLocalPort(localPort);
    openCmd->setRemoteAddr(remoteAddr);
    openCmd->setRemotePort(remotePort);

    msg->setControlInfo(openCmd);
    sendToTCP(msg);
}

void TCPSocket::send(cMessage *msg)
{
    msg->setKind(TCP_C_SEND);
    TCPSendCommand *cmd = new TCPSendCommand();
    cmd->setConnId(connId);
    msg->setControlInfo(cmd);
    sendToTCP(msg);
}

void TCPSocket::close()
{
    cMessage *msg = new cMessage("CLOSE", TCP_C_CLOSE);
    TCPCommand *cmd = new TCPCommand();
    cmd->setConnId(connId);
    msg->setControlInfo(cmd);
    sendToTCP(msg);
}

void TCPSocket::abort()
{
    cMessage *msg = new cMessage("ABORT", TCP_C_ABORT);
    TCPCommand *cmd = new TCPCommand();
    cmd->setConnId(connId);
    msg->setControlInfo(cmd);
    sendToTCP(msg);
}

void TCPSocket::requestStatus()
{
    cMessage *msg = new cMessage("STATUS", TCP_C_STATUS);
    TCPCommand *cmd = new TCPCommand();
    cmd->setConnId(connId);
    msg->setControlInfo(cmd);
    sendToTCP(msg);
}

bool TCPSocket::belongsToSocket(cMessage *msg)
{
    return dynamic_cast<TCPCommand *>(msg->controlInfo()) &&
           ((TCPCommand *)(msg->controlInfo()))->connId()==connId;
}

bool TCPSocket::belongsToAnyTCPSocket(cMessage *msg)
{
    return dynamic_cast<TCPCommand *>(msg->controlInfo());
}

void TCPSocket::setCallbackObject(CallBackInterface *callback, void *yourPointer)
{
    cb = callback;
    yourPtr = yourPointer;
}

void TCPSocket::processMessage(cMessage *msg)
{
    if (!cb)
        opp_error("TCPSocket: callback object must be set via setCallbackObject() "
                  "before processMessage() can be invoked");
    if (!belongsToSocket(msg))
        return;

    switch (msg->kind())
    {
        case TCP_I_DATA: break; // FIXME complete this...
        case TCP_I_URGENT_DATA: break;
        case TCP_I_ESTABLISHED: break;
        case TCP_I_CLOSING: break;
        case TCP_I_CLOSED: break;
        case TCP_I_CONNECTION_RESET: break;
        case TCP_I_STATUS: break;
    }
}
