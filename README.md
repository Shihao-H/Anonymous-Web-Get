
### File submitted:

* [Makefile] 
* [awget.h] - header file, contains all the library and defined function and value for other files.
* [awget.cpp] - reader 
* [ss.cpp] - stepping stone
***
### Compile the program:
```sh
$ make
```

### Clean up using:
```sh
$ make clean
```

### awget usage:
```sh
$ ./awget <URL>
$ ./awget <URL> -c <chainfile>
```

if no ***chainfile*** is specified, the program will default to look for file ***chaingang.txt*** in the current directory.

### ss usage:
```sh
$ ./ss <port>
$ ./ss -p <port>
```

Run the ***ss*** first to start the servers in the chainfile list machine for listening, then launch the ***awget***. 
