#pragma once

#include "task.h"
#include <unordered_map>
#include <vector>
#include <string>

// C++23 Feature: Multidimensional Subscript Operator
// TaskMatrix for organizing tasks by [category][priority]
class TaskMatrix {
private:
    std::unordered_map<std::string, std::unordered_map<int, std::vector<Task>>> matrix;
    
public:
    // C++23: Multidimensional subscript operator [category, priority]
    std::vector<Task>& operator[](const std::string& category, int priority) {
        return matrix[category][priority];
    }
    
    const std::vector<Task>& operator[](const std::string& category, int priority) const {
        static const std::vector<Task> empty;
        auto cat_it = matrix.find(category);
        if (cat_it == matrix.end()) return empty;
        
        auto prio_it = cat_it->second.find(priority);
        if (prio_it == cat_it->second.end()) return empty;
        
        return prio_it->second;
    }
    
    // Traditional single-dimension access for comparison
    std::unordered_map<int, std::vector<Task>>& operator[](const std::string& category) {
        return matrix[category];
    }
    
    // Add task to matrix
    void addTask(const Task& task) {
        const std::string& category = task.getMetadata().category.empty() ? 
            "Default" : task.getMetadata().category;
        int priority = task.getMetadata().priority;
        
        matrix[category][priority].push_back(task);
    }
    
    // Remove task from matrix
    bool removeTask(int task_id) {
        for (auto& [category, priority_map] : matrix) {
            for (auto& [priority, tasks] : priority_map) {
                auto it = std::find_if(tasks.begin(), tasks.end(),
                    [task_id](const Task& t) { return t.getId() == task_id; });
                if (it != tasks.end()) {
                    tasks.erase(it);
                    return true;
                }
            }
        }
        return false;
    }
    
    // Get all categories
    std::vector<std::string> getCategories() const {
        std::vector<std::string> categories;
        for (const auto& [category, _] : matrix) {
            categories.push_back(category);
        }
        return categories;
    }
    
    // Get priorities for a category
    std::vector<int> getPriorities(const std::string& category) const {
        std::vector<int> priorities;
        auto it = matrix.find(category);
        if (it != matrix.end()) {
            for (const auto& [priority, _] : it->second) {
                priorities.push_back(priority);
            }
        }
        return priorities;
    }
    
    // Get task count for [category, priority]
    size_t getTaskCount(const std::string& category, int priority) const {
        return (*this)[category, priority].size();
    }
    
    // Display matrix structure
    void displayMatrix() const {
        std::print("\n📊 Task Matrix Structure:\n");
        std::print("=========================\n");
        
        for (const auto& [category, priority_map] : matrix) {
            std::print("📂 Category: {}\n", category);
            for (const auto& [priority, tasks] : priority_map) {
                std::print("  🎯 Priority {}: {} task(s)\n", priority, tasks.size());
                for (const auto& task : tasks) {
                    std::print("    [{}] {}\n", task.getId(), task.getTitle());
                }
            }
        }
    }
    
    // Clear the matrix
    void clear() {
        matrix.clear();
    }
    
    // Check if category exists
    bool hasCategory(const std::string& category) const {
        return matrix.contains(category);  // C++23: .contains() method
    }
    
    // Get total task count
    size_t getTotalTaskCount() const {
        size_t total = 0uz;  // C++23: uz suffix
        for (const auto& [category, priority_map] : matrix) {
            for (const auto& [priority, tasks] : priority_map) {
                total += tasks.size();
            }
        }
        return total;
    }
};
