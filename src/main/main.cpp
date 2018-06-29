
#include <boost/thread/thread.hpp>
#include <iostream>
#include "QtCore/QFile"
#include "QtCore/QDebug"

void hello()
{
	QFile file("/home/wd/its/src/main/main.cpp");
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug()<<"Can't open the file!"<<endl;
	}
	QTextStream in(&file);
	while(!in.atEnd()){
		qDebug() << in.readLine();//读取一行,还有读取所有readAll();

	}
	file.close();
}

int main(int argc,char* argv[])
{
	boost::thread thrd(&hello);
	thrd.join();
	return 0;
}
