#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <format>

using namespace std;

#define LOG_INFO(msg) Logger::getInstance().log(LogLevel::INFO, msg, __FILE__, __LINE__)

enum class LogLevel{
    INFO,
    DEBUG,
    ERROR
};

string log_level_to_string(LogLevel level) {
    switch(level) {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::ERROR:
            return "ERROR";
    }
}

class IAppender {
    public:
        virtual ~IAppender() = default;
        virtual void log(const string& log_msg) = 0;
};

class ConsoleAppender : public IAppender {
    public:
        void log(const string& log_msg) override {
            cout<<log_msg<<endl;
        }
};

class FileAppender : public IAppender {
    public:
        void log(const string& log_msg) override {
            if (file_.is_open()) {
                file_<<log_msg<<endl;
            }
        }
    private:
        std::ofstream file_;
};

class Iformatter {
    public:
        virtual ~Iformatter() = default;
        virtual string format(LogLevel level, string msg, const string &file, int line) = 0;
};

class LogFormatter : public Iformatter {
    public:
        string format(LogLevel level, string msg, const string &file, int line) override {
            auto cur_time=chrono::system_clock::now();
            string my_time = std::format("{:%Y-%m-%d %H:%M:%S}",cur_time);
            return my_time+" "+to_string(line)+" "+log_level_to_string(level)+" "+msg;
        }
};

class Logger {
    public:
        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }

        void log(LogLevel level, string msg, const string &file, int line) {
            msg = formatter->format(level, msg, file, line);
            appender->log(msg);
        }
    private:
        ~Logger() = default;
        Logger(const Logger&) = delete;
        Logger operator=(const Logger&) = delete;
        unique_ptr<ConsoleAppender> appender;
        unique_ptr<LogFormatter> formatter;
        Logger() : appender(make_unique<ConsoleAppender>()),formatter(make_unique<LogFormatter>()) {}
};

int main()
{
    LOG_INFO("Hello World!!");
    return 0;
}