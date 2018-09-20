#ifndef KEYACCESS_H
#define KEYACCESS_H

#include <msgpack.hpp>

#include "Database.h"
#include "LayoutAccess.h"

struct SlotValue {
	string name;
	int type;
	string value;
	int layoutVersion;

	MSGPACK_DEFINE(name, type, value, layoutVersion);
};	


class KeyAccess {
	Database& _db;
	LayoutAccess& _la;	
public:
	KeyAccess(Database& db, LayoutAccess& la);
	int put(const string& name, vector<SlotValue> values, const string& lname);
};

#endif
