#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <fmt/core.h>

#include "webino/logging.hpp"

namespace webino::logging
{

    std::string _ansi::wrap(std::string_view ansi_code, std::string_view text){ return fmt::format("{}{}{}", ansi_code, text, _ansi::RESET); }
    
    Logger::Logger(std::string logger_name)
    {
        this->logger = std::make_shared<spdlog::logger>(logger_name, std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        this->logger->set_level(spdlog::level::trace);
        this->logger->set_pattern("[%T] [%n] %v");

        spdlog::set_default_logger(this->logger);
    }

    void Logger::trace(std::string message)
    {
        message = fmt::format(
            "{}",
            _ansi::wrap(_ansi::DIM, "[TRACE] " + message)
        );

        this->logger->trace(message);
    }

    void Logger::info(std::string message) 
    {
        message = fmt::format(
            "{}",
            _ansi::wrap(_ansi::GREEN, "[INFO] " + message)
        );

        this->logger->info(message);
    }
    void Logger::warn(std::string message) 
    {
        message = fmt::format(
            "{}",
            _ansi::wrap(_ansi::YELLOW, "[WARN] " + message)
        );

        this->logger->warn(message);
    }
    void Logger::error(std::string message) 
    {
        message = fmt::format(
            "{}",
            _ansi::wrap(_ansi::RED, "[ERROR] " + message)
        );

        this->logger->error(message);
    }
    void Logger::critical(std::string message) 
    {
        message = fmt::format(
            "{}",
            _ansi::wrap(_ansi::RED + _ansi::UNDERLINE, "[CRITICAL] " + message)
        );

        this->logger->critical(message);
    }
}