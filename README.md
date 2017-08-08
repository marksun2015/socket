+ 1.clientservertcp  
    $ server portnumber  
    $ clinet 127.0.0.1 portnumber  

+ 2.clientserverudp  
    $ server   
    $./client 127.0.0.1 4950  

+ 3.select   
    $ telnet 192.168.1.181 9034  

+ 4.filter
    //package filter
    $ gcc -o filter filter.c
    $ sudo ./filter

+ ref  
    socket:
    http://beej-zhtw.netdpi.net/

    filter:
    http://www.cnblogs.com/jinrize/archive/2009/11/24/1609902.html
