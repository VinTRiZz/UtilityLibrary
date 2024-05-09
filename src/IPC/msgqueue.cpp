#include "msgqueue.h"

#ifdef REWRITE_THIS_SHIT

// ************************************************************************* //
// ************************************************************************* //
// ************************************************************************* //
// ******************************** SERVER ********************************* //
// ************************************************************************* //
// ************************************************************************* //
// ************************************************************************* //

MSG_Q::Server::Server()
{
    srand(time(NULL));

    m_serv_attr.mq_flags = 0;
    m_serv_attr.mq_maxmsg = MAX_MESSAGE_COUNT;
    m_serv_attr.mq_msgsize = MAX_MESSAGE_SIZE;
    m_serv_attr.mq_curmsgs = 0;
}

MSG_Q::Server::Server(std::string path_to_exe)
{
    srand(time(NULL));

    m_serv_attr.mq_flags = 0;
    m_serv_attr.mq_maxmsg = MAX_MESSAGE_COUNT;
    m_serv_attr.mq_msgsize = MAX_MESSAGE_SIZE;
    m_serv_attr.mq_curmsgs = 0;

    m_path_to_server = path_to_exe;
}

MSG_Q::Server::~Server()
{

}

void MSG_Q::Server::write_log(std::string what)
{
    mx.lock();
    std::cout << "[INFO] " << what << std::endl;
    mx.unlock();
}
void MSG_Q::Server::write_err(std::string what)
{
    mx.lock();
    std::cout << "[--ERROR--] " << what << std::endl;
    mx.unlock();
}

int MSG_Q::Server::set_server_addr(std::string new_addr)
{
    if (m_is_running)
    {
        write_err("Changing server name error");
        return -1;
    }

    m_server_addr = new_addr;
    write_log(std::string("Changing server name success. New name: ") +  m_server_addr);
    return 0;
}

void MSG_Q::Server::start()
{
    std::string buf;

    mq_unlink(m_server_addr.c_str());

    m_server_queue = mq_open(m_server_addr.c_str(), O_CREAT | O_RDONLY, 0644, &m_serv_attr);

    write_log("Server started");

    m_is_running = true;

    nlohmann::json configs;

    nlohmann::json con_request;

    char read_buf[MAX_MESSAGE_SIZE + 1];
    m_read_buffers.push_back(read_buf);

    while (!m_stop_flag)
    {
        if (!new_con_available(&buf))
        {
            continue;
        }

        con_request = nlohmann::json::parse(buf);
        buf = con_request["connect"];
        con_request.clear();

        write_log( std::string("Pending connection: ") + buf);

        int id = rand();
        while (m_connection_map.find(id) != m_connection_map.end())
        {
            write_err("Connection ID not available, re-generate called");
            id = rand();
        }

        m_connection_map[id] = mq_open(buf.c_str(), O_WRONLY);

        if (m_connection_map[id] < 0)
        {
            write_err(std::string("Error opening client connection with client: ") + buf );
        }

        std::string temp_serv_name = std::string("/server_") + std::to_string(id);

        m_serv_io_map[id] = mq_open(temp_serv_name.c_str(), O_CREAT | O_RDONLY, 0644, &m_serv_attr);

        if (m_serv_io_map[id] < 0)
        {
            write_err("Error opening server io thread");
        }
        else
        {
            m_read_buffers.push_back(read_buf);

            m_serv_io_threads.push_back(
                std::thread(
                    [this, id]()
                    {
                        this->handle_connection(m_serv_io_map[id], m_serv_io_map.size(), id);
                    }
                )
            );
        }

        configs["server_addr"] = temp_serv_name;

        send(m_connection_map[id], configs.dump());
    }

    write_log("Joining threads...");
    std::for_each(m_serv_io_threads.begin(), m_serv_io_threads.end(),
    [](std::thread & th_it)
    {
        th_it.join();
    }
    );

    write_log("Connections closing...");
    std::for_each(m_serv_io_map.begin(), m_serv_io_map.end(),
    [](std::pair<int, mqd_t> serv_it)
    {
        mq_close(serv_it.second);
    }
    );

    write_log("Closing connection server...");
    mq_close(m_server_queue);

    m_is_running = false;
}

void MSG_Q::Server::add_handler(std::string command_text, handler_callback func )
{
    if (m_callback_map.find(command_text) == m_callback_map.end())
    {
        m_callback_map[command_text] = func;
    }
}

void MSG_Q::Server::remove_handler(std::string command_text, handler_callback func )
{
    if (m_callback_map.find(command_text) != m_callback_map.end())
    {
        m_callback_map.erase(m_callback_map.find(command_text));
    }
}

void MSG_Q::Server::stop()
{
    write_log("Server stop requested");
    m_stop_flag = true;
}

