#ifndef TASK_H
#define TASK_H

/**
 * @file task.h
 * @brief Defines the Task class and related utilities for the TaskTracker application.
 * @details This file contains the main Task class, TaskStatus enum, concepts, error types,
 *          and utility functions for task management with modern C++23 features.
 */

#include <string>
#include <vector>
#include <chrono>
#include <optional>
#include <expected>
#include <format>
#include <print>
#include <concepts>
#include <ranges>
#include <algorithm>
#include <iostream>
#include <type_traits>
#include <compare>
#include <fstream>
#include <sstream>

/**
 * @enum TaskStatus
 * @brief Represents the status of a task in its lifecycle
 */
enum class TaskStatus : std::uint8_t {
    Pending,     ///< Task is yet to be started
    InProgress,  ///< Task is currently being worked on
    Completed,   ///< Task has been completed successfully
    Cancelled    ///< Task has been cancelled
};

/**
 * @concept Stringable
 * @brief Concept requiring a to_string method that returns a string
 * @details C++23 Feature: Improved concepts for more expressive code
 */
template<typename T>
concept Stringable = requires(T t) {
    { t.to_string() } -> std::convertible_to<std::string>;
};

/**
 * @concept TaskLike
 * @brief Concept defining requirements for a Task-like type
 * @details C++23 Feature: Concepts for constraints on template parameters
 */
template<typename T>
concept TaskLike = requires(T t) {
    { t.getId() } -> std::convertible_to<int>;
    { t.getTitle() } -> std::convertible_to<std::string>;
    { t.getStatus() } -> std::convertible_to<TaskStatus>;
};

/**
 * @enum TaskError
 * @brief Represents possible errors in task operations
 */
enum class TaskError {
    InvalidId,       ///< The task ID is invalid
    TaskNotFound,    ///< The specified task was not found
    InvalidStatus,   ///< The task status is invalid
    EmptyTitle,      ///< The task title cannot be empty
    DuplicateTask,   ///< A task with the same title already exists
    InvalidPriority  ///< The priority value is outside the valid range
};

/**
 * @enum JsonError
 * @brief Represents possible errors in JSON serialization/deserialization
 */
enum class JsonError {
    FileNotFound,   ///< The JSON file could not be opened
    InvalidFormat,  ///< The JSON file has an invalid format
    WriteError,     ///< Failed to write to the JSON file
    ParseError      ///< Failed to parse the JSON content
};

/**
 * @typedef TaskResult
 * @brief Result type for task operations that return success/failure
 * @details C++23 Feature: Uses std::expected for better error handling
 */
using TaskResult = std::expected<bool, TaskError>;

/**
 * @typedef TaskOptional
 * @brief Result type for operations that return a Task or an error
 */
using TaskOptional = std::expected<class Task, TaskError>;

/**
 * @typedef TaskAddResult
 * @brief Result type for task creation operations that return the new task's ID
 * @details C++23 Feature: Returns task ID on success
 */
using TaskAddResult = std::expected<int, TaskError>;

/**
 * @typedef JsonResult
 * @brief Result type for JSON operations
 */
using JsonResult = std::expected<bool, JsonError>;

/**
 * @struct TaskMetadata
 * @brief Metadata associated with a Task
 * @details Contains timestamps, category, and priority information
 */
struct TaskMetadata {
    std::chrono::system_clock::time_point created_at;    ///< When the task was created
    std::chrono::system_clock::time_point updated_at;    ///< When the task was last updated
    std::optional<std::chrono::system_clock::time_point> completed_at; ///< When the task was completed (if applicable)
    std::string category;   ///< The task's category
    int priority = 0;       ///< The task's priority (0-10)
    
    /**
     * @brief Creates a tuple of all member variables for structured comparisons
     * @return A tuple containing all metadata fields
     */
    auto tie() const {
        return std::tie(created_at, updated_at, completed_at, category, priority);
    }
    
    /**
     * @brief Equality comparison operator
     * @param other The metadata to compare with
     * @return true if the metadata objects are equal
     */
    bool operator==(const TaskMetadata& other) const {
        return created_at == other.created_at && 
               category == other.category && 
               priority == other.priority;
    }
    
    /**
     * @brief Less-than comparison operator
     * @param other The metadata to compare with
     * @return true if this metadata is less than the other
     */
    bool operator<(const TaskMetadata& other) const {
        if (created_at != other.created_at) return created_at < other.created_at;
        if (category != other.category) return category < other.category;
        return priority < other.priority;
    }
};

/**
 * @class Task
 * @brief Represents a task with its properties and metadata
 * @details Core class for the task tracking application, demonstrating C++23 features
 */
