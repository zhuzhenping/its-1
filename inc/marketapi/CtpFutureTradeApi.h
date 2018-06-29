#ifndef EYEGLE_MARKETAPI_CTPMARKETTRADEAPI_H_
#define EYEGLE_MARKETAPI_CTPMARKETTRADEAPI_H_

#include <queue>
#include "common/QueueBuffer.h"
#include "common/Condition.h"
#include "marketapi/MarketTradeApi.h"
#include "CTP/ThostFtdcTraderApi.h"


namespace itstation {
namespace marketapi {

class CtpFutureTradeApi;

class CtpFutureTradeHandler : public CThostFtdcTraderSpi {
	friend CtpFutureTradeApi;
public:
	CtpFutureTradeHandler(CtpFutureTradeApi* market_trade_api)
			:CThostFtdcTraderSpi()
			,m_market_trade_api(market_trade_api)
			,m_reconnected(false)
			, pre_info_(NULL)
	{

	}

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
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* rsp_user_login, 
			CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last);

	/*
	 * @brief OnRspUserLogin 登出请求响应
	 */
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField* rsp_user_logout, 
			CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last);

	/*
	 * @brief OnRspOrderInsert ctp柜台认为报单请求错误 只有本连接才收到
	 */
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField* input_order, 
			CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last);

	/*
	 * @brief OnErrRtnOrderInsert 交易所认为报单错误 广播数据，所有连接都能收到
	 */
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField* input_order, CThostFtdcRspInfoField* response_infomation);

	///柜台认为撤单出错：比如可撤单不够
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///交易所认为撤单错误
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

	/*
	 * @brief OnRtnOrder 报单状态返回。报单、撤单成功
	 */
	virtual void OnRtnOrder(CThostFtdcOrderField* market_order);

	/*
	 * @brief OnRtnTrade 成交回报
	 */
	virtual void OnRtnTrade(CThostFtdcTradeField* market_trade);

	/*
	 * @brief OnRspQryTradingAccount 查询资金回报
	 */
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField* account_field,
			CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last);

	/*
	 * @brief OnRspQryInvestorPositionDetail 查询持仓回报
	 */
	virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField* account_field,
		CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last);

	/*
	 * @brief OnRspQrySettlementInfoConfirm 查询是否已经确认回报
	 */
	virtual void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* confirm_field,
		CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last);

	/*
	 * @brief OnRspQrySettlementInfo 查询资金确认信息回报
	 */
	virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField* pettlement_info,
		CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last);

	/*
	 * @brief OnRspSettlementInfoConfirm 资金确认回报
	 */
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* confirm_field,
		CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last);

	///请求查询报单响应
	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询成交响应
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


	//////////////////////////////////////////////////////////////////////////
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *rsp_inst, 
		CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last);

	///请求查询合约保证金率响应
	virtual void OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, 
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询合约手续费率响应
	virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

private:
	/*
	 * @brief IsErrorInfo 判断是否是错误信息
	 * @return 如果是错误信息返回true
	 */
	bool IsErrorInfo(CThostFtdcRspInfoField *response_infomation);

	///*
	// * @brief ToDateTime 将字符串转化为boost的时间
	// * @param date 格式： 20131128
	// * @param time 格式： 08:32:15
	// */
	//boost::posix_time::ptime ToDateTime(const std::string& date, const std::string& time);

	/*
	 * @brief ToOrderStatus 将字符转化订单状态
	 */
	OrderStatus ToOrderStatus(char status_char);	

	ExchangeIdType GetExchangeId(const char* id);

private:
	CtpFutureTradeApi* m_market_trade_api;
	bool m_reconnected;				/**< 是否断线重连 */
	//PositionMap m_position_map;		/**< 储存持仓列表 */
	InstrumentInfoData* pre_info_;
	std::map<std::string, std::string> tradeid2stratid_;
};

