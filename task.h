#pragma once

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

enum class TaskStatus : std::uint8_t {
    Pending,
    InProgress, 
    Completed,
    Cancelled
};

template<typename T>
concept Stringable = requires(T t) {
    { t.to_string() } -> std::convertible_to<std::string>;
};

template<typename T>
concept TaskLike = requires(T t) {
    { t.getId() } -> std::convertible_to<int>;
    { t.getTitle() } -> std::convertible_to<std::string>;
    { t.getStatus() } -> std::convertible_to<TaskStatus>;
};

enum class TaskError {
    InvalidId,
    TaskNotFound,
    InvalidStatus,
    EmptyTitle,
    DuplicateTask,
    InvalidPriority
};

// JSON serialization error handling
enum class JsonError {
    FileNotFound,
    InvalidFormat,
    WriteError,
    ParseError
};

using TaskResult = std::expected<bool, TaskError>;
using TaskOptional = std::expected<class Task, TaskError>;
using TaskAddResult = std::expected<int, TaskError>;  // C++23: Returns task ID on success
using JsonResult = std::expected<bool, JsonError>;

struct TaskMetadata {
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
    std::optional<std::chrono::system_clock::time_point> completed_at;
    std::string category;
    int priority = 0;
    
    auto tie() const {
        return std::tie(created_at, updated_at, completed_at, category, priority);
    }
    
    // Simpler comparison for compatibility
    bool operator==(const TaskMetadata& other) const {
        return created_at == other.created_at && 
               category == other.category && 
               priority == other.priority;
    }
    
    bool operator<(const TaskMetadata& other) const {
        if (created_at != other.created_at) return created_at < other.created_at;
        if (category != other.category) return category < other.category;
        return priority < other.priority;
    }
};

class Task {
private:
    int _id;
    std::string _title;
    std::string _description;
    TaskStatus _status;
    TaskMetadata _metadata;
    
public:
    Task(int id, std::string title, std::string description = "", 
         TaskStatus status = TaskStatus::Pending);
    
    // C++23: Deducing this - eliminates const/non-const duplication
    template<typename Self>
    auto getId(this Self&& self) -> decltype(auto) {
        return (std::forward<Self>(self)._id);
    }
    
    template<typename Self>
    auto getTitle(this Self&& self) -> decltype(auto) {
        return (std::forward<Self>(self)._title);
    }
    
    template<typename Self>
    auto getDescription(this Self&& self) -> decltype(auto) {
        return (std::forward<Self>(self)._description);
    }
    
    template<typename Self>
    auto getStatus(this Self&& self) -> decltype(auto) {
        return (std::forward<Self>(self)._status);
    }
    
    template<typename Self>
    auto getMetadata(this Self&& self) -> decltype(auto) {
        return (std::forward<Self>(self)._metadata);
    }
    
    // Setters with validation
    TaskResult setTitle(const std::string& title);
    TaskResult setDescription(const std::string& description);
    TaskResult setStatus(TaskStatus status);
    TaskResult setPriority(int priority);
    TaskResult setCategory(const std::string& category);
    
    std::string to_string() const;
    
    bool operator==(const Task& other) const {
        return _id == other._id && _title == other._title && _status == other._status;
    }
    
    bool operator<(const Task& other) const {
        if (_id != other._id) return _id < other._id;
        if (_title != other._title) return _title < other._title;
        return static_cast<int>(_status) < static_cast<int>(other._status);
    }
    
    // Utility methods
    bool isCompleted() const;
    std::chrono::duration<double> getAge() const;
    void markCompleted();
    
    // JSON serialization methods
    std::string toJson() const;
    static std::expected<Task, JsonError> fromJson(const std::string& json_str);
};

// Forward declare TaskManager for use in using declarations
class TaskManager;

// Function declaration
void demonstrateC23Features();

// Utility functions for TaskStatus
std::string taskStatusToString(TaskStatus status);
std::optional<TaskStatus> stringToTaskStatus(const std::string& str);
std::string taskErrorToString(TaskError error);
std::string jsonErrorToString(JsonError error);

// JSON serialization functions
JsonResult serializeTaskToJson(const Task& task, const std::string& filePath);
JsonResult deserializeTaskFromJson(Task& task, const std::string& filePath);

// JSON utility functions
std::string unescapeJsonString(const std::string& str);

// C++23 Feature: consteval functions for compile-time validation
consteval bool isValidPriority(int priority) {
    return priority >= 0 && priority <= 10;
}

consteval bool isValidTaskId(int id) {
    return id > 0;
}

consteval size_t maxTaskTitleLength() {
    return 100uz;  // C++23: uz suffix for size_t
}

consteval size_t maxTaskDescriptionLength() {
    return 500uz;
}

// C++23 Feature: if consteval for optimized string processing
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

// C++23 Feature: if consteval for performance optimization
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