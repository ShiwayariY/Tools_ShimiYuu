#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <regex>
#include <set>

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

void trim_left(std::string& s) {
	trim_left(s, static_cast<int (*)(int)>(std::isspace));
}
void trim_right(std::string& s) {
	trim_right(s, static_cast<int (*)(int)>(std::isspace));
}
void trim(std::string& s) {
	trim(s, static_cast<int (*)(int)>(std::isspace));
}

std::string sanitize_windows_filename(std::string filename) {
	static const std::set<char> BAD_CHARS { '\\', '/', ':', '*', '?', '"', '<', '>', '|' };
	std::erase_if(filename, [](unsigned char c) {
		return BAD_CHARS.contains(c) || c < 32;
	});
	trim(filename, [](char c) {
		return std::isspace(c) || c == '.';
	});
	return filename;
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

std::vector<std::string> split(std::string_view s, char delim, bool allow_empty) {
	std::vector<std::string> tokens;
	std::vector<std::string>::pointer tk;
	bool new_tk = true;

	if (allow_empty) {
		tk = &tokens.emplace_back("");
		new_tk = false;
	}

	for (const char c : s) {
		if (c == delim) {
			if (allow_empty && new_tk)
				tk = &tokens.emplace_back("");
			new_tk = true;
		} else {
			if (new_tk) {
				new_tk = false;
				tk = &tokens.emplace_back(std::string { c });
			} else
				*tk += c;
		}
	}

	if (allow_empty && new_tk)
		tokens.emplace_back("");

	return tokens;
}

Timestamp::Timestamp() :
		ms { 0 } {
}

Timestamp::Timestamp(const Timestamp& other) :
		ms { other.ms } {
}

Timestamp::Timestamp(unsigned hours, unsigned minutes, unsigned seconds, unsigned milliseconds) :
		ms { milliseconds } {
	ms += std::chrono::seconds { seconds } + std::chrono::minutes { minutes } + std::chrono::hours { hours };
}

Timestamp::Timestamp(const std::string& timestamp) {
	std::regex timestamp_regex { R"(([0-9]+):([0-5][0-9]):([0-5][0-9])(?:[.]([0-9]{3}))?)" };
	std::smatch timestamp_match;
	if (std::regex_match(timestamp, timestamp_match, timestamp_regex)) {
		try {
			ms = std::chrono::milliseconds { std::stoi(timestamp_match.str(4)) };
		} catch (...) {
			ms = std::chrono::milliseconds { 0 };
		}
		ms += std::chrono::hours { std::stoi(timestamp_match.str(1)) }
			+ std::chrono::minutes { std::stoi(timestamp_match.str(2)) }
			+ std::chrono::seconds { std::stoi(timestamp_match.str(3)) };
	} else
		throw std::invalid_argument { "invalid Timestamp" };
}

Timestamp Timestamp::operator+(const Timestamp& other) const {
	Timestamp sum { *this };
	sum.ms += other.ms;
	return sum;
}

Timestamp Timestamp::operator-(const Timestamp& other) const {
	if (ms >= other.ms) {
		Timestamp diff { *this };
		diff.ms -= other.ms;
		return diff;
	} else
		throw std::runtime_error("Timestamp::operator-(... result must be >= 0");
}

template<typename Duration> int Timestamp::count() const {
	return std::chrono::duration_cast<Duration>(ms).count();
}

Timestamp::operator std::string() const {
	auto rest = ms;
	const auto hours = duration_cast<std::chrono::hours>(rest);
	rest -= hours;
	const auto minutes = duration_cast<std::chrono::minutes>(rest);
	rest -= minutes;
	const auto seconds = duration_cast<std::chrono::seconds>(rest);
	rest -= seconds;
	std::ostringstream timestamp_oss;
	timestamp_oss << std::setfill('0')
		<< std::setw(2) << hours.count()
		<< ":" << std::setw(2) << minutes.count()
		<< ":" << std::setw(2) << seconds.count()
		<< "." << std::setw(3) << rest.count();
	return timestamp_oss.str();
}

std::ostream& operator<<(std::ostream& os, const Timestamp& timestamp) {
	os << timestamp.operator std::string();
	return os;
}

}