class Task {
private:
    int _id;               ///< Unique identifier for the task
    std::string _title;    ///< Title of the task
    std::string _description; ///< Detailed description of the task
    TaskStatus _status;    ///< Current status of the task
    TaskMetadata _metadata; ///< Metadata for the task
    
public:
    /**
     * @brief Constructs a new Task object
     * @param id Unique identifier for the task
     * @param title Title of the task
     * @param description Description of the task (optional)
     * @param status Initial status of the task (default: Pending)
     */
    Task(int id, std::string title, std::string description = "", 
         TaskStatus status = TaskStatus::Pending);
    
    /**
     * @brief Get the task's unique ID
     * @details C++23 Feature: Deducing this - eliminates const/non-const duplication
     * @tparam Self Deduced self type (const or non-const)
     * @return Reference to the task's ID
     */
    template<typename Self>
    auto getId(this Self&& self) -> decltype(auto) {
        return (std::forward<Self>(self)._id);
    }
    
    /**
     * @brief Get the task's title
     * @details C++23 Feature: Deducing this
     * @tparam Self Deduced self type (const or non-const)
     * @return Reference to the task's title
     */
    template<typename Self>
    auto getTitle(this Self&& self) -> decltype(auto) {
        return (std::forward<Self>(self)._title);
    }
    
    /**
     * @brief Get the task's description
     * @details C++23 Feature: Deducing this
     * @tparam Self Deduced self type (const or non-const)
     * @return Reference to the task's description
     */
    template<typename Self>
    auto getDescription(this Self&& self) -> decltype(auto) {
        return (std::forward<Self>(self)._description);
    }
    
    /**
     * @brief Get the task's status
     * @details C++23 Feature: Deducing this
     * @tparam Self Deduced self type (const or non-const)
     * @return Reference to the task's status
     */
    template<typename Self>
    auto getStatus(this Self&& self) -> decltype(auto) {
        return (std::forward<Self>(self)._status);
    }
    
    /**
     * @brief Get the task's metadata
     * @details C++23 Feature: Deducing this
     * @tparam Self Deduced self type (const or non-const)
     * @return Reference to the task's metadata
     */
    template<typename Self>
    auto getMetadata(this Self&& self) -> decltype(auto) {
        return (std::forward<Self>(self)._metadata);
    }
    
    /**
     * @brief Set the task's title
     * @param title New title for the task
     * @return TaskResult indicating success or the reason for failure
     */
    TaskResult setTitle(const std::string& title);
    
    /**
     * @brief Set the task's description
     * @param description New description for the task
     * @return TaskResult indicating success or the reason for failure
     */
    TaskResult setDescription(const std::string& description);
    
    /**
     * @brief Set the task's status
     * @param status New status for the task
     * @return TaskResult indicating success or the reason for failure
     */
    TaskResult setStatus(TaskStatus status);
    
    /**
     * @brief Set the task's priority
     * @param priority New priority for the task (0-10)
     * @return TaskResult indicating success or the reason for failure
     */
    TaskResult setPriority(int priority);
    
    /**
     * @brief Set the task's category
     * @param category New category for the task
     * @return TaskResult indicating success or the reason for failure
     */
    TaskResult setCategory(const std::string& category);
    
    /**
     * @brief Get a string representation of the task
     * @return Formatted string with task details
     */
    std::string to_string() const;
    
    /**
     * @brief Equality comparison operator
     * @param other The task to compare with
     * @return true if the tasks are equal
     */
    bool operator==(const Task& other) const {
        return _id == other._id && _title == other._title && _status == other._status;
    }
    
    /**
     * @brief Less-than comparison operator
     * @param other The task to compare with
     * @return true if this task is less than the other
     */
    bool operator<(const Task& other) const {
        if (_id != other._id) return _id < other._id;
        if (_title != other._title) return _title < other._title;
        return static_cast<int>(_status) < static_cast<int>(other._status);
    }
    
    /**
     * @brief Check if the task is completed
     * @return true if the task status is Completed
     */
    bool isCompleted() const;
    
    /**
     * @brief Get the age of the task
     * @return Duration since the task was created
     */
    std::chrono::duration<double> getAge() const;
    
    /**
     * @brief Mark the task as completed
     */
    void markCompleted();
    
    /**
     * @brief Convert the task to JSON format
     * @return JSON string representation of the task
     */
    std::string toJson() const;
    
    /**
     * @brief Create a Task from a JSON string
     * @param json_str JSON string to parse
     * @return Task object if successful, or JsonError if parsing failed
     */
    static std::expected<Task, JsonError> fromJson(const std::string& json_str);
};

