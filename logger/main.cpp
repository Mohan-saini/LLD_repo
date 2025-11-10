#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <format>

using namespace std;

#define LOG_INFO(msg) Logger::getInstance().log(LogLevel::INFO, msg, __FILE__, __LINE__)
#define LOG_DEBUG(msg) Logger::getInstance().log(LogLevel::DEBUG, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) Logger::getInstance().log(LogLevel::ERROR, msg, __FILE__, __LINE__)

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

// Strategy Pattern
class IAppender {
    public:
        virtual ~IAppender() = default;
        virtual void append(const string& log_msg) = 0;
};

class ConsoleAppender : public IAppender {
    public:
        void append(const string& log_msg) override {
            cout<<log_msg<<endl;
        }
};

class FileAppender : public IAppender {
    public:
        FileAppender(const string& filename) {
            file_.open(filename, ios_base::out | ios_base::app);
            if (!file_.is_open()) {
                throw runtime_error("failed to open log file: "+filename);
            }
        }
        void append(const string& log_msg) override {
            if (file_.is_open()) {
                file_<<log_msg<<endl;
                file_.flush();  //ensure immediate writing
            }
        }
        ~FileAppender() {
            if (file_.is_open()) {
                file_.close();
            }
        }
    private:
        std::ofstream file_;
};

// Strategy pattern
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
            stringstream ss;
            ss << std::left
               << std::setw(26) << my_time
               << std::setw(6) << line
               << std::setw(7) << log_level_to_string(level)
               << msg;
            return ss.str();
        }
};

// Singleton pattern
class Logger {
    public:
        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }

        void log(LogLevel level, string msg, const string &file, int line) {
            lock_guard<mutex> lock_(mtx);  //ensure thread safety
            msg = formatter->format(level, msg, file, line);
            // append log msg to all appenders
            for(const auto& appender : appenders) {
                appender->append(msg);
            }
        }

        //add new appender, like file appender
        void addAppender(unique_ptr<IAppender> appender) {
            appenders.push_back(move(appender));
        }

    private:
        ~Logger() = default;
        Logger(const Logger&) = delete;
        Logger operator=(const Logger&) = delete;
        //unique_ptr<ConsoleAppender> appender;
        unique_ptr<LogFormatter> formatter;
        vector<unique_ptr<IAppender>> appenders;
        mutex mtx;
        Logger() : formatter(make_unique<LogFormatter>()) {
            appenders.push_back(make_unique<ConsoleAppender>());  // Console appender is added by default
        }
};

int main()
{
    try {
        Logger::getInstance().addAppender(make_unique<FileAppender>("application.log"));
    } catch (const exception &e) {
        cout<<"Error in adding file appender\n";
        return 1;
    }
    LOG_INFO("Hello World!!");
    LOG_ERROR("Logger System is created.......");
    return 0;
}