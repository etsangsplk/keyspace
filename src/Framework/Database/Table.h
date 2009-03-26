#ifndef TABLE_H
#define TABLE_H

#include "Database.h"
#include "Cursor.h"
#include "System/Buffer.h"

class TableVisitor
{
public:
	virtual void Accept(const ByteString &key, const ByteString &value) = 0;
};

class Table
{
	friend class Transaction;
public:
	Table(Database* database, const char *name);
	~Table();
	
	bool		Iterate(Cursor& cursor);
	
	bool		Get(Transaction* transaction, const ByteString &key, ByteString &value);
	bool		Get(Transaction* transaction, const char* key, ByteString &value);
	
	bool		Set(Transaction* transaction, const ByteString &key, const ByteString &value);
	bool		Set(Transaction* transaction, const char* key, const ByteString &value);
	bool		Set(Transaction* transaction, const char* key, const char* value);
	
	bool		Drop();
	
	bool		Visit(TableVisitor &tv);
private:
	Database*	database;
	Db*			db;
};


#endif
