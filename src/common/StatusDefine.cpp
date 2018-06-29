#include <string.h>
#include <stdio.h>
#include "common/StatusDefine.h"


namespace zhongan {
namespace common {

char error_str[128];

char* StatusToCodeString(const EyegleStatus status) {
	memset(error_str, 0, 16);
	sprintf_s(error_str, 16, "0x%04X: ", status);
	return error_str;
}

const char* ToXmlConfigErrorMsg(const EyegleStatus status) {
	switch (status) {
	case STATUS_XMLCONFIG_FILE_NOT_FOUND:
		return "The config file is not exit";
	case STATUS_XMLCONFIG_XML_PARSER_ERROR:
		return "Fail to parse the xml file";
	case STATUS_XMLCONFIG_XML_NOT_LOADED:
		return "Has not loaded config file";
	case STATUS_XMLCONFIG_XML_KEY_ERROR:
		return "Key not exist or type of value is not correct";
	case STATUS_XMLCONFIG_XML_BAD_DATA:
		return "The type of value is not correct for writing config";
	case STATUS_XMLCONFIG_XML_WRITE_ERROR:
		return "The config's file path is ilegal";
	default:
		return "Undefined error";
	}
}

const char* ToAsyncLogErrorMsg(const EyegleStatus status) {
	switch (status) {
	case STATUS_ASYNCLOG_IS_RUNNING:
		return "The log writing thread is running, not allowed to set the log property";
	case STATUS_ASYNCLOG_PATH_ERROR:
		return "The log's path is not exist or can't access to";
	default:
		return "Undefined error";
	}
}

const char* ToProcessCommErrorMsg(const EyegleStatus status) {
	switch (status) {
	case STATUS_PROCESS_COMM_NAME_EMPTY:
		return "Invalid name, the name is not allowed to be empty";
	case STATUS_PROCESS_COMM_INIT_ERROR:
		return "Initiate error";
	case STATUS_PROCESS_COMM_LIST_FULL:
		return "The data list is full, can't put data any more";
	case STATUS_PROCESS_COMM_IS_RUNNING:
		return "The ProcessMsgReceiver has been started";
	case STATUS_PROCESS_HANDLER_IS_NULL:
		return "The handler is not allowed to be null";
	case STATUS_PROCESS_SIZE_TOO_SHORT:
		return "The SIZE must be greater than or equal to 2 for blocked ProcessMsgSender";
	default:
		return "Undefined error";
	}
}

const char* ToMarketDataErrorMsg(const EyegleStatus status) {
	switch (status) {
	case STATUS_MARKET_DATA_TIME_OUT:
		return "Request time out for reponse";
	case STATUS_MARKET_DATA_INVALID_ARG:
		return "Invalid argument";
	case STATUS_MARKET_DATA_NOT_CONNECT:
		return "Unconnected with ctp front address";
	case STATUS_MARKET_DATA_LOGIN_REQUEST_ERROR:
		return "Error on request login";
	case STATUS_MARKET_DATA_LOGIN_RESPONSE_ERROR:
		return "Error on response for login";
	case STATUS_MARKET_DATA_SUBSCRIBE_ERROR:
		return "Error on subscribe market price";
	case STATUS_MARKET_DATA_NOT_SUBSCRIBED:
		return "The symbol has not been subscribed";
	case STATUS_MARKET_DATA_UNSUBSCRIBE_ERROR:
		return "Error on unsubscribe market price";
	default:
		return "Undefined error";
	}
}

const char* ToMarketTradeErrorMsg(const EyegleStatus status) {
	switch (status) {
	case STATUS_MARKET_TRADE_TIME_OUT:
		return "Request time out for reponse";
	case STATUS_MARKET_TRADE_INVALID_ARG:
		return "Invalid argument";
	case STATUS_MARKET_TRADE_NOT_CONNECT:
		return "Unconnected with ctp front address";
	case STATUS_MARKET_TRADE_LOGIN_REQUEST_ERROR:
		return "Error on request login";
	case STATUS_MARKET_TRADE_LOGIN_RESPONSE_ERROR:
		return "Error on response for login";
	case STATUS_MARKET_TRADE_CONFIRM_ERROR:
		return "Error on confirm settlement information";
	case STATUS_MARKET_TRADE_LOGOUT_ERROR:
		return "Error on logout";
	case STATUS_MARKET_TRADE_NOT_LOGIN:
		return "Has not logined";
	default:
		return "Undefined error";
	}
}

const char* ToTradingCenterErrorMsg(const EyegleStatus status) {
	switch (status) {
	case STATUS_TRADING_CENTER_TYPE_ERROR:
		return "Api type is not defined";
	case STATUS_TRADING_CENTER_CALLBACK_NULL:
		return "Callback is not allowed to be null";
	case STATUS_TRADING_CENTER_CHANNEL_NOT_FOUND:
		return "Trading channel is not found";
	case STATUS_TRADING_CENTER_CHANNEL_EXISTED:
		return "Trading channel is already existed";
	default:
		return "Undefined error";
	}
}

const char* ToPositionManagerErrorMsg(const EyegleStatus status) {
	switch (status) {
	case STATUS_POSITION_MANAGER_CHILD_EXIST:
		return "The child has existed";
	case STATUS_POSITION_MANAGER_CHILD_NOT_EXIST:
		return "The child is not existed";
	case STATUS_POSITION_MANAGER_POSITION_EXIST:
		return "Add position error: the order_id is already existed";
	case STATUS_POSITION_MANAGER_POSITION_NOT_EXIST:
		return "Remove position error: the order_id is not existed";
	case STATUS_POSITION_MANAGER_POSITION_ACCOUNT_ERROR:
		return "Error on accounting for position";
	default:
		return "Undefined error";
	}
}

const char* ToErrorMsg(const EyegleStatus status) {
	if (STATUS_OK == status) { return "Success"; }

	unsigned short module_code = status & 0xFF00;
	switch (module_code) {
	case 0x0100:
		return strcat(StatusToCodeString(status), ToXmlConfigErrorMsg(status));
		break;
	case 0x0200:
		return strcat(StatusToCodeString(status), ToAsyncLogErrorMsg(status));
		break;
	case 0x0300:
		return strcat(StatusToCodeString(status), ToProcessCommErrorMsg(status));
		break;
	case 0x0400:
		return strcat(StatusToCodeString(status), ToMarketDataErrorMsg(status));
		break;
	case 0x0500:
		return strcat(StatusToCodeString(status), ToMarketTradeErrorMsg(status));
		break;
	case 0x0600:
		return strcat(StatusToCodeString(status), ToTradingCenterErrorMsg(status));
		break;
	case 0x0700:
		return strcat(StatusToCodeString(status), ToPositionManagerErrorMsg(status));
		break;
	default:
		return strcat(StatusToCodeString(status), "Undefined module");
	}
}

}
}
