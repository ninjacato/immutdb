#ifndef DATABASE_H
#define DATABASE_H

#include <string>

using namespace std;

class Database {
	const string& path;
public:
	Database(const string& path);
};
#endif
