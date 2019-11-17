#include "awget.h"

string URL = "";
string local_ip_port="";
void server(int, char*);
void downloadFile();
string get_ip_port(sockaddr_in, int);
void sendfile(int);
vector<string> handle_client_message(int);
void *client_request(void *);
string printformat();

string printformat(string str)
{
    stringstream ss(str);
    string ip,port;
    ss>>ip;
    ss>>port;
    return string("<")+ip+string(", ")+port+string(">");
}


void sendfile(int PeerSock)
{
    int LENGTH = 512;
    char buffer[LENGTH];
    string FILENAME = getFileName(URL);
    FILE *fs = fopen(FILENAME.c_str(), "r");
    if(fs == NULL){
         cerr<<"ERROR: File %s not found."<<endl;
         exit(0);
    }

    bzero(buffer, LENGTH);
    int fs_block_sz=0;
    while((fs_block_sz = fread(buffer, sizeof(char), LENGTH, fs)) > 0)
    {
        if(send(PeerSock, buffer, fs_block_sz, 0) < 0) {
            cerr<<"error!!!!"<<endl;
            break;
        }
        bzero(buffer, LENGTH);
    }
    fclose(fs);
    cout<<"Goodbye!"<<endl;
   if (remove(FILENAME.c_str()) != 0)
       cerr<<"File deletion failed"<<endl;

}

//get local ip and port
string get_ip_port(int newSd,sockaddr_in my_addr)
{
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;      
    vector<string> address;
    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
//             is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            address.push_back(string(ifa->ifa_name)+string(": ")+string(addressBuffer));
//             printf("'%s': %s\n", ifa->ifa_name, addressBuffer);
         } 
    }
    if (ifAddrStruct!=NULL)
        freeifaddrs(ifAddrStruct);//remember to free ifAddrStruct
    
    
        
    string hold = "";
    for(unsigned int i=0; i<address.size();i++)
    {
        size_t pos = address[i].find("eno1");
        size_t pos2 = address[i].find("en0");
        if(pos!= std::string::npos || pos2!= std::string::npos)
        {
            hold = address[i];
        }
    }
    
    size_t start = hold.find(": ");
    hold=hold.substr(start+2);
    
    char myIP[16];
    unsigned int myPort;
    bzero(&my_addr, sizeof(my_addr));
    socklen_t clientLen = sizeof(my_addr);
    getsockname(newSd, (struct sockaddr *) &my_addr, &clientLen);
    inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
    myPort = ntohs(my_addr.sin_port);
    
    return hold+string(" ")+to_string(myPort);
}


void downloadFile()
{
    string command="";
    string temp = URL.substr(URL.find_last_of("/") + 1);
    cout<< "issusing wget for file "<<temp<<endl;
    if (temp.find('.') != std::string::npos)
    {
        command = "wget " + URL;
    }// found
    else
    {
        command = "wget www.google.com";
    } // not found
    
    system((const char*)command.c_str());
    cout<< "File recieved"<< endl; 
    
}

vector<string> handle_client_message(int client_sk)
{
    //buffer to send and receive messages with
    char msg[1024];
    bzero(msg, 1024);
    string temp="";
    while(recv(client_sk, (char*)&msg, sizeof(msg), 0)>0)
    {
       temp+=msg;
       bzero(msg, 1024);
       if (temp.find("EOF") != string::npos)
       {
//            std::cout << "found!" << '\n';
           break;
       }
    }

    temp = temp.substr (0,temp.length()-3);
//     cout<< "temp2:   " <<temp<<endl;
    string delimiter = "<=>";
    size_t pos = 0;
    size_t set = temp.find(delimiter);

    //set url
    URL = temp.substr(0, set);
    cout << "Request: " << URL << std::endl;
    temp = temp.substr(set+3);
//     cout << "after-set " << temp << std::endl;

    string token;
    vector<string> vec;
    vector<string> vec2;
    while ((pos = temp.find(delimiter)) != std::string::npos)
    {
        token = temp.substr(0, pos);
//         cout << "token  " << token << std::endl;
        vec.push_back(token);
        temp.erase(0, pos + delimiter.length());
    }
//     cout << "last  " << temp << std::endl;
    vec.push_back(temp);

    for (unsigned int i=0; i<vec.size(); i++)
    {
        if(vec[i].compare(local_ip_port) != 0)
        {
            vec2.push_back(vec[i]);
        }
    }

    
//     cout << "vector size: " << vec2.size() << std::endl;
    
    return vec2;

}


