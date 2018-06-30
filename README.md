#  作者 吴典@上海  手机/微信（18621528717）

#  linux/win上运行的tick驱动的期货自动交易程序

# ENV
qt4.8.6

boost_1_57

## linux:

centos 7 
gcc-c++ version 4.8.5

## win:

同时还支持vs2010开发调试


# quick user it：
git clone https://github.com/wudian/its.git

sh shell/init_env.sh

make

# 模块简介
common: log、datetime、queue、thread等基础元素

datalib: 定义tick、kline、order、trade、position等

network: 基于boost asio的tcp通讯库

ctp: 期货的(行情+交易)的接口

accout: 资金结算模块
