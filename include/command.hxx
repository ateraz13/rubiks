#ifndef COMMAND_HXX
#define COMMAND_HXX
#include "action.hxx"
#include <functional>
#include <memory>
#include <string>
#include <vector>

using CommandFunction =
    std::function<void(const std::vector<std::string> &args)>;

class Command {
public:
  Command();
  Command(CommandFunction func);
  void execute(const std::vector<std::string> &args);

  Command &with_description(std::string);
  std::string description() const;

protected:
  CommandFunction m_func;

private:
  std::string m_description = "";
};

enum ConsoleOperation {

};

class CommandInterpreter {

};

#endif // COMMAND_HXX
