#include "databasehandler.h"

#ifdef SQLITE

#include <sqlite3.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

struct Utility::Network::DataBaseHandler::Impl
{
    bool inited {false};
    bool mustCloseDb {false};
    int m_database_fd = 0; ///< File descriptor for database
    sqlite3* m_database {nullptr}; ///< SQLite database pointer for database
    std::string path;
    std::string errorText;
};

Utility::Network::DataBaseHandler::DataBaseHandler() :
    d {new Impl()}
{
    // Create or open directory for MessageApp_Server::Server databases
    DIR* check_dir = opendir(DATABASE_DIR);

    if (!check_dir)
        return;

    closedir(check_dir);

    d->inited = true;
}

Utility::Network::DataBaseHandler::~DataBaseHandler()
{
    sqlite3_close(m_database);
}

bool Utility::Network::DataBaseHandler::setPath(const std::string &path)
{
    d->path = path;
    d->m_database_fd = sqlite3_open(path.c_str(), &d->m_database);
    if (d->m_database_fd != SQLITE_OK)
    {
        d->errorText = "Error opening database";
        return false;
    }
    return true;
}

bool Utility::Network::DataBaseHandler::requestVoid(const std::string &query)
{
    char * operation_error {nullptr};
    sqlite3_stmt* stmt;

    if (sqlite3_exec(d->m_database, query.c_str(), nullptr, 0, &operation_error) != SQLITE_OK)
    {
        d->errorText = std::string("Error: ") + operation_error;
        sqlite3_free(operation_error); // If in creating exist errors, then table already exist
        return false;
    }
}

bool Utility::Network::DataBaseHandler::requestText(const std::string &query, std::vector<std::string> & resultVect)
{
    char * operation_error {nullptr};
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_database, id_get_command.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        d->errorText = std::string("Error: ") + operation_error;
        sqlite3_free(operation_error); // If in creating exist errors, then table already exist
        return false;
    }

    resultVect.clear();
    char * text_ptr {nullptr};
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        text_ptr = reinterpret_cast<char *>(sqlite3_column_text(stmt, 0));

        if (text_ptr)
            resultVect.push_back( text_ptr );
    }

    return true;
}

bool Utility::Network::DataBaseHandler::requestInt(const std::string &query, std::vector<int> &resultVect)
{
    char * operation_error {nullptr};
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_database, id_get_command.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        d->errorText = std::string("Error: ") + operation_error;
        sqlite3_free(operation_error); // If in creating exist errors, then table already exist
        return false;
    }

    resultVect.clear();
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        resultVect.push_back( sqlite3_column_int(stmt, 0) );
    }

    return true;
}

#endif // SQLITE
