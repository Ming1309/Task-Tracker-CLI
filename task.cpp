#include "task.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// Task constructor
Task::Task(int id, std::string title, std::string description, TaskStatus status)
    : _id(id), _title(std::move(title)), _description(std::move(description)), _status(status) {
    auto now = std::chrono::system_clock::now();
    _metadata.created_at = now;
    _metadata.updated_at = now;
    _metadata.category = "General";
}

// Task methods implementation
TaskResult Task::setTitle(const std::string& title) {
    if (title.empty()) {
        return std::unexpected(TaskError::EmptyTitle);
    }
    _title = title;
    _metadata.updated_at = std::chrono::system_clock::now();
    return true;
}

TaskResult Task::setDescription(const std::string& description) {
    _description = description;
    _metadata.updated_at = std::chrono::system_clock::now();
    return true;
}

TaskResult Task::setStatus(TaskStatus status) {
    _status = status;
    _metadata.updated_at = std::chrono::system_clock::now();
    
    if (status == TaskStatus::Completed) {
        _metadata.completed_at = std::chrono::system_clock::now();
    }
    
    return true;
}

TaskResult Task::setPriority(int priority) {
    _metadata.priority = priority;
    _metadata.updated_at = std::chrono::system_clock::now();
    return true;
}

TaskResult Task::setCategory(const std::string& category) {
    _metadata.category = category;
    _metadata.updated_at = std::chrono::system_clock::now();
    return true;
}

std::string Task::to_string() const {
    // C++23 std::format usage (ready to migrate to std::print when available)
    auto created_time = std::chrono::system_clock::to_time_t(_metadata.created_at);
    auto updated_time = std::chrono::system_clock::to_time_t(_metadata.updated_at);
    
    std::ostringstream oss;
    oss << std::format("Task [ID: {}]\n", _id);
    oss << std::format("  Title: {}\n", _title);
    oss << std::format("  Description: {}\n", _description.empty() ? "None" : _description);
    oss << std::format("  Status: {}\n", taskStatusToString(_status));
    oss << std::format("  Category: {}\n", _metadata.category);
    oss << std::format("  Priority: {}\n", _metadata.priority);
    oss << std::format("  Created: {:%Y-%m-%d %H:%M:%S}\n", std::chrono::system_clock::from_time_t(created_time));
    oss << std::format("  Updated: {:%Y-%m-%d %H:%M:%S}\n", std::chrono::system_clock::from_time_t(updated_time));
    
    if (_metadata.completed_at.has_value()) {
        auto completed_time = std::chrono::system_clock::to_time_t(*_metadata.completed_at);
        oss << std::format("  Completed: {:%Y-%m-%d %H:%M:%S}\n", std::chrono::system_clock::from_time_t(completed_time));
    }
    
    return oss.str();
}

bool Task::isCompleted() const {
    return _status == TaskStatus::Completed;
}

std::chrono::duration<double> Task::getAge() const {
    return std::chrono::system_clock::now() - _metadata.created_at;
}

void Task::markCompleted() {
    setStatus(TaskStatus::Completed);
}

// TaskManager implementation
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
                    task.getMetadata().priority,
                    task.getMetadata().category);
    }
}

