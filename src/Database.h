#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice_transform.h"

using namespace std;
using namespace rocksdb;

class Database {
	bool closed;
	const string& _path;
	DB * db;
	Options options;
	vector<ColumnFamilyDescriptor> cfd;
	vector<ColumnFamilyHandle*> handles;
	ColumnFamilyHandle * getHandle(const string& keyspace);
	void init();
public:
	Database(const string& path);
	~Database();
	void open(void);
	void openReadByPrefix(const string& prefix);
	void close(void);
	bool isOpen(void);
	void put(const string& key, const string& val);
	void put(const string& key, const string& val, const string& keyspace);
	void del(const string& key);
	void del(const string& key, const string& keyspace);
	void deleteKeyspace(const string& keyspace);
	optional<unique_ptr<string>> get(const string& key);
	optional<unique_ptr<string>> get(const string& key, const string &keyspace);
};
#endif
