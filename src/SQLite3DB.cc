#include <sqlite3.h>

#include <SQLite3DB.hh>

namespace shimiyuu {

SQLite3DB::SQLite3DB(const std::filesystem::path& database_file) {
	sqlite3* db;
	if (sqlite3_open(database_file.string().c_str(), &db))
		throw std::runtime_error(sqlite3_errmsg(db));
	m_db.reset(db);
}

void SQLite3DB::sqlite3Deleter::operator()(sqlite3* db) const {
	sqlite3_close(db); // TODO can fail .. does it leak?
}

SQLite3DB::Column::Column(const std::string& name,
		DataType data_type, bool allow_null, KeyType key_type,
		const std::string& foreign_key_table, const std::string& foreign_key_name) :
		name(name), data_type(data_type), allow_null(allow_null), key_type(key_type),
		foreign_key_table(foreign_key_table), foreign_key_name(foreign_key_name) {
}

SQLite3DB::Column::operator std::string() const {
	std::string sql(name);
	sql += " " + to_sql(data_type)
		+ (key_type == KeyType::PRIMARY ? " PRIMARY KEY" : "")
		+ (allow_null ? " NOT NULL" : "")
		+ (key_type != KeyType::FOREIGN ? "" : ", FOREIGN KEY(" + name + ") REFERENCES "
			+ foreign_key_table + "("
			+ (foreign_key_name.empty() ? name : foreign_key_name) + ")");
	return sql;
}

void SQLite3DB::create(std::string_view table_name, const std::vector<Column>& columns) {
	std::string sql("CREATE TABLE ");
	sql += table_name;
	sql += "(";
	bool print_delim = false;
	for (const auto& col : columns) {
		if (print_delim) sql += ", ";
		sql += col;
		print_delim = true;
	}
	sql += ");";

	if (char* sql_err = nullptr; sqlite3_exec(m_db.get(), sql.c_str(), nullptr, nullptr, &sql_err) != SQLITE_OK) {
		const std::string err_msg(sql_err);
		sqlite3_free(sql_err);
		throw std::runtime_error(err_msg);
	}
}

SQLite3DB::Column SQLite3DB::default_key(const std::string& name) {
	return {name, DataType::INT, false, KeyType::PRIMARY};
}

std::string SQLite3DB::to_sql(DataType data_type) {
	switch (data_type) {
		case DataType::INT:
			return "INT";
		case DataType::REAL:
			return "REAL";
		case DataType::TEXT:
			return "TEXT";
		default:
			throw std::runtime_error("invalid DataType");
	}
}

}
