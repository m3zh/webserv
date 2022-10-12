#!/usr/bin/python

import sys, os

chunk_size = 1024

def cgi_script():
    message = ""
    # for param in os.environ.keys():
    #    message += param
    #    message += ":"
    #    message += os.environ[param]
    #    message += "\n"
    directory = os.environ["DIR_UPLOAD"].strip()

    # with sys.stdin as _in:
    #     body = _in.read(chunk_size)
    #     while body != b'':
    #         body += _in.read(chunk_size)
    #         handler(buffer)

    body = ""
    for line in sys.stdin:              # read in chunks          
        body += line

    message += body

    boundary = body[:body.find("\n")]
    boundary = boundary.strip()
    filename = body[body.find("filename=") + 10:]
    filename = filename[:filename.find("\"")].strip()

    # message += boundary
    # message += "\n"
    # message += filename

    content = body[body.find("\n"):]
    content = content[:content.find(boundary)].strip()

    # message += "***"
    # message += content
    # message += "***"
    # message += directory

    if not os.path.exists(os.getcwd() + "/" + directory):
        os.umask(0)
        os.makedirs(os.getcwd() + "/" + directory, mode=0o777)
    try:
        target_dir = os.getcwd() + "/" + directory
        with open(target_dir + "/" + filename, 'wb') as target:
            for line in content:
                target.write(line)
        message = 'The file "' + filename + '" was uploaded successfully to directory ' + directory
    except:
        message = 'The file "' + filename + '" could not be uploaded to directory ' + directory

    print ("""\
    <html><body>
    <p>%s</p>
    </body></html>
    """%message)
    
if __name__ == "__main__":
    os.write(2, b"Executing python script...\n")
    cgi_script()