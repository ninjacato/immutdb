#ifndef SCHEMA_H
#define SCHEMA_H

#include "Database.h"

class Schema {
	Database& _db;
	const string& _ks;
	bool _hasLock;
public:
	Schema(Database& db, const string& keyspace);
	bool hasLock(void);
	bool lock(void);
};

#endif
