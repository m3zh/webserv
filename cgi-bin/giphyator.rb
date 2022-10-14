#!/usr/bin/ruby

require "cgi"

cgi = CGI.new
h = cgi.params

cgi = CGI.new
h = cgi.params

begin
    val = h.fetch("gif")
    message = "HTTP/1.1 301 Moved Permanently\r\n" \
    "Location: https://giphy.com/search/#{val}\r\n\r\n" \
    "<!DOCTYPE html>" \
    "<html>" \
       "<head>" \
          "<title>Giphy</title>" \
       "</head>" \
       "<body>" \
       "</body>" \
    "</html>" \
rescue
    message = "<!DOCTYPE html>" \
    "<html>" \
       "<head>" \
          "<title>HTML Backgorund Color</title>" \
       "</head>" \
       "<body>" \
         "<p>Something went wrong</p>" \
       "</body>" \
    "</html>" \
end

puts message