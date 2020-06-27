#include <regex>

#include <helper.hh>

namespace shimiyuu::helper {

std::string sanitize_windows_filename(const std::string& filename) {
	static const std::regex BAD_CHAR_REGEX(R"(\\|/|:|\*|\?|"|<|>|\|)");
	return std::regex_replace(filename, BAD_CHAR_REGEX, " ");
}

}
