#include <thread>
#include "./client.h"

using namespace std;

void ReceiveMessageHandler(Client *client)
{
    while (true)
    {
        string message = client->Receive();
        if (message.length() > 0)
        {
            for (int i = 0; i < message.length() + 4; i++)
            {
                cout << "-";
            }
            cout << endl;
            cout << "| " << message << " |";
            cout << endl;
            for (int i = 0; i < message.length() + 4; i++)
            {
                cout << "-";
            }
            cout << endl;
        }
    }
}

int main()
{

    string ip = "127.0.0.1";
    Client client(ip, 8080);
    client.Connect();
    thread message_receiver_thread(ReceiveMessageHandler, &client);
    while (true)
    {
        string message;
        getline(cin, message);

        cout << "\rMe: " << message << endl;
        client.Send(message);
        // cout << "Message sent: " << message << endl;
    }

    client.Close();
    return 0;
}