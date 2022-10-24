#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#ifdef __linux__
#include <linux/in.h>
#endif

using namespace std;

#define _MSG_BUFFER_SIZE_ 1024

struct timeval read_timeout;

struct ConnectedClient
{
    int socket_fd;
    struct sockaddr_in client_address;
    int client_address_length;
};

class Server
{
    int socket_fd, port_number;
    char incoming_message[_MSG_BUFFER_SIZE_];
    struct sockaddr_in server_address;
    int incoming_message_length;
    vector<ConnectedClient> connected_clients;

public:
    // ---------------------------------------------------------------------------------------------
    // Summary        : Create new instance of tcp server class (Class constructor)
    //
    // Pre-Condition  : None
    // Post-Condition : None
    //
    // Input :-
    //        > [int] port_number : port of host node i.e [8080]
    //
    // Output:-
    //        > None
    // ---------------------------------------------------------------------------------------------
    Server(int port_number)
    {
        // initialize server address struct
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(port_number);

        // initialize connection socket
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        cout << "Socket created" << endl;

        // bind socket to port
        bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));
        cout << "Server started at port " << port_number << endl;

        // listen for incoming connections
        listen(socket_fd, 5);
        cout << "Listening for incoming connections on port " << port_number << endl;

        // // accept incoming connection
        // ConnectedClient connected_client;
        // connected_client.client_address_length = sizeof(connected_client.client_address);
        // connected_client.socket_fd = accept(socket_fd, (struct sockaddr *)&connected_client.client_address, (socklen_t *)&connected_client.client_address_length);
        // connected_clients.push_back(connected_client);
        // cout << "Connected to client with socket fd " << connected_client.socket_fd << endl;
    }

    // ---------------------------------------------------------------------------------------------
    // Summary : Wait for incoming connections
    // ---------------------------------------------------------------------------------------------

    // ---------------------------------------------------------------------------------------------
    // Summary : Accept incoming connection
    // ---------------------------------------------------------------------------------------------
    int Accept()
    {
        // accept incoming connection
        // int new_client = listen(socket_fd, 5);
        // if (new_client < 0)
        // {
        //     cout << "Error accepting new client" << endl;
        //     return -1;
        // }

        ConnectedClient connected_client;
        connected_client.client_address_length = sizeof(connected_client.client_address);
        connected_client.socket_fd = accept(socket_fd, (struct sockaddr *)&connected_client.client_address, (socklen_t *)&connected_client.client_address_length);
        connected_clients.push_back(connected_client);
        cout << "Connected to client with socket fd " << connected_client.socket_fd << endl;
        return connected_client.socket_fd;
    }

    // ---------------------------------------------------------------------------------------------
    // Summary : Send message to connected client
    // ---------------------------------------------------------------------------------------------
    void Send(string message, int client_socket_fd = -1)
    {
        // send message to client
        if (client_socket_fd == -1)
        {
            for (int i = 0; i < connected_clients.size(); i++)
            {
                ConnectedClient connected_client = connected_clients[i];
                send(connected_client.socket_fd, message.c_str(), message.length(), 0);
            }
        }
        else
        {
            send(client_socket_fd, message.c_str(), message.length(), 0);
        }
    }

    // ---------------------------------------------------------------------------------------------
    // Summary : Receive message from connected client
    // ---------------------------------------------------------------------------------------------
    string Receive()
    {
        // recieve incoming message from client
        for (int i = 0; i < connected_clients.size(); i++)
        {

            ConnectedClient connected_client = connected_clients[i];
            read_timeout.tv_sec = 0;
            read_timeout.tv_usec = 10;
            setsockopt(connected_client.socket_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
            incoming_message_length = recvfrom(connected_client.socket_fd, incoming_message, _MSG_BUFFER_SIZE_, 0, NULL, NULL);

            // check if message is not empty
            if (incoming_message_length > 0)
            {
                // put trailing character
                incoming_message[incoming_message_length] = '\0';

                // print the message [for testing purposes]
                cout << connected_client.socket_fd << ": " << incoming_message << std::endl;
                // send to all clients except the sender
                for (int j = 0; j < connected_clients.size(); j++)
                {
                    if (j != i)
                    {
                        ConnectedClient connected_client = connected_clients[j];
                        send(connected_client.socket_fd, incoming_message, incoming_message_length, 0);
                    }
                }
            }
        }
        return "";
    }

    // ---------------------------------------------------------------------------------------------
    // Summary : Close connection with connected client
    // ---------------------------------------------------------------------------------------------
    void Close(int socket_fd = -1)
    {
        // close connection with client
        for (int i = 0; i < connected_clients.size(); i++)
        {
            ConnectedClient connected_client = connected_clients[i];
            if (socket_fd == -1 || connected_client.socket_fd == socket_fd)
            {
                connected_clients.erase(connected_clients.begin() + i);
                close(socket_fd);
                cout << "Connection closed with client with socket fd " << socket_fd << endl;
                break;
            }
        }
    }
};

void ServerConnectionHandler(Server *server)
{
    while (true)
    {
        server->Accept();
    }
}

int main()
{
    Server server(8080);
    int itr = 0;
    int interval = 1000;
    thread thread_obj(ServerConnectionHandler, &server);
    while (true)
    {

        string message = server.Receive();
        if (message != "")
        {
            cout << "Message: " << message << endl;
        }
    }

    server.Close();
    return 0;
}