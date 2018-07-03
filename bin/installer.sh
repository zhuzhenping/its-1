
#!/usr/bin/env bash

h=~/work_home
bin=$h/bin
mkdir $h ; mkdir $bin; cd $h

rm -fr ./chromedriver_linux64.zip; wget -N https://chromedriver.storage.googleapis.com/2.30/chromedriver_linux64.zip  -O  ./chromedriver_linux64.zip
rm -fr /usr/bin/chromedriver; unzip chromedriver_linux64.zip -d /usr/bin/


python --version
#2.7


yum install -y  git nmon expect python2-pip redis xorg-x11-server-Xvfb xorg-x11-server-Xephyr killall htop  google-noto-sans-simplified-chinese-fonts
pip install selenium sqlalchemy pymysql pycurl flask dmidecode bypy pyvirtualdisplay Glances

echo """
[google64]
name=Google - x86_64
baseurl=http://dl.google.com/linux/rpm/stable/x86_64
enabled=1
gpgcheck=1
gpgkey=https://dl-ssl.google.com/linux/linux_signing_key.pub
""" > /etc/yum.repos.d/google64.repo
yum install google-chrome-stable -y


pip install shadowsocks
echo """
{
\"server\":\"0.0.0.0\",
\"server_port\":717,
\"password\":\"wudian\",
\"timeout\":600,
\"method\":\"rc4-md5\"
}
""" > /etc/shadowsocks.json
nohup ssserver -c /etc/shadowsocks.json -d start &
firewall-cmd --zone=public --add-port=717/tcp --permanent
firewall-cmd --reload



echo """
[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.1/centos7-amd64
gpgkey=https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck=1
""" > /etc/yum.repos.d/mariadb.repo
yum install -y MariaDB-server MariaDB-client
systemctl enable mariadb ; systemctl start mariadb
mysql -uroot -e"grant all privileges on *.* to root@'localhost' identified by 'root'";

wget -N https://raw.github.com/andreafabrizi/Dropbox-Uploader/master/dropbox_uploader.sh    -O  /usr/bin/dropbox_uploader.sh
chmod +x /usr/bin/dropbox_uploader.sh 



wget -N "https://docs.google.com/uc?id=0B3X9GlR6EmbnQ0FtZmJJUXEyRTA&export=download"  -O /usr/bin/gdrive ; chmod +x /usr/bin/gdrive

yum install fail2ban -y
echo """
[ssh-iptables]
enabled = true
filter = sshd
action = iptables[name=SSH, port=ssh, protocol=tcp]
sendmail-whois[name=SSH, dest=84020702@qq.com, sender=fail2ban@email.com]
# Debian 系的发行版 
# logpath = /var/log/auth.log
# # Red Hat 系的发行版
logpath = /var/log/secure
# # ssh 服务的最大尝试次数 
maxretry = 3
""" > /etc/fail2ban/jail.conf
systemctl restart fail2ban


yum install nginx -y
chkconfig nginx on
service nginx start
