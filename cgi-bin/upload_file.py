#!/usr/bin/python

import cgi, os
import cgitb

cgitb.enable()

def cgi_script():


    os.write(2, b"first...\n")
    form = cgi.FieldStorage(environ={'REQUEST_METHOD':'POST'})
    for param in form:                                     # <-- check params passed in env
        os.write(2, param.encode())
        os.write(2, b": ")
        os.write(2, form.getvalue(param).encode())
        os.write(2, b"--\n")
    
    
    # A nested FieldStorage instance holds the file
    fileitem = form.getvalue("file1")                              # . is added to create dir in cwd
    directory = "dump"
    os.write(2, b"******\n")
    os.write(2, fileitem.filename.encode())
    if os.path.exists(directory) == False:
        os.system("sudo mkdir ./" + directory)
    # Test if the file was uploaded
    if fileitem.filename:
        os.write(2, b"third...\n")
        # strip leading path from file name
        # to avoid directory traversal attacks
        # fn = os.path.basename(fileitem.filename)
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

    os.write(2, message.encode())

if __name__ == "__main__":
    os.write(2, b"Executing python script...\n")
    cgi_script()