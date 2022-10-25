#include <thread>
#include "server.h"
using namespace std;

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
    thread accept_connections(ServerConnectionHandler, &server);
    while (true)
    {
        pair<int, string> message = server.Receive();
        if (message.first != -1)
        {
            cout << message.first << ": " << message.second << endl;
            vector<ConnectedClient> connected_clients = server.getConnectedClients();
            // Forward message to all clients except the sender
            for (int j = 0; j < connected_clients.size(); j++)
            {
                ConnectedClient connected_client = connected_clients[j];
                if (connected_client.socket_fd != message.first)
                {
                    server.Send(to_string(message.first) + ": " + message.second, connected_client.socket_fd);
                }
            }
        }
    }

    server.Close();
    return 0;
}