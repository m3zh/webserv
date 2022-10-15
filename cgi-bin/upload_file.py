#!/usr/bin/python

import sys, os
import binascii

def cgi_script():
    message = ""
    directory = os.environ["DIR_UPLOAD"].strip()
    website_root = "www/website"

    body = ''
    for line in sys.stdin.read():
        body += line

    boundary = body[:body.find("\n")]
    boundary = boundary.strip()
    filename = body[body.find("filename=") + 10:]
    filename = filename[:filename.find("\"")].strip()

    content = body[body.find("\r\n\r\n"):]
    content = content[:content.find(boundary)].strip()

    if not os.path.exists(os.getcwd() + "/" + website_root + directory):
        os.umask(0)
        os.makedirs(os.getcwd() + "/" + website_root + directory, mode=0o777)
    try:
        target_dir = os.getcwd() + "/" + website_root + directory                                           # we save at the website root
        with open(target_dir + "/" + filename, 'wb') as target:
            for line in content:
                target.write(line)
        message = 'The file "' + filename + '" was uploaded successfully to directory ' + website_root + directory
    except:
        message = 'The file "' + filename + '" could not be uploaded to directory ' + website_root + directory

    print ("""\
    <html><body>
    <p>%s</p>
    </body></html>
    """%message)
    
if __name__ == "__main__":
    os.write(2, b"Executing python script...\n")
    cgi_script()