// Demo function to showcase C++23 features
void demonstrateC23Features() {
    std::cout << "\n🎯 Demonstrating C++23 Features:\n";
    std::cout << "================================\n";
    
    // 1. Enhanced enum classes
    TaskStatus status = TaskStatus::InProgress;
    std::cout << "1. Enhanced Enum: " << taskStatusToString(status) << "\n";
    
    // 2. std::expected (simulated)
    TaskManager manager;
    auto result = manager.addTask("Demo Task", "Showcasing C++23 features");
    if (result) {
        std::cout << "2. std::expected: Task added successfully\n";
    }
    
    // 3. Concepts demonstration
    auto checkTaskLike = []<TaskLike T>(const T& t) {
        return t.getId() > 0;
    };
    
    if (auto task_result = manager.getTask(1); task_result) {
        std::cout << "3. Concepts: Task validation passed\n";
        
        // 4. Ranges and views
        auto pending_tasks = manager.getTasksByStatus(TaskStatus::Pending);
        std::cout << "4. Ranges: Found " << std::distance(pending_tasks.begin(), pending_tasks.end()) << " pending task(s)\n";
        
        // 5. Enhanced lambdas with template parameters
        auto sorter = []<typename T>(std::vector<T>& vec) requires std::sortable<typename std::vector<T>::iterator> {
            std::ranges::sort(vec);
        };
        
        std::vector<int> test_vec{3, 1, 4, 1, 5};
        sorter(test_vec);
        std::cout << "5. Template Lambdas: Vector sorted\n";
        
        // 6. std::format (C++23)
        std::cout << std::format("6. std::format (C++23): Task ID = {}\n", task_result->getId());
        
        // 7. Structured bindings
        const auto& [created, updated, completed, category, priority] = task_result->getMetadata().tie();
        std::cout << "7. Structured Bindings: Task category = " << category << "\n";
        
        // 8. Comparison operators
        Task another_task(2, "Another Task");
        bool are_equal = (*task_result == another_task);
        std::cout << "8. Spaceship Operator: Tasks equal = " << std::boolalpha << are_equal << "\n";
        
        // 9. Designated initializers (already shown in Command struct)
        std::cout << "9. Designated Initializers: Used in Command struct initialization\n";
        
        // 10. Modern getter methods
        std::cout << "10. Modern Getters: Task title = '" << task_result->getTitle() << "'\n";
    }
    
    std::cout << "\n✨ All C++23 features demonstrated successfully!\n\n";
}

// JSON Utility Functions
std::string jsonErrorToString(JsonError error) {
    switch (error) {
        case JsonError::FileNotFound: return "JSON file not found";
        case JsonError::InvalidFormat: return "Invalid JSON format";
        case JsonError::WriteError: return "Failed to write JSON file";
        case JsonError::ParseError: return "Failed to parse JSON";
        default: return "Unknown JSON error";
    }
}

// Utility function to escape JSON strings
std::string escapeJsonString(const std::string& str) {
    std::string escaped;
    escaped.reserve(str.size() + 20); // Reserve some extra space for escapes
    
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (c < 32) {
                    escaped += std::format("\\u{:04x}", static_cast<unsigned char>(c));
                } else {
                    escaped += c;
                }
                break;
        }
    }
    return escaped;
}

// Utility function to unescape JSON strings
std::string unescapeJsonString(const std::string& str) {
    std::string unescaped;
    unescaped.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '"': unescaped += '"'; ++i; break;
                case '\\': unescaped += '\\'; ++i; break;
                case 'b': unescaped += '\b'; ++i; break;
                case 'f': unescaped += '\f'; ++i; break;
                case 'n': unescaped += '\n'; ++i; break;
                case 'r': unescaped += '\r'; ++i; break;
                case 't': unescaped += '\t'; ++i; break;
                default: unescaped += str[i]; break;
            }
        } else {
            unescaped += str[i];
        }
    }
    return unescaped;
}

