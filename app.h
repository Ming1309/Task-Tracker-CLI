#pragma once

#include "task.h"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <compare>
#include <expected>

class App {
private:
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
    
    // Utility methods
    std::vector<std::string> parseInput(const std::string& input) const;
    void handleError(TaskError error) const;
    void handleJsonError(JsonError error) const;
    void printTaskDetails(const Task& task) const;
    
    // C++23: std::expected-based parsing utilities
    enum class ParseError {
        InvalidFormat,
        OutOfRange,
        Empty
    };
    
    std::expected<int, ParseError> parseInteger(const std::string& str) const;
    std::string parseErrorToString(ParseError error) const;
    
public:
    App();
    void config();
    void run();
};