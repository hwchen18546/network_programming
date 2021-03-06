<pre>
Project #2: remote working ground (rwg)

In this homework, you are asked to design chat-like systems, called
remote working systems (server only).  In this system, users can
meet with/work with/talk to/make friends.  Basically, this system 
supports all functions, as you did in the first project.  In addition, 
all clients can see what all on-line users are working.  

/////////////////////////
///// Detailed Spec /////
/////////////////////////
1. Same as all detailed spec in hw1. especially...

* Initial setting:  you have to do this when new client connects.

    1. Environment variables: remove all the environment variables except PATH
        e.g.,
    PATH=bin:.

    !!!!! remove all the environment variables except PATH !!!!!
    
    Notice that every client wiil have its own environment variables settings
    e.g.,
    [client A]
    % setenv PATH .
    % printenv PATH
    PATH=.
    
    [client B]
    % printenv PATH
    PATH=bin:.

    2. Executable files in "bin/"
        "ls", "cat": you should copy them from "/usr/bin/" or "/bin/"
        "noop", "removetag", "removetag0", "number": TA provides source code for you to build them
        There are 6 executable files in it. Nothing else.
    
    3. There are only 2 objects in the directory: "bin" and "test.html".
        that means if I command "ls", it will show...
        e.g.,
        % ls
        bin
        test.html
        %

* If a command is not found in environment variable "PATH", show message "Unknown command: [command]."
    e.g.
    % echo "Project1 is too easy, kerker."
    Unknown command: [echo].                # It contains "[]."

* You don't need to write client by yourself, just use telnet.

* When you write something to file, like "ls > test1.txt", 
  if there is something in the file, please remove all of it.
  
  !!!! DO NOT append to the file !!!!
  
  that means 
  e.g.,
  % ls > test.html
  % ls > test.html
  % cat test.html
  bin
  test.html
  %
----------------------------------------------------------------------------------------------------
2. Commands [who], [tell], [yell], [name] are single line commands, which means there will be no
   pipe connected to these commands, just like [printenv] or [setenv].
   
   eg.
   % ls | yell   //It is illegal. This will not appear in testcases.
   % who |2      //It is illegal. This will not appear in testcases.