bool MSG_Q::Server::new_con_available(std::string * con_request)
{
    if ( mq_timedreceive(m_server_queue, m_connection_read_buf, MAX_MESSAGE_SIZE, &m_msg_prior, &m_timeout) > 0 )
    {
        *con_request = m_connection_read_buf;

        mq_unlink(m_server_addr.c_str());
        m_server_queue = mq_open(m_server_addr.c_str(), O_CREAT | O_RDONLY, 0644, &m_serv_attr);

        while ( (con_request->size() > 0) )
        {
            if ( (*con_request)[con_request->size()-1] != '}')
                con_request->pop_back();
            else
                break;
        }

        try {
            nlohmann::json::parse(*con_request);
        } catch(...)
        {
            con_request->clear();
            return 0;
        }
        return 1;
    }

    return 0;
}

int MSG_Q::Server::handle_connection(mqd_t &serv_fd, size_t server_id, int con_id)
{
    std::string msg_buffer;

    std::string request;

    std::string request_data;

    std::string result;

    nlohmann::json parser_json;

    while (!m_stop_flag)
    {
        if (mq_timedreceive(serv_fd, m_read_buffers[server_id], MAX_MESSAGE_SIZE, &m_msg_prior, &m_timeout) > 0)
        {
            mq_unlink( (std::string("/server_") + std::to_string(con_id)).c_str() );
            serv_fd = mq_open(m_server_addr.c_str(), O_CREAT | O_RDONLY, 0644, &m_serv_attr);

            try
            {
                msg_buffer = m_read_buffers[server_id];

                while ( (msg_buffer.size() > 0) )
                {
                    if ( msg_buffer[msg_buffer.size()-1] != '}')
                        msg_buffer.pop_back();
                    else
                        break;
                }

                parser_json = nlohmann::json::parse(msg_buffer);

                try
                {
                    request = parser_json["request"];

                    write_log(std::string("[HANDLE THREAD WITH NUMBER ") + std::to_string(server_id) + "]: Got request: " + request + " with data: " + request_data);

                    request_data = parser_json["request_data"];

                    if (m_callback_map[request] != nullptr)
                    {
                        result = m_callback_map[request](request_data);
                    }
                }
                catch(...)
                {
                    write_err("Bad json");
                    result = "json_error";
                }

                parser_json.clear();

                parser_json["response"] = result;

                result = parser_json.dump();

                send(m_connection_map[con_id], result);

            } catch(std::exception &ex)
            {
                write_err(std::string("[HANDLE THREAD WITH NUMBER ") + std::to_string(server_id) + "]: " + ex.what());
            }
        }
    }
    return 0;
}

int MSG_Q::Server::receive(int id, std::string * what)
{
    if (mq_receive(m_connection_map[id], m_read_buffers[0], MAX_MESSAGE_SIZE, &m_msg_prior) < 0)
    {
        write_err("Error receive message");
        return -1;
    }

    if (what->size() > 0)
        what->clear();

    std::for_each(m_read_buffers[0], &(m_read_buffers[0][MAX_MESSAGE_SIZE]),
        [&what](char s)
        {
            if (s != ' ')
                *what += s;
        }
    );
    return 0;
}

int MSG_Q::Server::send(std::string to, std::string what)
{
    int mq_fd = mq_open(to.c_str(), O_WRONLY);

    if (mq_fd < 0)
    {
        write_err("Send error(bad file descriptor)");
        return -1;
    }

    if (mq_send(mq_fd, what.c_str(), what.size()+1, m_msg_prior) < 0)
    {
        write_log("Error sending message");
    }

    if (mq_close(mq_fd) < 0)
    {
        write_err("Error closing connection");
    }

    return 0;
}

int MSG_Q::Server::send(int to, std::string what)
{
    if (to < 0)
    {
        write_err("Send error(bad file descriptor)");
        return -1;
    }

    if (mq_send(to, what.c_str(), what.size()+1, m_msg_prior) < 0)
    {
        write_err("Error sending message");
    }

    return 0;
}





// ************************************************************************* //
// ************************************************************************* //
// ************************************************************************* //
// ******************************** CLIENT ********************************* //
// ************************************************************************* //
// ************************************************************************* //
// ************************************************************************* //

MSG_Q::Client::Client()
{
    m_client_attr.mq_flags = 0;
    m_client_attr.mq_maxmsg = MAX_MESSAGE_COUNT;
    m_client_attr.mq_msgsize = MAX_MESSAGE_SIZE;
    m_client_attr.mq_curmsgs = 0;

    m_client_queue = mq_open(m_client_addr.c_str(), O_CREAT | O_RDWR, 0644, &m_client_attr);

    if (m_client_queue < 0)
    {
        write_err("Error initialising client");
    }
}

