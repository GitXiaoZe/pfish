//This file define various kinds of RPC message type
#ifndef RPC_MSG_H
#define RPC_MSG_H

/*
REGISTER_WORKER's format :
| type | cores | sockets | memory |
the *type* field's length is 1 byte, which indicate the type of the msg
the *cores* field's length is 4 byte, which indicate the # of cores of the worker
the *sockets* field's length is 4 byte, which indicate the # of sockets of the worker, 
        this field is only useful when the worker is NUMA architecture. 
        0 shows that the worker is not NUMA architecture
the *memory* field's length is 8 byte, which indicate the # of memory(in bytes) of the worker
*/
#define REGISTER_WORKER 1

#define WORKER_HEARTBEAT 2

#endif // !RPC_MSG_H

