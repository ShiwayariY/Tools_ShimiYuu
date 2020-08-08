#include <regex>

#include <helper.hh>

namespace shimiyuu::helper {

std::string sanitize_windows_filename(const std::string& filename) {
	static const std::regex BAD_CHAR_REGEX(R"(\\|/|:|\*|\?|"|<|>|\|)");
	return std::regex_replace(filename, BAD_CHAR_REGEX, " ");
}

bool is_valid_date(int d, int m, int y) {
	if(y < 0 || m < 1 || m > 12 || d < 1) return false;
	const bool leap = (((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0));
	switch (m) {
		case 2:
			return d <= (leap ? 29 : 28);
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			return d <= 30;
			break;
		default:
			return d <= 31;
	}
	return true;
}

bool is_valid_date(std::string_view date) {
	int d = -1, m = -1, y = -1;
	char dot;
	std::istringstream date_oss(date.data());
	date_oss >> d >> dot;
	if (date_oss.fail() || dot != '.') return false;
	date_oss >> m >> dot >> y;
	if (date_oss.fail() || dot != '.' || !date_oss.eof()) return false;

	return is_valid_date(d, m, y);
}

}
