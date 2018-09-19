#ifndef LAYOUTACCESS_H
#define LAYOUTACCESS_H

#include "Database.h"

#include <type_traits>

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}

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
public:
	LayoutAccess(Database& db);
	bool hasLock(const string& layoutName);
	bool lock(const string& layoutName);
	void release(const string& layoutName);
	void createLayout(const string& name, Layout& layout);
	void migrateLayout(const string& name, const Layout& layout);
	optional<unique_ptr<Layout>> getLayout(const string& name);
	optional<unique_ptr<Layout>> getLayout(const string& name, int version);
};

#endif
