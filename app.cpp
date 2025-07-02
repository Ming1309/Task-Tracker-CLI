#include "app.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

App::App() : _running(false) {}

void App::config() {
    initializeCommands();
}

void App::initializeCommands() {
    // Using C++23 designated initializers and lambda expressions
    _commands["add"] = Command{
        .name = "add",
        .description = "Add a new task (add \"title\" [description])",
        .handler = [this](const auto& args) { handleAdd(args); },
        .min_args = 1,
        .max_args = 2
    };
    
    _commands["list"] = Command{
        .name = "list",
        .description = "List all tasks or by status (list [status])",
        .handler = [this](const auto& args) { handleList(args); },
        .min_args = 0,
        .max_args = 1
    };
    
    _commands["complete"] = Command{
        .name = "complete",
        .description = "Mark task as completed (complete <task_id>)",
        .handler = [this](const auto& args) { handleComplete(args); },
        .min_args = 1,
        .max_args = 1
    };
    
    _commands["remove"] = Command{
        .name = "remove",
        .description = "Remove a task (remove <task_id>)",
        .handler = [this](const auto& args) { handleRemove(args); },
        .min_args = 1,
        .max_args = 1
    };
    
    _commands["status"] = Command{
        .name = "status",
        .description = "Update task status (status <task_id> <new_status>)",
        .handler = [this](const auto& args) { handleStatus(args); },
        .min_args = 2,
        .max_args = 2
    };
    
    _commands["priority"] = Command{
        .name = "priority",
        .description = "Set task priority (priority <task_id> <priority_number>)",
        .handler = [this](const auto& args) { handlePriority(args); },
        .min_args = 2,
        .max_args = 2
    };
    
    _commands["category"] = Command{
        .name = "category",
        .description = "Set task category (category <task_id> <category_name>)",
        .handler = [this](const auto& args) { handleCategory(args); },
        .min_args = 2,
        .max_args = 2
    };
    
    _commands["stats"] = Command{
        .name = "stats",
        .description = "Show task statistics",
        .handler = [this](const auto& args) { handleStats(args); },
        .min_args = 0,
        .max_args = 0
    };
    
    _commands["find"] = Command{
        .name = "find",
        .description = "Find tasks by title keyword (find <keyword>)",
        .handler = [this](const auto& args) { handleFind(args); },
        .min_args = 1,
        .max_args = 1
    };
    
    _commands["sort"] = Command{
        .name = "sort",
        .description = "Sort tasks by criteria (sort <priority|created|title>)",
        .handler = [this](const auto& args) { handleSort(args); },
        .min_args = 1,
        .max_args = 1
    };
    
    _commands["help"] = Command{
        .name = "help",
        .description = "Show this help message",
        .handler = [this](const auto& args) { displayHelp(); },
        .min_args = 0,
        .max_args = 0
    };
    
    _commands["exit"] = Command{
        .name = "exit",
        .description = "Exit the application",
        .handler = [this](const auto& args) { handleExit(args); },
        .min_args = 0,
        .max_args = 0
    };
    
    _commands["save"] = Command{
        .name = "save",
        .description = "Save tasks to JSON file (save [filename])",
        .handler = [this](const auto& args) { handleSave(args); },
        .min_args = 0,
        .max_args = 1
    };
    
    _commands["load"] = Command{
        .name = "load", 
        .description = "Load tasks from JSON file (load [filename])",
        .handler = [this](const auto& args) { handleLoad(args); },
        .min_args = 0,
        .max_args = 1
    };
    
    _commands["view"] = Command{
        .name = "view",
        .description = "View/print JSON file content (view [filename])",
        .handler = [this](const auto& args) { handleView(args); },
        .min_args = 0,
        .max_args = 1
    };
    
    _commands["matrix"] = Command{
        .name = "matrix",
        .description = "Show task matrix by category and priority (matrix)",
        .handler = [this](const auto& args) { handleMatrix(args); },
        .min_args = 0,
        .max_args = 0
    };
    
    _commands["get"] = Command{
        .name = "get",
        .description = "Get tasks by category and priority (get <category> <priority>)",
        .handler = [this](const auto& args) { handleGet(args); },
        .min_args = 2,
        .max_args = 2
    };
    
    _commands["recent"] = Command{
        .name = "recent",
        .description = "Show recent commands (recent)",
        .handler = [this](const auto& args) { handleRecent(args); },
        .min_args = 0,
        .max_args = 0
    };
}

