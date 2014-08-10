<pre>
Project IV: SOCKS Server (Version 4)

SOCKS 4 Protocol:
http://www.socks.nec.com/protocol/socks4.protocol
SOCKS 4a Protocol (extension)
http://www.socks.nec.com/protocol/socks4a.protocol

In this project, you are asked to implement the SOCKS 4 firewall protocol in the application layer of the OSI model.

SOCKS is similar to a proxy (i.e. intermediary-program) that acts as both server and client for the purpose of making request on behalf of other clients. Because the SOCKS protocol is independent of application protocols, it can be used for many different services: telnet, ftp, www, etc.

There are two types of the SOCKS operations (i.e. CONNECT and BIND). You have to implement both of them.
  
Project summary:
1.	Put the SOCKS client into your CGI program in Project 3 (1). The web page needs extra fields for SOCKS proxy IP and port for each server. 
2.	Write a SOCKS server
2.1. CONNECT Operation (use Web Browser)
[socks.conf] : permit c - - - -
2.2. BIND Operation (use FTP client + normal mode)
[socks.conf] : permit b - - - -
3.	Include the firewall rule (use Web Browser) in the server.
[socks.conf] : permit c - - 140.113. -	#	[NCTU only]
		or
[socks.conf] : permit c - - 140.114. - 	#	[NTHU only]
Additional Requirement:
	Use the concurrent, connection-oriented paradigm.
	Output necessary messages on the screen in the SOCKS server.
</pre>
