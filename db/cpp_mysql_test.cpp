
/* Standard C++ includes */
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

typedef std::string str;

const str SQL_PATH = "sql/";
const str SQL_CREATE_DB = SQL_PATH + "Create.sql";
const str SQL_DROP_DB = SQL_PATH + "DropDb.sql";

const str DB_NAME = "what";
const str STMT_CREATE_DB = "CREATE DATABASE IF NOT EXISTS `" + DB_NAME + "`;";
const str STMT_USE_DB = "USE `" + DB_NAME + "`;";

const str DB_HOST_NAME = "localhost";
const str DB_HOST_ADDRESS = "127.0.0.1";
const str DB_USER = "root";
const str DB_PASSWORD = "asa1ase3";
const str PROTOCOL = "tcp";
const str PORT = "3306";
const str SOCKET = PROTOCOL + "://" + DB_HOST_ADDRESS + ":" + PORT;

typedef std::vector<str> str_list;

void OutMessage(std::ostream& out, const sql::SQLException& e, const str& file, const str& function, int line) {
    out << "# ERR: SQLException in " << file;
    out << "(" << function << ") on line " << line << std::endl;
    out << "# ERR: " << e.what();
    out << " (MySQL error code: " << e.getErrorCode();
    out << ", SQLState: " << e.getSQLState() << " )" << std::endl;
}

#define OUT_MESSAGE(out, e) OutMessage(out, e, __FILE__, __FUNCTION__, __LINE__)

str_list GetSqlStatementsFromFile(const str& sqlFilePath) {
    str_list list;
    std::ifstream infile(sqlFilePath);
    str temp;
    std::regex pattern("^\\s*(--|$)");
    getline(infile, temp, ';');

    while (infile) {
        if (!std::regex_match(temp, pattern)) {
            temp += ";";
            list.push_back(temp.c_str());
        }

        getline(infile, temp, ';');
    }

    infile.close();
    return std::move(list);
}

class Statement {
private:
    sql::Statement* _payload;

public:
    Statement(sql::Connection* cnx) :
        _payload(cnx->createStatement()) {}

    Statement(const Statement&) = default;

    virtual ~Statement() {
        _payload->close();
        delete _payload;
    }

    Statement& operator=(const Statement&) = default;

    bool execute(const str& command) {
        /* Test */
        // std::cout << command << std::endl;
        return _payload->execute(command.c_str());
    }

    sql::ResultSet* executeQuery(const str& command) {
        /* Test */
        // std::cout << command << std::endl;
        return _payload->executeQuery(command.c_str());
    }

    int executeUpdate(const str& command) {
        /* Test */
        // std::cout << command << std::endl;
        return _payload->executeUpdate(command.c_str());
    }
};

class Connection {
private:
    class __InnerStruct {
    private:
        str _host;
        str _user;
        str _password;
        sql::Driver* _driver;

    protected:
        sql::Connection* _cnx;

    public:
        __InnerStruct(const str& host, const str& user, const str& password) {
            _host = host;
            _user = user;
            _password = password;

            _driver = get_driver_instance();
            _cnx = _driver->connect(_host.c_str(), _user.c_str(), _password.c_str());

            Statement smt(_cnx);
            smt.execute(STMT_CREATE_DB.c_str());
            smt.execute(STMT_USE_DB.c_str());
        }

        virtual ~__InnerStruct() {
            _cnx->commit();
            _cnx->close();
            delete _cnx;
        }

        str host() const {
            return _host;
        }

        str user() const {
            return _user;
        }

        str password() const {
            return _password;
        }

        sql::Connection* cnx() const {
            return _cnx;
        }
    };

    static std::shared_ptr<__InnerStruct> _instance;
public:
    Connection(const str& host, const str& user, const str& password) {
        if (_instance.use_count() == 0) {
            _instance = std::make_shared<__InnerStruct>(host, user, password);
        }
    }

    str host() const {
        return _instance->host();
    }

    str user() const {
        return _instance->user();
    }

    str password() const {
        return _instance->password();
    }

    Statement new_statement() const {
        return Statement(_instance->cnx());
    }

    void execute(const str& command) const {
        Statement smt = new_statement();
        smt.execute(command.c_str());
    }

    void try_execute(const str& command) const {
        Statement smt = new_statement();

        try {
            smt.execute(command.c_str());
        }
        catch (sql::SQLException& e) {
            OUT_MESSAGE(std::cout, e);
        }
    }

    sql::ResultSet* query(const str& command) const {
        Statement smt = new_statement();
        auto result = smt.executeQuery(command.c_str());
        return result;
    }

    sql::ResultSet* try_query(const str& command) const {
        Statement smt = new_statement();
        sql::ResultSet* result = NULL;

        try {
            result = smt.executeQuery(command.c_str());
        }
        catch (sql::SQLException& e) {
            OUT_MESSAGE(std::cout, e);
        }

        return result;
    }

    void try_run_script(const str& path) const {
        Statement smt = new_statement();

        try {
            for (auto line : GetSqlStatementsFromFile(path)) {
                smt.execute(line.c_str());
            }
        }
        catch (sql::SQLException& e) {
            OUT_MESSAGE(std::cout, e);
        }
    }
};

std::shared_ptr<Connection::__InnerStruct> Connection::_instance;

class Statements {
public:
    static str get_id_query(const str& table_name, const str& user) {
        return "SELECT `ID` FROM `" + table_name + "` WHERE `Name` = " + user + ";";
    }

    static str get_insert(const str& table_name, const str& user, const str& password) {
        return "INSERT INTO `Progress` (`AccountID`, `Name`) VALUES ('{}', '{}');";
    }

    static Connection get_connection() {
        return Connection(SOCKET, DB_USER, DB_PASSWORD);
    }

