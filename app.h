#pragma once

#include "task.h"
#include "task_matrix.h"  // C++23: Multidimensional subscript support
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <format>
#include <print>
#include <compare>
#include <expected>

// C++23: uz suffix for size_t constants
constexpr auto MAX_COMMAND_ARGS = 5uz;
constexpr auto MIN_COMMAND_LENGTH = 1uz;
constexpr auto MAX_INPUT_LENGTH = 1000uz;

class App {
private:
    // Helper struct for table display
    struct TaskInfo {
        int id;
        std::string title;
        std::string status;
        std::string category;
        int priority;
        std::string created_at;
        std::string description;
    };

    TaskManager _task_manager;
    bool _running;
    
    // Command structure using modern C++23 features
    struct Command {
        std::string name;
        std::string description;
        std::function<void(const std::vector<std::string>&)> handler;
        size_t min_args;
        size_t max_args;
        
        // Simpler comparison for compatibility
        bool operator==(const Command& other) const {
            return name == other.name;
        }
        
        bool operator<(const Command& other) const {
            return name < other.name;
        }
    };
    
    std::unordered_map<std::string, Command> _commands;
    
    // C++23: TaskMatrix for multidimensional access
    TaskMatrix _task_matrix;
    
    // C++23: uz suffix for constants
    static constexpr auto MAX_RECENT_COMMANDS = 10uz;
    std::vector<std::string> _recent_commands;
    
    void initializeCommands();
    void displayHelp() const;
    void displayWelcome() const;
    
    // Command handlers
    void handleAdd(const std::vector<std::string>& args);
    void handleList(const std::vector<std::string>& args);
    void handleComplete(const std::vector<std::string>& args);
    void handleRemove(const std::vector<std::string>& args);
    void handleStatus(const std::vector<std::string>& args);
    void handlePriority(const std::vector<std::string>& args);
    void handleCategory(const std::vector<std::string>& args);
    void handleStats(const std::vector<std::string>& args);
    void handleFind(const std::vector<std::string>& args);
    void handleSort(const std::vector<std::string>& args);
    void handleExit(const std::vector<std::string>& args);
    void handleSave(const std::vector<std::string>& args);
    void handleLoad(const std::vector<std::string>& args);
    void handleView(const std::vector<std::string>& args);
    
    // C++23: New handlers using advanced features
    void handleMatrix(const std::vector<std::string>& args);
    void handleGet(const std::vector<std::string>& args);
    void handleRecent(const std::vector<std::string>& args);
    
    // Utility methods
    std::vector<std::string> parseInput(const std::string& input) const;
    void handleError(TaskError error) const;
    void handleJsonError(JsonError error) const;
    void printTaskDetails(const Task& task) const;
    
    // Table display utilities
    void displayJsonAsTable(const std::string& json_content);
    std::vector<TaskInfo> parseTasksFromJson(const std::string& json_content);
    
    // File I/O utilities using C++23 std::expected
    std::expected<std::string, JsonError> readFileContent(const std::string& filename) const;
    
    // C++23: std::expected-based parsing utilities
    enum class ParseError {
        InvalidFormat,
        OutOfRange,
        Empty
    };
    
    std::expected<int, ParseError> parseInteger(const std::string& str) const;
    std::string parseErrorToString(ParseError error) const;
    
    // C++23: Enhanced command validation
    bool validateCommand(const std::string& cmd) const {
        return !cmd.empty() && 
               cmd.length() >= MIN_COMMAND_LENGTH &&
               _commands.contains(cmd);  // C++23: .contains() method
    }
    
public:
    App();
    void config();
    void run();
};