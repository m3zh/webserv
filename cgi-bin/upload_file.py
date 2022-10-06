#!/usr/bin/python

import cgi, os
import cgitb

cgitb.enable()

def cgi_script():
    os.write(2, b"first...\n")
    form = cgi.FieldStorage()
    cgi.print_form(form)
    os.write(2, b"******\n")
    
    # A nested FieldStorage instance holds the file
    fileitem = form.getvalue("filename")                              # . is added to create dir in cwd
    directory = "../dump"
    os.write(2, b"******\n")
    #os.write(2, fileitem.encode())
    if not os.path.exists(directory):
        os.makedirs(directory)
    # Test if the file was uploaded
    if fileitem:
        os.write(2, b"third...\n")
        # strip leading path from file name
        # to avoid directory traversal attacks
        fn = os.path.basename(fileitem)
        with open(fn,'rb') as source, open(directory + '/' + fileitem, 'wb') as target:
            for line in source:
                target.write(line)
        message = 'The file "' + fn + '" was uploaded successfully to directory ' + directory
    else:
        message = 'No file was uploaded'

    print ("""\
    Content-Type: text/html\r\n\r\n
    <html><body>
    <p>%s</p>
    </body></html>
    """%message)

if __name__ == "__main__":
    os.write(2, b"Executing python script...\n")
    cgi_script()