#include <rubiks/command.hxx>
#include <rubiks/glog.hxx>
#include <memory>
#include <iostream>

Command::Command() {};

Command::Command(CommandFunction func):
m_func(func){}

void Command::execute(const std::vector<std::string> &args) {
    if(m_func) {
        m_func(args);
    }
}
Command& Command::with_description(std::string desc) {
    m_description = desc;
    return *this;
}

std::string Command::description() const {
    return m_description;
}
