#!/usr/bin/ruby

require "cgi"

cgi = CGI.new
h = cgi.params

begin
    val = h.fetch("colour")
    message = "<!DOCTYPE html>" \
    "<html>" \
       "<head>" \
          "<title>HTML Backgorund Color</title>" \
       "</head>" \
       "<body style=\"background-color:#{val};\">" \
         "<p>Voilà</p>" \
       "</body>" \
    "</html>" \
rescue
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