#pragma once

namespace spdlog {

class spdlog_ex {
public:
	explicit spdlog_ex(const char *msg = "spdlog error") noexcept
			:
			_msg(msg) {}

	//const char *what() const noexcept override { return _msg; }

private:
	const char *_msg;
};

namespace level {
enum level_enum {
	trace,
	debug,
	info,
	warn,
	err,
	critical,
	off
};
}

/* =========================
   SINKS
   ========================= */
namespace sinks {
class sink {
public:
	virtual ~sink() = default;

	void set_level(level::level_enum) {}
	void set_pattern(const char *) {}
};

class stdout_color_sink_mt : public sink {
public:
	stdout_color_sink_mt() = default;
};

class rotating_file_sink_mt : public sink {
public:
	rotating_file_sink_mt(const char *, size_t, size_t) {}
};
} // namespace sinks

//using sink_ptr = std::shared_ptr<sinks::sink>;

/* =========================
   LOGGER
   ========================= */
class logger {
public:
	logger() = default;

	template <typename It>
	logger(const char *, It, It) {}

	//void set_level(level::level_enum) {}
	void set_pattern(const char *) {}
	void flush() {}

	const char *name() const { return "fake"; }

	template <typename... Args>
	void trace(const char *, Args...) {}

	template <typename... Args>
	void debug(const char *, Args...) {}

	template <typename... Args>
	void info(const char *, Args...) {}

	template <typename... Args>
	void warn(const char *, Args...) {}

	template <typename... Args>
	void error(const char *, Args...) {}

	template <typename... Args>
	void critical(const char *, Args...) {}
};

/* =========================
   REGISTRY
   ========================= */
//inline void register_logger(std::shared_ptr<logger>) {}
inline void drop(const char *) {}

} // namespace spdlog

namespace fmt {
// jen nosič pointeru, nic nedělá
struct ptr_wrapper {
	const void *value;
};

// fmt::ptr(...)
inline ptr_wrapper ptr(const void *p) {
	return { p };
}
} //namespace fmt
