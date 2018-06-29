/*!
* \brief       读取xml配置文件.
* \author      吴典@众安科技虚拟实验室.
* \date        -
*
* \usage
*
<DataServer>
	<broker_id>66666</broker_id>
	<ctp_address>
		<Address IP="tcp://180.169.101.177:41213" />
		<Address IP="tcp://123.124.247.8:41213" />
	</ctp_address>
</DataServer>

XmlConfig config("文件全路径");
if (!config.Load()) { "error"; }

XmlNode node = config.FindNode("DataServer");
string broker_id = node.GetValue("broker_id");

XmlNodeVec Addresses = config.FindChileren("DataServer/ctp_address", "Address");
for (int i = 0; i < Addresses.size(); ++i) {
	string IP = Addresses[i].GetValue("IP");
}
*
*/

#ifndef COMMON_XML_CONFIG_H_  
#define COMMON_XML_CONFIG_H_  

#include <string>
#include <vector>
#include "common/Global.h"

class QDomElement;
class QDomDocument;

namespace zhongan {
namespace common {

class XmlNode;
typedef std::vector<XmlNode> XmlNodeVec;

class COMMON_API XmlNode
{
public:
	XmlNode(const QDomElement& elem, QDomDocument* dom_document);
	XmlNode(const XmlNode& other);
	XmlNode& operator=(const XmlNode& other);
	~XmlNode();

	///如果拷贝到外部使用，需要深复制.
	void DeapCopy();
	bool IsNull();

	///寻找子节点.
	///path为空返回本身.
	XmlNode FindNode(const std::string& path);

	///name为空，返回所有子节点；否则返回tag为name的所有子节点.
	///path为空，返回本节点下相应的子节点.
	XmlNodeVec FindChileren(const std::string& path = "", const std::string& name = "");
	void ClearChildren(const std::string& path = "", const std::string& name = "");

	///key为空，读取本身的text.
	///先寻找属性为key的值，如果找不到再去找子节点为key的text.
	std::string GetValue(const std::string& key = "");
	///key为空，设置本身的text.
	///先寻找值为key的属性，如果找不到再去找子节点;如果子节点还找不到，则添加值为key的子节点.
	void SetValue(const std::string& val, const std::string& key = "", bool arrt_prefer = false);

	std::string GetName();

	XmlNode AppendChild(const std::string& path, const std::string& name);
	XmlNode InsertChild(const std::string& path, const std::string& name, int idx);
	void RemoveChild(const std::string& path, const std::string& name, int idx);
	void RemoveNode(const std::string& path);

private:
	QDomElement* elem_;
	QDomDocument* dom_document_;
};

class COMMON_API XmlConfig
{
public:
	XmlConfig(const std::string& file_path);
	~XmlConfig();

	///加载文件的根节点下面的内容；root_name不为空会对根节点的名字进行校验.
	bool Load(const std::string& root_name = "");
	///SetValue后必须Flush才能写到文件中去.
	void Flush();
	std::string GetLastError() { return last_err_; }

	///path为相对于根节点的路径，不用加根节点名字.
	XmlNode FindNode(const std::string& path);
	XmlNodeVec FindChileren(const std::string& path = "", const std::string& name = "");
	void ClearChildren(const std::string& path = "", const std::string& name = "");

	std::string GetValue(const std::string& path, const std::string& key = "");
	void SetValue(const std::string& path, const std::string& val, const std::string& key = "", bool arrt_prefer = false);

	XmlNode AppendChild(const std::string& path, const std::string& name);
	XmlNode InsertChild(const std::string& path, const std::string& name, int idx);
	void RemoveChild(const std::string& path, const std::string& name, int idx);
	void RemoveNode(const std::string& path);

private:
	std::string file_path_;
	std::string last_err_;
	XmlNode* root_node_;
	QDomDocument* dom_document_;
};

}
}
#endif

