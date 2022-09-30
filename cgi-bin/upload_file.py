#!/usr/bin/python

import cgi, os
import cgitb

def cgi_script()
    cgitb.enable()

    for param in os.environ.keys():                                     # <-- check params passed in env
    print "<b>%20s</b>: %s<\br>" % (param, os.environ[param])

    form = cgi.FieldStorage()

    # A nested FieldStorage instance holds the file
    fileitem = form.getvalue('file')                                       # . is added to create dir in cwd
    directory = "dump"

    if os.path.exists(directory) == False:
        os.system("sudo mkdir ." + directory)
    # Test if the file was uploaded
    if fileitem.filename:
        # strip leading path from file name
        # to avoid directory traversal attacks
        fn = os.path.basename(fileitem.filename)
        fn = os.path.basename(fileitem.filename.replace("\", \"/" ))            # <-- for linux
        open(directory + '/' + fn, 'wb').write(fileitem.file.read())
        message = 'The file "' + fn + '" was uploaded successfully\nto directory ' + directory
    else:
        message = 'No file was uploaded'

    print ("""\
    Content-Type: text/html\n\r\n\r
    <html><body>
    <p>%s</p>
    </body></html>
    """%message)

if __name__ == "__main__":
    os.write(2, b"Executing python script...\n")
    cgi_script()