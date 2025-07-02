#ifndef TASK_MATRIX_H
#define TASK_MATRIX_H

/**
 * @file task_matrix.h
 * @brief Defines the TaskMatrix class for organizing tasks by category and priority
 * @details Uses C++23 features like flat_map, deducing this, and multidimensional subscript operators
 */

#include "task.h"
#include <flat_map>         // C++23: std::flat_map for better cache locality
#include <unordered_map>    // Keep as fallback/comparison
#include <vector>
#include <string>

/**
 * @class TaskMatrix
 * @brief Matrix for organizing tasks by category and priority
 * @details C++23 Feature: Multidimensional Subscript Operator with flat_map optimization
 *          for better cache locality and performance
 */
class TaskMatrix {
private:
    /**
     * @brief Two-dimensional map of tasks organized by category and priority
     * @details C++23: Uses flat_map for better cache locality and iteration performance
     */
    std::flat_map<std::string, std::flat_map<int, std::vector<Task>>> matrix;
    
public:
    /**
     * @brief Multidimensional subscript operator for accessing tasks by category and priority
     * @details C++23: Uses deducing this to handle both const and non-const access
     * 
     * @tparam Self Deduced self type (const or non-const)
     * @param category The category name
     * @param priority The priority level
     * @return Reference to vector of tasks (creates if non-const, returns empty if const and not found)
     */
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
    
    /**
     * @brief Traditional single-dimension access operator (by category)
     * @details C++23: Uses deducing this to handle both const and non-const access
     * 
     * @tparam Self Deduced self type (const or non-const)
     * @param category The category name
     * @return Reference to category's priority map
     */
    template<typename Self>
    decltype(auto) operator[](this Self&& self, const std::string& category) {
        return (std::forward<Self>(self).matrix[category]);
    }
    
    /**
     * @brief Add a task to the matrix in its appropriate category and priority
     * @param task The task to add
     */
    void addTask(const Task& task);
    
    /**
     * @brief Remove a task from the matrix by its ID
     * @param task_id ID of the task to remove
     * @return true if task was found and removed, false otherwise
     */
    bool removeTask(int task_id);
    
    /**
     * @brief Get a list of all categories in the matrix
     * @return Vector of category names
     */
    std::vector<std::string> getCategories() const;
    
    /**
     * @brief Get a list of all priorities used within a category
     * @param category The category name to look up
     * @return Vector of priority levels
     */
    std::vector<int> getPriorities(const std::string& category) const;
    
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
    
    /**
     * @brief Display matrix structure
     * @details Outputs a hierarchical view of categories, priorities, and tasks
     */
    void displayMatrix() const;
    
    /**
     * @brief Clear the matrix
     * @details Removes all tasks from all categories and priorities
     */
    void clear();
    
    // Check if category exists with deducing this
    template<typename Self>
    auto hasCategory(this Self&& self, const std::string& category) -> bool {
        return self.matrix.contains(category);  
    }
    
    // Get total task count with deducing this
    /**
     * @brief Get total number of tasks across all categories and priorities
     * @details C++23: Uses deducing this for both const and non-const access
     * @tparam Self Deduced self type (const or non-const)
     * @return Total number of tasks in the matrix
     */
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

#endif // TASK_MATRIX_H