void App::run() {
    _running = true;
    displayWelcome();
    
    std::string input;
    while (_running) {
        std::cout << "\n🚀 TaskTracker> ";
        if (!std::getline(std::cin, input)) {
            // EOF or input error, exit gracefully
            break;
        }
        
        if (input.empty()) continue;
        
        auto tokens = parseInput(input);
        if (tokens.empty()) continue;
        
        const std::string& command = tokens[0];
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        
        // C++23: Validate command using .contains()
        if (!validateCommand(command)) {
            std::cout << "❌ Unknown command: " << command << "\n";
            std::cout << "💡 Type 'help' to see available commands.\n";
            continue;
        }
        
        const Command& cmd = _commands[command];
        if (args.size() < cmd.min_args || args.size() > cmd.max_args) {
            std::print("❌ Invalid number of arguments for '{}'\n", command);
            std::print("📋 Usage: {}\n", cmd.description);
            continue;
        }
        
        // C++23: Track recent commands with auto(x) decay copy
        if (_recent_commands.size() >= MAX_RECENT_COMMANDS) {
            _recent_commands.erase(_recent_commands.begin());
        }
        _recent_commands.push_back(auto(command));  // Clean copy
        
        // C++23: std::expected pattern - commands handle their own errors
        cmd.handler(args);
    }
}

void App::displayWelcome() const {
    std::cout << R"(
╔══════════════════════════════════════════╗
║           🎯 Task Tracker CLI            ║
╚══════════════════════════════════════════╝

Welcome to your personal task management system!
Type 'help' to see available commands.
)";
}

void App::displayHelp() const {
    std::cout << "\n📋 Available Commands:\n";
    std::cout << "═══════════════════════\n";
    
    // Sort commands by name for better presentation
    std::vector<std::pair<std::string, Command>> sorted_commands;
    for (const auto& [name, cmd] : _commands) {
        sorted_commands.emplace_back(name, cmd);
    }
    
    std::ranges::sort(sorted_commands, [](const auto& a, const auto& b) {
        return a.first < b.first;
    });
    
    for (const auto& [name, cmd] : sorted_commands) {
        std::cout << std::format("  📌 {:<15} - {}\n", cmd.name, cmd.description);
    }
    
    std::cout << "\n💡 Examples:\n";
    std::cout << "  add \"Buy groceries\" \"Get milk, bread, and eggs\"\n";
    std::cout << "  list pending\n";
    std::cout << "  complete 1\n";
    std::cout << "  priority 2 5\n";
    std::cout << "  category 1 Shopping\n";
    std::cout << "  save tasks.json\n";
    std::cout << "  load tasks.json\n";
}

void App::handleAdd(const std::vector<std::string>& args) {
    const std::string& title = args[0];
    std::string description = args.size() > 1 ? args[1] : "";
    
    auto result = _task_manager.addTask(title, description);
    if (result) {
        std::cout << std::format("✅ Task '{}' added successfully with ID = {}\n", title, *result);
    } else {
        handleError(result.error());
    }
}

void App::handleList(const std::vector<std::string>& args) {
    if (args.empty()) {
        _task_manager.listTasks();
    } else {
        auto status = stringToTaskStatus(args[0]);
        if (status) {
            _task_manager.listTasksByStatus(*status);
        } else {
            std::cout << std::format("❌ Invalid status: {}\n", args[0]);
            std::cout << "📋 Valid statuses: pending, progress, completed, cancelled\n";
        }
    }
}

void App::handleComplete(const std::vector<std::string>& args) {
    auto id_result = parseInteger(args[0]);
    if (!id_result) {
        std::cout << std::format("❌ {}\n", parseErrorToString(id_result.error()));
        return;
    }
    
    int id = *id_result;
    auto result = _task_manager.updateTaskStatus(id, TaskStatus::Completed);
    if (result) {
        std::cout << std::format("✅ Task {} marked as completed!\n", id);
    } else {
        handleError(result.error());
    }
}

void App::handleRemove(const std::vector<std::string>& args) {
    auto id_result = parseInteger(args[0]);
    if (!id_result) {
        std::cout << std::format("❌ {}\n", parseErrorToString(id_result.error()));
        return;
    }
    
    int id = *id_result;
    auto result = _task_manager.removeTask(id);
    if (result) {
        std::cout << std::format("🗑️ Task {} removed successfully!\n", id);
    } else {
        handleError(result.error());
    }
}

