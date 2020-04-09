
#include "Connect.h"

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
