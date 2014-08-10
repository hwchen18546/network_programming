<pre>
Part1 CGI

Write a CGI program to receive an HTTP request described as follows.
The parameters of the HTTP request are:

    h1=140.113.210.101      # the first server's IP.
    p1=7000                 # the first server's port.
    f1=batch_file1          # the batch file name redirected to the first ras server.
    h2=140.113.210.103      # the second server's IP.
    p2=7000                 # the second server's port.
    f2=batch_file2          # the batch file name redirected to the second ras server.
    h3=140.113.210.103      # the third server's IP.
    p3=7001                 # the third server's port.
    f3=batch_file3          # the batch file name redirected to the third rwg server.
    h4=                     # no more server. 
    h5=                     # no more server. 

Then, the CGI program connects to the three ras or rwg servers and then
redirects the batch file (stored in the HTTP server) as input to these servers.  
When receiving messages, send these messages back to the browser as 
the returning web page.  Note that the returning web page include three columns, 
one for displaying the returning messages from each server.  
A sample of the web page is given on the board. 
```html
        <html>
        <head>
        <meta http-equiv="Content-Type" content="text/html; charset=big5" />
        <title>Network Programming Homework 3</title>
        </head>
        <body bgcolor=#336699>
        <font face="Courier New" size=2 color=#FFFF99>
        <table width="800" border="1">
        <tr>
        <td>140.113.210.145</td><td>140.113.210.145</td><td>140.113.210.145</td></tr>
        <tr>
        <td valign="top" id="m0"></td><td valign="top" id="m1"></td><td valign="top" id="m2"></td></tr>
        </table>
        <script>document.all['m0'].innerHTML += "****************************************************************<br>";</script>
        <script>document.all['m1'].innerHTML += "****************************************************************<br>";</script>
        <script>document.all['m0'].innerHTML += "** Welcome to the information server, dist5.csie.nctu.edu.tw. **<br>";</script>
        <script>document.all['m0'].innerHTML += "** You are in the directory, /.<br>";</script>
        <script>document.all['m1'].innerHTML += "** You are in the directory, /.<br>";</script>
        <script>document.all['m2'].innerHTML += "****************************************************************<br>";</script>
        <script>document.all['m1'].innerHTML += "% <b>removetag test.html</b><br>";</script>
        <script>document.all['m1'].innerHTML += "<br>";</script>
        <script>document.all['m1'].innerHTML += "Test<br>";</script>
        <script>document.all['m1'].innerHTML += "This is a test program<br>";</script>
        <script>document.all['m1'].innerHTML += "for ras.<br>";</script>
        <script>document.all['m1'].innerHTML += "<br>";</script>
        <script>document.all['m1'].innerHTML += "<br>";</script>
        <script>document.all['m1'].innerHTML += "% <b></b><br>";</script>
        ....
        </font>
        </body>
        </html>
```

Requirements: 

  1. You must implement a CGI program to provide users to do batch process in your ras/rwg server.
  2. It must work correctly with the http server used in your development environment(e.g. workstations of dept. CS).
  3. The number of target servers is not greater than 5.
  4. The information about batch is sent by users by GET method.
  5. Those parameters must be in two form. One is that hostname, port, and file name are provided, and the other is that hostname is not specified.
  6. Your CGI program should show one table which lists the results from your ras/rwg servers.
  7. There are five cells in the table, and the results from your ras/rwg server must be listed in corresponding cell.
     That is, the content from 1st ras/rwg server will not be displayed in 2nd cell to 5th cell.
  8. You should not use fork() in CGI, instead, you should use non-blocking connect/select/read/write.
  9. A large file will be tested, that is, you should not suppose that write will always success. You should handle write event properly.


Part2 http server in UNIX
  1. You must implement a http server which can invoke requested CGI program to execute.
  2. Only parameters in GET method should be processed. 
  3. All files name of which ends with ".cgi" should be interpreted as a executable file and should be invoked.
  4. It is not cared if requested file does not exist.
  5. You have to setup all the environmant variables (totally 9) in page 8 of http.pdf when you execute CGI program,
	 and the content of these environmant variables except "QUERY_STRING" can be everything you want.(Content of QUERY_STRING MUST be correct.)
	Ex: REMOTE_ADDR="140.113.185.117" or "remote address" or "everything I want"
  6. Don't use the http server provided by CS workstation.

 CGI programs 透過環境變數與http daemon 溝通
幾個重要的環境變數
−
QUERY_STRING
−
CONTENT_LENGTH
−
REQUEST_METHOD
REQUEST_METHOD=“GET” or “POST”
−
SCRIPT_NAME
SCRIPT_NAME = “/~icwu/chat/cgi-bin/echo-cgi”
−
REMOTE_HOST
REMOTE_HOST=“java.csie.nctu.edu.tw”
−
REMOTE_ADDR
REMOTE_ADDR=“140.113.185.117”
−
AUTH_TYPE, REMOTE_USER, REMOTE_IDENT 
</pre>