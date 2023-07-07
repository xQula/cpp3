#include <iostream>
#include <memory>
#include <fstream>
#include <exception>

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

class Observer{
public:
    virtual ~Observer() = default;
    virtual void onWarning(const std::string& message) {}
    virtual void onError(const std::string& message) {}
    virtual void onFatalError(const std::string& message) {}
};

class ObserverFatalError : public  Observer{
private:
    std::string way_txt_;
protected:
    void onError(const std::string& message) override {
        std::fstream f(way_txt_);
        if(!f.is_open())
            throw;
        f << message << std::endl;

    }
    void onWarning(const std::string& message) override {
        std::cout << message << std::endl;
    }
public:
    ~ObserverFatalError() override = default;
    ObserverFatalError() = default;
    explicit ObserverFatalError(const std::string& way_txt): way_txt_(way_txt){}
    void onFatalError(const std::string& message) override {
        onError(message);
        onWarning(message);
    }
};

class ObserverWarning : public ObserverFatalError{
public:
    ~ObserverWarning() override = default;
    void onWarning(const std::string& message) override{
        ObserverFatalError::onError(message);
    }
};

class ObserverError : public ObserverFatalError{
public:
    ~ObserverError() override = default;
    explicit ObserverError(const std::string& way_txt): ObserverFatalError(way_txt){}
    void onError(const std::string& message) override{
        ObserverFatalError::onError(message);
    }
};

class Observed{
private:
    std::weak_ptr<Observer> observer_;
public:
    Observed() = default;
    virtual  ~Observed() = default;
    void add_observer(const std::weak_ptr<Observer>& var){
        observer_ = var;
    }
    void warning(const std::string& message) const{
        if(auto strong_ptr = observer_.lock())
            strong_ptr->onWarning(message);
    }
    void error(const std::string& message) const{
        if(auto strong_ptr = observer_.lock())
            strong_ptr->onError(message);
    }
    void fatalError(const std::string& message) const{
        if(auto strong_ptr = observer_.lock())
            strong_ptr->onFatalError(message);
    }
};



int main() {
    try {
        auto st_ptr = std::make_shared<ObserverFatalError>("../out.txt");
        Observed ob;
        ob.add_observer(st_ptr);
        ob.fatalError("asd");
    }catch (const MyException &e){
        std::cout << e.what() << std::endl;
    }
}
