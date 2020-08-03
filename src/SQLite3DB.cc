#include <stdexcept>
#include <iostream>

#include <sqlite3.h>

#include <SQLite3DB.hh>

namespace shimiyuu {

SYLogger<int> sqlite3db_logger(1, std::cout);

SQLite3DB::SQLite3DB(const std::string& database_file) {
	sqlite3* db;
	if (sqlite3_open(database_file.c_str(), &db))
		throw std::runtime_error(sqlite3_errmsg(db));
	m_db.reset(db);

	exec("PRAGMA foreign_keys = on;");
}

void SQLite3DB::sqlite3Deleter::operator()(sqlite3* db) const {
	sqlite3_close(db); // TODO can fail .. does it leak?
}

SQLite3DB::Column::Column(const std::string& name,
		DataType data_type, bool allow_null, bool unique, KeyType key_type,
		const std::string& foreign_key_table, const std::string& foreign_key_name) :
		name(name), data_type(data_type), allow_null(allow_null), unique(unique), key_type(key_type),
		foreign_key_table(foreign_key_table), foreign_key_name(foreign_key_name) {
}

SQLite3DB::Column::operator std::string() const {
	std::string sql;
	sql += name + " " + to_sql(data_type)
		+ (key_type == KeyType::PRIMARY ? " PRIMARY KEY" : "")
		+ (allow_null ? " NOT NULL" : "")
		+ (unique && key_type != KeyType::PRIMARY ? " UNIQUE" : "")
		+ (key_type == KeyType::FOREIGN ? ", FOREIGN KEY(" + name + ") REFERENCES "
			+ foreign_key_table + "("
			+ (foreign_key_name.empty() ? name : foreign_key_name) + ") ON DELETE CASCADE" : "");
	return sql;
}

SQLite3DB::ConstraintComposite::operator std::string() const {
	std::string sql;
	sql += ", ";
	switch (key_type) {
		case KeyType::NONE:
			sql += "UNIQUE";
			break;
		case KeyType::PRIMARY:
			sql += "PRIMARY KEY";
			break;
		default:
			throw std::runtime_error("invalid key constraint");
	}
	sql += "(";
	concat_list(sql, columns);
	sql += ")";
	return sql;
}

void SQLite3DB::create_with_constraints(std::string_view table_name, const std::vector<Column>& columns,
		std::string_view constraint_clauses) {
	std::string sql("CREATE TABLE ");
	sql += table_name;
	sql += "(";
	concat_list(sql, columns);
	sql += constraint_clauses;
	sql += ");";

	exec(sql);
}

void SQLite3DB::insert(const std::string_view table_name,
		const std::vector<std::string_view>& columns,
		const std::vector<std::string_view>& data) {
	std::string sql("INSERT INTO ");
	sql += table_name;
	sql += " (";
	concat_list(sql, columns);
	sql += ") VALUES(";
	concat_list(sql, data, ",", "\"", "\"");
	sql += ");";

	exec(sql);
}

void SQLite3DB::remove(std::string_view table_name, std::string_view column, std::string_view value) {
	std::string sql("DELETE FROM ");
	sql += table_name;
	sql += "WHERE";
	sql += column;
	sql += " = \"";
	sql += value;
	sql += "\";";
	exec(sql);
}

SQLite3DB::Column SQLite3DB::default_key(const std::string& name) {
	return {name, DataType::INT, false, true, KeyType::PRIMARY};
}

std::string SQLite3DB::to_sql(DataType data_type) {
	switch (data_type) {
		case DataType::INT:
			return "INTEGER";
		case DataType::REAL:
			return "REAL";
		case DataType::TEXT:
			return "TEXT";
		default:
			throw std::runtime_error("invalid DataType");
	}
}

void SQLite3DB::exec(const std::string& sql) {
	sqlite3db_logger(0) << "exec(\"" << sql << "\") ...\n";
	if (char* sql_err = nullptr; sqlite3_exec(m_db.get(), sql.c_str(), nullptr, nullptr, &sql_err) != SQLITE_OK) {
		const std::string err_msg(sql_err);
		sqlite3_free(sql_err);
		throw std::runtime_error(err_msg);
	}
}

}