void App::handleStatus(const std::vector<std::string>& args) {
    auto id_result = parseInteger(args[0]);
    if (!id_result) {
        std::cout << std::format("❌ {}\n", parseErrorToString(id_result.error()));
        return;
    }
    
    int id = *id_result;
    auto status = stringToTaskStatus(args[1]);
    if (!status) {
        std::cout << std::format("❌ Invalid status: {}\n", args[1]);
        return;
    }
    
    auto result = _task_manager.updateTaskStatus(id, *status);
    if (result) {
        std::cout << std::format("📝 Task {} status updated to {}\n", id, taskStatusToString(*status));
    } else {
        handleError(result.error());
    }
}

void App::handlePriority(const std::vector<std::string>& args) {
    auto id_result = parseInteger(args[0]);
    if (!id_result) {
        std::cout << std::format("❌ {}\n", parseErrorToString(id_result.error()));
        return;
    }
    
    auto priority_result = parseInteger(args[1]);
    if (!priority_result) {
        std::cout << std::format("❌ {}\n", parseErrorToString(priority_result.error()));
        return;
    }
    
    int id = *id_result;
    int priority = *priority_result;
    
    auto task_result = _task_manager.getTask(id);
    if (!task_result) {
        handleError(task_result.error());
        return;
    }
    
    // Get mutable reference to task in manager - no const_cast needed with deducing this
    auto& tasks = _task_manager.getAllTasks();
    auto it = std::ranges::find_if(tasks, [id](Task& task) {
        return task.getId() == id;
    });
    
    if (it != tasks.end()) {
        auto result = it->setPriority(priority);
        if (result) {
            std::cout << std::format("🎯 Task {} priority set to {}\n", id, priority);
        } else {
            handleError(result.error());
        }
    }
}

void App::handleCategory(const std::vector<std::string>& args) {
    auto id_result = parseInteger(args[0]);
    if (!id_result) {
        std::cout << std::format("❌ {}\n", parseErrorToString(id_result.error()));
        return;
    }
    
    int id = *id_result;
    const std::string& category = args[1];
    
    auto task_result = _task_manager.getTask(id);
    if (!task_result) {
        handleError(task_result.error());
        return;
    }
    
    // Get mutable reference to task in manager - no const_cast needed with deducing this
    auto& tasks = _task_manager.getAllTasks();
    auto it = std::ranges::find_if(tasks, [id](Task& task) {
        return task.getId() == id;
    });
    
    if (it != tasks.end()) {
        auto result = it->setCategory(category);
        if (result) {
            std::cout << std::format("🏷️ Task {} category set to '{}'\n", id, category);
        } else {
            handleError(result.error());
        }
    }
}

void App::handleStats(const std::vector<std::string>& args) {
    size_t total = _task_manager.getTaskCount();
    size_t completed = _task_manager.getCompletedTasksCount();
    size_t pending = _task_manager.getPendingTasksCount();
    double completion_rate = _task_manager.getCompletionRate();
    
    std::cout << "\n📊 Task Statistics\n";
    std::cout << "══════════════════\n";
    std::cout << std::format("📋 Total Tasks:     {}\n", total);
    std::cout << std::format("✅ Completed:       {}\n", completed);
    std::cout << std::format("⏳ Pending:         {}\n", pending);
    std::cout << std::format("🚧 In Progress:     {}\n", 
                total - completed - pending - std::ranges::count_if(_task_manager.getAllTasks(), 
                [](const Task& t) { return t.getStatus() == TaskStatus::Cancelled; }));
    std::cout << std::format("📈 Completion Rate: {:.1f}%\n", completion_rate);
}

