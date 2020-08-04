#include <string_view>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <helper.hh>

#include <logger.hh>

#include <sqlite3.h>

#ifndef INCLUDE_SHIMIYUU_SQLITE3DB_HH_
#define INCLUDE_SHIMIYUU_SQLITE3DB_HH_

namespace shimiyuu {

extern SYLogger<int> sqlite3db_logger;

class SQLite3DB {

public:

	enum class DataType {
		TEXT, INT, REAL
	};
	enum class KeyType {
		NONE, PRIMARY, FOREIGN
	};

	class Column {
		friend class SQLite3DB;

		const std::string name;
		const DataType data_type;
		const bool allow_null;
		const bool unique;
		const KeyType key_type;
		const std::string foreign_key_table, foreign_key_name;

		std::string definition_sql() const;
		std::string constraint_sql() const;

	public:

		Column(const std::string& name, DataType data_type = DataType::TEXT,
				bool allow_null = false, bool unique = false,
				KeyType key_type = KeyType::NONE,
				const std::string& foreign_key_table = "", const std::string& foreign_key_name = "");
	};

	class ConstraintComposite {
		friend class SQLite3DB;

		const std::vector<std::string> columns;
		KeyType key_type = KeyType::NONE;

		std::string to_sql() const;

	public:
		template<typename ... ColumnNames,
				typename = std::enable_if_t<std::conjunction_v<std::is_constructible<std::string, ColumnNames> ...> >
		> ConstraintComposite(const ColumnNames& ... columns) :
				columns { columns ... } {
			if (this->columns.size() < 2) throw std::runtime_error("invalid unique constraints");
		}

		template<typename ... ColumnNames,
				typename = std::enable_if_t<std::conjunction_v<std::is_constructible<std::string, ColumnNames> ...> >
		> ConstraintComposite(KeyType key_type, const ColumnNames& ... columns) :
				ConstraintComposite(columns ...) {
			this->key_type = key_type;
		}
	};

private:
	struct sqlite3Deleter {
		void operator()(sqlite3* db) const;
	};
	std::unique_ptr<sqlite3, sqlite3Deleter> m_db;

	static std::string to_sql(DataType data_type);

	void exec(const std::string& sql);

	void create_with_constraints(std::string_view table_name, const std::vector<Column>& columns,
			const std::string& constraint_clauses);

public:
	SQLite3DB(const std::string& database_file);

	template<typename ... ConstraintCompositeT>
	std::enable_if_t<std::conjunction_v<
			std::is_same<ConstraintCompositeT, ConstraintComposite> ...>
	> create(std::string_view table_name, const std::vector<Column>& columns,
			const ConstraintCompositeT& ... unique_sets) {
		std::vector<std::string> constraints_sql { unique_sets.to_sql() ... };
		create_with_constraints(table_name, columns,
				helper::interleave(constraints_sql.begin(), constraints_sql.end(), ""));
	}

	void insert(std::string_view table_name,
			const std::vector<std::string_view>& columns,
			const std::vector<std::string_view>& data);

	void remove(std::string_view table_name,
			std::string_view column,
			std::string_view value);

	static Column default_key(const std::string& name);

};

}

#endif
