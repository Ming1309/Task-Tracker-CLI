#ifndef APP_H
#define APP_H

#include "task.h"
#include "task_manager.h"
#include "task_matrix.h"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <format>
#include <print>
#include <compare>
#include <expected>

/**
 * @brief Maximum number of arguments for a command
 * @details C++23: uz suffix for size_t constants
 */
constexpr auto MAX_COMMAND_ARGS = 5uz;

/**
 * @brief Minimum length for a valid command
 */
constexpr auto MIN_COMMAND_LENGTH = 1uz;

/**
 * @brief Maximum allowed input length
 */
constexpr auto MAX_INPUT_LENGTH = 1000uz;

/**
 * @class App
 * @brief Main application class for the Task Tracker CLI
 * @details Manages the user interface, command processing, and task operations
 */
class App {
private:
    /**
     * @struct TaskInfo
     * @brief Helper structure for table display of task information
     * @details Contains simplified task data for UI presentation
     */
    struct TaskInfo {
        int id;                 /**< Task identifier */
        std::string title;      /**< Task title */
        std::string status;     /**< Task status as string */
        std::string category;   /**< Task category */
        int priority;           /**< Task priority level */
        std::string created_at; /**< Creation timestamp */
        std::string description; /**< Task description */
    };

    TaskManager _task_manager;  /**< Task manager instance */
    bool _running;              /**< Application running state */
    
    /**
     * @struct Command
     * @brief Command structure using modern C++23 features
     * @details Defines a CLI command with its handler and metadata
     */
    struct Command {
        std::string name;       /**< Command name */
        std::string description; /**< Command description */
        std::function<void(const std::vector<std::string>&)> handler; /**< Command handler function */
        size_t min_args;        /**< Minimum required arguments */
        size_t max_args;        /**< Maximum allowed arguments */
        
        /**
         * @brief Equality comparison operator
         * @param other Another Command to compare with
         * @return true if commands have the same name
         */
        bool operator==(const Command& other) const {
            return name == other.name;
        }
        
        /**
         * @brief Less-than comparison operator
         * @param other Another Command to compare with
         * @return true if this command's name comes before other's name
         */
        bool operator<(const Command& other) const {
            return name < other.name;
        }
    };
    
    std::unordered_map<std::string, Command> _commands; /**< Registered commands map */
    
    /**
     * @brief TaskMatrix for multidimensional task access
     * @details C++23: Uses flat_map for better cache locality
     */
    TaskMatrix _task_matrix;
    
    /**
     * @brief Maximum number of recent commands to store
     * @details C++23: uz suffix for size_t constant
     */
    static constexpr auto MAX_RECENT_COMMANDS = 10uz;
    
    std::vector<std::string> _recent_commands; /**< History of recent commands */
    
    /**
     * @brief Initialize all command handlers
     */
    void initializeCommands();
    
    /**
     * @brief Display help information to the user
     */
    void displayHelp() const;
    
    /**
     * @brief Display welcome message on application startup
     */
    void displayWelcome() const;
    
