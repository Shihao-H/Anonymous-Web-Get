#include "awget.h"

string URL = "";
string FILENAME = "";
unsigned int localport;
void client(vector<string>,int);
int receiveFile();
void chainfile_error_message();
int Str_to_Digit(string);
bool isNumber(const string& );
vector<string> split(const string&, char);
bool valid_IP(string);
int chainfile_content_check(string);
Pack processFile(string);


bool isNumber(const string& str)
{
    // std::find_first_not_of searches the string for the first character
    // that does not match any of the characters specified in its arguments
    return !str.empty() &&
        (str.find_first_not_of("[0123456789]") == std::string::npos);
}

// Function to split string str using given delimiter
vector<string> split(const string& str, char delim)
{
    auto i = 0;
    vector<string> list;

    auto pos = str.find(delim);
    
    while (pos != string::npos)
    {
        list.push_back(str.substr(i, pos - i));
        i = ++pos;
        pos = str.find(delim, pos);
    }
    
    list.push_back(str.substr(i, str.length()));
    
    return list;
}


bool valid_IP(string IP)
{
    // split the string into tokens
    vector<string> list = split(IP, '.');

    // if token size is not equal to four
    if (list.size() != 4)
        return false;

    // validate each token
    for (string str : list)
    {
        // verify that string is number or not and the numbers
        // are in the valid range
        if (!isNumber(str) || stoi(str) > 255 || stoi(str) < 0)
            return false;
    }

    return true;
}


int chainfile_content_check(string line)
{
    stringstream ss(line);
    string ip;
    ss >> ip;
    if (!ss || ss.eof())
        return -1;
    if (!valid_IP(ip))
    {
        cerr << "Invalid IP detected." << endl;
        return -1;
    }

    
    string port;
    ss >> port;
    if (!ss || !ss.eof())
        return -1;
    if (!valid_Port(stoi(port)))
    {
        cerr << "Invalid port detected." << endl;
        return -1;
    }

    cout << "<"<<ip << ", " << port <<">"<< endl;

    return 0;
}

Pack processFile(string chainName = "chaingang.txt")
{
    cout<<"awget:"<<endl;
    cout<<"Request: "<<endl;
    Pack pack;
    string line;
    ifstream infile;
    infile.open(chainName);
    if(infile.is_open())
    {
        getline(infile, line);
        int length = Str_to_Digit(line);
        pack.mod = seedRandom(length);
        for(int i=0;i<length;i++)
        {
            getline(infile, line);
            if(chainfile_content_check(line)!=0)
            {
                exit(0);
            }
            pack.element.push_back(line);
        }
    }
    else
    {
        cerr<<"Can't locate chaingang.txt file"<<endl;
        exit(0);
    }
    return pack;
}



int Str_to_Digit(string line)
{
    int length = -1;
    try{
        length=stoi(line);
        if(length<=0)
        {
            throw exception();
        }
    }
    catch(exception &e)
    {
        chainfile_error_message();
        return -1;
    }
    return length;
}

void chainfile_error_message()
{
    cerr << "Error reading the chain file. Please check chain file format." << endl;
    exit(0);
}

void receiveFile(int sockfd)
{
    cout<<"waiting for file..."<<endl;
    FILE *recvFile = fopen(getFileName(URL).c_str(), "a");
    char receivedBuffer [1024];
    if (recvFile == NULL)
        cerr<<"Error!: Unable to open file"<<endl;

    bzero(receivedBuffer, 1024);
    int recvSize = 0;
    while((recvSize = recv(sockfd, receivedBuffer, 1024,0)) > 0)
    {
        int saveSize = fwrite(receivedBuffer, sizeof(char), recvSize, recvFile);
        if(saveSize < recvSize)
        {
            printf("%s\n","Error!: File write failed on server.");
        }
        bzero(receivedBuffer, 1024);
        if (recvSize <= 0 || recvSize > 1024)
        {
            cout << "EOF! Break!" << endl;
            break;
        }

    }
    cout << "Goodbye!" << endl;
    fclose(recvFile);
}


void client(vector<string> chainfile, int mod)
{
    string ip_port = chainfile[mod];
    stringstream ss(ip_port);
    string ip,port;
    ss>>ip;
    ss>>port;

    cout<<"Next SS is <"<<ip<<", "<<port<<">"<<endl;
    char* serverIp = const_cast<char*>(ip.c_str());
    int _port = stoi(port);
//    //create a message buffer
    char msg[1024];
//    //setup a socket and connection tools
    struct hostent* host = gethostbyname(serverIp);
    sockaddr_in sendSockAddr;
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr =
       inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(_port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
//    //try to connect...
    int status = connect(clientSd, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
       cerr<<"Error connecting to socket!"<<endl;
        exit(0);
    }
    cout << "Connected to the server!" << endl;    
    
    //send url to ss
    strncpy(msg, (string(URL+"<=>").c_str()), sizeof(msg));
    send(clientSd, (char*)&msg, strlen(msg), 0);
    
    for(unsigned int i = 0; i < chainfile.size(); i++)
    {
        string temp="";
        bzero(msg, 1024);
        if(i!=chainfile.size()-1)
        {
            temp = chainfile.at(i)+"<=>";
        }
        else
        {
            temp = chainfile.at(i);
        }

        strncpy(msg, temp.c_str(), sizeof(msg));
        send(clientSd, (char*)&msg, strlen(msg), 0);
        bzero(msg, 1024);
    }
    send(clientSd, "EOF", strlen("EOF"), 0);
    bzero(msg, 1024);
    receiveFile(clientSd);

}

int main(int argc, char** argv)
{
    string chainFileName="";
    Pack pack;
    if(argc==2)
    {
        URL=argv[1];
        pack=processFile();
        client(pack.element,pack.mod);
        
    }
    else if(argc==4)
    {
        URL=argv[1];
        chainFileName=argv[3];
        pack=processFile(chainFileName);
        client(pack.element,pack.mod);
    }
    else
    {
        cerr<<"Argument length can only be 2 or 4! The usage is as follows:"<<endl;
        cout << " ./awget <URL>" <<endl;
        cout << " ./awget <URL> -c <chainfile>" <<endl;
        cout<<"For example: ./awget www.google.com or ./awget www.google.com -c MachineList.txt"<<endl;
        cout<<"If no chainfile is specified, the program will default to look for file chaingang.txt in current directory."<<endl;
        exit(0);
    }
        
    
    return 0;
}