class CtpFutureTradeApi : public TradeApi {
private:
	enum {
		kInitWaitTime	     = 3,		/**< Init等待时间 */
		kLoginWaitTime	     = 3,		/**< Login等待时间 */
		kLogoutWaitTime      = 3,		/**< Logout等待时间 */
		kQueryConfirm	     = 3,		/**< ReqQrySettlementInfoConfirm等待时间 */
		kQuerySettlementInfo = 3,		/**< ReqQrySettlementInfo等待时间 */
		kConfirmSettlement   = 3		/**< ReqSettlementInfoConfirm等待时间 */
	};

	const long long kMaxOrderNum;

	enum RequestType {
		E_QryTradingAccountField,
		E_QryInvestorPositionDetailField,
		E_QryOrder,
		E_QryTrade,
		E_QryInstrumentField,
		E_QryInstrumentMarginRateField,
		E_QryInstrumentCommissionRateField		
	};

public:
	friend class CtpFutureTradeHandler;
	friend class CtpRequestBuffer;

	CtpFutureTradeApi(void);
	virtual ~CtpFutureTradeApi(void);

	virtual bool Init(const std::string& front_addr_str, TradeSpi* spi, std::string& err);
	virtual void Deinit();
	virtual bool Login(const std::string& broker_id, const std::string& user_id, const std::string& password, std::string& err);
	virtual bool Logout(std::string& err);
	virtual bool InitPreTrade(std::string& err);

	///返回OrderRef
	virtual int SubmitOrder(const OrderParamData& param, std::string& err);
	virtual int CancelOrder(const OrderData& param, std::string& err);

	virtual int QueryAccount(std::string& err);
	virtual int QueryPosition(std::string& err);
	virtual int QueryOrder(std::string& err);
	virtual int QueryTrade(std::string& err);
	virtual int QueryCancelAbledOrder(std::string& err) { return 0; }	

	virtual bool QryInstrument(std::string& err);
	virtual bool QryMargin(const std::string& symbol, std::string& err);
	virtual bool QryCommision(const std::string& symbol, std::string& err);

private:
	bool ReLogin(std::string& err);
	bool ReqQrySettlementInfoConfirm(std::string& err);
	bool ReqQrySettlementInfo(std::string& err);
	bool ReqSettlementInfoConfirm(std::string& err);

	static std::string ToExchangeStr(ExchangeIdType id);

private:
	CThostFtdcTraderApi* m_ctp_market_api;	/**< CTP交易接口 */
	CtpFutureTradeHandler* m_ctp_market_handler;	/**< CTP行情回调 */

	
	bool m_succed_query_confirm_info;		/**< 查询当日是否已经资金确认成功 */
	bool m_succed_query_settlement_info;	/**< 查询资金确认信息成功 */
	bool m_has_confirmed;		/**< 当日是否已经资金确认 */
	int m_request_id;		/**< 请求ID号 */
	int m_order_ref;		/**< 订单编号 */
	common::SpinLock m_order_ref_lock;
	int m_front_id;			/**< 前置地址编号 */
	int m_session_id;		/**< 会话编号 */

	//保存资金账号信息用于断线重新登录
	std::string m_broker_id;

	bool been_logout_;

	// 为解决流控
	class CtpRequestBuffer : public common::QueueBuffer<RequestType, 50> {
		friend class CtpFutureTradeApi;
	public:
		CtpRequestBuffer(CtpFutureTradeApi* market_trade_api) : m_market_trade_api(market_trade_api) {}
	private:
		virtual bool Comsume(const RequestType& val);
		bool QueryAccount();
		bool QueryPosition();
		bool QueryOrder();
		bool QueryTrade();
		bool QryInstrument();
		bool QryMargin();
		bool QryCommision();
	private:
		CtpFutureTradeApi* m_market_trade_api;
		int	m_nSleep;

		std::queue<std::string> margin_symbols_;
		common::SpinLock margin_mutex_;

		std::queue<std::string> commision_symbols_;
		common::SpinLock commision_mutex_;
	};
	CtpRequestBuffer ctp_req_buf_;
};

}
}
#endif	//EYEGLE_MARKETAPI_CTPMARKETTRADEAPI_H_

