#!/usr/bin/python

import sys, os

def cgi_script():
    message = ""
    # for param in os.environ.keys():
    #    message += param
    #    message += ":"
    #    message += os.environ[param]
    #    message += "\n"
    # directory = os.environ["DIR_UPLOAD"]
    # # content_type = os.environ["CONTENT_TYPE"]
    # if not os.path.exists(directory):
    #     os.makedirs(directory)

    body = ""
    for line in sys.stdin:              # read in chunks          
        body += line

    message += body

    boundary = body[:body.find("\n")]
    boundary = boundary.strip()
    filename = body[body.find("filename") + 10:]
    filename = filename[:filename.find("\"")].strip()

    message += boundary
    message += "\n"
    message += filename

    content = body[body.find("\n") + 4:]
    content = content[:content.find(boundary)].strip()

    message += "***\n"
    message += content

# def cgi_script():
#     form = cgi.FieldStorage()

#     # Generator to buffer file chunks
#     def fbuffer(f, chunk_size=1024):
#         while True:
#             chunk = f.read(chunk_size)
#             if not chunk: break
#             yield chunk
    
#     # A nested FieldStorage instance holds the file
#     fileitem = form.getvalue("files")                              # . is added to create dir in cwd
#     directory = "../dump"
#     #os.write(2, fileitem.encode())
#     if not os.path.exists(directory):
#         os.makedirs(directory)
#     # Test if the file was uploaded
#     if fileitem:
#         # strip leading path from file name
#         # to avoid directory traversal attacks
#         fn = os.path.basename(fileitem)
#         try:
#             with open(fn,'rb') as source, open(directory + '/' + fileitem, 'wb') as target:
#                 for line in source:
#                     target.write(line)
#             message = 'The file "' + fn + '" was uploaded successfully to directory ' + directory
#         except:
#             message = cgi.FieldStorage()
#     else:
#         message = cgi.FieldStorage()

    print ("""\
    Content-Type: text/html; charset=utf-8;\r\n\r\n
    <html><body>
    <p>%s</p>
    </body></html>
    """%message)

if __name__ == "__main__":
    os.write(2, b"Executing python script...\n")
    cgi_script()