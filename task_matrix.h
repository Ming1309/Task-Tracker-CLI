#pragma once

#include "task.h"
#include <flat_map>         // C++23: std::flat_map for better cache locality
#include <unordered_map>    // Keep as fallback/comparison
#include <vector>
#include <string>

// C++23 Feature: Multidimensional Subscript Operator with flat_map optimization
// TaskMatrix for organizing tasks by [category][priority] using contiguous memory layout
class TaskMatrix {
private:
    // C++23: Using flat_map for better cache locality and iteration performance
    std::flat_map<std::string, std::flat_map<int, std::vector<Task>>> matrix;
    
public:
    // C++23: Multidimensional subscript operator with deducing this
    template<typename Self>
    decltype(auto) operator[](this Self&& self, const std::string& category, int priority) {
        if constexpr (std::is_const_v<std::remove_reference_t<Self>>) {
            // Const version - return empty if not found
            static const std::vector<Task> empty;
            auto cat_it = self.matrix.find(category);
            if (cat_it == self.matrix.end()) return empty;
            
            auto prio_it = cat_it->second.find(priority);
            if (prio_it == cat_it->second.end()) return empty;
            
            return prio_it->second;
        } else {
            // Non-const version - create if not exists
            return (std::forward<Self>(self).matrix[category][priority]);
        }
    }
    
    // Traditional single-dimension access with deducing this
    template<typename Self>
    decltype(auto) operator[](this Self&& self, const std::string& category) {
        return (std::forward<Self>(self).matrix[category]);
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
    
    // Get all categories
    std::vector<std::string> getCategories() const {
        std::vector<std::string> categories;
        for (auto it = matrix.begin(); it != matrix.end(); ++it) {
            categories.push_back(it->first);
        }
        return categories;
    }
    
    // Get priorities for a category
    std::vector<int> getPriorities(const std::string& category) const {
        std::vector<int> priorities;
        auto it = matrix.find(category);
        if (it != matrix.end()) {
            for (auto pit = it->second.begin(); pit != it->second.end(); ++pit) {
                priorities.push_back(pit->first);
            }
        }
        return priorities;
    }
    
    // Get task count for [category, priority] with deducing this
    template<typename Self>
    auto getTaskCount(this Self&& self, const std::string& category, int priority) -> size_t {
        if constexpr (std::is_const_v<std::remove_reference_t<Self>>) {
            // Const version - return 0 if not found
            auto cat_it = self.matrix.find(category);
            if (cat_it == self.matrix.end()) return 0;
            
            auto prio_it = cat_it->second.find(priority);
            if (prio_it == cat_it->second.end()) return 0;
            
            return prio_it->second.size();
        } else {
            return self[category, priority].size();
        }
    }
    
    // Display matrix structure
    void displayMatrix() const {
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
    
    // Clear the matrix
    void clear() {
        matrix.clear();
    }
    
    // Check if category exists with deducing this
    template<typename Self>
    auto hasCategory(this Self&& self, const std::string& category) -> bool {
        return self.matrix.contains(category);  
    }
    
    // Get total task count with deducing this
    template<typename Self>
    auto getTotalTaskCount(this Self&& self) -> size_t {
        size_t total = 0uz; 
        for (auto it = self.matrix.begin(); it != self.matrix.end(); ++it) {
            for (auto pit = it->second.begin(); pit != it->second.end(); ++pit) {
                total += pit->second.size();
            }
        }
        return total;
    }
};
