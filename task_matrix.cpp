/**
 * @file task_matrix.cpp
 * @brief Implementation of TaskMatrix class
 * @details Contains the implementation of non-template TaskMatrix methods
 */

#include "task_matrix.h"
#include <print>
#include <algorithm>

/**
 * @brief Add a task to the matrix in its appropriate category and priority
 * @param task The task to add
 */
void TaskMatrix::addTask(const Task& task) {
    const std::string& category = task.getMetadata().category.empty() ? 
        "Default" : task.getMetadata().category;
    int priority = task.getMetadata().priority;
    
    matrix[category][priority].push_back(task);
}

/**
 * @brief Remove a task from the matrix by its ID
 * @param task_id ID of the task to remove
 * @return true if task was found and removed, false otherwise
 */
bool TaskMatrix::removeTask(int task_id) {
    for (auto it = matrix.begin(); it != matrix.end(); ++it) {
        auto& priority_map = it->second;
        for (auto pit = priority_map.begin(); pit != priority_map.end(); ++pit) {
            auto& tasks = pit->second;
            auto task_it = std::find_if(tasks.begin(), tasks.end(),
                [task_id](const Task& t) { return t.getId() == task_id; });
            if (task_it != tasks.end()) {
                tasks.erase(task_it);
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Get a list of all categories in the matrix
 * @return Vector of category names
 */
std::vector<std::string> TaskMatrix::getCategories() const {
    std::vector<std::string> categories;
    for (auto it = matrix.begin(); it != matrix.end(); ++it) {
        categories.push_back(it->first);
    }
    return categories;
}

/**
 * @brief Get a list of all priorities used within a category
 * @param category The category name to look up
 * @return Vector of priority levels
 */
std::vector<int> TaskMatrix::getPriorities(const std::string& category) const {
    std::vector<int> priorities;
    auto it = matrix.find(category);
    if (it != matrix.end()) {
        for (auto pit = it->second.begin(); pit != it->second.end(); ++pit) {
            priorities.push_back(pit->first);
        }
    }
    return priorities;
}

/**
 * @brief Display matrix structure
 * @details Outputs a hierarchical view of categories, priorities, and tasks
 */
void TaskMatrix::displayMatrix() const {
    std::print("\n📊 Task Matrix Structure:\n");
    std::print("=========================\n");
    
    for (auto it = matrix.begin(); it != matrix.end(); ++it) {
        const auto& category = it->first;
        const auto& priority_map = it->second;
        std::print("📂 Category: {}\n", category);
        for (auto pit = priority_map.begin(); pit != priority_map.end(); ++pit) {
            const auto& priority = pit->first;
            const auto& tasks = pit->second;
            std::print("  🎯 Priority {}: {} task(s)\n", priority, tasks.size());
            for (const auto& task : tasks) {
                std::print("    [{}] {}\n", task.getId(), task.getTitle());
            }
        }
    }
}

/**
 * @brief Clear the matrix
 * @details Removes all tasks from all categories and priorities
 */
void TaskMatrix::clear() {
    matrix.clear();
}
