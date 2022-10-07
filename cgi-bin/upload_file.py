#!/usr/bin/python

import cgi, os
import cgitb
from BaseHTTPServer import BaseHTTPRequestHandler

cgitb.enable()

def cgi_script():
    form = cgi.FieldStorage(
        fp=self.rfile, 
        headers=self.headers,
        environ={'REQUEST_METHOD':'POST',
                'CONTENT_TYPE':self.headers['Content-Type'],
                })

    # Generator to buffer file chunks
    def fbuffer(f, chunk_size=1024):
        while True:
            chunk = f.read(chunk_size)
            if not chunk: break
            yield chunk
    
    # A nested FieldStorage instance holds the file
    fileitem = form.getvalue("files")                              # . is added to create dir in cwd
    directory = "../dump"
    #os.write(2, fileitem.encode())
    if not os.path.exists(directory):
        os.makedirs(directory)
    # Test if the file was uploaded
    if fileitem:
        os.write(2, b"third...\n")
        # strip leading path from file name
        # to avoid directory traversal attacks
        fn = os.path.basename(fileitem)
        try:
            with open(fn,'rb') as source, open(directory + '/' + fileitem, 'wb') as target:
                for line in source:
                    target.write(line)
            message = 'The file "' + fn + '" was uploaded successfully to directory ' + directory
        except:
            message = cgi.FieldStorage()
    else:
        message = cgi.FieldStorage()

    print ("""\
    Content-Type: text/html; charset=utf-8;\r\n\r\n
    <html><body>
    <p>%s</p>
    </body></html>
    """%message)

if __name__ == "__main__":
    os.write(2, b"Executing python script...\n")
    cgi_script()


"""# A nested FieldStorage instance holds the file
fileitem = form['file']

# Test if the file was uploaded
if fileitem.filename:

    # strip leading path from file name to avoid directory traversal attacks
    fn = os.path.basename(fileitem.filename)
    f = open('files/' + fn, 'wb', 10000)

    # Read the file in chunks
    for chunk in fbuffer(fileitem.file):
      f.write(chunk)
    f.close()
    message = 'The file "' + fn + '" was uploaded successfully'
"""