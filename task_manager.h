#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "task.h"
#include <vector>
#include <ranges>
#include <algorithm>
#include <string>
#include <expected>

/**
 * @class TaskManager
 * @brief Manages a collection of tasks with various operations
 * @details Provides methods for adding, removing, updating, and querying tasks
 */
class TaskManager {
private:
    std::vector<Task> _tasks;     /**< Collection of all tasks */
    int _next_id = 1;            /**< Next available task ID */
    
public:
    /**
     * @brief Add a new task to the collection
     * @param title The title of the task
     * @param description Optional description of the task
     * @return Task ID on success, or error code on failure
     */
    TaskAddResult addTask(const std::string& title, const std::string& description = "");
    
    /**
     * @brief Remove a task by its ID
     * @param id The ID of the task to remove
     * @return Success or error code
     */
    TaskResult removeTask(int id);
    
    /**
     * @brief Get a task by its ID
     * @param id The ID of the task to retrieve
     * @return Task object or error code if not found
     */
    TaskOptional getTask(int id);
    
    /**
     * @brief Update the status of a task
     * @param id The ID of the task to update
     * @param status The new status to set
     * @return Success or error code
     */
    TaskResult updateTaskStatus(int id, TaskStatus status);
    
    /**
     * @brief Filter tasks using a predicate
     * @tparam Predicate Function type that takes a Task and returns bool
     * @param pred Predicate function for filtering
     * @return Range of tasks that satisfy the predicate
     */
    template<std::predicate<const Task&> Predicate>
    auto filterTasks(Predicate&& pred) const {
        return _tasks | std::views::filter(std::forward<Predicate>(pred));
    }
    
    /**
     * @brief Get tasks filtered by status
     * @param status Status to filter by
     * @return Range of tasks with the specified status
     */
    auto getTasksByStatus(TaskStatus status) const {
        return filterTasks([status](const Task& task) {
            return task.getStatus() == status;
        });
    }
    
    /**
     * @brief Get tasks filtered by priority range
     * @param min_priority Minimum priority (inclusive)
     * @param max_priority Maximum priority (inclusive)
     * @return Range of tasks within the priority range
     */
    auto getTasksByPriority(int min_priority, int max_priority) const {
        return filterTasks([min_priority, max_priority](const Task& task) {
            int priority = task.getMetadata().priority;
            return priority >= min_priority && priority <= max_priority;
        });
    }
    
    /**
     * @brief Get tasks sorted by custom criteria
     * @tparam Compare Comparison function type
     * @param comp Comparison function for sorting
     * @return Vector of tasks sorted according to comparator
     */
    template<typename Compare>
    requires std::strict_weak_order<Compare, Task, Task>
    std::vector<Task> getSortedTasks(Compare&& comp) const {
        auto sorted_tasks = _tasks;
        std::ranges::sort(sorted_tasks, std::forward<Compare>(comp));
        return sorted_tasks;
    }
    
    /**
     * @brief Get the number of tasks in the collection
     * @details C++23: Uses deducing this for const/non-const resolution
     * @tparam Self Deduced self type (const or non-const)
     * @return Number of tasks
     */
    template<typename Self>
    auto getTaskCount(this Self&& self) -> size_t { 
        return self._tasks.size(); 
    }
    
    /**
     * @brief Get all tasks in the collection
     * @details C++23: Uses deducing this for const/non-const resolution
     * @tparam Self Deduced self type (const or non-const)
     * @return Reference to the tasks vector
     */
    template<typename Self>
    auto getAllTasks(this Self&& self) -> decltype(auto) { 
        return (std::forward<Self>(self)._tasks);
    }
    
    /**
     * @brief Access a task by its index in the collection
     * @details C++23: Uses deducing this for const/non-const resolution
     * @tparam Self Deduced self type (const or non-const)
     * @param index Position of the task in the collection
     * @return Reference to the task
     * @throws std::out_of_range if index is out of bounds
     */
    template<typename Self>
    auto getTaskByIndex(this Self&& self, size_t index) -> decltype(auto) {
        if (index >= self._tasks.size()) {
            throw std::out_of_range("Task index out of range");
        }
        return (std::forward<Self>(self)._tasks[index]);
    }
    
    /**
     * @name Statistics Methods
     * @brief Methods for calculating task statistics
     */
    ///@{
    /**
     * @brief Count completed tasks
     * @return Number of completed tasks
     */
    size_t getCompletedTasksCount() const;
    
    /**
     * @brief Count pending tasks
     * @return Number of pending tasks
     */
    size_t getPendingTasksCount() const;
    
    /**
     * @brief Calculate task completion rate
     * @return Percentage of completed tasks
     */
    double getCompletionRate() const;
    ///@}
    
    /**
     * @name Display Methods
     * @brief Methods for displaying tasks
     */
    ///@{
    /**
     * @brief Display all tasks in the collection
     */
    void listTasks() const;
    
    /**
     * @brief Display tasks filtered by status
     * @param status Status to filter by
     */
    void listTasksByStatus(TaskStatus status) const;
    ///@}
    
    /**
     * @name JSON Serialization Methods
     * @brief Methods for JSON persistence
     */
    ///@{
    /**
     * @brief Save tasks to a JSON file
     * @param filename Path to the output file
     * @return Success or error code
     */
    JsonResult saveToJson(const std::string& filename) const;
    
    /**
     * @brief Load tasks from a JSON file
     * @param filename Path to the input file
     * @return Success or error code
     */
    JsonResult loadFromJson(const std::string& filename);
    
    /**
     * @brief Convert tasks to JSON string
     * @return JSON representation of all tasks
     */
    std::string toJsonString() const;
    
    /**
     * @brief Parse tasks from JSON string
     * @param json_str JSON string to parse
     * @return Success or error code
     */
    JsonResult fromJsonString(const std::string& json_str);
    ///@}
};

#endif // TASK_MANAGER_H
