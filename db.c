// core api docs: https://www.sqlite.org/cintro.html
// full api docs: https://www.sqlite.org/c3ref/intro.html
// note: sqlite api often has different versions of the same routine. bewware of incorrect version footguns


#include <stdio.h>

#include "sqlite3.h"

#define STMT_CREATE_TRIPLES_TABLE \
    "CREATE TABLE IF NOT EXISTS triples (" \
    "   id INTEGER PRIMARY KEY AUTOINCREMENT," \
    "   subject TEXT NOT NULL," \
    "   predicate TEXT NOT NULL," \
    "   object TEXT NOT NULL," \
    "   UNIQUE(subject, predicate, object)" \
    ");"

#define STMT_CREATE_TRIPLES_INDEXES \
    "CREATE INDEX IF NOT EXISTS idx_triples_subject ON triples(subject);" \
    "CREATE INDEX IF NOT EXISTS idx_triples_predicate ON triples(predicate);" \
    "CREATE INDEX IF NOT EXISTS idx_triples_object ON triples(object);"


int main(void) {
    sqlite3*    db;
    char*       err_msg = 0;

    //
    if (sqlite3_open("db.sqlite3", &db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    //
    if (sqlite3_exec(db, STMT_CREATE_TRIPLES_TABLE, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s", err_msg);
        sqlite3_free(err_msg);
    }

    //
    if (sqlite3_exec(db, STMT_CREATE_TRIPLES_INDEXES, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s", err_msg);
        sqlite3_free(err_msg);
    }

    // todo: if db open
    sqlite3_close(db);

    return 0;
}