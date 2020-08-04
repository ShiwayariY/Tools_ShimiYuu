#include <string>
#include <string_view>
#include <iterator>

#ifndef INCLUDE_SHIMIYUU_HELPER_HH_
#define INCLUDE_SHIMIYUU_HELPER_HH_

namespace shimiyuu::helper {

std::string sanitize_windows_filename(const std::string& filename);

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
