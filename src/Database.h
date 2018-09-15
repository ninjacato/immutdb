#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "rocksdb/db.h"
#include "rocksdb/options.h"

using namespace std;
using namespace rocksdb;

class Database {
	const string& _path;
	DB * _db;
	Options options;
public:
	Database(const string& path);
	void open(void);
	void close(void);
	void put(const string& key, const string& val);
	unique_ptr<string> get(const string& key);
};
#endif
