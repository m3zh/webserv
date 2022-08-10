#!/usr/bin/env python3

import cgi, os
import cgitb;
cgitb.enable()

form = cgi.FieldStorage()

for param in os.environ.keys():
   print "<b>%20s</b>: %s<\br>" % (param, os.environ[param])

if "name" not in form or "age" not in form:
    message = "<H1>Error</H1>\nPlease fill in the name and age fields."
else:
    name = form.getvalue('name')
    age = form.getvalue('age')
    message = name + ' is a girl of ' + age + ' years old'

print ("""\
Content-Type: text/html\n\r\n\r
<html><body>
<p>%s</p>
</body></html>
"""%message)