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
}

void App::run() {
    _running = true;
    displayWelcome();
    
    std::string input;
    while (_running) {
        std::cout << "\n🚀 TaskTracker> ";
        std::getline(std::cin, input);
        
        if (input.empty()) continue;
        
        auto tokens = parseInput(input);
        if (tokens.empty()) continue;
        
        const std::string& command = tokens[0];
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        
        auto cmd_it = _commands.find(command);
        if (cmd_it == _commands.end()) {
            std::cout << "❌ Unknown command: " << command << "\n";
            std::cout << "💡 Type 'help' to see available commands.\n";
            continue;
        }
        
        const Command& cmd = cmd_it->second;
        if (args.size() < cmd.min_args || args.size() > cmd.max_args) {
            std::cout << std::format("❌ Invalid number of arguments for '{}'\n", command);
            std::cout << std::format("📋 Usage: {}\n", cmd.description);
            continue;
        }
        
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
        std::cout << std::format("✅ Task '{}' added successfully!\n", title);
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
    
    // Get mutable reference to task in manager
    auto& tasks = const_cast<std::vector<Task>&>(_task_manager.getAllTasks());
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
    
    // Get mutable reference to task in manager
    auto& tasks = const_cast<std::vector<Task>&>(_task_manager.getAllTasks());
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
    
    auto matching_tasks = _task_manager.filterTasks([&keyword](const Task& task) {
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
    });
    
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