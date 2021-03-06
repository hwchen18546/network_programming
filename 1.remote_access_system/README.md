<pre>
Project #1: Remote Access System (ras). 

In this homework, you are asked to design rsh-like access systems,
called remote access systems, including both 
client and server.  In this system, the server designates a directory, 
say ras/; then, clients can only run executable programs inside the 
directory, ras/.  

The following is a scenario of using the system.

csh> telnet myserver.nctu.edu.tw 7000 # the server port number 
**************************************************************
** Welcome to the information server, myserver.nctu.edu.tw. **
**************************************************************
** You are in the directory, /home/studentA/ras/.
** This directory will be under "/", in this system.  
** This directory includes the following executable programs. 
** 
**	bin/
**	test.html	(test file)
**
** The directory bin/ includes: 
**	cat
**	ls
**	removetag		(Remove HTML tags.)
**	number  		(Add a number in each line.)
**
** In addition, the following two commands are supported by ras. 
**	setenv	
**	printenv	
** 
% printenv PATH                       # Initial PATH is bin/ and ./
PATH=bin:.
% setenv PATH bin                     # Set to bin/ only
% printenv PATH
PATH=bin
% ls
bin/		test.html
% ls bin
ls		cat		removetag     removetag0    number
% cat test.html > test1.txt
% cat test1.txt
<!test.html>
<TITLE>Test<TITLE>
<BODY>This is a <b>test</b> program
for ras.
</BODY>
% removetag test.html

Test 
This is a test program
for ras.

% removetag test.html > test2.txt
% cat test2.txt

Test 
This is a test program
for ras.

% removetag0 test.html
Error: illegal tag "!test.html"

Test 
This is a test program
for ras.

% removetag0 test.html > test2.txt
Error: illegal tag "!test.html"
% cat test2.txt

Test 
This is a test program
for ras.

% removetag test.html | number
  1 
  2 Test 
  3 This is a test program
  4 for ras.
  5 
% removetag test.html |1           # This pipe will pipe stdout to next command. 
% number                           # The command's stdin is from the previous pipe.
  1 
  2 Test 
  3 This is a test program
  4 for ras.
  5 
% removetag test.html |2           # |2 will skip two commands and then pipe stdout to the next command. 
% ls
bin/		test.html   test1.txt   test2.txt
% number                           # The command's stdin is from the previous pipe.
  1 
  2 Test 
  3 This is a test program
  4 for ras.
  5 
% removetag test.html |2           # This pipe will pipe stdout to next next command. 
% removetag test.html |1           # This pipe will pipe stdout to next command. (Note: merge with the previous one)
% number                           # The command's stdin is from the previous pipe.
  1 
  2 Test 
  3 This is a test program
  4 for ras.
  5 
  6 
  7 Test 
  8 This is a test program
  9 for ras.
 10 
% removetag test.html |2           # This pipe will pipe stdout to next next command. 
% removetag test.html |1           # This pipe will pipe stdout to next command. (Note: merge with the previous one)
% number |1                        # The command's stdin is from the previous pipe, but piped to next one. 
% number                           # The command's stdin is from the previous pipe. (Note: the output may not be the same order.)
  1   1 
  2   2 Test 
  3   3 This is a test program
  4   4 for ras.
  5   5 
  6   6 
  7   7 Test 
  8   8 This is a test program
  9   9 for ras.
 10  10 
% removetag test.html | number |1 
% number 
  1   1 
  2   2 Test 
  3   3 This is a test program
  4   4 for ras.
  5   5 
% ls |2
% ls
bin/		test.html   test1.txt   test2.txt
% number > test3.txt
% cat test3.txt
  1 bin/
  2 test.html
  3 test1.txt
  4 test2.txt
% removetag0 test.html !1          # This pipe will pipe both stdout and stderr to next command. 
                                   # !n is the same as |n, except pipe both stdout and stderr. 
% number                           # The command's stdin is from the previous pipe.
  1 Error: illegal tag "!test.html"
  2 
  3 Test 
  4 This is a test program
  5 for ras.
  6 
% date
Unknown Command: date
# Let TA do this "cp /bin/date bin"  in your csh directory
% date
Wed Oct  1 00:41:50 CST 2003
% exit
csh> 


Requirements and Hints: 

1. All data to stdout and stderr from server programs return to
clients.

2. The remote directory in the server at least needs to include
"removetag", "removetag0" and "number" and a test html file.  

3. The programs removetag and number are not important in this project. 
TAs will provide you with these two programs. 

4. You MUST use "exec" to run "ls", etc.  You MUST NOT use functions 
   like "system()" or some other functions (in lib) to do the job.
   That is, you cannot use a function which will include "exec".  

5. commands |n and !n The level number n is no more than 1000.

6. For commands that are empty or have errors, the pipe to the command is closed
   subsequently.  

Additional comments: 

1. All arguments MUST NOT INCLUDE "/" for security.
   You should print out error message instead.

2. You can still let stderr be the console output for your debugging messages. 

3. In addition to demo, you also need to prepare a simple report.

If you find some commands confusing or not working, please let me
know. Thanks.
</pre>