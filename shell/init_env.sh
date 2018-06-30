#qt:
# yum install libX11-devel libXext-devel libXtst-devel
# ./configure -release -platform linux-g++-64 -static -opensource -nomake examples -nomake demos -no-exceptions -no-stl  -no-openssl -no-opengl -no-webkit


#boost:
#./b2 install --prefix=/home/wd/its/inc link=static runtime-link=shared threading=multi



echo "export its=/home/wd/its">>~/.bashrc
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/wd/its/lib/qt:/home/wd/its/lib/ctp:/home/wd/its/lib/Debug:/home/wd/its/lib/Release">>~/.bashrc

echo "[credential]
        helper = store">>/home/wd/its/.git/config