MSG_Q::Client::Client(std::string path_to_exe)
{
    m_client_attr.mq_flags = 0;
    m_client_attr.mq_maxmsg = MAX_MESSAGE_COUNT;
    m_client_attr.mq_msgsize = MAX_MESSAGE_SIZE;
    m_client_attr.mq_curmsgs = 0;

    m_client_queue = mq_open(m_client_addr.c_str(), O_CREAT | O_RDWR, 0644, &m_client_attr);

    if (m_client_queue < 0)
    {
        write_err("Error initialising client");
    }

    m_path_to_client = path_to_exe;
}

MSG_Q::Client::~Client()
{
    mq_unlink(m_client_addr.c_str());
    mq_close(m_client_queue);

}

void MSG_Q::Client::write_log(std::string what)
{
    std::cout << "[INFO] " << what << std::endl;
}
void MSG_Q::Client::write_err(std::string what)
{
    std::cout << "[--ERROR--] " << what << std::endl;
}

int MSG_Q::Client::set_client_addr(std::string new_addr)
{
    if ( m_is_running )
    {
        write_err("Client address set error");
        return -1;
    }

    m_client_addr = new_addr;

    mq_unlink(m_client_addr.c_str());
    mq_close(m_client_queue);

    m_client_queue = mq_open(m_client_addr.c_str(), O_CREAT | O_RDWR, 0644, &m_client_attr);

    if (m_client_queue < 0)
    {
        write_err("Error setting new client address");
    }
    else
        write_log("Client address set to: " + m_client_addr);

    return 0;
}

int MSG_Q::Client::connect(std::string serv_addr)
{
    write_log("Connection requested");

    serv_addr += '\0';

    nlohmann::json response_json;

    std::string response_str;

    write_log("Sending request...");

    // Send addr to receive addr for continuous i/o

    nlohmann::json to_send;
    to_send["connect"] = m_client_addr;
    response_str = to_send.dump();

    int mq_fd = mq_open(serv_addr.c_str(), O_WRONLY);

    if (mq_fd < 0)
    {
        write_err("Send error(bad file descriptor)");
        return -1;
    }

    if (mq_timedsend(mq_fd, response_str.c_str(), response_str.size(), m_msg_prior, &m_timeout) < 0)
    {
        write_err("Send error");
    }

    if (mq_close(mq_fd) < 0)
    {
        write_err("Close error");
    }

    write_log("Connection request sent");


    write_log("Receiving reqsponse...");

    // Get packet with addr for i/o
    if (mq_receive(m_client_queue, m_read_buf, MAX_MESSAGE_SIZE, &m_msg_prior) < 0)
    {
        write_err("Receive error");
    }

    if (response_str.size() > 0)
        response_str.clear();

    response_str = m_read_buf;

    try {
        write_log("Parsing response...");
        response_json = nlohmann::json::parse(response_str);

        write_log("Getting server address...");
        response_str = response_json["server_addr"];
    } catch(...)
    {
        write_err("Bad json got");
        return -1;
    }

    write_log("Opening connection...");
    m_server_queue = mq_open(response_str.c_str(), O_WRONLY, 0644, &m_client_attr);

    if (m_server_queue < 0)
    {
        write_err("Bad server descriptor");
        return -1;
    }

    write_log("Server connected");
    m_is_running = true;
    return 0;
}

void MSG_Q::Client::disconnect()
{
    if (send("close", "init") < 0)
        write_err("Error sending close signal");
    else
        write_log("Close signal sent");

    if ( mq_close(m_server_queue) < 0)
        write_err("Error closing connection");
    else
        write_log("Connection closed");

    m_is_running = false;
}

std::string MSG_Q::Client::get_message()
{
    std::string buf;

    if (receive(&buf) < 0)
    {
        write_log("No message received");
        buf = "";
    }

    return buf;
}

int MSG_Q::Client::receive(std::string * what)
{
    if (mq_timedreceive(m_client_queue, m_read_buf, MAX_MESSAGE_SIZE, &m_msg_prior, &m_timeout) < 0)
    {
        write_err("Receive error");
        return -1;
    }

    if (what->size() > 0)
        what->clear();

    *what = m_read_buf;

    return 0;
}

int MSG_Q::Client::send(std::string request, std::string request_data)
{
    nlohmann::json to_send;
    to_send["request"] = request;

    to_send["request_data"] = request_data;

    request = to_send.dump();

    if (m_server_queue < 0)
    {
        write_err("Send error(bad file descriptor)");
        return -1;
    }

    if (mq_send(m_server_queue, request.c_str(), request.size(), m_msg_prior) < 0)
    {
        write_err("Sending error");
        return -1;
    }

    return 0;
}
#endif // REWRITE_THIS_SHIT
