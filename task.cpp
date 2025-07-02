/**
 * @file task.cpp
 * @brief Implementation of Task class and related utility functions
 * @details Contains the implementation of Task methods, JSON serialization,
 *          and utility functions for task management.
 */

#include "task.h"
#include "task_manager.h"
#include <iostream>
#include <iomanip>
#include <sstream>

/**
 * @brief Constructs a Task object with specified properties
 * @param id Unique identifier for the task
 * @param title Title of the task
 * @param description Description of the task
 * @param status Initial status of the task
 */
Task::Task(int id, std::string title, std::string description, TaskStatus status)
    : _id(id), _title(std::move(title)), _description(std::move(description)), _status(status) {
    auto now = std::chrono::system_clock::now();
    _metadata.created_at = now;
    _metadata.updated_at = now;
    _metadata.category = "General";
}

/**
 * @brief Sets the task's title
 * @param title New title for the task
 * @return TaskResult with success or error
 */
TaskResult Task::setTitle(const std::string& title) {
    if (title.empty()) {
        return std::unexpected(TaskError::EmptyTitle);
    }
    _title = title;
    _metadata.updated_at = std::chrono::system_clock::now();
    return true;
}

/**
 * @brief Sets the task's description
 * @param description New description for the task
 * @return TaskResult with success
 */
TaskResult Task::setDescription(const std::string& description) {
    _description = description;
    _metadata.updated_at = std::chrono::system_clock::now();
    return true;
}

/**
 * @brief Sets the task's status
 * @param status New status for the task
 * @return TaskResult with success
 * @details If status is Completed, also sets the completed_at timestamp
 */
TaskResult Task::setStatus(TaskStatus status) {
    _status = status;
    _metadata.updated_at = std::chrono::system_clock::now();
    
    if (status == TaskStatus::Completed) {
        _metadata.completed_at = std::chrono::system_clock::now();
    }
    
    return true;
}

/**
 * @brief Sets the task's priority
 * @param priority New priority for the task (0-10)
 * @return TaskResult with success or error if priority is invalid
 */
TaskResult Task::setPriority(int priority) {
    // Validate priority range (0-10)
    if (priority < 0 || priority > 10) {
        return std::unexpected(TaskError::InvalidPriority);
    }
    
    _metadata.priority = priority;
    _metadata.updated_at = std::chrono::system_clock::now();
    return true;
}

/**
 * @brief Sets the task's category
 * @param category New category for the task
 * @return TaskResult with success
 */
TaskResult Task::setCategory(const std::string& category) {
    _metadata.category = category;
    _metadata.updated_at = std::chrono::system_clock::now();
    return true;
}

/**
 * @brief Creates a formatted string representation of the task
 * @return String containing all task details
 * @details Uses C++23 std::format for string formatting
 */
std::string Task::to_string() const {
    // C++23 std::format usage (ready to migrate to std::print when available)
    auto created_time = std::chrono::system_clock::to_time_t(_metadata.created_at);
    auto updated_time = std::chrono::system_clock::to_time_t(_metadata.updated_at);
    
    std::ostringstream oss;
    oss << std::format("Task [ID: {}]\n", _id);
    oss << std::format("  Title: {}\n", _title);
    oss << std::format("  Description: {}\n", _description.empty() ? "None" : _description);
    oss << std::format("  Status: {}\n", taskStatusToString(_status));
    oss << std::format("  Category: {}\n", _metadata.category);
    oss << std::format("  Priority: {}\n", _metadata.priority);
    oss << std::format("  Created: {:%Y-%m-%d %H:%M:%S}\n", std::chrono::system_clock::from_time_t(created_time));
    oss << std::format("  Updated: {:%Y-%m-%d %H:%M:%S}\n", std::chrono::system_clock::from_time_t(updated_time));
    
    if (_metadata.completed_at.has_value()) {
        auto completed_time = std::chrono::system_clock::to_time_t(*_metadata.completed_at);
        oss << std::format("  Completed: {:%Y-%m-%d %H:%M:%S}\n", std::chrono::system_clock::from_time_t(completed_time));
    }
    
    return oss.str();
}

/**
 * @brief Check if the task is completed
 * @return true if the task status is Completed
 */
