#include <string>
#include <string_view>
#include <iterator>
#include <sstream>
#include <filesystem>
#include <numeric>
#include <algorithm>

#ifndef INCLUDE_SHIMIYUU_HELPER_HH_
#define INCLUDE_SHIMIYUU_HELPER_HH_

namespace shimiyuu::helper {

std::string timestamp();

bool is_valid_date(int d, int m, int y);

bool is_valid_date(std::string_view date);

std::string sanitize_windows_filename(std::string filename);

bool file_exists(const std::string& filename);

std::filesystem::path next_unused_filepath(
		const std::filesystem::path& start_path,
		const std::string& suffix_sep = "_");

std::vector<std::string> split(std::string_view s, char delim, bool allow_empty = false);

class Timestamp {
	std::chrono::milliseconds ms;

public:
	Timestamp();
	Timestamp(const Timestamp&);
	Timestamp(unsigned hours, unsigned minutes, unsigned seconds, unsigned milliseconds);
	Timestamp(const std::string&);

	Timestamp operator+(const Timestamp&) const;
	Timestamp operator-(const Timestamp&) const;

	template<typename Duration = std::chrono::milliseconds> int count() const;
	operator std::string() const;
	friend std::ostream& operator<<(std::ostream&, const Timestamp&);
};
std::ostream& operator<<(std::ostream&, const Timestamp&);

template<typename Predicate> void trim_left(std::string& s, Predicate p);
void trim_left(std::string& s);
template<typename Predicate> void trim_right(std::string& s, Predicate p);
void trim_right(std::string& s);
template<typename Predicate> void trim(std::string& s, Predicate p);
void trim(std::string& s);

template<typename IterT>
std::string interleave(
		IterT begin, IterT end,
		std::string delim = ", ",
		std::string (*to_string)(const typename std::iterator_traits<IterT>::value_type&)
		= [](const typename std::iterator_traits<IterT>::value_type& token) {
			return token;
		},
		std::string token_prefix = "", std::string token_suffix = "");

template<typename T, typename ... S>
void apply_permutation(
		const std::vector<std::size_t>& permutation,
		std::vector<T>& to_sort,
		std::vector<S>& ... rest);

template<typename T, typename Compare, typename ... S>
void sort_according_to(
		const std::vector<T>& reference,
		Compare compare,
		std::vector<S>& ... to_sort);

}

#include <detail/helper.tcc>

#endif
