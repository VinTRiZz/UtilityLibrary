#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#warning "Need refactor"

#ifdef REWRITE_THIS_SHIT

#define MAX_MESSAGE_SIZE 256

#define MAX_MESSAGE_COUNT 10

#define TIMEOUT 100000

// Debug
#include <iostream>

// Containers
#include <map>
#include <string>

// Message queue
#include <mqueue.h>

// Packet using
#include <nlohmann/json.hpp>

// Encrypt
#include <random>

// Algorithms
#include <string.h>

// For I/O in server
#include <thread>
#include <mutex>

/*
/// Client example usage

    MSG_Q::Client client(argv[0]);

    client.set_client_addr("/test_client");

    client.connect("/test_server");

    client.send("test", "no data");

    this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::string buf;
    buf = client.get_message();

    std::cout << "\nReceived: " << buf << std::endl;

    client.disconnect();

/// ------------------------------------------------------ ///
/// Server example usage

    MSG_Q::Server server(argv[0]);

    server.set_server_addr("/test_server");

    server.add_handler("test", test_function);

    std::thread serv_th(
        [&server]()
        {
            server.start();
        }
    );

    std::string input;
    bool done = false;

    while (!done)
    {
        cin >> input;
        if (input == "exit")
            done = true;
    }

    server.stop();

    serv_th.join();
*/



namespace MSG_Q
{
    typedef std::string (*handler_callback)(std::string);

    class Server
    {
        public:
            Server();

            Server(std::string path_to_exe);

            virtual ~Server();

            int set_server_addr(std::string new_addr);

            void start();

            void add_handler(std::string command_text, handler_callback func );

            void remove_handler(std::string command_text, handler_callback func );

            void stop();

        protected:
            std::string m_path_to_server;

            std::map < int, mqd_t > m_connection_map;

            std::map < std::string, handler_callback > m_callback_map =
            {
                {"bind_function", nullptr}
            };

            std::map < int, mqd_t > m_serv_io_map;

            std::vector<std::thread> m_serv_io_threads;

            std::mutex mx;

            const struct timespec m_timeout = {TIMEOUT / 1000, (TIMEOUT % 1000) * 1000000};

            std::string m_server_addr = "/default_server";
            struct mq_attr m_serv_attr;
            mqd_t m_server_queue;

            std::vector<char*> m_read_buffers;

            char m_connection_read_buf[MAX_MESSAGE_SIZE + 1];

            unsigned int m_msg_prior;

            bool m_stop_flag = false;

            bool m_is_running = false;

            void write_log(std::string what);

            void write_err(std::string what);

            bool new_con_available(std::string * con_request);

            int handle_connection(mqd_t &serv_fd, size_t server_id, int con_id);

            int receive(int id, std::string * what);

            int send(std::string to, std::string what);

            int send(int to, std::string what);
    };



    class Client
    {
        public:
            Client();

            Client(std::string path_to_exe);

            virtual ~Client();

            int set_client_addr(std::string new_addr);

            int connect(std::string serv_addr);

            void disconnect();

            int send(std::string request, std::string request_data);

            std::string get_message();

        protected:
            std::string m_path_to_client;

            std::string m_client_addr = "/default_client";

            struct mq_attr m_client_attr;

            mqd_t m_client_queue, m_server_queue;

            char m_read_buf[MAX_MESSAGE_SIZE + 1];

            unsigned int m_msg_prior;

            const struct timespec m_timeout = {TIMEOUT / 1000, (TIMEOUT % 1000) * 1000000};

            bool m_is_running = false;

            void write_log(std::string what);

            void write_err(std::string what);

            int receive(std::string * what);
    };
}

#endif // REWRITE_THIS_SHIT

#endif // MSGQUEUE_H
