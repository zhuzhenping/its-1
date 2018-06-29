#include "common/XmlConfig.h"
#include <QtXml/QDomElement>
#include <QtXml/QDomDocument>
#include <QtCore/QTextCodec>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

using namespace std;

namespace zhongan {
namespace common {

XmlNode::XmlNode(const QDomElement& elem, QDomDocument* dom_document) : dom_document_(dom_document)
{
	elem_ = new QDomElement(elem);
}

XmlNode::XmlNode(const XmlNode& other)
{
	elem_ = new QDomElement(*other.elem_);
	dom_document_ = other.dom_document_;
}

XmlNode& XmlNode::operator=(const XmlNode& other)
{
	if (this != &other)
	{
		delete elem_;
		elem_ = new QDomElement(*other.elem_);
		dom_document_ = other.dom_document_;
	}

	return *this;
}

XmlNode::~XmlNode()
{
	delete elem_;
}

void XmlNode::DeapCopy()
{
	*elem_ = elem_->cloneNode().toElement();
}

bool XmlNode::IsNull()
{
	return elem_->isNull();
}

XmlNode XmlNode::FindNode(const std::string& path)
{
	if (path == "") { return *this; }

	QString q_path = QString::fromLocal8Bit(path.c_str());
	QStringList tag_names = q_path.split('/', QString::SkipEmptyParts);
	if (IsNull() || tag_names.size() == 0)
	{
		return XmlNode(QDomElement(), NULL);
	}

	QDomElement ele = *elem_;
	for (int i=0; i < tag_names.size(); ++i)
	{
		ele = ele.firstChildElement(tag_names[i]);
		if (ele.isNull())
		{
			break;
		}
	}

	return XmlNode(ele, dom_document_);
}

XmlNodeVec XmlNode::FindChileren(const std::string& path, const std::string& name)
{
	XmlNode node = FindNode(path);
	if (node.IsNull())
	{
		return XmlNodeVec();
	}

	XmlNodeVec node_vec;
	QString key_str = QString::fromLocal8Bit(name.c_str());
	for (QDomElement elt = node.elem_->firstChildElement(key_str); !elt.isNull(); elt = elt.nextSiblingElement(key_str))
	{
		node_vec.push_back(XmlNode(elt, dom_document_));
	}
	return node_vec;
}

void XmlNode::ClearChildren(const std::string& path, const std::string& name)
{
	XmlNode node = FindNode(path);
	if (node.IsNull())
	{
		return;
	}
	
	QDomElement next_elt;
	QString key_str = QString::fromLocal8Bit(name.c_str());
	for (QDomElement elt = node.elem_->firstChildElement(key_str); !elt.isNull();)
	{
		next_elt = elt.nextSiblingElement(key_str);
		node.elem_->removeChild(elt);
		elt = next_elt;
	}
}

XmlNode XmlNode::AppendChild(const std::string& path, const std::string& name)
{
	XmlNode node = FindNode(path);
	if (node.IsNull())
	{
		return node;
	}

	QString key_str = QString::fromLocal8Bit(name.c_str());
	QDomElement new_ele = dom_document_->createElement(key_str);
	QDomText text = dom_document_->createTextNode("");
	new_ele.appendChild(text);
	node.elem_->appendChild(new_ele);
	return XmlNode(new_ele, node.dom_document_);
}

XmlNode XmlNode::InsertChild(const std::string& path, const std::string& name, int idx)
{
	XmlNode node = FindNode(path);
	if (node.IsNull())
	{
		return node;
	}

	QDomElement elt;
	QString key_str = QString::fromLocal8Bit(name.c_str());
	for (elt = node.elem_->firstChildElement(key_str); !elt.isNull() && idx > 0; elt = elt.nextSiblingElement(key_str), --idx)
	{
	}
	
	QDomElement new_ele = dom_document_->createElement(key_str);
	QDomText text = dom_document_->createTextNode("");
	new_ele.appendChild(text);
	if (elt.isNull())
	{
		node.elem_->appendChild(new_ele);
	} 
	else
	{
		node.elem_->insertBefore(new_ele, elt);
	}
	return XmlNode(new_ele, node.dom_document_);
}

void XmlNode::RemoveChild(const std::string& path, const std::string& name, int idx)
{
	XmlNode node = FindNode(path);
	if (node.IsNull())
	{
		return;
	}

	QDomElement elt;
	QString key_str = QString::fromLocal8Bit(name.c_str());
	for (elt = node.elem_->firstChildElement(key_str); !elt.isNull() && idx > 0; elt = elt.nextSiblingElement(key_str), --idx)
	{
	}

	if (!elt.isNull())
	{
		node.elem_->removeChild(elt);
	}
}

void XmlNode::RemoveNode(const std::string& path)
{
	XmlNode node = FindNode(path);
	if (!node.IsNull())
	{
		QDomNode parent_ele = node.elem_->parentNode();
		parent_ele.removeChild(*node.elem_);
	}

	if (!node.IsNull())
	{
		int a = 1;
	}
}

std::string XmlNode::GetValue(const std::string& key)
{
	if (IsNull()) { return ""; }

	QString val_str = "";
	QString key_str = QString::fromLocal8Bit(key.c_str());
	do 
	{
		if (key == "")
		{
			val_str = elem_->text();
			break;
		}

		if (elem_->hasAttribute(key_str))
		{
			val_str = elem_->attribute(key_str);
			break;
		}

		QDomElement ele = elem_->firstChildElement(key_str);
		if (ele.isNull()) { return ""; }

		val_str = ele.text();
	} while (0);

	QTextCodec *codec = QTextCodec::codecForName("GB2312");
	if (NULL == codec) { return val_str.toStdString(); }
	QByteArray encodedString = codec->fromUnicode(val_str);
	return encodedString.data();
}

void XmlNode::SetValue(const std::string& val, const std::string& key, bool arrt_prefer)
{
	if (IsNull()) { return; }

	QString key_str = QString::fromLocal8Bit(key.c_str());
	QString val_str = QString::fromLocal8Bit(val.c_str());

	if (key == "")
	{
		elem_->firstChild().setNodeValue(val_str);
	}
	else if (elem_->hasAttribute(key_str))
	{
		elem_->setAttribute(key_str, val_str);
	}
	else
	{
		QDomElement ele = elem_->firstChildElement(key_str);
		if (ele.isNull())
		{
			if (arrt_prefer)
			{
				elem_->setAttribute(key_str, val_str);
			} 
			else
			{
				QDomElement new_ele = dom_document_->createElement(key_str);
				QDomText text = dom_document_->createTextNode(val_str);
				new_ele.appendChild(text);
				elem_->appendChild(new_ele);
			}
		} 
		else
		{
			ele.firstChild().setNodeValue(val_str);
		}
	}
}

std::string XmlNode::GetName()
{
	if (IsNull()) { return ""; }

	QString tag_str = elem_->tagName();
	QTextCodec *codec = QTextCodec::codecForName("GB2312");
	if (NULL == codec) { return tag_str.toStdString(); }
	QByteArray encodedString = codec->fromUnicode(tag_str);
	return encodedString.data();
}
////////////////////////////////////////////////////////////////////////

XmlConfig::XmlConfig(const std::string& file_path)
	: file_path_(file_path), root_node_(NULL), last_err_(""), dom_document_(NULL)
{
	
}

XmlConfig::~XmlConfig()
{
	if (NULL != dom_document_) { delete dom_document_; }
	if (NULL != root_node_) { delete root_node_; }
}

bool XmlConfig::Load(const std::string& root_name)
{
	if (root_node_ != NULL)  { return true; }  //已经加载

	if (file_path_ == "") 
	{ 
		last_err_ = "file path empty";
		return false; 
	}

	QFile file(file_path_.c_str());  
	if(!file.open(QFile::ReadOnly | QFile::Text)) 
	{ 
		last_err_ = std::string("open file error: ") + file_path_;
		return false; 
	}

	dom_document_ = new QDomDocument();
	QString error;  
	int row = 0, column = 0;  
	if(!dom_document_->setContent(&file, false, &error, &row, &column))
	{
		error = QObject::tr("parse xml error(row=%1, colum=%2):%3").arg(row).arg(column).arg(error);
		last_err_ = error.toStdString();
		return false;
	}

	if (dom_document_->isNull())
	{
		last_err_ = "document is null";
		return false; 
	}

	root_node_ = new XmlNode(dom_document_->documentElement(), dom_document_);
	if (root_name != "" && root_name != root_node_->GetName())
	{
		last_err_ = std::string("file's root name is not ") + root_name;
		delete root_node_;
		root_node_ = NULL;
		return false;
	}

	return true;
}

void XmlConfig::Flush()
{
	if (NULL != dom_document_) 
	{ 
		QFile file(file_path_.c_str());  
		if(file.open(QFile::WriteOnly | QFile::Text)) 
		{
			QTextStream out(&file);
			dom_document_->save(out, 4);
			file.close();
		}
	}
}

XmlNode XmlConfig::FindNode(const std::string& path)
{
	return NULL == root_node_ ? XmlNode(QDomElement(), NULL) : root_node_->FindNode(path);
}

XmlNodeVec XmlConfig::FindChileren(const std::string& path, const std::string& name)
{
	return NULL == root_node_ ? XmlNodeVec() : root_node_->FindChileren(path, name);
}

void XmlConfig::ClearChildren(const std::string& path, const std::string& name)
{
	return NULL == root_node_ ? XmlNodeVec() : root_node_->ClearChildren(path, name);
}

std::string XmlConfig::GetValue(const std::string& path, const std::string& key)
{
	XmlNode node = FindNode(path);
	return node.GetValue(key);
}

void XmlConfig::SetValue(const std::string& path, const std::string& val, const std::string& key, bool arrt_prefer)
{
	XmlNode node = FindNode(path);
	node.SetValue(val, key, arrt_prefer);
}

XmlNode XmlConfig::AppendChild(const std::string& path, const std::string& name)
{
	return NULL == root_node_ ? XmlNode(QDomElement(), NULL) : root_node_->AppendChild(path, name);
}

XmlNode XmlConfig::InsertChild(const std::string& path, const std::string& name, int idx)
{
	return NULL == root_node_ ? XmlNode(QDomElement(), NULL) : root_node_->InsertChild(path, name, idx);
}

void XmlConfig::RemoveChild(const std::string& path, const std::string& name, int idx)
{
	return NULL == root_node_ ? XmlNode(QDomElement(), NULL) : root_node_->RemoveChild(path, name, idx);
}

void XmlConfig::RemoveNode(const std::string& path)
{
	return NULL == root_node_ ? XmlNode(QDomElement(), NULL) : root_node_->RemoveNode(path);
}

}
}
