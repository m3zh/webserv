#!/usr/bin/ruby

require "cgi"

def cgi_script()
    cgi = CGI.new
    val = cgi["colour"]

    if val != ""
        message = "<!DOCTYPE html>" \
        "<html>" \
        "<head>" \
            "<title>HTML Backgorund Color</title>" \
        "</head>" \
        "<body style=\"background-color:#{val};\">" \
            "<p>Voilà, it's #{val}</p>" \
        "</body>" \
        "</html>" \
    else
        message = "<!DOCTYPE html>" \
        "<html>" \
        "<head>" \
            "<title>HTML Backgorund Color</title>" \
        "</head>" \
        "<body>" \
            "<p>Why didn\'t you choose a color?</p>" \
        "</body>" \
        "</html>" \
    end
    puts message
end

cgi_script()