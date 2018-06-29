#ifndef EYEGLE_MARKETAPI_CTPMARKETDATAAPI_H_
#define EYEGLE_MARKETAPI_CTPMARKETDATAAPI_H_

#include <list>
#include <map>
#include "marketapi/MarketDataApi.h"
#include "ThostFtdcMdApi.h"
#include <vector>
#include "common/DateTime.h"

class QStringList;

namespace itstation {
namespace marketapi {

class CtpFutureMarketDataApi;

/*
 * @brief CtpFutureMarketDataHandler CTP市场数据回调接口的实现
 */
class CtpFutureMarketDataHandler : public CThostFtdcMdSpi {
public:
	CtpFutureMarketDataHandler(CtpFutureMarketDataApi* api) : CThostFtdcMdSpi(), api_(api) ,reconn_(false) {}

	/*
	 * @brief OnRspError 错误应答
	 */
	virtual void OnRspError(CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last);

	/*
	 * @brief OnFrontDisconnected 当客户端与交易后台通信连接断开时，该方法被调用。
	 * @param reason 错误原因
	 * 0x1001 网络读失败
	 * 0x1002 网络写失败
	 * 0x2001 接收心跳超时、
	 * 0x2002 发送心跳失败
	 * 0x2003 收到错误报文
	 */
	virtual void OnFrontDisconnected(int reason);

	/*
	 * @brief OnHeartBeatWarning 心跳超时警告。当长时间未收到报文时，该方法被调用。
	 * @param time_lapse 距离上次接收报文的时间
	 */
	virtual void OnHeartBeatWarning(int time_lapse);

	/*
	 * @brief OnFrontConnected 当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	 */
	virtual void OnFrontConnected();

	/*
	 * @brief OnRspUserLogin 登录请求响应
	 */
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *rsp_user_login, 
			CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last);

	/*
	 * @brief OnRspSubMarketData 订阅行情应答
	 */
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *specific_instrument, 
			CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last);

	/*
	 * @brief OnRspUnSubMarketData 取消订阅行情应答
	 */
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *specific_instrument, 
			CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last);

	/*
	 * @brief OnRtnDepthMarketData 市场行情回调
	 */
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *depth_market_data);

private:
	/*
	 * @brief IsErrorInfo 判断是否是错误信息
	 * @return 如果是错误信息返回true
	 */
	bool IsErrorInfo(CThostFtdcRspInfoField *response_infomation);

private:
	CtpFutureMarketDataApi* api_;
	bool reconn_;		/**< 是否短线重连 */
};


/*
 * @brief CtpFutureMarketDataApi 市场数据订阅接口的CTP实现
 */
class CtpFutureMarketDataApi :public MarketDataApi {
private:
	enum {
		kInitWaitTime	= 3,		/**< Init等待时间 */
		kLoginWaitTime	= 3			/**< Login等待时间 */
	};
public:
	friend class CtpFutureMarketDataHandler;

	CtpFutureMarketDataApi();
	virtual ~CtpFutureMarketDataApi();

	virtual bool Init(const std::string& front_addr_str, MarketDataSpi* spi, std::string& err);
	virtual void Deinit();
	bool Login(const std::string& broker_id, const std::string& user_id, const std::string& password, std::string& err);
	bool Logout(std::string& err);
	virtual bool Subscribe(const std::string& symbol, std::string& err);
	virtual bool UnSubscribe(const std::string& symbol, std::string& err);
	virtual void Join();

private:
	void Split(const std::string& s, const std::string delim,std::vector<std::string>* ret);
	void Trim(std::string &s);

	/*
	 * @brief ReLogin 用于断线后重新登录
	 */
	bool ReLogin(std::string& err);

	/*
	 * @brief ReReqMarketPrice 用于断线后重新请求市场行情
	 */
	bool ReReqMarketPrice(std::string& err);

private:
	CThostFtdcMdApi* m_ctp_market_api;	/**< CTP行情接口 */
	CtpFutureMarketDataHandler* m_ctp_market_handler;	/**< CTP行情回调 */

	int m_request_id;		/**< 请求ID号 */

	//保存资金账号信息用于断线重新登录
	std::string m_broker_id;

	common::DateTime low_time_;
	common::DateTime high_time_;
};

}
}

#endif	//EYEGLE_MARKETAPI_CTPMARKETDATAAPI_H_