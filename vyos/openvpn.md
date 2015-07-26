```
/*
* Generating Keys
* FROM http://www.v12n.com/mediawiki/index.php/Vyatta_How_To#OpenVPN_RoadWarrior
*/

1. Copy the Easy-RSA files to /etc/openvpn

vyatta@vyatta01# sudo su -
root@vyatta01:~# cp -R /usr/share/doc/openvpn/examples/easy-rsa/2.0/* /etc/openvpn/

2. At the end of the vars file there are settings for company, location and so on. Edit to reflect your organization

root@vyatta01:/etc/openvpn#nano vars
export KEY_COUNTRY="NO"
export KEY_PROVINCE="NA"
export KEY_CITY="Oslo"
export KEY_ORG="v12n"
export KEY_EMAIL="me@v12n.com"

3. Source the vars and clean the keys directory before start

root@vyatta01:/etc/openvpn#source ./vars
root@vyatta01:/etc/openvpn#./clean-all

4. Create the certificate Authority certificate:

root@vyatta01:/etc/openvpn#./build-ca

5. Create a key and certificate for the vyatta router. Accept defaults and enter a password when prompted:

root@vyatta01:/etc/openvpn# ./build-key-server V1

6. Create a Diffie-Hellman file

root@vyatta01:/etc/openvpn#./build-dh

7. Create a client key. Change the client name to reflect your client:

root@vyatta01:/etc/openvpn# ./build-key client

8. The outcome of this process should be something like this:

root@vyattaHome:/etc/openvpn# ls keys/
01.pem	ca.key	    index.txt.attr	    client.crt  serial	        V1.csr
02.pem	dh1024.pem  index.txt.attr.old	client.csr  serial.old      V1.key
ca.crt	index.txt   index.txt.old   	client.key  V1.crt
root@vyattaHome:/etc/openvpn#

9. Setting up to revoke keys:

set interfaces openvpn vtun0 openvpn-option “--client-config-dir /config/auth/ccd”
set interfaces openvpn vtun0 openvpn-option “--ccd-exclusive”

And touch a file with same name as key in the ccd directory
```
