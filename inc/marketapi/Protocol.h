#pragma once

#include "marketapi/MarketDefine.h"
#include "dataserver/DataServerStruct.h"

using namespace itstation;

#define RSP_VALID_ORDERS 'a'
#define RSP_POSITIONS 'b'
#define RSP_ACCOUNT 'c'

#define RSP_TICK 'h'

typedef char SockDataType;

#pragma pack(1)
struct ProtoHead 
{
	SockDataType type;
	ProtoHead (SockDataType t = '0') : type(t) {}
};



struct OrderRsp : public ProtoHead {
	int num;
	OrderData orders[0];
	OrderRsp():ProtoHead(RSP_VALID_ORDERS){}
};
struct PositionRsp : public ProtoHead {
	int num;
	PositionData positions[0]; 
	PositionRsp() : ProtoHead(RSP_POSITIONS), num(0) {}
};
struct AccountRsp : public ProtoHead{
	AccountData account;
	AccountRsp() : ProtoHead(RSP_ACCOUNT){}
};

struct TickRsp : public ProtoHead{
	FutureTick tick;
	TickRsp():ProtoHead(RSP_TICK){}
};

#pragma pack()
