#include <string>
#include <iterator>
#include <sstream>
#include <filesystem>
#include <numeric>
#include <algorithm>

#ifndef INCLUDE_SHIMIYUU_HELPER_HH_
#define INCLUDE_SHIMIYUU_HELPER_HH_

namespace shimiyuu::helper {

std::string timestamp();

template<typename Predicate>
void trim_left(std::string& s, Predicate p) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&](char c) {
		return !p(c);
	}));
}
template<typename Predicate>
void trim_right(std::string& s, Predicate p) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [&](char c) {
		return !p(c);
	}).base(), s.end());
}
template<typename Predicate>
void trim(std::string& s, Predicate p) {
	trim_left(s, p);
	trim_right(s, p);
}
void trim_left(std::string& s);
void trim_right(std::string& s);
void trim(std::string& s);

std::string sanitize_windows_filename(std::string filename);

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

template<typename T, typename ... S>
void apply_permutation(const std::vector<std::size_t>& permutation,
		std::vector<T>& to_sort, std::vector<S>& ... rest) {
	if (!(to_sort.size() == permutation.size()
			&& ((rest.size() == permutation.size()) && ...)
	)) throw std::runtime_error("apply_permutation(.. sizes do not match");

	std::vector<bool> done(to_sort.size());
	for (size_t i = 0; i < to_sort.size(); ++i) {
		if (done[i]) continue;
		done[i] = true;
		std::size_t prev = i;
		std::size_t curr = permutation[i];
		while (i != curr) {
			std::swap(to_sort[prev], to_sort[curr]);
			(std::swap(rest[prev], rest[curr]), ...);
			done[curr] = true;
			prev = curr;
			curr = permutation[curr];
		}
	}
}

template<typename T, typename Compare, typename ... S>
void sort_according_to(const std::vector<T>& reference, Compare compare, std::vector<S>& ... to_sort) {

	std::vector<std::size_t> sort_permutation(reference.size());
	std::iota(sort_permutation.begin(), sort_permutation.end(), 0);
	std::sort(sort_permutation.begin(), sort_permutation.end(),
			[&](std::size_t i, std::size_t j) {
				return compare(
						reference[i],
						reference[j]);
			});
	apply_permutation(sort_permutation, to_sort ...);
}

}

#endif
