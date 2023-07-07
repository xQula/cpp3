#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <utility>
#include <memory>

class MyException : std::exception{
private:
    std::string msg_;
public:
    explicit MyException(std::string msg) : msg_(std::move(msg)){}
    ~MyException() override = default;
    [[nodiscard]]  const char* what() const noexcept override {
        return msg_.c_str();
    }
};

class LogCommand {
public:
    virtual ~LogCommand() = default;
    virtual void print(const std::string& message) = 0;
};

class LogCommandConsole : LogCommand{
public:
    ~LogCommandConsole() override = default;
    void print(const std::string& message) final{
        std::cout << message << std::endl;
    }

};

class  LogCommandTxt : LogCommand{
private:
    std::string path_txt_;
public:
    LogCommandTxt(const std::string &path) : path_txt_(path){}
    ~LogCommandTxt() override = default;
    void print(const std::string& message) final{
        std::fstream f(path_txt_);
        if(!f.is_open())
            throw MyException("Not open file <out.txt>");
        f << message << std::endl;
    }
};

int main() {
    try{
        auto i = std::make_unique<LogCommandTxt>("../out.txt");
        auto k = std::make_unique<LogCommandConsole>();
        i->print("zxc");
        k->print("zxc");
    }catch (const MyException &e){
        std::cout << e.what() << std::endl;
    }
}