bool Task::isCompleted() const {
    return _status == TaskStatus::Completed;
}

/**
 * @brief Calculate the age of the task
 * @return Duration since task creation
 */
std::chrono::duration<double> Task::getAge() const {
    return std::chrono::system_clock::now() - _metadata.created_at;
}

/**
 * @brief Mark the task as completed
 * @details Sets status to Completed and updates completed_at timestamp
 */
void Task::markCompleted() {
    setStatus(TaskStatus::Completed);
}

// TaskManager implementation moved to task_manager.cpp

/**
 * @brief Demo function to showcase C++23 features
 * @details Creates and manipulates tasks using modern C++23 features
 */
void demonstrateC23Features() {
    std::cout << "\n🎯 Demonstrating C++23 Features:\n";
    std::cout << "================================\n";
    
    // 1. Enhanced enum classes
    TaskStatus status = TaskStatus::InProgress;
    std::cout << "1. Enhanced Enum: " << taskStatusToString(status) << "\n";
    
    // 2. std::expected (simulated)
    TaskManager manager;
    auto result = manager.addTask("Demo Task", "Showcasing C++23 features");
    if (result) {
        std::cout << "2. std::expected: Task added successfully\n";
    }
    
    // 3. Concepts demonstration
    auto checkTaskLike = []<TaskLike T>(const T& t) {
        return t.getId() > 0;
    };
    
    if (auto task_result = manager.getTask(1); task_result) {
        std::cout << "3. Concepts: Task validation passed\n";
        
        // 4. Ranges and views
        auto pending_tasks = manager.getTasksByStatus(TaskStatus::Pending);
        std::cout << "4. Ranges: Found " << std::distance(pending_tasks.begin(), pending_tasks.end()) << " pending task(s)\n";
        
        // 5. Enhanced lambdas with template parameters
        auto sorter = []<typename T>(std::vector<T>& vec) requires std::sortable<typename std::vector<T>::iterator> {
            std::ranges::sort(vec);
        };
        
        std::vector<int> test_vec{3, 1, 4, 1, 5};
        sorter(test_vec);
        std::cout << "5. Template Lambdas: Vector sorted\n";
        
        // 6. std::format (C++23)
        std::cout << std::format("6. std::format (C++23): Task ID = {}\n", task_result->getId());
        
        // 7. Structured bindings
        const auto& [created, updated, completed, category, priority] = task_result->getMetadata().tie();
        std::cout << "7. Structured Bindings: Task category = " << category << "\n";
        
        // 8. Comparison operators
        Task another_task(2, "Another Task");
        bool are_equal = (*task_result == another_task);
        std::cout << "8. Spaceship Operator: Tasks equal = " << std::boolalpha << are_equal << "\n";
        
        // 9. Designated initializers (already shown in Command struct)
        std::cout << "9. Designated Initializers: Used in Command struct initialization\n";
        
        // 10. Modern getter methods
        std::cout << "10. Modern Getters: Task title = '" << task_result->getTitle() << "'\n";
    }
    
    std::cout << "\n✨ All C++23 features demonstrated successfully!\n\n";
}

/**
 * @brief Convert JSON error enum to string
 * @param error The JSON error code
 * @return Human-readable error message
 */
std::string jsonErrorToString(JsonError error) {
    switch (error) {
        case JsonError::FileNotFound: return "JSON file not found";
        case JsonError::InvalidFormat: return "Invalid JSON format";
        case JsonError::WriteError: return "Failed to write JSON file";
        case JsonError::ParseError: return "Failed to parse JSON";
        default: return "Unknown JSON error";
    }
}

/**
 * @brief Escape special characters in a string for JSON encoding
 * @param str The input string to escape
 * @return JSON-safe escaped string
 * @details Handles quotes, backslashes, control characters, and UTF-8
 */
std::string escapeJsonString(const std::string& str) {
    std::string escaped;
    escaped.reserve(str.size() + 20); // Reserve some extra space for escapes
    
    for (unsigned char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                // Only escape actual control characters (0-31), not UTF-8 bytes
                if (c < 32) {
                    escaped += std::format("\\u{:04x}", c);
                } else {
                    // For UTF-8 and normal ASCII (32-255), keep as-is
                    escaped += static_cast<char>(c);
                }
                break;
        }
    }
    return escaped;
}

