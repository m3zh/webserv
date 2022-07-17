# webserv

## Ressources
```
# config file will be based on nginx default config
https://www.nginx.com/resources/wiki/start/topics/examples/full/
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
