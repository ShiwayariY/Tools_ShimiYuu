#include <ostream>
#include <vector>
#include <map>
#include <initializer_list>
#include <type_traits>
#include <utility>

#ifndef INCLUDE_SHIMIYUU_LOGGER_HH_
#define INCLUDE_SHIMIYUU_LOGGER_HH_

namespace shimiyuu {

namespace {
class SYLoggerRelay {
	std::ostream* os;

public:
	SYLoggerRelay(std::ostream* os) : os(os) {
	}

	template<typename MessageT>
	SYLoggerRelay& operator<<(const MessageT& message) {
		if (os) *os << message;
		return *this;
	}

	SYLoggerRelay& operator<<(std::ostream& (*manip)(std::ostream&)) {
		if (os) manip(*os);
		return *this;
	}
};
}

template<typename LogLevelT>
class SYLogger {
	std::map<LogLevelT, std::pair<bool, std::ostream*> > m_logs;
	const LogLevelT* m_level;
	SYLoggerRelay m_relay;

public:
	SYLogger(const LogLevelT& level, std::ostream& log) :
			m_logs { { level, { true, &log } } },
			m_level(&m_logs.begin()->first), m_relay(nullptr) {
	}

	/**
	 * @return a stream object correspoding to the given level
	 */
	SYLoggerRelay& operator()(const LogLevelT& level) {
		std::ostream* log = nullptr;
		if (level >= *m_level) {
			const auto& [enabled, level_log] = m_logs.at(level);
			if (enabled) log = level_log;
		}
		m_relay = SYLoggerRelay(log);
		return m_relay;
	}

	/** set the log level of the logger
	 *	@return true if the level was set, false otherwise (when the level has no associated log)
	 */
	bool level(const LogLevelT& level) {
		if (const auto it = m_logs.find(level); it != m_logs.end()) {
			m_level = &it->first;
			return true;
		} else
			return false;
	}

	/**
	 *	@return the current log level
	 */
	const LogLevelT& level() const {
		return *m_level;
	}

	/**
	 * Add a log level or change the stream of an existing one and enable it
	 */
	void level(const LogLevelT& level, std::ostream& log) {
		m_logs[level] = { true, &log };
	}

	/**
	 *  @return the status of a log level or false if it doesn't exist
	 */
	bool enabled(const LogLevelT& level) {
		if (const auto it = m_logs.find(level); it != m_logs.end())
			return it->second->first;
		else
			return false;
	}
	//TODO enable, disable logs / check if a log is enabled ...

	/**
	 * enable or disable a log level
	 * @return	true if the status was successfully changed, false otherwise (when the level does not exist)
	 */
	bool enabled(const LogLevelT& level, bool enable) {
		if (auto it = m_logs.find(level); it != m_logs.end()) {
			it->second->first = enable;
			return true;
		} else
			return false;
	}
};

}

#endif
