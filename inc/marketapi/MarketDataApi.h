#ifndef EYEGLE_MARKETAPI_MARKETDATAAPI_H_
#define EYEGLE_MARKETAPI_MARKETDATAAPI_H_

#include <string>
#include <list>
#include <set>
#include <vector>
//#include <QStringList>
#include "datalib/DataServerStruct.h"
#include "common/Global.h"
#include "common/Condition.h"

class QStringList;

namespace itstation {
namespace marketapi {

using namespace common;

/*
 * @brief MarketDataCallback MarketDataApi的回调函数接口
 */
class MarketDataSpi
{
public:
	/*
	 * @brief OnMarketPrice 市场行情回调
	 * @param market_price 市场行情结构体,即返回的结果
	 */
	virtual void OnMarketPrice(BaseTick* tick) = 0;

	/*
	 * @brief OnMdError 服务器端返回的失败消息
	 * @param request_name 发生错误的请求的函数名字,common表示未知的函数名字
	 * @param error_msg 详细错误信息
	 * @param request_id 对应的请求ID
	 */
	virtual void OnMdError(const std::string& request_name, const std::string& error_msg, const std::string& request_id) = 0;

	/*
	 * @brief OnMdDisconnect 与服务器断开连接
	 */
	virtual void OnMdDisconnect(const std::string& reson) = 0;

	/*
	 * @brief OnReconnected 与服务器连接
	 */
	virtual void OnMdConnect() = 0;
};

class MARKET_DATA_API MarketDataApi {
public:
	MarketDataApi();
	virtual ~MarketDataApi();

	/*
	 * @brief Init 初始化操作
	 * @return 成功返回STATUS_OK,失败通过get_last_error()获取错误详细信息
	 * @param front_addr_str 行情服务器的ip:port
	 * @param spi 回调接口
	 */
	virtual bool Init(const std::string& front_addr_str, MarketDataSpi* spi, std::string& err) = 0;
	virtual void Deinit() {}

	/*
	 * @brief Login 登陆
	 * @param broker_id 经纪公司ID
	 * @param user_id 用户名
	 * @param password 密码
	 */
	virtual bool Login(const std::string& broker_id, const std::string& user_id, const std::string& password, std::string& err) = 0;

	/// logout后重新登录需 Init -> Login
	virtual bool Logout(std::string& err) = 0;

	virtual bool Subscribe(const std::string& symbol, std::string& err) = 0;
	virtual bool UnSubscribe(const std::string& symbol, std::string& err) = 0;

	virtual bool Subscribe(const std::vector<std::string>& syms, std::string& err) { return false; }
	virtual bool UnSubscribe(const std::vector<std::string>& syms, std::string& er) { return false; }
	virtual bool GetCodeTable(std::set<SymbolEx>& syms, std::string& err) { return false; }

	virtual void Join() {};

protected:
	//用于异步请求的等待
	bool TimeWait(int sec);
	void ReleaseWait();

protected:
	MarketDataSpi* spi_;

	bool m_succed_connect;	/**< 与CTP前置地址连接成功 */
	bool m_succed_login;	/**< 是否登陆成功 */
	//bool m_api_valid;

	std::string m_user_id;
	std::string m_password;

	std::list<std::string> m_symbols;	/**< 保存订阅过的股票，用于断线重连时重新订阅 */

	std::string error_msg_;

	QStringList* holidays_;

private:
	common::Mutex wait_mutex_;
	common::Condition wait_cond_;
};

}
}

#endif	//EYEGLE_MARKETAPI_MARKETDATAAPI_H_
