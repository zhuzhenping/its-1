#ifndef COMMON_XML_CONFIG_H_  
#define COMMON_XML_CONFIG_H_  

#include <string>
#include <vector>
#include "common/Global.h"

class QDomElement;
class QDomDocument;


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


#endif