void App::handleFind(const std::vector<std::string>& args) {
    const std::string& keyword = args[0];
    
    // C++23: auto(x) decay copy for safer lambda captures
    auto filterTasks = [keyword = auto(keyword)](const Task& task) {
        const std::string& title = task.getTitle();
        const std::string& desc = task.getDescription();
        
        // Case-insensitive search
        auto to_lower = [](std::string str) {
            std::ranges::transform(str, str.begin(), ::tolower);
            return str;
        };
        
        std::string lower_keyword = to_lower(keyword);
        std::string lower_title = to_lower(title);
        std::string lower_desc = to_lower(desc);
        
        return lower_title.contains(lower_keyword) || lower_desc.contains(lower_keyword);
    };
    
    auto matching_tasks = _task_manager.filterTasks(filterTasks);
    
    auto task_vector = std::vector<Task>(matching_tasks.begin(), matching_tasks.end());
    
    if (task_vector.empty()) {
        std::cout << std::format("🔍 No tasks found containing: '{}'\n", keyword);
    } else {
        std::cout << std::format("🔍 Found {} task(s) containing '{}'\n", task_vector.size(), keyword);
        for (const auto& task : task_vector) {
            std::cout << std::format("  [{}] {} - {}\n", 
                        task.getId(), task.getTitle(), taskStatusToString(task.getStatus()));
        }
    }
}

void App::handleSort(const std::vector<std::string>& args) {
    const std::string& criteria = args[0];
    
    if (criteria == "priority") {
        auto sorted = _task_manager.getSortedTasks([](const Task& a, const Task& b) {
            return a.getMetadata().priority > b.getMetadata().priority; // High priority first
        });
        
        std::cout << "📊 Tasks sorted by priority (highest first):\n";
        for (const auto& task : sorted) {
            std::cout << std::format("  [{}] {} - Priority: {}\n", 
                        task.getId(), task.getTitle(), task.getMetadata().priority);
        }
    }
    else if (criteria == "created") {
        auto sorted = _task_manager.getSortedTasks([](const Task& a, const Task& b) {
            return a.getMetadata().created_at > b.getMetadata().created_at; // Newest first
        });
        
        std::cout << "📊 Tasks sorted by creation date (newest first):\n";
        for (const auto& task : sorted) {
            std::cout << std::format("  [{}] {} - Age: {:.1f} hours\n", 
                        task.getId(), task.getTitle(), task.getAge().count() / 3600.0);
        }
    }
    else if (criteria == "title") {
        auto sorted = _task_manager.getSortedTasks([](const Task& a, const Task& b) {
            return a.getTitle() < b.getTitle(); // Alphabetical
        });
        
        std::cout << "📊 Tasks sorted alphabetically:\n";
        for (const auto& task : sorted) {
            std::cout << std::format("  [{}] {}\n", task.getId(), task.getTitle());
        }
    }
    else {
        std::cout << std::format("❌ Invalid sort criteria: {}\n", criteria);
        std::cout << "📋 Valid options: priority, created, title\n";
    }
}

void App::handleExit(const std::vector<std::string>& args) {
    std::cout << "\n👋 Thank you for using Task Tracker! Have a productive day!\n";
    _running = false;
}

void App::handleJsonError(JsonError error) const {
    std::cout << std::format("❌ JSON Error: {}\n", jsonErrorToString(error));
}

void App::handleSave(const std::vector<std::string>& args) {
    std::string filename = args.empty() ? "tasks.json" : args[0];
    
    std::cout << std::format("💾 Saving tasks to {}...\n", filename);
    
    auto result = _task_manager.saveToJson(filename);
    if (result) {
        std::cout << std::format("✅ Tasks saved successfully to {}\n", filename);
        std::cout << std::format("📊 Total tasks saved: {}\n", _task_manager.getTaskCount());
    } else {
        handleJsonError(result.error());
        std::cout << "💡 Make sure the directory exists and you have write permissions.\n";
    }
}

void App::handleLoad(const std::vector<std::string>& args) {
    std::string filename = args.empty() ? "tasks.json" : args[0];
    
    std::cout << std::format("📂 Loading tasks from {}...\n", filename);
    
    auto result = _task_manager.loadFromJson(filename);
    if (result) {
        std::cout << std::format("✅ Tasks loaded successfully from {}\n", filename);
        std::cout << std::format("📊 Total tasks loaded: {}\n", _task_manager.getTaskCount());
        
        // Show a summary of loaded tasks
        if (_task_manager.getTaskCount() > 0) {
            std::cout << "\n📋 Task Summary:\n";
            std::cout << std::format("  • Pending: {}\n", _task_manager.getPendingTasksCount());
            std::cout << std::format("  • Completed: {}\n", _task_manager.getCompletedTasksCount());
            std::cout << std::format("  • Completion Rate: {:.1f}%\n", _task_manager.getCompletionRate());
        }
    } else {
        handleJsonError(result.error());
        if (result.error() == JsonError::FileNotFound) {
            std::cout << std::format("💡 File '{}' not found. Use 'save' command to create it.\n", filename);
        } else {
            std::cout << "💡 Make sure the file exists and contains valid JSON.\n";
        }
    }
}