----------------------------------------------------------------------------------------------------
3. Format of command [tell]: 
   % tell (client id) (message)
   And the client will get the message with following format:
   *** (sender's name) told you ***: (message)
   
   e.g., Assume my name is 'IamUser'.
   [terminal of mine]
   % tell 3 Hello World.
   
   [terminal of client id 3]
   % *** IamUser told you ***: Hello World.

   If the client you want to send message to does not exist, print the following message:
   *** Error: user #(client id) does not exist yet. *** 
   
   e.g.,
   [terminal of mine]
   % tell 3 Hello World.
   *** Error: user #3 does not exist yet. *** 
   % 
----------------------------------------------------------------------------------------------------
4. Format of command [yell]: 
   % yell (message)
   All the clients will get the message with following format:
   *** (sender's name) yelled ***: (message)
   
   e.g., Assume my name is 'IamUser'.
   [terminal of mine]
   % yell Hi everybody
   
   [terminal of all clients]
   % *** IamUser yelled ***: Hi everybody
----------------------------------------------------------------------------------------------------
5. Format of command [name]: 
   % name (name)
   All the clients will get the message with following format:
   *** User from (IP/port) is named '(name)'. ***
   
   eg.
   [terminal of mine]
   % name IamUser
   
   [terminal of all clients]
   % *** User from 140.113.215.62/1201 is named 'IamUser'. ***
   
   Notice that the name CAN NOT be the same as the name which on-line users have,
   or you will get the following message:
   *** User '(name)' already exists. ***
   
   e.g.,
   Mike is on-line, and I want to have the name "Mike" too.
   
   [terminal of mine]
   % name Mike
   *** User 'Mike' already exists. ***
   % 
----------------------------------------------------------------------------------------------------
6. The output format of [who]:
   You have to print a tab between each of tags. 
   Notice that the first column does not print socket fd but client id.
   
   <ID>[Tab]<nickname>[Tab]<IP/port>[Tab]<indicate me>
   (1st id)[Tab](1st name)[Tab](1st IP/port)([Tab](<-me))
   (2nd id)[Tab](2nd name)[Tab](2nd IP/port)([Tab](<-me))
   (3rd id)[Tab](3rd name)[Tab](3rd IP/port)([Tab](<-me))
   ...   

   For example:
   % who 
   <ID>	<nickname>	<IP/port>	<indicate me>
   1	IamStudent	140.113.215.62/1201	<-me
   2	(no name)	140.113.215.63/1013
   3	student3	140.113.215.64/1302
   
   Notice that
   The client's id should be assigned in the range of number 1~30. 
   The server should always assign a smallest unused id to new connected client.
   
   eg.
   <new client login> // server assigns this client id = 1
   <new client login> // server assigns this client id = 2
   <client 1 logout>
   <new client login> // server assigns this client id = 1, not 3
----------------------------------------------------------------------------------------------------
7. When a new client connects to the server, the client does not have a name.
   So the broadcast message will be as follows. 
   *** User '(no name)' entered from (IP/port). ***
   The new client can also receive this message.
   See Notice 7 below for detail.
   
   e.g.
   [terminal of all clients]
   % *** User '(no name)' entered from 140.113.215.63/1013. ***
----------------------------------------------------------------------------------------------------
8. When a client disconnects from the server, broadcast as follows. 
   *** User '(name)' left. ***
   
   e.g.
   [terminal of all clients]
   % *** User 'student5' left. ***
   
   Notice that 
   If someone pipe to you, but you don't receive the message and disconnect from the server
   the pipe should be closed.
   that means next time someone connects to server, he gets the same clinet id,
   but he can't get the message because the pipe is gone.
----------------------------------------------------------------------------------------------------
9. Any client can use '>(client id)' to pipe something to other client.
   You need to redirect stdout and stderr to your target user.
   When a client pipes something to others, broadcast the following message:
   *** (name) (#<client id>) just piped '(command line)' to (receiver's name) (#<receiver's client_id>) ***

   e.g. Assume my name is 'IamUser' and client id = 3.
        Assume there is another client named 'Iam1' with client id = 1.
   [terminal of mine]
   % cat test.html | cat >1
   
   [terminal of all clients]
   % *** IamUser (#3) just piped 'cat test.html | cat >1' to Iam1 (#1) *** 
----------------------------------------------------------------------------------------------------
10. When a client pipes something to others, but the user doesn't exist.
   
   e.g. 
   [terminal of mine]
   % cat test.html | cat >1
   *** Error: user #1 does not exist yet. *** 
   % 
----------------------------------------------------------------------------------------------------
11. Any client can use '<(client id)' to receive something from other clients.
   When a client received pipe from other clients, broadcast the following message:
   *** (my name) (#<my client id>) just received the pipe from (other client's name) (#<other client's id>) by '(command line)' *** 

   e.g. Assume my name is 'IamUser' and client id = 3.
        Assume there is another client named 'student7' with client id = 7.
   [terminal of mine]
   % cat <7
   
   [terminal of all clients]
   *** IamUser (#3) just received from student7 (#7) by 'cat <7' ***
----------------------------------------------------------------------------------------------------


[Notice]
----------------------------------------------------------------------------------------------------
1. When doing broadcast, all clients should receive the message, including me.

   eg. Assume my name is 'IamUser'.
   [terminal of mine]
   % yell Hi...
   *** IamUser yelled ***:  Hi...
   % 
---------------------------------------------------------------------------------------------------- 
2. The length of a client's name is at most 20 characters. 
   Also, there are only alphabet and digits in names.
----------------------------------------------------------------------------------------------------
3. If an user wants to receive a pipe and the pipe does not exist, he/she will receive the
   following error message:
   *** Error: the pipe #(client id)->#(client id) does not exist yet. *** 
   
   A pipe can be taken only one time. After the pipe is taken, it is distroyed.
   If he/she tries to take it again, he/she will receive the error message above.

   e.g. Assume there are only 2 clients connect to server with client id 1, and 2.
   
   [terminal of client id = 1]
   % cat <5
   *** Error: the pipe #5->#1 does not exist yet. *** 
   % ls >2
   
   [terminal of client id = 2]
   % cat <1

   [terminal of client id = 2]
   % cat <1
   *** Error: the pipe #1->#2 does not exist yet. *** 
   % 
----------------------------------------------------------------------------------------------------
4. Before the pipe is taken, client cannot send to its pipe again.
   Otherwise, he/she will receive the following error message. 
   *** Error: the pipe #(client id)->#(client id) already exists. *** 
   
   e.g.
   [terminal of client id = 2]
   % ls >1
   % ls >1
   *** Error: the pipe #2->#1 already exists. *** 
   % 
----------------------------------------------------------------------------------------------------
5. When a client receive message from other client's pipe and then redirect the stdout into its pipe using ">1",
   Server will broadcast two messages to all clients.
   Server shows message about receiving pipe first, and then shows message about sending pipe.
   For example:
   
   eg. Assume my name is 'IamUser' and client id = 3.
       Assume there is another client named 'student7' with client id = 7.
   
   % cat <7 >1
   *** IamUser (#3) just received from student7 (#7) by 'cat <7 >1' ***
   *** IamUser (#3) just piped 'cat <7 >1' to Iam1 (#1) *** 
   % 
   
   or
   
   % cat >1 <7
   *** IamUser (#3) just received from student7 (#7) by 'cat >1 <7' ***
   *** IamUser (#3) just piped 'cat >1 <7' to Iam1 (#1) *** 
   % 
----------------------------------------------------------------------------------------------------
6. Do not worry about synchronization problem.
----------------------------------------------------------------------------------------------------
7. Server sends "% " only when the client connected to it or current command finished.

   e.g. A possible situation
   ****************************************
   ** Welcome to the information server. **
   ****************************************
   *** User '(no name)' entered from 140.113.215.63/1013. ***
   % *** User '(no name)' entered from 140.113.215.64/1014. ***
   *** User '(no name)' entered from 140.113.215.65/1015. ***
   *** User '(no name)' entered from 140.113.215.66/1016. ***
   who
   <ID>	<nickname>	<IP/port>	<indicate me>
   1	(no name)	140.113.215.63/1013	<-me
   2	(no name)	140.113.215.64/1014
   3	(no name)	140.113.215.65/1015
   4	(no name)	140.113.215.66/1016
   % 
   
   The user connected to the server, showing his/her online message and "% ".
   3 other users also connected to the server before the user send first command, showing 3 online messages.
   The user entered 'who', showing online user information.
   After command 'who' finished, server send "% " to client.
   
   Notice the "% " position.
----------------------------------------------------------------------------------------------------

/////////////////////////
///// Requirements  /////
/////////////////////////

* Write two different server programs. 
  (1) Use the single-process concurrent paradigm.
		由於只有一個 process, 直接用 pipe 即可要用 FIFO 也可
  (2) Use the concurrent connection-oriented paradigm with shared memory.  \
		那是強烈的建議你用 FIFO 去做的意思

* For each client, all behaviors required by project 1 are still required
  in this project.  

Hints: 

* You can assume that the # of users is <= 30. 

* ">3" or "<4" has no space between them.  So, you can distinct them 
  from "> filename" easily.  

* For the second program (2), 
  * One chat buffer has at most 10 unread messages, each of which has 
    at most 1024 bytes. 
  
  * For each pair of clients (A, B), one chat buffer is allocated for A 
    to send messages to B. If the chat buffer is full (10 messages), 
    A needs to wait for B to consume messages.  
  
  * For each pipe for ">3", use FIFO instead of pipe. 
  
  * If a message has more than 1024 bytes, simply truncate it to 1024 bytes. 
  
  * If you use BBS-like method, you can use signal "SIGUSR1" or "SIGUSR2" to help.  
  
  * For "who", the master process maintain an id for each forked process. 
    Since there are no more than 30 processes, id <= 30 and let the id be client id.

If you find some commands confusing or not workable, please let us know. 
</pre>