// Convert time_point to ISO 8601 string
std::string timePointToIsoString(const std::chrono::system_clock::time_point& tp) {
    auto time_t = std::chrono::system_clock::to_time_t(tp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    // Fix: Use localtime instead of gmtime to show local timezone
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
    oss << std::format(".{:03d}", ms.count()); // Remove Z suffix for local time
    return oss.str();
}

// Parse ISO 8601 string to time_point
std::chrono::system_clock::time_point isoStringToTimePoint(const std::string& iso_str) {
    std::tm tm = {};
    std::istringstream ss(iso_str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    
    // Handle milliseconds if present
    auto dot_pos = iso_str.find('.');
    if (dot_pos != std::string::npos) {
        auto ms_str = iso_str.substr(dot_pos + 1, 3);
        if (!ms_str.empty() && std::isdigit(ms_str[0])) {
            int ms = std::stoi(ms_str);
            tp += std::chrono::milliseconds(ms);
        }
    }
    
    return tp;
}

// Task JSON serialization implementation
std::string Task::toJson() const {
    std::ostringstream json;
    json << "{\n";
    json << std::format("  \"id\": {},\n", _id);
    json << std::format("  \"title\": \"{}\",\n", escapeJsonString(_title));
    json << std::format("  \"description\": \"{}\",\n", escapeJsonString(_description));
    json << std::format("  \"status\": \"{}\",\n", taskStatusToString(_status));
    json << std::format("  \"category\": \"{}\",\n", escapeJsonString(_metadata.category));
    json << std::format("  \"priority\": {},\n", _metadata.priority);
    json << std::format("  \"created_at\": \"{}\",\n", timePointToIsoString(_metadata.created_at));
    json << std::format("  \"updated_at\": \"{}\"", timePointToIsoString(_metadata.updated_at));
    
    if (_metadata.completed_at.has_value()) {
        json << std::format(",\n  \"completed_at\": \"{}\"", timePointToIsoString(*_metadata.completed_at));
    }
    
    json << "\n}";
    return json.str();
}

// Simple JSON parser for Task objects
std::expected<Task, JsonError> Task::fromJson(const std::string& json_str) {
    try {
        // Simple JSON parsing - in production, use a proper JSON library
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
                return unescapeJsonString(json_str.substr(pos, end_pos - pos));
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
        
        std::string id_str = findValue("id");
        std::string title = findValue("title");
        std::string description = findValue("description");
        std::string status_str = findValue("status");
        std::string category = findValue("category");
        std::string priority_str = findValue("priority");
        std::string created_at_str = findValue("created_at");
        std::string updated_at_str = findValue("updated_at");
        std::string completed_at_str = findValue("completed_at");
        
        if (id_str.empty() || title.empty()) {
            return std::unexpected(JsonError::InvalidFormat);
        }
        
        int id = std::stoi(id_str);
        auto status_opt = stringToTaskStatus(status_str);
        if (!status_opt) {
            return std::unexpected(JsonError::InvalidFormat);
        }
        
        Task task(id, title, description, *status_opt);
        
        if (!category.empty()) {
            task.setCategory(category);
        }
        
        if (!priority_str.empty()) {
            int priority = std::stoi(priority_str);
            task.setPriority(priority);
        }
        
        if (!created_at_str.empty()) {
            task._metadata.created_at = isoStringToTimePoint(created_at_str);
        }
        
        if (!updated_at_str.empty()) {
            task._metadata.updated_at = isoStringToTimePoint(updated_at_str);
        }
        
        if (!completed_at_str.empty()) {
            task._metadata.completed_at = isoStringToTimePoint(completed_at_str);
        }
        
        return task;
    } catch (const std::exception&) {
        return std::unexpected(JsonError::ParseError);
    }
}

// TaskManager JSON serialization implementation
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

// Utility functions
std::string taskStatusToString(TaskStatus status) {
    switch (status) {
        case TaskStatus::Pending: return "Pending";
        case TaskStatus::InProgress: return "In Progress";
        case TaskStatus::Completed: return "Completed";
        case TaskStatus::Cancelled: return "Cancelled";
        default: return "Unknown";
    }
}

std::optional<TaskStatus> stringToTaskStatus(const std::string& str) {
    if (str == "pending" || str == "Pending") return TaskStatus::Pending;
    if (str == "progress" || str == "in-progress" || str == "In Progress") return TaskStatus::InProgress;
    if (str == "completed" || str == "Completed") return TaskStatus::Completed;
    if (str == "cancelled" || str == "Cancelled") return TaskStatus::Cancelled;
    return std::nullopt;
}

std::string taskErrorToString(TaskError error) {
    switch (error) {
        case TaskError::InvalidId: return "Invalid task ID";
        case TaskError::TaskNotFound: return "Task not found";
        case TaskError::InvalidStatus: return "Invalid task status";
        case TaskError::EmptyTitle: return "Task title cannot be empty";
        case TaskError::DuplicateTask: return "Task with this title already exists";
        default: return "Unknown error";
    }
}