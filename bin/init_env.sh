#qt:
# yum install libX11-devel libXext-devel libXtst-devel
# ./configure


#boost:
#./b2 install --prefix=/home/wd/its/inc link=static runtime-link=shared threading=multi



echo "export its=/home/wd/its">>~/.bashrc
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${its}/lib/qt:${its}/lib/ctp:${its}/lib/Debug:${its}/lib/Release">>~/.bashrc

echo "[credential]
        helper = store">>.git/config