    /**
     * @name Command Handlers
     * @brief Methods for processing different CLI commands
     */
    ///@{
    /**
     * @brief Handle the 'add' command to create new tasks
     * @param args Command arguments (task title, description)
     */
    void handleAdd(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'list' command to display tasks
     * @param args Command arguments (optional status filter)
     */
    void handleList(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'complete' command to mark tasks as completed
     * @param args Command arguments (task ID)
     */
    void handleComplete(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'remove' command to delete tasks
     * @param args Command arguments (task ID)
     */
    void handleRemove(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'status' command to update task status
     * @param args Command arguments (task ID, new status)
     */
    void handleStatus(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'priority' command to set task priority
     * @param args Command arguments (task ID, priority level)
     */
    void handlePriority(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'category' command to set task category
     * @param args Command arguments (task ID, category name)
     */
    void handleCategory(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'stats' command to display task statistics
     * @param args Command arguments (none)
     */
    void handleStats(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'find' command to search for tasks
     * @param args Command arguments (search keyword)
     */
    void handleFind(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'sort' command to sort tasks by criteria
     * @param args Command arguments (sort criterion)
     */
    void handleSort(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'exit' command to quit the application
     * @param args Command arguments (none)
     */
    void handleExit(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'save' command to save tasks to JSON
     * @param args Command arguments (filename)
     */
    void handleSave(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'load' command to load tasks from JSON
     * @param args Command arguments (filename)
     */
    void handleLoad(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'view' command to display task JSON data
     * @param args Command arguments (filename)
     */
    void handleView(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'matrix' command to show task organization
     * @details C++23: Uses flat_map for displaying task matrix
     * @param args Command arguments (none)
     */
    void handleMatrix(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'get' command to retrieve specific task
     * @details C++23: Uses deducing this for accessing tasks
     * @param args Command arguments (task ID)
     */
    void handleGet(const std::vector<std::string>& args);
    
    /**
     * @brief Handle the 'recent' command to show command history
     * @param args Command arguments (none)
     */
    void handleRecent(const std::vector<std::string>& args);
    ///@}
    
    /**
     * @name Utility Methods
     * @brief Helper functions for command processing and UI
     */
    ///@{
    /**
     * @brief Parse user input into command and arguments
     * @param input Raw input string from the user
     * @return Vector of parsed command components
     */
    std::vector<std::string> parseInput(const std::string& input) const;
    
    /**
     * @brief Handle and display task-related errors
     * @param error The task error that occurred
     */
    void handleError(TaskError error) const;
    
    /**
     * @brief Handle and display JSON-related errors
     * @param error The JSON error that occurred
     */
    void handleJsonError(JsonError error) const;
    
    /**
     * @brief Print detailed information about a task
     * @param task The task to display
     */
    void printTaskDetails(const Task& task) const;
    ///@}
    
    /**
     * @name Table Display Utilities
     * @brief Functions for formatted table output
     */
    ///@{
    /**
     * @brief Display JSON content in tabular format
     * @param json_content Raw JSON string to display
     */
    void displayJsonAsTable(const std::string& json_content);
    
    /**
     * @brief Parse task data from JSON into TaskInfo structures
     * @param json_content Raw JSON string to parse
     * @return Vector of TaskInfo objects
     */
    std::vector<TaskInfo> parseTasksFromJson(const std::string& json_content);
    ///@}
    
    /**
     * @name File I/O Utilities
     * @brief File operations with error handling
     */
    ///@{
    /**
     * @brief Read content from a file with error handling
     * @details C++23: Uses std::expected for functional error handling
     * @param filename Name of file to read
     * @return Content or error code
     */
    std::expected<std::string, JsonError> readFileContent(const std::string& filename) const;
    ///@}
    
    /**
     * @enum ParseError
     * @brief Error types for parsing operations
     */
    enum class ParseError {
        InvalidFormat, /**< Input has invalid format */
        OutOfRange,    /**< Value is outside allowed range */
        Empty          /**< Input is empty */
    };
    
    /**
     * @name Parsing Utilities
     * @brief Input parsing with error handling
     */
    ///@{
    /**
     * @brief Parse string to integer with error handling
     * @details C++23: Uses std::expected for functional error handling
     * @param str String to parse
     * @return Integer value or error code
     */
    std::expected<int, ParseError> parseInteger(const std::string& str) const;
    
    /**
     * @brief Convert parse error to human-readable string
     * @param error Parse error code
     * @return Error description
     */
    std::string parseErrorToString(ParseError error) const;
    ///@}
    
    /**
     * @brief Validate if a command exists and is properly formatted
     * @details C++23: Uses .contains() method for cleaner validation
     * @param cmd Command string to validate
     * @return true if command is valid
     */
    bool validateCommand(const std::string& cmd) const {
        return !cmd.empty() && 
               cmd.length() >= MIN_COMMAND_LENGTH &&
               _commands.contains(cmd);
    }
    
public:
    /**
     * @brief Constructor for the App class
     */
    App();
    
    /**
     * @brief Configure the application before running
     * @details Initializes commands and settings
     */
    void config();
    
    /**
     * @brief Run the application main loop
     * @details Handles input and command execution
     */
    void run();
};

#endif // APP_H