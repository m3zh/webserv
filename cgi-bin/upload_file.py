#!/usr/bin/python

import cgi, os
import cgitb

cgitb.enable()

def cgi_script():


    os.write(2, b"first...\n")
    form = cgi.FieldStorage(environ={'REQUEST_METHOD':'POST'})
    # cgi.print_form(form)
    os.write(2, b"******\n")
    
    # A nested FieldStorage instance holds the file
    fileitem = form.getvalue("filename")                              # . is added to create dir in cwd
    directory = "dump"
    os.write(2, b"******\n")
    #os.write(2, fileitem.encode())
    if os.path.exists(directory) == False:
        os.system("sudo mkdir ./" + directory)
    # Test if the file was uploaded
    if fileitem:
        os.write(2, b"third...\n")
        # strip leading path from file name
        # to avoid directory traversal attacks
        # fn = os.path.basename(fileitem.filename)
        fn = os.path.basename(fileitem.replace("\", \"/" ))            # <-- for linux
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