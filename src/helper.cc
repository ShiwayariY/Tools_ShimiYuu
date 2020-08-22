#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <regex>

#include <helper.hh>

namespace shimiyuu::helper {

std::string timestamp() {
	const auto now = std::chrono::system_clock::now();
	const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
	const auto now_t = std::chrono::system_clock::to_time_t(now);
	const std::tm now_tm = *std::localtime(&now_t);
	std::ostringstream oss;
	oss << std::put_time(&now_tm, "%H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
	return oss.str();
}

std::string sanitize_windows_filename(const std::string& filename) {
	static const std::regex BAD_CHAR_REGEX(R"(\\|/|:|\*|\?|"|<|>|\|)");
	return std::regex_replace(filename, BAD_CHAR_REGEX, " ");
}

bool file_exists(const std::string& filename) {
	std::ifstream ifs(filename);
	return ifs.is_open();
}

std::filesystem::path next_unused_filepath(
		const std::filesystem::path& start_path,
		const std::string& suffix_sep) {
	if (!start_path.has_filename()) throw std::invalid_argument("not a filepath");

	const auto parent_dir = start_path.parent_path();
	const auto stem = start_path.stem();
	const auto ext = start_path.extension();

	auto next_filename = start_path.filename();
	for (int counter = 1; file_exists((parent_dir / next_filename).string()); ++counter) {
		next_filename = stem;
		next_filename += suffix_sep + std::to_string(counter);
		next_filename += ext;
	}

	return parent_dir / next_filename;
}

bool is_valid_date(int d, int m, int y) {
	if (y < 0 || m < 1 || m > 12 || d < 1) return false;
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