std::expected<std::string, JsonError> App::readFileContent(const std::string& filename) const {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return std::unexpected(JsonError::FileNotFound);
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    
    if (file.fail() && !file.eof()) {
        return std::unexpected(JsonError::ParseError);
    }
    
    return buffer.str();
}

void App::handleView(const std::vector<std::string>& args) {
    std::string filename = args.empty() ? "tasks.json" : args[0];
    
    std::cout << std::format("👁️ Viewing JSON file: {}...\n", filename);
    
    // Use std::expected for file reading
    auto content_result = readFileContent(filename);
    if (!content_result) {
        handleJsonError(content_result.error());
        std::cout << "💡 Make sure the file exists and is readable.\n";
        return;
    }
    
    std::string json_content = *content_result;
    
    if (json_content.empty()) {
        std::cout << "⚠️ Note: File is empty\n";
        return;
    }
    
    // Parse and display in table format
    displayJsonAsTable(json_content);
}

void App::displayJsonAsTable(const std::string& json_content) {
    // Simple parsing to extract basic info
    auto findValue = [&json_content](const std::string& key) -> std::string {
        std::string search_key = "\"" + key + "\":";
        size_t pos = json_content.find(search_key);
        if (pos == std::string::npos) return "";
        
        pos += search_key.length();
        while (pos < json_content.length() && std::isspace(json_content[pos])) ++pos;
        
        if (pos >= json_content.length()) return "";
        
        if (json_content[pos] == '"') {
            ++pos;
            size_t end_pos = pos;
            while (end_pos < json_content.length() && json_content[end_pos] != '"') {
                if (json_content[end_pos] == '\\') ++end_pos;
                ++end_pos;
            }
            return unescapeJsonString(json_content.substr(pos, end_pos - pos));
        } else {
            size_t end_pos = pos;
            while (end_pos < json_content.length() && 
                   json_content[end_pos] != ',' && 
                   json_content[end_pos] != '}' && 
                   json_content[end_pos] != '\n' &&
                   json_content[end_pos] != '\r') {
                ++end_pos;
            }
            std::string value = json_content.substr(pos, end_pos - pos);
            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);
            return value;
        }
    };
    
    std::string version = findValue("version");
    std::string next_id = findValue("next_id");
    
    // Display file metadata
    std::cout << "\n📊 File Information:\n";
    std::cout << "┌─────────────┬────────────────────────────┐\n";
    std::cout << "│ Property    │ Value                      │\n";
    std::cout << "├─────────────┼────────────────────────────┤\n";
    std::cout << std::format("│ Version     │ {:<26} │\n", version.empty() ? "N/A" : version);
    std::cout << std::format("│ Next ID     │ {:<26} │\n", next_id.empty() ? "N/A" : next_id);
    std::cout << std::format("│ File Size   │ {:<26} │\n", std::to_string(json_content.length()) + " bytes");
    std::cout << "└─────────────┴────────────────────────────┘\n";
    
    // Parse tasks and display in table format
    std::vector<TaskInfo> tasks = parseTasksFromJson(json_content);
    
    if (tasks.empty()) {
        std::cout << "\n📝 No tasks found in the file.\n";
        return;
    }
    
    std::cout << std::format("\n📋 Tasks ({} total):\n", tasks.size());
    
    // Table header
    std::cout << "┌────┬─────────────────────┬─────────────┬─────────────┬──────────┬─────────────────────┐\n";
    std::cout << "│ ID │ Title               │ Status      │ Category    │ Priority │ Created At          │\n";
    std::cout << "├────┼─────────────────────┼─────────────┼─────────────┼──────────┼─────────────────────┤\n";
    
    // Table rows
    for (const auto& task : tasks) {
        std::string truncated_title = task.title.length() > 19 ? 
            task.title.substr(0, 16) + "..." : task.title;
        std::string truncated_category = task.category.length() > 11 ? 
            task.category.substr(0, 8) + "..." : task.category;
        
        std::cout << std::format("│{:>3} │ {:<19} │ {:<11} │ {:<11} │{:>9} │ {:<19} │\n",
            task.id,
            truncated_title,
            task.status,
            truncated_category,
            task.priority,
            task.created_at.substr(0, 19) // Show date with seconds (19 chars for YYYY-MM-DDTHH:MM:SS)
        );
    }
    
    std::cout << "└────┴─────────────────────┴─────────────┴─────────────┴──────────┴─────────────────────┘\n";
    
    // Summary statistics
    int pending = 0, completed = 0, in_progress = 0, cancelled = 0;
    for (const auto& task : tasks) {
        if (task.status == "Pending") pending++;
        else if (task.status == "Completed") completed++;
        else if (task.status == "InProgress") in_progress++;
        else if (task.status == "Cancelled") cancelled++;
    }
    
    std::cout << "\n📈 Summary:\n";
    std::cout << std::format("  • Pending: {} | Completed: {} | In Progress: {} | Cancelled: {}\n", 
        pending, completed, in_progress, cancelled);
    
    if (tasks.size() > 0) {
        double completion_rate = (double)completed / tasks.size() * 100.0;
        std::cout << std::format("  • Completion Rate: {:.1f}%\n", completion_rate);
    }
}

