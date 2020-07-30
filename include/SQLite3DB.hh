#include <string_view>
#include <string>
#include <filesystem>
#include <vector>
#include <utility>
#include <type_traits>

#include <sqlite3.h>

#ifndef INCLUDE_SHIMIYUU_SQLITE3DB_HH_
#define INCLUDE_SHIMIYUU_SQLITE3DB_HH_

namespace shimiyuu {

class SQLite3DB {

public:
	SQLite3DB(const std::filesystem::path& database_file);

	enum class DataType {
		TEXT, INT, REAL
	};
	enum class KeyType {
		NONE, PRIMARY, FOREIGN
	};

	class Column {
	public:
		const std::string name;
		const DataType data_type;
		const bool allow_null;
		const KeyType key_type;
		const std::string foreign_key_table, foreign_key_name;

		Column(const std::string& name, DataType data_type = DataType::TEXT, bool allow_null = false,
				KeyType key_type = KeyType::NONE,
				const std::string& foreign_key_table = "", const std::string& foreign_key_name = "");

	private:
		operator std::string() const;
		friend class SQLite3DB;
	};

	void create(std::string_view table_name, const std::vector<Column>& columns);
	template<typename ... Data>
	decltype(std::to_string(std::declval<std::common_type_t<Data ...> >()), void()) insert(std::string_view table_name);

	static Column default_key(const std::string& name);

private:
	struct sqlite3Deleter {
		void operator()(sqlite3* db) const;
	};
	std::unique_ptr<sqlite3, sqlite3Deleter> m_db;

	static std::string to_sql(DataType data_type);
};

}

#endif
