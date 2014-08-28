/*\
 * test-sql.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <libpq-fe.h>
#include <string.h> // for strcpy(), ...
#include "sprtf.hxx"
#include "test-sql.hxx"

static const char *module = "test-sql";

#ifdef ADD_POSTGRESQL_TEST

#ifndef DEF_PORT
#define DEF_PORT "5432"
#endif

#ifndef DEF_DATABASE
#define DEF_DATABASE "fgtracker"
#endif

#ifndef DEF_USER_LOGIN
#define DEF_USER_LOGIN "fgtracker"
#endif

#ifndef DEF_USER_PWD
#define DEF_USER_PWD "fgtracker"
#endif

#ifndef DEF_IP_ADDRESS
#define DEF_IP_ADDRESS "127.0.0.1"
#endif


static const char *port = DEF_PORT;
static const char *database = DEF_DATABASE;
static const char *user = DEF_USER_LOGIN;
static const char *pwd = DEF_USER_PWD;
static const char *ip_address = DEF_IP_ADDRESS;
static const char *pgoptions = "";
static const char *pgtty = "";

#ifndef MAXLINE
#define MAXLINE 1024
#endif

// implementation
#define PQ_EXEC_SUCCESS(res) ((PQresultStatus(res) == PGRES_COMMAND_OK)||(PQresultStatus(res) == PGRES_TUPLES_OK))
int check_tables(PGconn *conn)
{
    PGresult *res;
    static char buff[MAXLINE];
    char *cp = buff;
    char *val;
    int i, j, nFields, nRows;
    int got_flights = 0;
    int got_waypts = 0;
    if (PQstatus(conn) == CONNECTION_OK) {
        res = PQexec(conn,"BEGIN");
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            PQclear(res);
            return 1;
        }
        /* should PQclear PGresult whenever it is no longer needed to avoid memory leaks */
        PQclear(res);
        strcpy(cp,"SELECT table_name FROM information_schema.tables WHERE table_schema = 'public';");
        res = PQexec(conn, cp);
        if (PQ_EXEC_SUCCESS(res)) {
            nFields = PQnfields(res);
            nRows = PQntuples(res);
            for (j = 0; j < nFields; j++) {
                for (i = 0; i < nRows; i++) {
                    val = PQgetvalue(res, i, j);
                    if (val) {
                        if (strcmp(val,"flights") == 0)
                            got_flights = 1;
                        else if (strcmp(val,"waypoints") == 0)
                            got_waypts = 1;
                    }
                }
            }
       } else {
            return 1;
       }
       PQclear(res);
       /* end the transaction */
       res = PQexec(conn, "END");
       PQclear(res);
    } else {
        return 1;
    }
    return ((got_flights && got_waypts) ? 0 : 1);
}

void test_sql()
{
    SPRTF("\n");
    SPRTF("%s: test sql connection...\n", module );
    PGconn *conn;
    conn = PQsetdbLogin(ip_address, port, pgoptions, pgtty, database, user, pwd);

    if (PQstatus(conn) != CONNECTION_OK) {
        char *err = PQerrorMessage(conn);
        SPRTF("%s: Connection FAILED! - %s\n", module, err );
        goto cleanup;
    }

    SPRTF("%s: Got connection on ip=%s, port=%s, db=%s, usr=%s\n", module,
        ip_address, port, database, user );

    SPRTF("%s: Check for tables 'flights' and 'waypoints' ", module);
    if (check_tables(conn)) {
        SPRTF("FAILED!\n");
    } else {
        SPRTF("succeeded.\n");
    }

cleanup:

    PQfinish(conn);
   
    SPRTF("%s: end test sql connection...\n", module );

}

#endif // ADD_POSTGRESQL_TEST

// eof = test-sql.cxx
