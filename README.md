# misc

compile sqlite with: ```gcc -c lib/sqlite3/sqlite3.c -o lib/sqlite3/sqlite3.o -lpthread -ldl```
create static lib with: ```ar rcs lib/sqlite3/libsqlite3.a lib/sqlite3/sqlite3.o```

compile db.c with: ```gcc db.c -Llib/sqlite3 -lsqlite3 -o db -lpthread -ldl```

- or -
compile all in one go: ```gcc db.c lib/sqlite3/sqlite3.c -o db -lpthread -ldl``` 