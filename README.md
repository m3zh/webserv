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
git checkout common
git pull origin common
git checkout your-login
git merge common

## to merge your branch into common
[ in your branch: commit + push ]
git checkout common
git merge your-login
git add -u
git commit -m bulah-bulah
git push origin common  
```
:warning:  Before pushing to the branch `common`, make sure the code compiles
```
## side note: 
## git add -u > adds only modified ( updated files )
## git add -A > adds newly created and modified files
```
