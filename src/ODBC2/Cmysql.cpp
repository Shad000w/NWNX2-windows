/***************************************************************************
    CMySQL.cpp: implementation of the CMySQL class.
    Copyright (C) 2004 Jeroen Broekhuizen (nwnx@jengine.nl)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/

#include "stdafx.h"
#include "mysql.h"
#include <assert.h>



CMySQL::CMySQL() : CDB()
{
	result = NULL;
}

CMySQL::~CMySQL()
{
}

BOOL CMySQL::Connect ()
{
	// try to establish a default connection
	return Connect ("localhost", "nwn", "nwnpwd", "nwn");
}

BOOL CMySQL::Connect (const char *server, const char *user, const char *pass, const char *db)
{
	// initialize the mysql structure
	if (mysql_init (&mysql) == NULL) {
		return FALSE;
	}


	// try to connect to the mysql server
	connection = mysql_real_connect (&mysql, server, user, pass, db, 0, NULL, 0);
	if (connection == NULL) {
		mysql_close (&mysql);
		return FALSE;
    }
	return TRUE;
}

void CMySQL::Disconnect ()
{
	// close the connection
	mysql_close (&mysql);
}

BOOL CMySQL::Execute (const uchar* query)
{
	if (connection == NULL)
		return FALSE;

	// release a previous result set
	if (result != NULL) {
		mysql_free_result (result);
		result = NULL;
	}

	// execute the query
	if (mysql_query (connection, (const char *) query) != 0) {
		return FALSE;
	}

	// store the resultset in local memory
	result = mysql_store_result (&mysql);
	if (result == NULL) {
		if (mysql_field_count (&mysql) != 0) {
			return FALSE;
		}
	}
	else {
		// successfull retreived the results from the SELECT
		NumCol = mysql_num_fields (result);
	}
	return TRUE;
}

uint CMySQL::Fetch (char* buffer, uint size)
{
	uint totalbytes = 0;
	ulong *lengths;
	ulong i, total;
	MYSQL_ROW row;

	if (connection == NULL)
		return -1;

	buffer[0] = '\0';

	// walk through the resultset
	if (result == NULL) return -1;
	row = mysql_fetch_row (result);
	if (row)
	{
		lengths = mysql_fetch_lengths (result);
		// add each column to buffer
		for (i = 0; i < NumCol; i++)
		{
			//performance issue
		    total = totalbytes + lengths[i];
			if ((lengths[i] > 0) && (total < size))	{
				memcpy (&buffer[totalbytes], row[i], lengths[i]);
				totalbytes = total;
	    	}

			// add seperator as long we are not at last column
			if ((i != NumCol - 1) && (totalbytes + 1 < size)) {
				buffer[totalbytes] = '�'; // ascii 170
				totalbytes++;
	    	}
		}
		buffer[totalbytes] = 0;
	}
	return totalbytes;
}

BOOL CMySQL::WriteScorcoData(char* SQL, BYTE* pData, int Length)
{
	int res;
	unsigned long len;
	char* Data = new char[Length * 2 + 1 + 2];
	char* pSQL = new char[MAXSQL + Length * 2 + 1];

	len = mysql_real_escape_string (&mysql, Data + 1, (const char*)pData, Length);
	Data[0] = Data[len + 1] = 39; //'
	Data[len + 2] = 0x0; 
	sprintf(pSQL, SQL, Data);

	MYSQL_RES *result = mysql_store_result (&mysql);
	res = mysql_query(&mysql, (const char *) pSQL);

	mysql_free_result(result);
	delete[] pSQL;
	delete[] Data;

	if (res == 0)
		return TRUE;
	else
		return FALSE;
}

BYTE* CMySQL::ReadScorcoData(char* SQL, char *param, BOOL* pSqlError, int *size)
{
	if (strcmp(param, "FETCHMODE") != 0)
	{	
		if (mysql_query(&mysql, (const char *) SQL) != 0)
		{
			*pSqlError = true;
			return NULL;
		}

		mysql_free_result(result);
		result = mysql_store_result (&mysql);
		if (!result)
		{
			*pSqlError = true;
			return NULL;
		}
	}

	MYSQL_ROW row;
	*pSqlError = false;
    row = mysql_fetch_row(result);
    if (row)
	{
		unsigned long* length = mysql_fetch_lengths(result);
		if (*length > MAXRESULT)
			return NULL;

		memcpy(pReturn, row[0], length[0]);
		*size = length[0];
		return pReturn;	
	}
	else
		return NULL;
}

const char* CMySQL::GetErrorMessage ()
{
	// return the error message
	return mysql_error (&mysql);
}