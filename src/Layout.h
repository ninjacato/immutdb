#ifndef LAYOUT_H
#define LAYOUT_H

#include "Database.h"

class Layout {
	Database& _db;
	const string& _ks;
	bool _hasLock;
public:
	Layout(Database& db, const string& keyspace);
	bool hasLock(void);
	bool lock(void);
	void release(void);
};

#endif
