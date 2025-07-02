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

// C++23 Technique 1: Enhanced enum classes with underlying type deduction
enum class TaskStatus : std::uint8_t {
    Pending,
    InProgress, 
    Completed,
    Cancelled
};

// C++23 Technique 2: Concepts for type constraints
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

// C++23 Technique 3: std::expected for error handling
enum class TaskError {
    InvalidId,
    TaskNotFound,
    InvalidStatus,
    EmptyTitle,
    DuplicateTask
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
using JsonResult = std::expected<bool, JsonError>;

// C++23 Technique 4: Enhanced structured bindings with pack expansion
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
    // C++23 Technique 5: Defaulted comparison operators with auto
    Task(int id, std::string title, std::string description = "", 
         TaskStatus status = TaskStatus::Pending);
    
    // C++23 Technique 6: Modern getter methods (explicit object parameter simulation)
    int getId() const { return _id; }
    const std::string& getTitle() const { return _title; }
    const std::string& getDescription() const { return _description; }
    TaskStatus getStatus() const { return _status; }
    const TaskMetadata& getMetadata() const { return _metadata; }
    
    // Non-const versions
    int& getId() { return _id; }
    std::string& getTitle() { return _title; }
    std::string& getDescription() { return _description; }
    TaskStatus& getStatus() { return _status; }
    TaskMetadata& getMetadata() { return _metadata; }
    
    // Setters with validation
    TaskResult setTitle(const std::string& title);
    TaskResult setDescription(const std::string& description);
    TaskResult setStatus(TaskStatus status);
    TaskResult setPriority(int priority);
    TaskResult setCategory(const std::string& category);
    
    // C++23 Technique 7: std::format integration (ready for std::print when available)
    std::string to_string() const;
    
    // C++23 Technique 8: Custom comparison operators
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

// C++23 Technique 9: Ranges and views integration
class TaskManager {
private:
    std::vector<Task> _tasks;
    int _next_id = 1;
    
public:
    TaskResult addTask(const std::string& title, const std::string& description = "");
    TaskResult removeTask(int id);
    TaskOptional getTask(int id);
    TaskResult updateTaskStatus(int id, TaskStatus status);
    
    // C++23 Technique 10: Enhanced lambda expressions with template parameters
    template<std::predicate<const Task&> Predicate>
    auto filterTasks(Predicate&& pred) const {
        return _tasks | std::views::filter(std::forward<Predicate>(pred));
    }
    
    // Get tasks by status using ranges
    auto getTasksByStatus(TaskStatus status) const {
        return filterTasks([status](const Task& task) {
            return task.getStatus() == status;
        });
    }
    
    // Get tasks by priority range
    auto getTasksByPriority(int min_priority, int max_priority) const {
        return filterTasks([min_priority, max_priority](const Task& task) {
            int priority = task.getMetadata().priority;
            return priority >= min_priority && priority <= max_priority;
        });
    }
    
    // Sort tasks by various criteria
    template<typename Compare>
    requires std::strict_weak_order<Compare, Task, Task>
    std::vector<Task> getSortedTasks(Compare&& comp) const {
        auto sorted_tasks = _tasks;
        std::ranges::sort(sorted_tasks, std::forward<Compare>(comp));
        return sorted_tasks;
    }
    
    size_t getTaskCount() const { return _tasks.size(); }
    const std::vector<Task>& getAllTasks() const { return _tasks; }
    
    // Statistics
    size_t getCompletedTasksCount() const;
    size_t getPendingTasksCount() const;
    double getCompletionRate() const;
    
    void listTasks() const;
    void listTasksByStatus(TaskStatus status) const;
    
    // JSON serialization/deserialization methods  
    JsonResult saveToJson(const std::string& filename) const;
    JsonResult loadFromJson(const std::string& filename);
    std::string toJsonString() const;
    JsonResult fromJsonString(const std::string& json_str);
};

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