#include "task_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <format>

TaskAddResult TaskManager::addTask(const std::string& title, const std::string& description) {
    if (title.empty()) {
        return std::unexpected(TaskError::EmptyTitle);
    }
    
    // Check for duplicate titles
    auto existing = std::ranges::find_if(_tasks, [&title](const Task& task) {
        return task.getTitle() == title;
    });
    
    if (existing != _tasks.end()) {
        return std::unexpected(TaskError::DuplicateTask);
    }
    
    int new_id = _next_id++;
    _tasks.emplace_back(new_id, title, description);
    return new_id;  // C++23: Return the ID of the newly created task
}

TaskResult TaskManager::removeTask(int id) {
    auto it = std::ranges::find_if(_tasks, [id](const Task& task) {
        return task.getId() == id;
    });
    
    if (it == _tasks.end()) {
        return std::unexpected(TaskError::TaskNotFound);
    }
    
    _tasks.erase(it);
    return true;
}

TaskOptional TaskManager::getTask(int id) {
    auto it = std::ranges::find_if(_tasks, [id](const Task& task) {
        return task.getId() == id;
    });
    
    if (it == _tasks.end()) {
        return std::unexpected(TaskError::TaskNotFound);
    }
    
    return *it;
}

TaskResult TaskManager::updateTaskStatus(int id, TaskStatus status) {
    auto it = std::ranges::find_if(_tasks, [id](Task& task) {
        return task.getId() == id;
    });
    
    if (it == _tasks.end()) {
        return std::unexpected(TaskError::TaskNotFound);
    }
    
    return it->setStatus(status);
}

size_t TaskManager::getCompletedTasksCount() const {
    return std::ranges::count_if(_tasks, [](const Task& task) {
        return task.isCompleted();
    });
}

size_t TaskManager::getPendingTasksCount() const {
    return std::ranges::count_if(_tasks, [](const Task& task) {
        return task.getStatus() == TaskStatus::Pending;
    });
}

double TaskManager::getCompletionRate() const {
    if (_tasks.empty()) return 0.0;
    return static_cast<double>(getCompletedTasksCount()) / _tasks.size() * 100.0;
}

void TaskManager::listTasks() const {
    if (_tasks.empty()) {
        std::cout << "No tasks available.\n";
        return;
    }
    
    std::cout << std::format("=== Task List ({} tasks) ===\n", _tasks.size());
    for (const auto& task : _tasks) {
        std::cout << std::format("[{}] {} - {} (Priority: {}, Category: {})\n",
                    task.getId(),
                    task.getTitle(),
                    taskStatusToString(task.getStatus()),
                    task.getMetadata().priority,
                    task.getMetadata().category);
    }
    std::cout << std::format("Completion Rate: {:.1f}%\n", getCompletionRate());
}

void TaskManager::listTasksByStatus(TaskStatus status) const {
    auto filtered_tasks = getTasksByStatus(status);
    auto task_vector = std::vector<Task>(filtered_tasks.begin(), filtered_tasks.end());
    
    if (task_vector.empty()) {
        std::cout << std::format("No tasks with status: {}\n", taskStatusToString(status));
        return;
    }
    
    std::cout << std::format("=== {} Tasks ({} tasks) ===\n", 
                taskStatusToString(status), task_vector.size());
    
    for (const auto& task : task_vector) {
        std::cout << std::format("[{}] {} (Priority: {}, Category: {})\n",
                    task.getId(),
                    task.getTitle(),
                    taskStatusToString(task.getStatus()),
                    task.getMetadata().priority,
                    task.getMetadata().category);
    }
}

std::string TaskManager::toJsonString() const {
    std::ostringstream json;
    json << "{\n";
    json << std::format("  \"version\": \"1.0\",\n");
    json << std::format("  \"next_id\": {},\n", _next_id);
    json << "  \"tasks\": [\n";
    
    for (size_t i = 0; i < _tasks.size(); ++i) {
        std::string task_json = _tasks[i].toJson();
        // Indent the task JSON
        std::istringstream task_stream(task_json);
        std::string line;
        bool first_line = true;
        while (std::getline(task_stream, line)) {
            if (!first_line) json << "\n";
            json << "    " << line;
            first_line = false;
        }
        
        if (i < _tasks.size() - 1) {
            json << ",";
        }
        json << "\n";
    }
    
    json << "  ]\n";
    json << "}";
    return json.str();
}

JsonResult TaskManager::saveToJson(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return std::unexpected(JsonError::FileNotFound);
        }
        
        std::string json_content = toJsonString();
        file << json_content;
        
        if (file.fail()) {
            return std::unexpected(JsonError::WriteError);
        }
        
        return true;
    } catch (const std::exception&) {
        return std::unexpected(JsonError::WriteError);
    }
}

JsonResult TaskManager::loadFromJson(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return std::unexpected(JsonError::FileNotFound);
        }
        
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string json_content = buffer.str();
        
        return fromJsonString(json_content);
    } catch (const std::exception&) {
        return std::unexpected(JsonError::ParseError);
    }
}

JsonResult TaskManager::fromJsonString(const std::string& json_str) {
    try {
        // Simple JSON parsing - extract next_id and tasks array
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
                return json_str.substr(pos, end_pos - pos);
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
        
        std::string next_id_str = findValue("next_id");
        if (!next_id_str.empty()) {
            _next_id = std::stoi(next_id_str);
        }
        
        // Find tasks array
        size_t tasks_pos = json_str.find("\"tasks\":");
        if (tasks_pos == std::string::npos) {
            return std::unexpected(JsonError::InvalidFormat);
        }
        
        size_t array_start = json_str.find("[", tasks_pos);
        if (array_start == std::string::npos) {
            return std::unexpected(JsonError::InvalidFormat);
        }
        
        size_t array_end = json_str.find("]", array_start);
        if (array_end == std::string::npos) {
            return std::unexpected(JsonError::InvalidFormat);
        }
        
        // Parse individual task objects
        _tasks.clear();
        size_t pos = array_start + 1;
        
        while (pos < array_end) {
            // Find next task object
            size_t obj_start = json_str.find("{", pos);
            if (obj_start >= array_end) break;
            
            // Find matching closing brace
            size_t obj_end = obj_start + 1;
            int brace_count = 1;
            while (obj_end < array_end && brace_count > 0) {
                if (json_str[obj_end] == '{') ++brace_count;
                else if (json_str[obj_end] == '}') --brace_count;
                ++obj_end;
            }
            
            if (brace_count == 0) {
                std::string task_json = json_str.substr(obj_start, obj_end - obj_start);
                auto task_result = Task::fromJson(task_json);
                if (task_result) {
                    _tasks.push_back(*task_result);
                } else {
                    return std::unexpected(task_result.error());
                }
            }
            
            pos = obj_end;
        }
        
        return true;
    } catch (const std::exception&) {
        return std::unexpected(JsonError::ParseError);
    }
}
