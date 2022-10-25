#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#ifdef __linux__
#include <linux/in.h>
#endif
using namespace std;

#define _MSG_BUFFER_SIZE_ 1024

struct timeval read_timeout;

class Client
{
    int socket_fd, port_number;
    char outgoing_message[_MSG_BUFFER_SIZE_];
    struct sockaddr_in server_address;
    int outgoing_message_length;
    string username;

public:
    // ---------------------------------------------------------------------------------------------
    // Summary        : Create new instance of tcp client class (Class constructor)
    //
    // Pre-Condition  : None
    // Post-Condition : None
    //
    // Input :-
    //        > [string] host_address_name : address name of the host i.e ["
    Client(string host_address_name, int port_number)
    {
        // initialize host address struct
        server_address.sin_family = AF_INET;
        inet_pton(AF_INET, host_address_name.c_str(), &server_address.sin_addr.s_addr);
        server_address.sin_port = htons(port_number);

        // initialize connection socket
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    }
    void Connect()
    {
        // connect to host
        connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));
        cout << "Connected to server from " << inet_ntoa(server_address.sin_addr) << ":" << ntohs(server_address.sin_port) << endl;
        cout << "Start Texting!" << endl;
    }
    void Send(string message)
    {
        // send message to host
        outgoing_message_length = write(socket_fd, message.c_str(), message.length());
    }
    string Receive()
    {
        // receive message from host
        char incoming_message[_MSG_BUFFER_SIZE_];
        read_timeout.tv_sec = 0;
        read_timeout.tv_usec = 10;
        setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
        int incoming_message_length = read(socket_fd, incoming_message, _MSG_BUFFER_SIZE_);
        if (incoming_message_length > 0)
        {
            incoming_message[incoming_message_length] = '\0';
            return string(incoming_message);
        }
        return "";
    }
    void Close()
    {
        // close connection
        close(socket_fd);
    }
};