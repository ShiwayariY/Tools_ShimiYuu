#include <string_view>
#include <string>
#include <vector>
#include <memory>

#include <sqlite3.h>

#ifndef INCLUDE_SHIMIYUU_SQLITE3DB_HH_
#define INCLUDE_SHIMIYUU_SQLITE3DB_HH_

namespace shimiyuu {

class SQLite3DB {

	template<typename ListItem>
	static std::enable_if_t<std::is_constructible_v<std::string, ListItem> > concat_list(
			std::string& concat_to, const std::vector<ListItem>& list,
			std::string delim = ",", const std::string before_item = "", const std::string after_item = "") {
		bool print_delim = false;
		concat_to += before_item;
		delim = after_item + delim + before_item;
		for (const auto& val : list) {
			if (print_delim) concat_to += delim;
			concat_to += val;
			print_delim = true;
		}
		concat_to += after_item;
	}

public:

	enum class DataType {
		TEXT, INT, REAL
	};
	enum class KeyType {
		NONE, PRIMARY, FOREIGN
	};

	class Column {
		const std::string name;
		const DataType data_type;
		const bool allow_null;
		const bool unique;
		const KeyType key_type;
		const std::string foreign_key_table, foreign_key_name;

	public:

		Column(const std::string& name, DataType data_type = DataType::TEXT,
				bool allow_null = false, bool unique = false,
				KeyType key_type = KeyType::NONE,
				const std::string& foreign_key_table = "", const std::string& foreign_key_name = "");

		operator std::string() const;
	};

	class ConstraintComposite {
		const std::vector<std::string> columns;
		KeyType key_type = KeyType::NONE;

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

		operator std::string() const;
	};

private:
	struct sqlite3Deleter {
		void operator()(sqlite3* db) const;
	};
	std::unique_ptr<sqlite3, sqlite3Deleter> m_db;

	static std::string to_sql(DataType data_type);

	void exec(const std::string& sql);

	void create_with_constraints(std::string_view table_name, const std::vector<Column>& columns,
			std::string_view constraint_clauses);

public:
	SQLite3DB(const std::string& database_file);

	template<typename ... ConstraintCompositeT>
	std::enable_if_t<std::conjunction_v<
			std::is_same<ConstraintCompositeT, ConstraintComposite> ...>
	> create(std::string_view table_name, const std::vector<Column>& columns,
			const ConstraintCompositeT& ... unique_sets) {
		std::string constraint_clauses;
		concat_list(constraint_clauses, std::vector<ConstraintComposite> { unique_sets ... });
		create_with_constraints(table_name, columns, constraint_clauses);
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
