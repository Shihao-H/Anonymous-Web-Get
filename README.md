### File submitted:

* [Makefile] - HTML enhanced for web apps!
* [awget.h] - awesome web-based text editor
* [awget.cpp] - Markdown parser done right. Fast and easy to extend.
* [ss.cpp] - great UI boilerplate for modern web apps


### Compile the program:
* make

### Clean up using:
* make clean

### awget usage:
* ./awget <URL>
* ./awget <URL> -c <chainfile>

if no chainfile is specified, the program will default to look for file "chaingang.txt" in the current directory.

### ss usage:
* ./ss <port>
* ./ss -p <port> 

Run the ss first to start the servers in the chainfile list machine for listening, then lanuch the awget. 