std::vector<App::TaskInfo> App::parseTasksFromJson(const std::string& json_content) {
    std::vector<TaskInfo> tasks;
    
    // Find tasks array
    size_t tasks_pos = json_content.find("\"tasks\":");
    if (tasks_pos == std::string::npos) {
        return tasks; // No tasks array found
    }
    
    // Find opening bracket of tasks array
    size_t array_start = json_content.find('[', tasks_pos);
    if (array_start == std::string::npos) {
        return tasks;
    }
    
    // Simple parser to extract task objects
    size_t pos = array_start + 1;
    while (pos < json_content.length()) {
        // Skip whitespace
        while (pos < json_content.length() && std::isspace(json_content[pos])) {
            pos++;
        }
        
        if (pos >= json_content.length() || json_content[pos] == ']') {
            break; // End of array
        }
        
        if (json_content[pos] == '{') {
            // Parse task object
            TaskInfo task;
            task.id = 0;
            task.priority = 0;
            
            size_t obj_end = pos;
            int brace_count = 1;
            obj_end++;
            
            // Find end of object
            while (obj_end < json_content.length() && brace_count > 0) {
                if (json_content[obj_end] == '{') brace_count++;
                else if (json_content[obj_end] == '}') brace_count--;
                obj_end++;
            }
            
            std::string task_obj = json_content.substr(pos, obj_end - pos);
            
            // Extract task properties
            auto extractValue = [&task_obj](const std::string& key) -> std::string {
                std::string search_key = "\"" + key + "\":";
                size_t key_pos = task_obj.find(search_key);
                if (key_pos == std::string::npos) return "";
                
                key_pos += search_key.length();
                while (key_pos < task_obj.length() && std::isspace(task_obj[key_pos])) {
                    key_pos++;
                }
                
                if (key_pos >= task_obj.length()) return "";
                
                if (task_obj[key_pos] == '"') {
                    key_pos++;
                    size_t end_pos = key_pos;
                    while (end_pos < task_obj.length() && task_obj[end_pos] != '"') {
                        if (task_obj[end_pos] == '\\') end_pos++;
                        end_pos++;
                    }
                    return unescapeJsonString(task_obj.substr(key_pos, end_pos - key_pos));
                } else {
                    size_t end_pos = key_pos;
                    while (end_pos < task_obj.length() && 
                           task_obj[end_pos] != ',' && 
                           task_obj[end_pos] != '}' && 
                           task_obj[end_pos] != '\n' &&
                           task_obj[end_pos] != '\r') {
                        end_pos++;
                    }
                    std::string value = task_obj.substr(key_pos, end_pos - key_pos);
                    value.erase(0, value.find_first_not_of(" \t\n\r"));
                    value.erase(value.find_last_not_of(" \t\n\r") + 1);
                    return value;
                }
            };
            
            // Parse task fields
            std::string id_str = extractValue("id");
            if (!id_str.empty()) {
                task.id = std::stoi(id_str);
            }
            
            task.title = extractValue("title");
            task.status = extractValue("status");
            task.category = extractValue("category");
            task.created_at = extractValue("created_at");
            task.description = extractValue("description");
            
            std::string priority_str = extractValue("priority");
            if (!priority_str.empty()) {
                task.priority = std::stoi(priority_str);
            }
            
            tasks.push_back(task);
            pos = obj_end;
        } else {
            pos++;
        }
    }
    
    return tasks;
}

