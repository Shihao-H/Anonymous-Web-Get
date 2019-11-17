#ifndef AWGET_HEADER
#define AWGET_HEADER

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <vector>
#include <sstream>
#include <cctype>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <random>
#include <ifaddrs.h>
#include <pthread.h>

using namespace std;

struct Pack
{
    int mod;
    vector<string> element;
};

string getFileName (string URL){
    string filename = "";
    if(URL.find("/") > (strlen(URL.c_str()) +1)){
        return "index.html";
    }
    filename = strrchr(URL.c_str(), '/') + 1;
    if (filename.compare("") == 0){
        filename = "index.html";
    }
    return filename;
}

int seedRandom(int length)
{
    random_device seeder;
    mt19937 engine(seeder());
    uniform_int_distribution<int> dist(0, 65535);
    int mod = dist(engine)%length;
    return mod;
}

bool valid_Port(int port)
{
    if(port>65535 || port<1024)
    {
        return false;
    }
    return true;
}

#endif