    static void create_db(const Connection& cnx) {
        cnx.try_execute(SQL_CREATE_DB);
    }

    static void drop_db(const Connection& cnx) {
        cnx.try_execute(SQL_DROP_DB);
    }

    static int try_new(const Connection& cnx, const str& id_query, const str& insert) {
        try {
            if (cnx.query(id_query)->next()) {
                return -1;
            }

            int id = 0;
            cnx.execute(insert);
            auto result = cnx.query(id_query);

            if (result->next()) {
                id = result->getInt(1);
                result->close();
            }

            delete result;
            return id;
        }
        catch (sql::SQLException& e) {
            OUT_MESSAGE(std::cout, e);
            return -2;
        }
    }

    static int try_new_account(const Connection& cnx, const str& user, const str& password) {
        str id_query = "SELECT `ID` FROM `Account` WHERE `Name` = '" + user + "';";
        str insert = "INSERT INTO `Account` (`Name`, `Password`) VALUES ('" + user + "', '" + password + "');";
        return Statements::try_new(cnx, id_query, insert);
    }

    static int try_new_progress(const Connection& cnx, int account_id, const str& name) {
        str id_str = std::to_string(account_id);
        str id_query = "SELECT `ID` FROM `Progress` WHERE `AccountID` = '" + id_str + "' AND `Name` = '" + name + "';";
        str insert = "INSERT INTO `Progress` (`AccountID`, `Name`) VALUES ('" + id_str + "', '" + name + "');";
        return Statements::try_new(cnx, id_query, insert);
    }
};

class Record {
protected:
    str _table;
    int _id;
    Connection _cnx;

    Record() : _cnx(Statements::get_connection()), _id(0) {}

public:
    Record(const str& table, int id) :
        _cnx(Statements::get_connection()),
        _table(table), _id(id) {}

    str_list read(const str& column) {
        str_list list;
        sql::ResultSet* result = NULL;
        result = _cnx.try_query("SELECT `" + column + "` FROM `" + table() + "` WHERE ID = '" + std::to_string(_id) + "';");

        if (result) {
            while (result->next()) {
                list.push_back(result->getString(1));
            }

            result->close();
        }

        delete result;
        return list;
    }

    template <typename T>
    void write(const str& column, const T& field) {
        _cnx.try_execute("UPDATE `" + table() + "` SET `" + column + "` = '" + std::to_string(field) + "' WHERE `ID` = '" + std::to_string(_id) + "';");
    }

    int id() {
        return _id;
    }

    str table() {
        return _table;
    }
};

class Progress : public Record {
private:
    str _name;

public:
    Progress(int account_id, const str& name) {
        _cnx = Statements::get_connection();
        auto rows = _cnx.try_query("SELECT `ID` FROM `Progress` WHERE `AccountID` = '" + std::to_string(account_id) + "' AND `Name` = '" + name + "';");

        if (rows->next()) {
            _table = "Progress";
            _id = rows->getInt(1);
            _name = name;
            rows->close();
        }

        delete rows;
    }

    static Progress try_new(int account_id, const str& name) {
        auto cnx = Statements::get_connection();
        auto rows = cnx.try_query("SELECT `ID` FROM `Progress` WHERE `AccountID` = '" + std::to_string(account_id) + "' AND `Name` = '" + name + "';");

        if (!rows->next()) {
            Statements::try_new_progress(cnx, account_id, name);
            rows->close();
        }

        delete rows;
        return Progress(account_id, name);
    }

    str name() const {
        return _name;
    }
};

class Account : public Record {
private:
    str _user;

public:
    Account(const str& user, const str& password) {
        _cnx = Statements::get_connection();
        auto rows = _cnx.try_query("SELECT `ID` FROM `Account` WHERE `Name` = '" + user + "' AND `Password` = '" + password + "';");

        if (rows->next()) {
            _table = "Account";
            _id = rows->getInt(1);
            _user = user;
            rows->close();
        }

        delete rows;
    }

    static Account try_new(const str& user, const str& password) {
        auto cnx = Statements::get_connection();
        auto rows = cnx.try_query("SELECT `ID` FROM `Account` WHERE `Name` = '" + user + "' AND `Password` = '" + password + "';");

        if (!rows->next()) {
            Statements::try_new_account(cnx, user, password);
            rows->close();
        }

        delete rows;
        return Account(user, password);
    }

    str user() const {
        return _user;
    }

    std::vector<Progress> progresses() {
        std::vector<Progress> list;
        str query = "SELECT `Name` FROM `Progress` WHERE `AccountID` = '" + std::to_string(id()) + "';";
        auto rows = _cnx.try_query(query);

        if (rows->next()) {
            list.push_back(Progress(id(), rows->getString(1).c_str()));

            while (rows->next()) {
                list.push_back(Progress(id(), rows->getString(1).c_str()));
            }

            rows->close();
        }

        delete rows;
        return list;
    }

    Progress progress(const str& name) {
        return Progress(id(), name);
    }

    Progress new_progress(const str& name) {
        return Progress::try_new(id(), name);
    }
};

int main() {

    // Get connection to database
    Connection cnx(SOCKET, DB_USER, DB_PASSWORD);

    // Run DropDb.sql
    cnx.try_run_script(SQL_DROP_DB);

    // Run Create.sql
    cnx.try_run_script(SQL_CREATE_DB);

    // Try to create a new Account with username and password
    Account::try_new("hornduber", "the");

    // Log in to account
    auto acc = Account("hornduber", "the");

    // Create a new character
    auto prg = acc.new_progress("That Guy");

    // Update character's score
    prg.write("Score", 2000);

    return EXIT_SUCCESS;
}
