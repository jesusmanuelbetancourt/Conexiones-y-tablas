#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <iomanip>
using namespace std;

// Función para mostrar errores de ODBC
void show_error(unsigned int handleType, const SQLHANDLE& handle) {
    SQLWCHAR SQLState[1024];
    SQLWCHAR message[1024];
    if (SQL_SUCCESS == SQLGetDiagRec(handleType, handle, 1, SQLState, NULL, message, 1024, NULL)) {
        wcout << L"SQL Error: " << message << L", SQLSTATE: " << SQLState << endl;
    }
}

int main() {
    SQLHENV hEnv;
    SQLHDBC hDbc;
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // Asignar un gestor de entorno
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

    // Asignar un gestor de conexión
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

    // Conectarse a la base de datos
    ret = SQLConnect(hDbc, (SQLWCHAR*)L"sqlserver", SQL_NTS, (SQLWCHAR*)L"Username", SQL_NTS, (SQLWCHAR*)L"Password", SQL_NTS);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        wcout << L"Conectado a la base de datos exitosamente." << endl;

        // Asignar el gestor de declaración
        ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

        // Ejecutar la consulta SQL
        ret = SQLExecDirect(hStmt, (SQLWCHAR*)L"SELECT * FROM Empleados", SQL_NTS);
        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            // Imprimir encabezados de la tabla
            wcout << L"+-------------------+------------------------------+-------------------+-------------------+-----------------+---------------------------+--------------------------------------------------------------+-------------------+" << endl;
            wcout << L"| " << left << setw(17) << L"NumeroEmpleado"
                << L" | " << left << setw(28) << L"Nombre Completo"
                << L" | " << left << setw(17) << L"FechaNac"
                << L" | " << left << setw(17) << L"RFC"
                << L" | " << left << setw(15) << L"CentroTrabajo"
                << L" | " << left << setw(25) << L"Puesto"
                << L" | " << left << setw(60) << L"DescripcionPuesto"
                << L" | " << left << setw(17) << L"Directivo" << L" |" << endl;
            wcout << L"+-------------------+------------------------------+-------------------+-------------------+-----------------+---------------------------+--------------------------------------------------------------+-------------------+" << endl;

            int num_empleado, directivo;
            SQLWCHAR nombre[50], apellidoPat[50], apellidoMat[50], fechaNac[50], rfc[50], centroTrabajo[50], puesto[100], descripcionPuesto[150];
            wchar_t nombreCompleto[150];

            while (SQLFetch(hStmt) == SQL_SUCCESS) {
                SQLGetData(hStmt, 1, SQL_C_LONG, &num_empleado, 0, NULL);
                SQLGetData(hStmt, 2, SQL_C_WCHAR, nombre, sizeof(nombre), NULL);
                SQLGetData(hStmt, 3, SQL_C_WCHAR, apellidoPat, sizeof(apellidoPat), NULL);
                SQLGetData(hStmt, 4, SQL_C_WCHAR, apellidoMat, sizeof(apellidoMat), NULL);
                SQLGetData(hStmt, 5, SQL_C_WCHAR, fechaNac, sizeof(fechaNac), NULL);
                SQLGetData(hStmt, 6, SQL_C_WCHAR, rfc, sizeof(rfc), NULL);
                SQLGetData(hStmt, 7, SQL_C_WCHAR, centroTrabajo, sizeof(centroTrabajo), NULL);
                SQLGetData(hStmt, 8, SQL_C_WCHAR, puesto, sizeof(puesto), NULL);
                SQLGetData(hStmt, 9, SQL_C_WCHAR, descripcionPuesto, sizeof(descripcionPuesto), NULL);
                SQLGetData(hStmt, 10, SQL_C_LONG, &directivo, 0, NULL);

                // Fusionar los campos del nombre completo
                wsprintf(nombreCompleto, L"%s %s %s", nombre, apellidoPat, apellidoMat);

                // Convertir el valor de directivo 
                wchar_t directivoStr[4];
                wsprintf(directivoStr, L"%s", (directivo == 1) ? L"Si" : L"No");

                // Imprimir cada fila de la tabla
                wcout << L"| " << left << setw(17) << num_empleado
                    << L" | " << left << setw(28) << nombreCompleto
                    << L" | " << left << setw(17) << fechaNac
                    << L" | " << left << setw(17) << rfc
                    << L" | " << left << setw(15) << centroTrabajo
                    << L" | " << left << setw(25) << puesto
                    << L" | " << left << setw(60) << descripcionPuesto
                    << L" | " << left << setw(17) << directivoStr << L" |" << endl;
            }
            wcout << L"+-------------------+------------------------------+-------------------+-------------------+-----------------+---------------------------+--------------------------------------------------------------+-------------------+" << endl;
        }
        else {
            wcout << L"Error al ejecutar la consulta SQL" << endl;
            show_error(SQL_HANDLE_STMT, hStmt); // Mostrar error detallado
        }

        // Liberar el manejador de la declaración
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }
    else {
        wcout << L"Fallo la conexion a la base de datos" << endl;
        show_error(SQL_HANDLE_DBC, hDbc); // Mostrar error detallado
    }

    // Desconectar y liberar gestores de entorno
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    return 0;
}




