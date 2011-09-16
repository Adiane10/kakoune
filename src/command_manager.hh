#ifndef command_manager_hh_INCLUDED
#define command_manager_hh_INCLUDED

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <initializer_list>

#include "exception.hh"
#include "completion.hh"

namespace Kakoune
{

struct wrong_argument_count : runtime_error
{
    wrong_argument_count() : runtime_error("wrong argument count") {}
};

typedef std::vector<std::string> CommandParameters;
typedef std::function<void (const CommandParameters&)> Command;

typedef std::function<CandidateList (const CommandParameters&,
                                     size_t, size_t)> CommandCompleter;

class PerArgumentCommandCompleter
{
public:
    typedef std::function<CandidateList (const std::string&, size_t)> ArgumentCompleter;
    typedef std::vector<ArgumentCompleter> ArgumentCompleterList;

    PerArgumentCommandCompleter(const ArgumentCompleterList& completers)
        : m_completers(completers) {}

    PerArgumentCommandCompleter(ArgumentCompleterList&& completers)
        : m_completers(completers) {}

    PerArgumentCommandCompleter(std::initializer_list<ArgumentCompleter> completers)
        : m_completers(completers) {}

    CandidateList operator()(const CommandParameters& params,
                             size_t token_to_complete,
                             size_t pos_in_token) const;

private:
    ArgumentCompleterList m_completers;
};

class CommandManager
{
public:
    void execute(const std::string& command_line);
    Completions complete(const std::string& command_line, size_t cursor_pos);

    void register_command(const std::string& command_name,
                          Command command,
                          const CommandCompleter& completer = CommandCompleter());

    void register_command(const std::vector<std::string>& command_names,
                          Command command,
                          const CommandCompleter& completer = CommandCompleter());

private:
    struct CommandAndCompleter
    {
        Command command;
        CommandCompleter completer;
    };
    std::unordered_map<std::string, CommandAndCompleter> m_commands;
};

}

#endif // command_manager_hh_INCLUDED
