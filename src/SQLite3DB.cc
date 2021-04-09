#include <stdexcept>
#include <iostream>

#include <sqlite3.h>

#include <SQLite3DB.hh>

namespace shimiyuu {

SYLogger<int> sqlite3db_logger(2, std::cerr);

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

std::string SQLite3DB::Column::definition_sql() const {
	std::string sql;
	sql += name + " " + to_sql(data_type)
		+ (key_type == KeyType::PRIMARY ? " PRIMARY KEY" : "")
		+ (allow_null ? "" : " NOT NULL")
		+ (unique && key_type != KeyType::PRIMARY ? " UNIQUE" : "");
	return sql;
}

std::string SQLite3DB::Column::constraint_sql() const {
	return (key_type == KeyType::FOREIGN ?
		", FOREIGN KEY(" + name + ") REFERENCES "
			+ foreign_key_table + "("
			+ (foreign_key_name.empty() ? name : foreign_key_name)
			+ ") ON DELETE CASCADE" : "");
}

std::string SQLite3DB::ConstraintComposite::to_sql() const {
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
	sql += "(" + helper::interleave(columns.begin(), columns.end(), ",") + ")";
	return sql;
}

void SQLite3DB::create_with_constraints(std::string_view table_name, const std::vector<Column>& columns,
		const std::string& constraint_clauses) {
	std::string sql("CREATE TABLE ");
	sql += table_name;
	sql += "("
		+ helper::interleave(columns.begin(), columns.end(), ", ", [](const Column& col) {
			return col.definition_sql();
		})
		+ helper::interleave(columns.begin(), columns.end(), "", [](const Column& col) {
			return col.constraint_sql();
		})
		+ constraint_clauses + ");";
	sqlite3db_logger(0) << "constaint composites: " << constraint_clauses << std::endl;

	exec(sql);
}

void SQLite3DB::insert(const std::string_view table_name,
		const std::vector<std::pair<std::string, std::string> >& column_data) {
	std::string sql("INSERT INTO ");
	sql += table_name;
	sql += " ("
		+ helper::interleave(column_data.begin(), column_data.end(), ",", [](const auto& p) {
			return p.first;
		})
		+ ") VALUES("
		+ helper::interleave(column_data.begin(), column_data.end(), ",", [](const auto& p) {
			return p.second;
		}, "\"", "\"")
		+ ");";

	exec(sql);
}

void SQLite3DB::update(std::string_view table_name,
		const std::vector<std::pair<std::string, std::string> > new_column_data,
		const std::vector<std::pair<std::string, std::string> > column_data_conditions) {
	std::string sql("UPDATE ");
	sql += table_name;
	sql += " SET "
		+ helper::interleave(new_column_data.begin(), new_column_data.end(), ", ", [](const auto& p) {
			return p.first + " = \"" + p.second + "\"";
		})
		+ " WHERE "
		+ helper::interleave(column_data_conditions.begin(), column_data_conditions.end(), " AND ", [](const auto& p) {
			return p.first + " = \"" + p.second + "\"";
		}) + ";";

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
	sqlite3db_logger(0) << "exec(\"" << sql << "\") ..." << std::endl;
	if (char* sql_err = nullptr; sqlite3_exec(m_db.get(), sql.c_str(), nullptr, nullptr, &sql_err) != SQLITE_OK) {
		const std::string err_msg(sql_err);
		sqlite3_free(sql_err);
		throw std::runtime_error(err_msg);
	}
}

SQLite3DB::Transaction::Transaction(SQLite3DB& db) : m_db(db) {
	m_db.exec("BEGIN");
}

SQLite3DB::Transaction::~Transaction() {
	// Must end SQLite transaction even on query error <- guaranteed on exception by RAII
	m_db.exec("COMMIT");
}

SQLite3DB::Transaction SQLite3DB::start_transaction() {
	return Transaction(*this);
}

}
