#ifndef LAYOUTACCESS_H
#define LAYOUTACCESS_H

#include "Database.h"

class LayoutAccess {
	Database& _db;
	bool _hasLock;
	unordered_map<string, bool> _locks;
public:
	LayoutAccess(Database& db);
	bool hasLock(const string& layoutName);
	bool lock(const string& layoutName);
	void release(const string& layoutName);
};

#endif