void *client_request(void *connected_clientSK)
{
    int PeerSock = *(int*)connected_clientSK;
    vector<string> chainfile = handle_client_message(PeerSock);
    
    if(chainfile.size()>0)
    {
        cout<< "chainlist is "<<endl;
        for (unsigned int i=0; i<chainfile.size(); i++)
        {
            cout<< printformat(chainfile[i]) <<endl;
        }
        
        
        int index = seedRandom(chainfile.size());
        string ip_port = chainfile[index];
        
        stringstream ss(ip_port);
        string ip,port;
        ss>>ip;
        ss>>port;
        
        cout<<"Next SS is <"<<ip<<", "<<port<<">"<<endl;
        
        
        char* serverIp = const_cast<char*>(ip.c_str());
        int _port = stoi(port);
        
        char sendbuffer[1024];
        //    //setup a socket and connection tools
        struct hostent* host = gethostbyname(serverIp);
        sockaddr_in sendSockAddr;
        bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
        sendSockAddr.sin_family = AF_INET;
        sendSockAddr.sin_addr.s_addr =
           inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
        sendSockAddr.sin_port = htons(_port);
        int clientSd = socket(AF_INET, SOCK_STREAM, 0);
        if(clientSd >= 0)
        {
            //    //try to connect...
            int status = connect(clientSd, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
            if(status >= 0)
            {
                cout<<"waiting for file..."<<endl;
                    //send url to ss
               strncpy(sendbuffer, (string(URL+"<=>")).c_str(), sizeof(sendbuffer));
               send(clientSd, (char*)&sendbuffer, strlen(sendbuffer), 0);

               for(unsigned int i = 0; i < chainfile.size(); i++)
               {
                   string temp="";
                   bzero(sendbuffer, 1024);
                   if(i!=chainfile.size()-1)
                   {
                       temp = chainfile.at(i)+"<=>";
                   }
                   else
                   {
                       temp = chainfile.at(i);
                   }
                   strncpy(sendbuffer, temp.c_str(), sizeof(sendbuffer));
                   send(clientSd, (char*)&sendbuffer, strlen(sendbuffer), 0);
               }
               send(clientSd, "EOF", strlen("EOF"), 0);
               
               
               char receivedBuffer [1024];
               int recvSize;
              cout << "Relaying file ..." << endl;
               while((recvSize = recv(clientSd, receivedBuffer, 1024,0)) > 0)
               {
//                    cout<<"pass: "<<receivedBuffer<<endl;
                   if (send(PeerSock, receivedBuffer,1024,0) > 0)
                   {
                       bzero(receivedBuffer, 1024);
                   }
               }
              cout << "Goodbye!" << endl;
               shutdown(PeerSock,2);
                }
            
                else
                {
                   cerr<<"Error connecting to socket!"<<endl;
                    exit(0);
                }
            }
        else
        {
           cerr<<"Error creating socket!"<<endl;
            exit(0);
        }
        
    }
    else
    {
        cout<< "chainlist is empty"<<endl;
        downloadFile();
        sendfile(PeerSock);
        shutdown(PeerSock,2);
    }
    cout<<"\nWaiting for new request..."<<endl;
    return 0;
}

void printUsage()
{
    cout<<"Usage:\n"<<" ./ss <port> \n"<< " ./ss -p <port>"<<endl;
    cout<<"for example: ./ss 8088 or ./ss -p 8088"<<endl;
}

int main(int argc, char *argv[])
{
    int port;
    if(argc == 2)
    {
        port = atoi(argv[1]);
        if(!valid_Port(port))
        {
            cerr<< "Invalid port number!"<<endl;
            exit(0);
        }
        port = atoi(argv[1]);
    }
    
    if(argc==3)
    {
        int c;
        while((c = getopt(argc,argv, "p:"))!=-1)
        {
            switch(c)
            {
                case 'p': port = atoi(optarg);break;
                default: printUsage(); exit(0);
            }
        }
        
        if(!valid_Port(port))
        {
            cerr<< "Invalid port number!"<<endl;
            exit(0);
        }
    }
    
    if(!(argc==2||argc==3))
    {
        printUsage(); 
        exit(0);
    }
    
    //setup a socket and connection tools
    sockaddr_in servAddr, my_addr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    
    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    //bind the socket to its local address
    int bindStatus = ::bind(serverSd, (struct sockaddr*) &servAddr,
        sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
//        cout << "Waiting for a client to connect..." << endl;

    //listen for up to 10 requests at a time
    listen(serverSd, 10);

    local_ip_port = get_ip_port(serverSd, my_addr);
    cout << "ss " << printformat(local_ip_port)<<":"<< endl;
    
       //receive a request from client using accept
       //we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
       //accept, create a new socket descriptor to
       //handle the new connection with client

    
    pthread_t thread_id;
    int client_sock;
    while((client_sock = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize)))
    {
        
        if( pthread_create( &thread_id , NULL ,  client_request , (void*) &client_sock) < 0)
        {
            cerr<<"Could not create thread."<<endl;
        }
    }
    
    if(client_sock < 0)
    {
        cerr << "Error accepting request from client!" << endl;
        exit(0);
    }
    return 0;
    
 
    
}