/**
 * @class TaskManager
 * @brief Forward declaration for use in using declarations
 */
class TaskManager;


/**
 * @brief Convert a TaskStatus enum to its string representation
 * @param status The status to convert
 * @return String representation of the status
 */
std::string taskStatusToString(TaskStatus status);

/**
 * @brief Convert a string to a TaskStatus enum
 * @param str The string to convert
 * @return TaskStatus if valid, nullopt if invalid
 */
std::optional<TaskStatus> stringToTaskStatus(const std::string& str);

/**
 * @brief Convert a TaskError enum to its string representation
 * @param error The error to convert
 * @return String representation of the error
 */
std::string taskErrorToString(TaskError error);

/**
 * @brief Convert a JsonError enum to its string representation
 * @param error The error to convert
 * @return String representation of the error
 */
std::string jsonErrorToString(JsonError error);

/**
 * @brief Serialize a task to a JSON file
 * @param task The task to serialize
 * @param filePath Path to the output file
 * @return JsonResult indicating success or the reason for failure
 */
JsonResult serializeTaskToJson(const Task& task, const std::string& filePath);

/**
 * @brief Deserialize a task from a JSON file
 * @param task The task object to populate
 * @param filePath Path to the input file
 * @return JsonResult indicating success or the reason for failure
 */
JsonResult deserializeTaskFromJson(Task& task, const std::string& filePath);

/**
 * @brief Unescape a JSON string
 * @param str The JSON string to unescape
 * @return Unescaped string
 */
std::string unescapeJsonString(const std::string& str);

/**
 * @brief Validates priority value at compile time
 * @details C++23 Feature: consteval functions for compile-time validation
 * @param priority The priority value to validate
 * @return true if the priority is valid (0-10)
 */
consteval bool isValidPriority(int priority) {
    return priority >= 0 && priority <= 10;
}

/**
 * @brief Validates task ID at compile time
 * @details C++23 Feature: consteval function for compile-time validation
 * @param id The task ID to validate
 * @return true if the ID is valid (positive)
 */
consteval bool isValidTaskId(int id) {
    return id > 0;
}

/**
 * @brief Maximum allowed length for task titles
 * @details C++23 Feature: consteval and uz suffix for size_t
 * @return Maximum title length (100)
 */
consteval size_t maxTaskTitleLength() {
    return 100uz;  // C++23: uz suffix for size_t
}

/**
 * @brief Maximum allowed length for task descriptions
 * @details C++23 Feature: consteval and uz suffix for size_t
 * @return Maximum description length (500)
 */
consteval size_t maxTaskDescriptionLength() {
    return 500uz;
}

/**
 * @brief Get string representation of a task status
 * @details C++23 Feature: if consteval for optimized string processing with different
 *          behavior at compile-time vs runtime
 * @param status The task status to convert
 * @return String view representing the status
 */
constexpr std::string_view getTaskStatusString(TaskStatus status) {
    if consteval {
        // Compile-time: Simple lookup
        switch (status) {
            case TaskStatus::Pending: return "Pending";
            case TaskStatus::InProgress: return "InProgress";
            case TaskStatus::Completed: return "Completed";
            case TaskStatus::Cancelled: return "Cancelled";
        }
        return "Unknown";
    } else {
        // Runtime: More flexible processing
        switch (status) {
            case TaskStatus::Pending: return "⏳ Pending";
            case TaskStatus::InProgress: return "🚧 In Progress";
            case TaskStatus::Completed: return "✅ Completed";
            case TaskStatus::Cancelled: return "❌ Cancelled";
            default: return "❓ Unknown";
        }
    }
}

/**
 * @brief Validate task data fields
 * @details C++23 Feature: if consteval for performance optimization with different
 *          behavior at compile-time vs runtime
 * @param title The task title to validate
 * @param description The task description to validate
 * @param priority The priority value to validate
 * @return true if all data is valid
 */
constexpr bool validateTaskData(std::string_view title, std::string_view description, int priority) {
    if consteval {
        // Compile-time validation (basic checks)
        return !title.empty() && 
               title.length() <= maxTaskTitleLength() &&
               description.length() <= maxTaskDescriptionLength() &&
               priority >= 0 && priority <= 10;  // Direct check instead of consteval call
    } else {
        // Runtime validation (more comprehensive)
        if (title.empty()) return false;
        if (title.length() > maxTaskTitleLength()) return false;
        if (description.length() > maxTaskDescriptionLength()) return false;
        if (priority < 0 || priority > 10) return false;  // Direct check
        
        // Runtime-only checks
        if (title.front() == ' ' || title.back() == ' ') return false;
        
        return true;
    }
}

#endif // TASK_H