std::vector<std::string> App::parseInput(const std::string& input) const {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    bool in_quotes = false;
    std::string current_token;
    
    char c;
    while (iss.get(c)) {
        if (c == '"') {
            in_quotes = !in_quotes;
        } else if (c == ' ' && !in_quotes) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        } else {
            current_token += c;
        }
    }
    
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    
    return tokens;
}

// C++23: std::expected-based parsing utilities
std::expected<int, App::ParseError> App::parseInteger(const std::string& str) const {
    if (str.empty()) {
        return std::unexpected(ParseError::Empty);
    }
    
    try {
        size_t pos;
        int result = std::stoi(str, &pos);
        
        // Check if entire string was consumed
        if (pos != str.length()) {
            return std::unexpected(ParseError::InvalidFormat);
        }
        
        return result;
    } catch (const std::invalid_argument&) {
        return std::unexpected(ParseError::InvalidFormat);
    } catch (const std::out_of_range&) {
        return std::unexpected(ParseError::OutOfRange);
    }
}

std::string App::parseErrorToString(ParseError error) const {
    switch (error) {
        case ParseError::InvalidFormat: return "Invalid number format";
        case ParseError::OutOfRange: return "Number out of range";
        case ParseError::Empty: return "Empty input";
        default: return "Unknown parse error";
    }
}

void App::handleError(TaskError error) const {
    std::cout << std::format("❌ Error: {}\n", taskErrorToString(error));
}

void App::printTaskDetails(const Task& task) const {
    std::cout << task.to_string() << "\n";
}

// C++23: New handlers using multidimensional subscript and other features
void App::handleMatrix(const std::vector<std::string>& args) {
    // Rebuild matrix from current tasks
    _task_matrix.clear();
    
    // C++23: auto(x) decay copy for clean iteration
    for (const auto& task : _task_manager.getAllTasks()) {
        _task_matrix.addTask(auto(task));  // Clean copy
    }
    
    if (_task_matrix.getTotalTaskCount() == 0uz) {  // C++23: uz suffix
        std::print("📭 No tasks to display in matrix\n");
        return;
    }
    
    _task_matrix.displayMatrix();
    
    // Display statistics
    std::print("\n📈 Matrix Statistics:\n");
    std::print("  📊 Total tasks: {}\n", _task_matrix.getTotalTaskCount());
    std::print("  📂 Categories: {}\n", _task_matrix.getCategories().size());
}

void App::handleGet(const std::vector<std::string>& args) {
    const std::string& category = args[0];
    
    auto priority_result = parseInteger(args[1]);
    if (!priority_result) {
        std::print("❌ {}\n", parseErrorToString(priority_result.error()));
        return;
    }
    
    int priority = *priority_result;
    
    // Rebuild matrix
    _task_matrix.clear();
    for (const auto& task : _task_manager.getAllTasks()) {
        _task_matrix.addTask(task);
    }
    
    // C++23: Multidimensional subscript operator [category, priority]
    const auto& tasks = _task_matrix[category, priority];
    
    if (tasks.empty()) {
        std::print("📭 No tasks found for category '{}' with priority {}\n", 
                   category, priority);
        return;
    }
    
    std::print("🎯 Tasks in category '{}' with priority {}:\n", category, priority);
    std::print("===============================================\n");
    
    for (const auto& task : tasks) {
        std::print("  [{}] {} - {}\n", 
                   task.getId(), 
                   task.getTitle(), 
                   getTaskStatusString(task.getStatus()));  // C++23: consteval function
    }
    
    std::print("\n📊 Found {} task(s)\n", tasks.size());
}

void App::handleRecent(const std::vector<std::string>& args) {
    if (_recent_commands.empty()) {
        std::print("📭 No recent commands\n");
        return;
    }
    
    std::print("🕐 Recent Commands:\n");
    std::print("==================\n");
    
    // C++23: uz suffix and ranges
    for (size_t i = 0uz; i < std::min(_recent_commands.size(), MAX_RECENT_COMMANDS); ++i) {
        std::print("  {}. {}\n", i + 1, _recent_commands[i]);
    }
}