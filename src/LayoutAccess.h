#ifndef LAYOUTACCESS_H
#define LAYOUTACCESS_H

#include "Database.h"

class LayoutAccess {
	Database& _db;
	const string& _ks;
	bool _hasLock;
public:
	LayoutAccess(Database& db, const string& keyspace);
	bool hasLock(void);
	bool lock(void);
	void release(void);
};

#endif
