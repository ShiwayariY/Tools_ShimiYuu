#include <string>
#include <iterator>
#include <sstream>
#include <filesystem>

#ifndef INCLUDE_SHIMIYUU_HELPER_HH_
#define INCLUDE_SHIMIYUU_HELPER_HH_

namespace shimiyuu::helper {

std::string sanitize_windows_filename(const std::string& filename);

bool file_exists(const std::string& filename);

std::filesystem::path next_unused_filepath(
		const std::filesystem::path& start_path,
		const std::string& suffix_sep = "_");

bool is_valid_date(int d, int m, int y);

bool is_valid_date(std::string_view date);

template<typename IterT>
std::string interleave(IterT begin, IterT end, std::string delim = ", ",
		std::string (*to_string)(const typename std::iterator_traits<IterT>::value_type&)
		= [](const typename std::iterator_traits<IterT>::value_type& token) {
			return token;
		}, std::string token_prefix = "", std::string token_suffix = "") {
	std::string ret;

	bool print_delim = false;
	while (begin != end) {
		if (print_delim) ret += delim;
		ret += token_prefix + to_string(*begin++) + token_suffix;
		print_delim = true;
	}

	return ret;
}

}

#endif
