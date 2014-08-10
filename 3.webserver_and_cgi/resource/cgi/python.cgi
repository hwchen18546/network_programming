#!/usr/local/bin/python3.3

import cgi
import cgitb
cgitb.enable()  # for troubleshooting

#print header
print "Content-type: text/html"
print
print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
print "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict //EN
  \" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
print "<html>"
print "<head>"
print "<title>Python CGI test</title>"
print "</head>"
print "<body>"
print "<p>Hello, world!</p>"
print "</body>"
print "</html>"