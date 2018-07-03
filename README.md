# linux/win上运行的tick驱动的期货自动交易程序
> 作者 吴典@上海  手机/微信（18621528717）

# 环境(qt和boost无需安装，工程已自带)
qt4.8.6

boost_1_57

（linux）

centos 7 

gcc-c++ version 4.8.5

（win）

vs2010


# quick user it：
git clone https://github.com/wudian/its.git

（linux）

sh bin/init_env.sh

make

（win）

用vs2010打开sln/its.sln

# 目录结构说明
bin： 可执行程序、linux脚本等

cfg: 配置文件

data： 存储行情、交易等数据

inc： 头文件

lib： 静态库和动态链接库

prj： 工程文件和Makefile,以及vs2010的solution

src： 代码源文件

# 功能模块简介
common: log、datetime、queue、thread等基础元素

datalib: 定义tick、kline、order、trade、position等

network: 基于boost asio的tcp通讯库

ctp: 期货的(行情+交易)的接口

accout: 资金结算模块

strategy： 交易策略，tick驱动的追逐趋势的快速交易

