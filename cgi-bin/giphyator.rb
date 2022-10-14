#!/usr/bin/ruby

require "cgi"

cgi = CGI.new
h = cgi.params

begin
    val = h.fetch("gif")
rescue
end