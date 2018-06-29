#ifndef _COMMON_STATUS_H_
#define _COMMON_STATUS_H_

#include <string>
#include "common/Global.h"

#ifndef WIN32
#ifndef sprintf_s
#define sprintf_s snprintf
#endif

#ifndef strcpy_s(des, len, sou)
#define strcpy_s(des, len, sou) strncpy(des, sou, len)
#endif
#endif

namespace zhongan {
namespace common {

/*
 * @brief Status 用于整个工程的错误管理方案；0x0000表示成功，其它的都为错误代码；
 * 4位16进制的数字中，前两位表示模块号，后两位表示该模块的具体错误代码；例如0x03A5在模块
 * 03中的错误代码A5；
 */
typedef enum {
	STATUS_OK										= 0x0000,	/**< 成功 */

	//01表示XmlConfig模块
	STATUS_XMLCONFIG_FILE_NOT_FOUND					= 0x0101,	/**< 配置文件找不到 */
	STATUS_XMLCONFIG_XML_PARSER_ERROR				= 0x0102,	/**< 解析xml失败 */
	STATUS_XMLCONFIG_XML_NOT_LOADED					= 0x0103,	/**< 未载入配置文件 */
	STATUS_XMLCONFIG_XML_KEY_ERROR					= 0x0104,	/**< 键不存在或者值类型不匹配 */
	STATUS_XMLCONFIG_XML_BAD_DATA					= 0x0105,	/**< 写配置文件时值不匹配 */
	STATUS_XMLCONFIG_XML_WRITE_ERROR				= 0x0106,	/**< 文件路径不合法 */

	//02表示AsyncLog模块
	STATUS_ASYNCLOG_IS_RUNNING						= 0x0201,	/**< 日志线程已启动，不允许设置日志属性 */
	STATUS_ASYNCLOG_PATH_ERROR						= 0x0202,	/**< 路径不存在或无法访问 */

	//03表示ProcessCommunication模块
	STATUS_PROCESS_COMM_NAME_EMPTY					= 0x0301,	/**< 名字为空无效 */
	STATUS_PROCESS_COMM_INIT_ERROR					= 0x0302,	/**< 初始化失败 */
	STATUS_PROCESS_COMM_LIST_FULL					= 0x0303,	/**< 缓存列表已满，写入无效 */
	STATUS_PROCESS_COMM_IS_RUNNING					= 0x0304,	/**< 接收数据模块已经在运行中 */
	STATUS_PROCESS_HANDLER_IS_NULL					= 0x0305,	/**< 回调函数为空 */
	STATUS_PROCESS_SIZE_TOO_SHORT					= 0x0306,	/**< 对于非阻塞发送的缓充区的容量必须大于2 */

	//04表示MarketData模块
	STATUS_MARKET_DATA_TIME_OUT						= 0x0401,	/**< 请求超时 */
	STATUS_MARKET_DATA_INVALID_ARG					= 0x0402,	/**< 无效参数 */
	STATUS_MARKET_DATA_NOT_CONNECT					= 0x0403,	/**< 没有连接 */
	STATUS_MARKET_DATA_LOGIN_REQUEST_ERROR			= 0x0404,	/**< 请求登录失败 */
	STATUS_MARKET_DATA_LOGIN_RESPONSE_ERROR			= 0x0405,	/**< 登录返回失败 */
	STATUS_MARKET_DATA_SUBSCRIBE_ERROR				= 0x0406,	/**< 订阅行情失败 */
	STATUS_MARKET_DATA_NOT_SUBSCRIBED				= 0x0407,	/**< 该合约未被订阅 */
	STATUS_MARKET_DATA_UNSUBSCRIBE_ERROR			= 0x0408,	/**< 取消订阅行情失败 */

	//05表示MarketTrade模块
	STATUS_MARKET_TRADE_TIME_OUT					= 0x0501,	/**< 请求超时 */
	STATUS_MARKET_TRADE_INVALID_ARG					= 0x0502,	/**< 无效参数 */
	STATUS_MARKET_TRADE_NOT_CONNECT					= 0x0503,	/**< 没有连接 */
	STATUS_MARKET_TRADE_LOGIN_REQUEST_ERROR			= 0x0504,	/**< 请求登录失败 */
	STATUS_MARKET_TRADE_LOGIN_RESPONSE_ERROR		= 0x0505,	/**< 登录返回失败 */
	STATUS_MARKET_TRADE_CONFIRM_ERROR				= 0x0506,	/**< 资金确认失败 */
	STATUS_MARKET_TRADE_LOGOUT_ERROR				= 0x0507,	/**< 登出失败 */
	STATUS_MARKET_TRADE_NOT_LOGIN					= 0x0508,	/**< 未登录 */

	//06表示TradingCenter模块
	STATUS_TRADING_CENTER_TYPE_ERROR				= 0x0601,	/**< 交易API类型错误 */
	STATUS_TRADING_CENTER_CALLBACK_NULL				= 0x0602,	/**< 回调函数为空 */
	STATUS_TRADING_CENTER_CHANNEL_NOT_FOUND			= 0x0603,	/**< 未找到交易通道 */
	STATUS_TRADING_CENTER_CHANNEL_EXISTED			= 0x0604,	/**< 交易通道已存在 */

	//07表示PositionManager模块
	STATUS_POSITION_MANAGER_CHILD_EXIST				= 0x0701,	/**< 子节点已存在 */
	STATUS_POSITION_MANAGER_CHILD_NOT_EXIST			= 0x0702,	/**< 子节点不存在 */
	STATUS_POSITION_MANAGER_POSITION_EXIST			= 0x0703,	/**< 添加持仓信息失败，已存在该订单号的持仓记录 */
	STATUS_POSITION_MANAGER_POSITION_NOT_EXIST		= 0x0704,	/**< 添加持仓信息失败，已存在该订单号的持仓记录 */
	STATUS_POSITION_MANAGER_POSITION_ACCOUNT_ERROR	= 0x0705,	/**< 持仓信息合计有误 */
}EyegleStatus;

/*
 * @brief ToErrorMsg 通用的错误代码信息接口
 */
COMMON_API const char*  ToErrorMsg(const EyegleStatus status);

}
}

#endif	//_COMMON_STATUS_H_

