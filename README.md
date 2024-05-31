# webserv

## Ressources
```
# config file will be based on nginx default config
https://www.nginx.com/resources/wiki/start/topics/examples/full/
# socket programming in C
https://aticleworld.com/socket-programming-in-c-using-tcpip/  
https://bousk.developpez.com/cours/reseau-c++/TCP/05-envoi-reception-serveur/#LII
# How to Load a Webpage in C
https://www.youtube.com/watch?v=CymFHNqC7n4
# Web programming (CGI) in C ( from 8:40 )
https://www.youtube.com/watch?v=rFaRFCyewpA
# CGI 101
http://www.cgi101.com/book/ch3/text.html
```
## Clone and Fetch

```
git clone git@github.com:m3zh/webserv.git
git fetch -p # to copy remote branches locally
git checkout your-login # to move over to your branch
```  

## Pull and Merge  
``` 
# to merge most recent code into your branch
git checkout main
git pull origin main
git checkout your-login
git merge main

## to merge your branch into main
[ in your branch: commit + push ]
git checkout main
git merge your-login
git add -u
git commit -m bulah-bulah
git push origin main  
```
:warning:  Before pushing to the branch `main`, make sure the code compiles
```
## side note: 
## git add -u > adds only modified ( updated files )
## git add -A > adds newly created and modified files
```
