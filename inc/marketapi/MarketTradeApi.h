#ifndef EYEGLE_MARKETAPI_MARKETTRADEAPI_H_
#define EYEGLE_MARKETAPI_MARKETTRADEAPI_H_

#include <string>
#include "marketapi/MarketDefine.h"
#include "common/StatusDefine.h"
#include "common/Condition.h"

class QSettings;

namespace itstation {
namespace marketapi {

using namespace common;

class SecurityInfoSpi
{
public:
	/*
	 * @brief OnInstrumentInfo 查询合约回报
	 */
	virtual void OnInstrumentInfo(const InstrumentInfoData& info, bool is_last) {}
	virtual void OnMarginInfo(const MarginInfo& info, bool is_last) {}
	virtual void OnCommisionInfo(const CommisionInfo& info, bool is_last) {}
};

class TradeSpi
{
public:
	/*
	 * @brief OnError 服务器端返回的失败消息
	 * @param error_msg 详细错误信息
	 * @param request_id 对应的请求ID
	 */
	virtual void OnError(int request_id, const std::string& error_msg) {}

	/*
	 * @brief OnReconnected 与服务器断开连接
	 */
	virtual void OnConnect() {}

	/*
	 * @brief OnDisconnect 与服务器断开连接
	 */
	virtual void OnDisconnect(const std::string& reson) {}

	/*
	 * @brief OnOrder 委托订单回报、撤单回报
	 */
	virtual void OnOrder(OrderData* order_data) = 0;

	/*
	 * @brief OnOrderError 委托订单回报错误、撤单错误
	 */
	virtual void OnOrderError(OrderData* order_data) = 0;	

	/*
	 * @brief OnTrade 成交回报
	 */
	virtual void OnTrade(TradeData* trade_data) = 0;

	/*
	 * @brief OnOrder 查询委托回报
	 */
	virtual void OnQryOrder(int req_id, OrderData* order_data, const std::string& err, bool is_last) = 0;

	/*
	 * @brief OnTrade 成交回报
	 */
	virtual void OnQryTrade(int req_id, TradeData* trade_data, const std::string& err, bool is_last) = 0;

	/*
	 * @brief OnAccount 查询资金回报
	 */
	virtual void OnQryAccount(int req_id, AccountData* trade_data, const std::string& err, bool is_last) = 0;

	/*
	 * @brief OnPosition 查询持仓回报
	 */
	virtual void OnQryPosition(int req_id, PositionData* trade_data, const std::string& err, bool is_last) = 0;
};

class MARKET_TRADE_API TradeApi
{
public:
	TradeApi();
	virtual ~TradeApi();

	/*
	 * @brief Init 连接交易服务器 
	 * @param front_addr_str ip:port
	 */
	virtual bool Init(const std::string& front_addr_str, TradeSpi* spi, std::string& err) = 0;
	virtual void Deinit()=0;
	/*
	 * @brief Login 登陆交易服务器 
	 */
	virtual bool Login(const std::string& , const std::string& user_id, const std::string& password, std::string& err) = 0;
	virtual bool Logout(std::string& err) = 0;
	virtual bool InitPreTrade(std::string& err) { return true; }

	virtual int QueryAccount(std::string& err) = 0;
	virtual int QueryPosition(std::string& err) = 0;
	virtual int QueryOrder(std::string& err) = 0;
	virtual int QueryTrade(std::string& err) = 0;
	virtual int QueryCancelAbledOrder(std::string& err) { return 0; };

	virtual void SetSecurityInfoSpi(SecurityInfoSpi* sec_info_spi) { sec_info_spi_ = sec_info_spi; }
	virtual bool QryInstrument(std::string& err) { return true; }
	virtual bool QryMargin(const std::string& symbol, std::string& err) { return true; } // 保证金
	virtual bool QryCommision(const std::string& symbol, std::string& err) { return true; } // 手续费
	
	///返回本地委托编号
	virtual int SubmitOrder(const OrderParamData& param, std::string& err) = 0;
	// 为国君证券 
	// 用[410411]实现 委托买卖业务: 
	// 可做深圳的转股回售，但是BSFLAG必须送G或H，前台需要单独菜单】【也可以做交易所基金的申购和赎回，买卖类别送3或4
	// 可做深圳的权证行权，但是BSFLAG必须送7，前台需要单独菜单
	// 注意：只能用同一个委托批号ordergroup对同一市场（上证或深证）的股票进行同时买或同时卖
	virtual int SubmitOrder(const OrderParamData *param, int num, std::string& err) { return 0; }; 
	virtual int CancelOrder(const OrderData& param, std::string& err) = 0;

protected:
	//用于异步请求的等待
	bool TimeWait(int sec);
	void ReleaseWait();

protected:
	TradeSpi* spi_;
	SecurityInfoSpi* sec_info_spi_;

	bool m_succed_connect;	/**< 连接成功 */
	bool m_succed_login;	/**< 登陆成功 */

	std::string m_user_id;
	std::string m_password;

	std::string error_msg_;

	//QSettings *config_settings_;

private:
	common::Mutex wait_mutex_;
	common::Condition wait_cond_;
};

}
}
#endif	//EYEGLE_MARKETAPI_MARKETTRADEAPI_H_