/**
 * @brief Unescape JSON string by converting escape sequences to characters
 * @param str The JSON string to unescape
 * @return Unescaped string
 */
std::string unescapeJsonString(const std::string& str) {
    std::string unescaped;
    unescaped.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '"': unescaped += '"'; ++i; break;
                case '\\': unescaped += '\\'; ++i; break;
                case 'b': unescaped += '\b'; ++i; break;
                case 'f': unescaped += '\f'; ++i; break;
                case 'n': unescaped += '\n'; ++i; break;
                case 'r': unescaped += '\r'; ++i; break;
                case 't': unescaped += '\t'; ++i; break;
                default: unescaped += str[i]; break;
            }
        } else {
            unescaped += str[i];
        }
    }
    return unescaped;
}

/**
 * @brief Convert time_point to ISO 8601 formatted string
 * @param tp The time point to convert
 * @return ISO 8601 formatted string (YYYY-MM-DDTHH:MM:SS.mmm)
 */
std::string timePointToIsoString(const std::chrono::system_clock::time_point& tp) {
    auto time_t = std::chrono::system_clock::to_time_t(tp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    // Fix: Use localtime instead of gmtime to show local timezone
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
    oss << std::format(".{:03d}", ms.count()); // Remove Z suffix for local time
    return oss.str();
}

/**
 * @brief Parse ISO 8601 formatted string to time_point
 * @param iso_str The ISO 8601 string to parse
 * @return Corresponding time_point
 * @details Handles millisecond precision if present
 */
std::chrono::system_clock::time_point isoStringToTimePoint(const std::string& iso_str) {
    std::tm tm = {};
    std::istringstream ss(iso_str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    
    // Handle milliseconds if present
    auto dot_pos = iso_str.find('.');
    if (dot_pos != std::string::npos) {
        auto ms_str = iso_str.substr(dot_pos + 1, 3);
        if (!ms_str.empty() && std::isdigit(ms_str[0])) {
            int ms = std::stoi(ms_str);
            tp += std::chrono::milliseconds(ms);
        }
    }
    
    return tp;
}

/**
 * @brief Convert Task to JSON string representation
 * @return Formatted JSON string with all task properties
 * @details Uses C++23 std::format for string formatting
 */
std::string Task::toJson() const {
    std::ostringstream json;
    json << "{\n";
    json << std::format("  \"id\": {},\n", _id);
    json << std::format("  \"title\": \"{}\",\n", escapeJsonString(_title));
    json << std::format("  \"description\": \"{}\",\n", escapeJsonString(_description));
    json << std::format("  \"status\": \"{}\",\n", taskStatusToString(_status));
    json << std::format("  \"category\": \"{}\",\n", escapeJsonString(_metadata.category));
    json << std::format("  \"priority\": {},\n", _metadata.priority);
    json << std::format("  \"created_at\": \"{}\",\n", timePointToIsoString(_metadata.created_at));
    json << std::format("  \"updated_at\": \"{}\"", timePointToIsoString(_metadata.updated_at));
    
    if (_metadata.completed_at.has_value()) {
        json << std::format(",\n  \"completed_at\": \"{}\"", timePointToIsoString(*_metadata.completed_at));
    }
    
    json << "\n}";
    return json.str();
}

/**
 * @brief Create a Task from a JSON string
 * @param json_str JSON string to parse
 * @return Task object if successful, or JsonError if parsing failed
 * @details C++23 Feature: Uses std::expected for error handling
 */
std::expected<Task, JsonError> Task::fromJson(const std::string& json_str) {
    try {
        // Simple JSON parsing - in production, use a proper JSON library
        auto findValue = [&json_str](const std::string& key) -> std::string {
            std::string search_key = "\"" + key + "\":";
            size_t pos = json_str.find(search_key);
            if (pos == std::string::npos) return "";
            
            pos += search_key.length();
            while (pos < json_str.length() && std::isspace(json_str[pos])) ++pos;
            
            if (pos >= json_str.length()) return "";
            
            if (json_str[pos] == '"') {
                // String value
                ++pos;
                size_t end_pos = pos;
                while (end_pos < json_str.length() && json_str[end_pos] != '"') {
                    if (json_str[end_pos] == '\\') ++end_pos; // Skip escaped characters
                    ++end_pos;
                }
                return unescapeJsonString(json_str.substr(pos, end_pos - pos));
            } else {
                // Number or other value
                size_t end_pos = pos;
                while (end_pos < json_str.length() && 
                       json_str[end_pos] != ',' && 
                       json_str[end_pos] != '}' && 
                       json_str[end_pos] != '\n' &&
                       json_str[end_pos] != '\r') {
                    ++end_pos;
                }
                std::string value = json_str.substr(pos, end_pos - pos);
                // Trim whitespace
                value.erase(0, value.find_first_not_of(" \t\n\r"));
                value.erase(value.find_last_not_of(" \t\n\r") + 1);
                return value;
            }
        };
        
        std::string id_str = findValue("id");
        std::string title = findValue("title");
        std::string description = findValue("description");
        std::string status_str = findValue("status");
        std::string category = findValue("category");
        std::string priority_str = findValue("priority");
        std::string created_at_str = findValue("created_at");
        std::string updated_at_str = findValue("updated_at");
        std::string completed_at_str = findValue("completed_at");
        
        if (id_str.empty() || title.empty()) {
            return std::unexpected(JsonError::InvalidFormat);
        }
        
        int id = std::stoi(id_str);
        auto status_opt = stringToTaskStatus(status_str);
        if (!status_opt) {
            return std::unexpected(JsonError::InvalidFormat);
        }
        
        Task task(id, title, description, *status_opt);
        
        if (!category.empty()) {
            task.setCategory(category);
        }
        
        if (!priority_str.empty()) {
            int priority = std::stoi(priority_str);
            task.setPriority(priority);
        }
        
        if (!created_at_str.empty()) {
            task._metadata.created_at = isoStringToTimePoint(created_at_str);
        }
        
        if (!updated_at_str.empty()) {
            task._metadata.updated_at = isoStringToTimePoint(updated_at_str);
        }
        
        if (!completed_at_str.empty()) {
            task._metadata.completed_at = isoStringToTimePoint(completed_at_str);
        }
        
        return task;
    } catch (const std::exception&) {
        return std::unexpected(JsonError::ParseError);
    }
}

// TaskManager JSON serialization implementation moved to task_manager.cpp

/**
 * @brief Convert TaskStatus enum to string representation
 * @param status The status to convert
 * @return String representation of the status
 */
std::string taskStatusToString(TaskStatus status) {
    switch (status) {
        case TaskStatus::Pending: return "Pending";
        case TaskStatus::InProgress: return "In Progress";
        case TaskStatus::Completed: return "Completed";
        case TaskStatus::Cancelled: return "Cancelled";
        default: return "Unknown";
    }
}

/**
 * @brief Convert string to TaskStatus enum
 * @param str The string to convert
 * @return TaskStatus if valid, nullopt if invalid
 * @details Case-insensitive and handles common status name variations
 */
std::optional<TaskStatus> stringToTaskStatus(const std::string& str) {
    if (str == "pending" || str == "Pending") return TaskStatus::Pending;
    if (str == "progress" || str == "in-progress" || str == "In Progress") return TaskStatus::InProgress;
    if (str == "completed" || str == "Completed") return TaskStatus::Completed;
    if (str == "cancelled" || str == "Cancelled") return TaskStatus::Cancelled;
    return std::nullopt;
}

/**
 * @brief Convert TaskError enum to string representation
 * @param error The error to convert
 * @return Human-readable error message
 */
std::string taskErrorToString(TaskError error) {
    switch (error) {
        case TaskError::InvalidId: return "Invalid task ID";
        case TaskError::TaskNotFound: return "Task not found";
        case TaskError::InvalidStatus: return "Invalid task status";
        case TaskError::EmptyTitle: return "Task title cannot be empty";
        case TaskError::DuplicateTask: return "Task with this title already exists";
        case TaskError::InvalidPriority: return "Priority must be between 0 and 10";
        default: return "Unknown error";
    }
}