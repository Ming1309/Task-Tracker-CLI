#pragma once

#include "task.h"
#include <vector>
#include <ranges>
#include <algorithm>
#include <string>
#include <expected>

class TaskManager {
private:
    std::vector<Task> _tasks;
    int _next_id = 1;
    
public:
    TaskAddResult addTask(const std::string& title, const std::string& description = "");
    TaskResult removeTask(int id);
    TaskOptional getTask(int id);
    TaskResult updateTaskStatus(int id, TaskStatus status);
    
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
    
    // C++23: Deducing this for task access
    template<typename Self>
    auto getTaskCount(this Self&& self) -> size_t { 
        return self._tasks.size(); 
    }
    
    template<typename Self>
    auto getAllTasks(this Self&& self) -> decltype(auto) { 
        return (std::forward<Self>(self)._tasks);
    }
    
    // Task access with deducing this
    template<typename Self>
    auto getTaskByIndex(this Self&& self, size_t index) -> decltype(auto) {
        if (index >= self._tasks.size()) {
            throw std::out_of_range("Task index out of range");
        }
        return (std::forward<Self>(self)._tasks[index]);
    }
    
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
