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

int max_number = 1000000;
vector<bool> is_prime(max_number, true);
void generatePrimes()
{
    is_prime[0] = is_prime[1] = false;
    for (int i = 2; i <= max_number; i++)
    {
        if (is_prime[i] && (long long)i * i <= max_number)
        {
            for (int j = i * i; j <= max_number; j += i)
                is_prime[j] = false;
        }
    }
    cout << "Prime Sieve Generated!\n";
}

int main()
{
    generatePrimes();
    Server server(8080);
    int itr = 0;
    int interval = 1000;
    thread accept_connections(ServerConnectionHandler, &server);
    while (true)
    {
        pair<int, string> message = server.Receive();
        if (message.first != -1)
        {
            int received_number;
            try
            {
                received_number = stoi(message.second);
            }
            catch (const std::exception &e)
            {
                cout << "INVALID NUMBER RECEIVED!";
                server.Send("SERVER: INVALID NUMBER RECEIVED!", message.first);
                continue;
            }

            cout << message.first << ": " << message.second << endl;
            cout << "Parsed number: " << received_number << endl;
            vector<ConnectedClient> connected_clients = server.getConnectedClients();

            if (received_number < max_number and is_prime[received_number])
            {
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
            else
            {
                server.Send("SERVER: Non Prime number received!", message.first);
            }
        }
    }

    server.Close();
    return 0;
}