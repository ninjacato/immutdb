#ifndef LAYOUTACCESS_H
#define LAYOUTACCESS_H

#include "Database.h"

#include <type_traits>

enum SlotType {
	INT = 0,
	FLOAT,
	DOUBLE,
	STRING
};

struct Slot {
	string name;
	SlotType type;
};

struct Layout {
	int version;
	vector<Slot> slots;
};

class LayoutAccess {
	Database& _db;
	bool _hasLock;
	unordered_map<string, bool> _locks;
	void _createLayout(const string& name, Layout& layout, int version);
public:
	LayoutAccess(Database& db);
	bool hasLock(const string& layoutName);
	bool lock(const string& layoutName);
	void release(const string& layoutName);
	void createLayout(const string& name, Layout& layout);
	void migrateLayout(const string& name, Layout& layout);
	optional<unique_ptr<Layout>> getLayout(const string& name);
	optional<unique_ptr<Layout>> getLayout(const string& name, int version);
};

#endif
