#include <iostream>
#include <string>
#include <exception>
#include <fstream>

class MyException: std::exception{
private:
    std::string msg_;
public:
    explicit MyException(std::string msg) : msg_(std::move(msg)){}
    ~MyException() override = default;
    [[nodiscard]]  const char* what() const noexcept override {
        return msg_.c_str();
    }
};

class MyExceptionFatalError : std::exception{
private:
    std::string msg_;
public:
    explicit MyExceptionFatalError(std::string msg) : msg_(std::move(msg)){}
    ~MyExceptionFatalError() override = default;
    [[nodiscard]]  const char* what() const noexcept override {
        return msg_.c_str();
    }
};

class MyExceptionUnknownMsg : std::exception{
private:
    std::string msg_;
public:
    explicit MyExceptionUnknownMsg(std::string msg) : msg_(std::move(msg)){}
    ~MyExceptionUnknownMsg() override = default;
    [[nodiscard]]  const char* what() const noexcept override {
        return msg_.c_str();
    }
};

class LogMessage {
public:
    virtual LogMessage *set_next(LogMessage *log_message) = 0;
    virtual void message(const std::string &mes) = 0;
    virtual ~LogMessage() = default;
};

class AbstractLogMessage : public LogMessage{
private:
    LogMessage *next_log_message_;
public:
    AbstractLogMessage(): next_log_message_(nullptr){}
    LogMessage *set_next(LogMessage *log_message) override{
        this->next_log_message_ = log_message;
        return  log_message;
    }
    void message(const std::string &mes) override{
        if(this->next_log_message_){
            return this->next_log_message_->message(mes);
        }
    }
    ~AbstractLogMessage() override {delete next_log_message_;}
};

class LogMessageWarning : public AbstractLogMessage{
public:
    void message(const std::string &mes) override{
        if(mes == "warning"){
            std::cout << mes << std::endl;
        }else{
            return AbstractLogMessage::message(mes);
        }
    }
};

class  LogMessageError : public AbstractLogMessage{
private:
    std::string path_txt_;
public:
    explicit LogMessageError(const std::string &path_txt): path_txt_(path_txt){}
    void message(const std::string &mes) override{
        if(mes == "error"){
        std::fstream f(path_txt_);
        if(!f.is_open())
            throw MyException("not open file");
        f << mes;
        }else{
            return AbstractLogMessage::message(mes);
        }
    }
};

class  LogMessageFatalError : public AbstractLogMessage{
public:
    void message(const std::string &mes) override{
        if(mes == "fatal error"){
            throw MyExceptionFatalError(mes);
        }else{
            return AbstractLogMessage::message(mes);
        }
    }
};

class LogMessageUnknownMessage : public AbstractLogMessage{
public:
    void message(const std::string &mes) override{
        if(mes == "Unknown message"){
            throw MyExceptionUnknownMsg(mes);
        }else{
            return AbstractLogMessage::message(mes);
        }
    }
};

int main() {

    try {
        auto error = new LogMessageError("../out.txt");
        auto warning = new LogMessageWarning();
        auto fat_error = new LogMessageFatalError();
        auto un_msg = new LogMessageUnknownMessage();

        fat_error->set_next(un_msg)->set_next(warning)->set_next(error);
        fat_error->message("Unknown message");
        delete error; delete warning; delete fat_error; delete un_msg;
    }catch (const MyException &e){
        std::cout << e.what() << std::endl;
    }catch (const MyExceptionUnknownMsg &e){
        std::cout << e.what() << std::endl;
    }catch (const MyExceptionFatalError &e){
        std::cout << e.what() << std::endl;
    }
}
