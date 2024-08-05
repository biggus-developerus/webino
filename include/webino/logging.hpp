#pragma once

#include <string>
#include <string_view>
#include <memory>

namespace spdlog
{
    class logger;
}

namespace webino::logging
{
    namespace _ansi
    {
        static const std::string RESET = "\033[0m";
        static const std::string BOLD = "\033[1m";
        static const std::string DIM = "\033[2m";
        static const std::string ITALIC = "\033[3m";
        static const std::string UNDERLINE = "\033[4m";
        static const std::string BLINK = "\033[5m";
        static const std::string REVERSE = "\033[7m";
        static const std::string HIDDEN = "\033[8m";
        static const std::string STRIKETHROUGH = "\033[9m";
        static const std::string RED = "\033[31m";
        static const std::string GREEN = "\033[32m";
        static const std::string YELLOW = "\033[33m";
        static const std::string BLUE = "\033[34m";
    
        std::string wrap(std::string_view ansi_code, std::string_view text);
    }

    class Logger
    {
        public:
            Logger(std::string logger_name);
            ~Logger() = default;

            // wrap spdlog::logger methods
            void trace(std::string message);
            void info(std::string message);
            void warn(std::string message);
            void error(std::string message);
            void critical(std::string message);
        private:
            std::shared_ptr<spdlog::logger> logger;
    };

    static Logger _global_logger("WEBINO");

    inline void log_trace(std::string m){ _global_logger.trace(m); }
    inline void log_info(std::string m){ _global_logger.info(m); }
    inline void log_warn(std::string m){ _global_logger.warn(m); }
    inline void log_error(std::string m){ _global_logger.error(m); }
    inline void log_critical(std::string m){ _global_logger.critical(m); }
}