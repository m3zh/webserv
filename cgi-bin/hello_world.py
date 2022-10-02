#!/usr/bin/python

import cgi, os

def cgi_script():
    form = cgi.FieldStorage()

    # check params passed in env
    #for param in os.environ.keys():
    #    print "<b>%20s</b>: %s</br>" % (param, os.environ[param])

    if "name" not in form or "age" not in form:
        message = "<H1>Error</H1>\nNo name or age field provided."
    else:
        name = form.getvalue('name')
        age = form.getvalue('age')
        message = "Hey there " + name + ',\nyou\'re, what, ' + age + ' years old already?'

    print ("""\
    Content-Type: text/html\n\r\n\r
    <html><body>
    <p>%s</p>
    </body></html>
    """%message)

    os.write(2, message.encode())

if __name__ == "__main__":
    os.write(2, b"Executing python script...\n")
    cgi_script()
