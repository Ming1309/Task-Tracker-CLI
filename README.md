# 🎯 Task Tracker CLI

Một ứng dụng quản lý công việc được phát triển để minh họa và áp dụng các tính năng mới nhất của C++23. Dự án này tập trung vào việc sử dụng thực tế các kỹ thuật hiện đại để xây dựng một ứng dụng CLI hoàn chỉnh và hiệu quả.

## 👨‍🎓 Thông tin sinh viên
| Thông tin | Nội dung |
|-----------|---------|
| **Họ và tên** | Phạm Quang Minh |
| **MSSV** | 23120298 |

## 🚀 Tổng Quan Ứng Dụng

Task Tracker CLI là một ứng dụng dòng lệnh cho phép người dùng:
- Quản lý danh sách công việc
- Phân loại công việc theo danh mục và mức độ ưu tiên
- Theo dõi trạng thái thực hiện (Pending, In Progress, Completed, Cancelled)
- Tìm kiếm và sắp xếp công việc theo nhiều tiêu chí
- Lưu trữ và tải dữ liệu từ file JSON
- Hiển thị thống kê chi tiết về hiệu suất công việc

### ✨ Điểm Nổi Bật

- **C++23**: Sử dụng các tính năng mới nhất của C++23
- **Modern Architecture**: Thiết kế theo các nguyên tắc SOLID và C++ hiện đại
- **Type Safety**: Sử dụng concepts và strong typing để đảm bảo an toàn kiểu
- **Error Handling**: Xử lý lỗi functional với `std::expected` thay vì exceptions
- **Performance**: Tối ưu hóa với ranges, views và zero-cost abstractions
- **User Experience**: Giao diện CLI đẹp mắt với emoji và formatting

## 🔧 Hướng Dẫn Build

### Yêu Cầu Hệ Thống
- **Compiler**: Clang 17+ hoặc GCC 14+ (hỗ trợ C++23)
- **Build System**: CMake 3.20+
- **OS**: macOS, Linux, hoặc Windows

### Các Bước Build
```bash
# tạo thư mục build
mkdir build && cd build

# Configure với CMake
cmake ..
# Nếu sử dụng clang: cmake -DCMAKE_CXX_COMPILER=clang++ ..

# Build dự án
make

# Chạy ứng dụng
./TaskTracker
```

## 📋 Hướng Dẫn Sử Dụng

### Các Lệnh Cơ Bản

| Lệnh | Mô tả | Ví dụ |
|------|-------|-------|
| `add` | Thêm task mới | `add "Mua sắm" "Mua sữa và bánh mì"` |
| `list` | Liệt kê tasks | `list`, `list pending` |
| `complete` | Đánh dấu hoàn thành | `complete 1` |
| `remove` | Xóa task | `remove 2` |
| `status` | Thay đổi trạng thái | `status 1 progress` |
| `priority` | Thiết lập độ ưu tiên | `priority 1 5` |
| `category` | Thiết lập danh mục | `category 1 Shopping` |
| `find` | Tìm kiếm theo từ khóa | `find sữa` |
| `sort` | Sắp xếp công việc | `sort priority`, `sort created` |
| `stats` | Hiển thị thống kê | `stats` |
| `save` | Lưu vào file JSON | `save tasks.json` |
| `load` | Tải từ file JSON | `load tasks.json` |
| `view` | Xem thông tin file JSON | `view tasks.json` |
| `matrix` | Hiển thị dạng ma trận | `matrix` |
| `help` | Hiển thị trợ giúp | `help` |
| `exit` | Thoát ứng dụng | `exit` |


## 🎯 10 Kỹ Thuật C++23 Được Sử Dụng

### 1. **Deducing This (Explicit Object Parameter)**
```cpp
// Trước C++23: Cần duplicate code cho const và non-const versions
// class Task {
//   public:
//     const std::string& getTitle() const { return _title; }
//     std::string& getTitle() { return _title; }
// };

// C++23: Một method duy nhất cho cả const và non-const
class Task {
  public:
    template<typename Self>
    auto getTitle(this Self&& self) -> decltype(auto) {
        return std::forward<Self>(self)._title;
    }
};
```
**Mô tả**: Cho phép sử dụng từ khóa `this` làm tham số đầu tiên của method, hỗ trợ perfect forwarding cho phương thức và loại bỏ code duplication.\
**Ứng dụng**: Trong Task Tracker, được dùng cho tất cả accessors (getId, getTitle, getDescription) và TaskMatrix methods để giảm code duplication.

### 2. **std::print**
```cpp
// Trước C++23: Output dùng iostream
// std::cout << "Task '" << title << "' added successfully!" << std::endl;
// std::cout << "Found " << tasks.size() << " task(s)" << std::endl;

// C++23: std::print - API đơn giản và mạnh mẽ hơn
std::print("✅ Task '{}' added successfully!\n", title);
std::print("📊 Found {} task(s)\n", tasks.size());
```
**Mô tả**: API formatting mới thay thế cho `std::cout` và `std::format`, cung cấp cú pháp đơn giản, hiệu suất cao và type-safe.\
**Ứng dụng**: Sử dụng trong toàn bộ ứng dụng Task Tracker cho output (hiển thị danh sách, thông báo, thống kê).

### 3. **std::expected**
```cpp
// C++23: Functional error handling thay vì exceptions
struct TaskError {
    enum Code { InvalidId, EmptyTitle, DuplicateId } code;
    std::string message;
};

// Return type biểu thị rõ ràng khả năng lỗi
using TaskResult = std::expected<bool, TaskError>;

TaskResult TaskManager::addTask(const std::string& title, const std::string& desc) {
    if (title.empty()) {
        return std::unexpected(TaskError{
            .code = TaskError::EmptyTitle,
            .message = "Task title cannot be empty"
        });
    }
    // Add task implementation...
    return true;
}

// Sử dụng kết quả
auto result = taskManager.addTask("New Task", "Description");
if (result) {
    std::print("Task added successfully\n");
} else {
    std::print("Error: {}\n", result.error().message);
}
```
**Mô tả**: Cung cấp cách xử lý lỗi functional, rõ ràng hơn so với exception, hiệu suất cao hơn, và giúp API biểu thị rõ khả năng lỗi trong kiểu trả về.\
**Ứng dụng**: Sử dụng trong toàn bộ Task Tracker cho mọi operation có thể gặp lỗi (thêm, xóa, cập nhật task, I/O operations).

### 4. **Multidimensional Subscript Operator**
```cpp
// C++23: Operator[] có thể nhận nhiều tham số
class TaskMatrix {
    std::map<std::string, std::map<int, std::vector<Task>>> matrix;

public:
    // Truy cập theo category và priority trong một lần gọi
    template<typename Self>
    decltype(auto) operator[](this Self&& self, 
                             const std::string& category, 
                             int priority) {
        if constexpr (std::is_const_v<std::remove_reference_t<Self>>) {
            // Const version: chỉ đọc, không tạo mới
            static const std::vector<Task> empty;
            auto it = self.matrix.find(category);
            if (it == self.matrix.end()) return empty;
            auto pit = it->second.find(priority);
            if (pit == it->second.end()) return empty;
            return pit->second;
        } else {
            // Non-const version: tạo mới nếu không tồn tại
            return self.matrix[category][priority];
        }
    }
};

// Sử dụng
taskMatrix["Work", 5].push_back(task);  // Thêm task vào "Work" với priority 5
auto highPriorityTasks = taskMatrix["Personal", 10];  // Lấy tasks với priority 10
```
**Mô tả**: Cho phép operator[] nhận nhiều tham số, tạo ra cú pháp tự nhiên cho truy cập đa chiều.\
**Ứng dụng**: Trong TaskMatrix để tổ chức và truy cập tasks theo category và priority một cách trực quan.

### 5. **auto(x) Lambda Capture**
```cpp
// Trước C++23: Cần tạo bản copy thủ công 
// auto keyword_copy = keyword;
// auto filterTasks = [keyword = keyword_copy](const Task& task) { ... };

// C++23: auto(x) tạo decay-copy tự động
auto filterTasks = [keyword = auto(keyword)](const Task& task) {
    const std::string& title = task.getTitle();
    const std::string& desc = task.getDescription();
    
    return title.contains(keyword) || desc.contains(keyword);
};
```
**Mô tả**: Cú pháp `auto(x)` tạo decay-copy tự động trong lambda captures, giải quyết dangling reference problem, an toàn hơn và ít code hơn.\
**Ứng dụng**: Trong Task Tracker dùng cho filtering operations, search functionality và handlers khi cần capture biến từ scope bên ngoài.

### 6. **size_t Literal Suffix (uz)**
```cpp
// Trước C++23: Phải cast hoặc dùng static_cast
// constexpr std::size_t MAX_TASKS = 100;
// for (std::size_t i = 0; i < tasks.size(); ++i) { ... }

// C++23: Size-specific literal suffix
constexpr auto MAX_COMMAND_ARGS = 5uz;  // uz suffix cho std::size_t
constexpr auto MAX_INPUT_LENGTH = 1000uz;
static constexpr auto MAX_RECENT_COMMANDS = 10uz;

// Sử dụng trong loops
for (std::size_t i = 0uz; i < tasks.size(); ++i) {
    // Process tasks
}

// So sánh với size()
if (command.size() > MAX_COMMAND_LENGTH) { ... }
```
**Mô tả**: Suffix `uz` cho phép tạo literals kiểu `std::size_t` mà không cần cast, giúp tránh warnings và type-safety.\
**Ứng dụng**: Trong Task Tracker dùng để định nghĩa constants, indices và so sánh với container sizes.

### 7. **std::string::contains**
```cpp
// Trước C++23: Kiểm tra substring với find() != npos
// if (title.find(keyword) != std::string::npos) { ... }

// C++23: Sử dụng contains() rõ ràng và ngắn gọn hơn
bool Task::containsKeyword(const std::string& keyword) const {
    return _title.contains(keyword) || _description.contains(keyword);
}

// Kiểm tra command hợp lệ
bool AppController::isValidCommand(const std::string& cmd) const {
    return !cmd.empty() && _commands.contains(cmd);
}
```
**Mô tả**: Method `contains()` cho phép kiểm tra sự tồn tại của substring hoặc key trong containers một cách ngắn gọn và trực quan hơn.\
**Ứng dụng**: Trong Task Tracker dùng cho validation, search functionality và map lookups.

### 8. **if consteval**
```cpp
// C++23: if consteval cho compile-time vs runtime behavior
constexpr std::string_view getTaskStatusString(TaskStatus status) {
    if consteval {
        // Compile-time: Basic strings
        switch (status) {
            case TaskStatus::Pending: return "Pending";
            case TaskStatus::InProgress: return "InProgress";
            case TaskStatus::Completed: return "Completed";
            case TaskStatus::Cancelled: return "Cancelled";
        }
        return "Unknown";
    } else {
        // Runtime: Rich formatted strings
        switch (status) {
            case TaskStatus::Pending: return "⏳ Pending";
            case TaskStatus::InProgress: return "🚧 In Progress";
            case TaskStatus::Completed: return "✅ Completed";
            case TaskStatus::Cancelled: return "❌ Cancelled";
            default: return "❓ Unknown";
        }
    }
}
```
**Mô tả**: `if consteval` cho phép có hành vi khác nhau tại compile-time và runtime trong constexpr functions.\
**Ứng dụng**: Trong Task Tracker dùng cho biến đổi enum sang string, validation và formatting khác nhau giữa serialization và display.

### 9. **consteval Functions**
```cpp
// C++23: consteval yêu cầu compile-time evaluation
consteval bool isValidPriority(int priority) {
    return priority >= 0 && priority <= 10;
}

consteval std::size_t maxTaskTitleLength() {
    return 100uz;  // C++23: uz suffix
}

// Sử dụng
static_assert(isValidPriority(5));  // OK
static_assert(!isValidPriority(11)); // Compile error
static_assert(maxTaskTitleLength() == 100uz); // OK

// Runtime check với constexpr variable
constexpr auto MAX_PRIORITY = 10;
if (priority > MAX_PRIORITY) { 
    return std::unexpected(TaskError{.code = TaskError::InvalidPriority});
}
```
**Mô tả**: Functions được đánh dấu `consteval` bắt buộc phải được tính toán tại compile-time, không thể dùng ở runtime, giúp enforce compile-time checks và tối ưu code.\
**Ứng dụng**: Trong Task Tracker dùng cho validation logic, defining constants và compile-time checks.

### 10. **Designated Initializers với Nested Structs**
```cpp
// C++23: Designated initializers với nested structs
struct Command {
    std::string name;
    std::string description;
    std::function<void(const std::vector<std::string>&)> handler;
    size_t min_args;
    size_t max_args;
    struct Help {
        std::string usage;
        std::string example;
        bool is_advanced;
    } help;
};

// Initialization đầy đủ và rõ ràng
_commands["add"] = Command{
    .name = "add",
    .description = "Add a new task",
    .handler = [this](const auto& args) { handleAdd(args); },
    .min_args = 1,
    .max_args = 2,
    .help = {
        .usage = "add \"title\" [\"description\"]",
        .example = "add \"Buy groceries\" \"Milk, eggs, bread\"",
        .is_advanced = false
    }
};
```
**Mô tả**: C++23 mở rộng designated initializers cho nested structs, cho phép initialization rõ ràng và có cấu trúc tốt hơn.\
**Ứng dụng**: Trong Task Tracker dùng cho command definitions, configuration và complex data structures.

---
## Demo 

```bash
╔══════════════════════════════════════════╗
║           🎯 Task Tracker CLI            ║
╚══════════════════════════════════════════╝

Welcome to your personal task management system!
Type 'help' to see available commands.

🚀 TaskTracker> help

📋 Available Commands:
═══════════════════════
  📌 add             - Add a new task (add "title" [description])
  📌 category        - Set task category (category <task_id> <category_name>)
  📌 complete        - Mark task as completed (complete <task_id>)
  📌 exit            - Exit the application
  📌 find            - Find tasks by title keyword (find <keyword>)
  📌 get             - Get tasks by category and priority (get <category> <priority>)
  📌 help            - Show this help message
  📌 list            - List all tasks or by status (list [status])
  📌 load            - Load tasks from JSON file (load [filename])
  📌 matrix          - Show task matrix by category and priority (matrix)
  📌 priority        - Set task priority (priority <task_id> <priority_number>)
  📌 recent          - Show recent commands (recent)
  📌 remove          - Remove a task (remove <task_id>)
  📌 save            - Save tasks to JSON file (save [filename])
  📌 sort            - Sort tasks by criteria (sort <priority|created|title>)
  📌 stats           - Show task statistics
  📌 status          - Update task status (status <task_id> <new_status>)
  📌 view            - View/print JSON file content (view [filename])

💡 Examples:
  add "Buy groceries" "Get milk, bread, and eggs"
  list pending
  complete 1
  priority 2 5
  category 1 Shopping
  save tasks.json
  load tasks.json

🚀 TaskTracker> add "Viết báo cáo nhóm"
✅ Task 'Viết báo cáo nhóm' added successfully with ID = 1

🚀 TaskTracker> add "Chuẩn bị bài thuyết trình" "PowerPoint cho thứ 5"
✅ Task 'Chuẩn bị bài thuyết trình' added successfully with ID = 2

🚀 TaskTracker> priority 1 7
🎯 Task 1 priority set to 7

🚀 TaskTracker> priority 2 9
🎯 Task 2 priority set to 9

🚀 TaskTracker> category 1 "School"
🏷️ Task 1 category set to 'School'

🚀 TaskTracker> 

🚀 TaskTracker> category 2 "Work"
🏷️ Task 2 category set to 'Work'

🚀 TaskTracker> complete 1
✅ Task 1 marked as completed!
🚀 TaskTracker> find "báo cáo"
🔍 Found 1 task(s) containing 'báo cáo'
  [1] Viết báo cáo nhóm - Completed

🚀 TaskTracker> find PowerPoint
🔍 Found 1 task(s) containing 'PowerPoint'
  [2] Chuẩn bị bài thuyết trình - Pending

🚀 TaskTracker> list
=== Task List (2 tasks) ===
[1] Viết báo cáo nhóm - Completed (Priority: 7, Category: School)
[2] Chuẩn bị bài thuyết trình - Pending (Priority: 9, Category: Work)
Completion Rate: 50.0%

🚀 TaskTracker> list Completed
=== Completed Tasks (1 tasks) ===
[1] Viết báo cáo nhóm (Priority: Completed, Category: 7)

🚀 TaskTracker> list Pending
=== Pending Tasks (1 tasks) ===
[2] Chuẩn bị bài thuyết trình (Priority: Pending, Category: 9)

🚀 TaskTracker> get Work 9
🎯 Tasks in category 'Work' with priority 9:
===============================================
  [2] Chuẩn bị bài thuyết trình - ⏳ Pending

📊 Found 1 task(s)

🚀 TaskTracker> get School 7
🎯 Tasks in category 'School' with priority 7:
===============================================
  [1] Viết báo cáo nhóm - ✅ Completed

📊 Found 1 task(s)

🚀 TaskTracker> matrix

📊 Task Matrix Structure:
=========================
📂 Category: School
  🎯 Priority 7: 1 task(s)
    [1] Viết báo cáo nhóm
📂 Category: Work
  🎯 Priority 9: 1 task(s)
    [2] Chuẩn bị bài thuyết trình

📈 Matrix Statistics:
  📊 Total tasks: 2
  📂 Categories: 2

🚀 TaskTracker> status 1 Cancelled
📝 Task 1 status updated to Cancelled

🚀 TaskTracker> status 2 InProgress
📝 Task 2 status updated to In Progress

🚀 TaskTracker> save demo.json
💾 Saving tasks to demo.json...
✅ Tasks saved successfully to demo.json
📊 Total tasks saved: 2

🚀 TaskTracker> view demo.json
👁️ Viewing JSON file: demo.json...

📊 File Information:
┌─────────────┬────────────────────────────┐
│ Property    │ Value                      │
├─────────────┼────────────────────────────┤
│ Version     │ 1.0                        │
│ Next ID     │ 3                          │
│ File Size   │ 667 bytes                  │
└─────────────┴────────────────────────────┘

📋 Tasks (2 total):
┌────┬─────────────────────┬─────────────┬─────────────┬──────────┬─────────────────────┐
│ ID │ Title               │ Status      │ Category    │ Priority │ Created At          │
├────┼─────────────────────┼─────────────┼─────────────┼──────────┼─────────────────────┤
│  1 │ Viết báo cáo...     │ Cancelled   │ School      │        7 │ 2025-07-02T23:12:47 │
│  2 │ Chuẩn bị bà...      │ In Progress │ Work        │        9 │ 2025-07-02T23:12:52 │
└────┴─────────────────────┴─────────────┴─────────────┴──────────┴─────────────────────┘

� Use 'stats' command to view detailed task statistics

🚀 TaskTracker> stats

📊 Task Statistics
══════════════════
📋 Total Tasks:     2
✅ Completed:       0
⏳ Pending:         0
🚧 In Progress:     1
📈 Completion Rate: 0.0%

🚀 TaskTracker> sort priority
📊 Tasks sorted by priority (highest first):
  [2] Chuẩn bị bài thuyết trình - Priority: 9
  [1] Viết báo cáo nhóm - Priority: 7

🚀 TaskTracker> sort title
📊 Tasks sorted alphabetically:
  [2] Chuẩn bị bài thuyết trình
  [1] Viết báo cáo nhóm

🚀 TaskTracker> sort created
📊 Tasks sorted by creation date (newest first):
  [2] Chuẩn bị bài thuyết trình - Age: 0.1 hours
  [1] Viết báo cáo nhóm - Age: 0.1 hours

🚀 TaskTracker> remove 1
🗑️ Task 1 removed successfully!

🚀 TaskTracker> list
=== Task List (1 tasks) ===
[2] Chuẩn bị bài thuyết trình - In Progress (Priority: 9, Category: Work)
Completion Rate: 0.0%

🚀 TaskTracker> save tasks.json
💾 Saving tasks to tasks.json...
✅ Tasks saved successfully to tasks.json
📊 Total tasks saved: 1

🚀 TaskTracker> remove 2
🗑️ Task 2 removed successfully!

🚀 TaskTracker> load tasks.json
📂 Loading tasks from tasks.json...
✅ Tasks loaded successfully from tasks.json
📊 Total tasks loaded: 1
� Use 'stats' command to view detailed task statistics

🚀 TaskTracker> recent
🕐 Recent Commands:
==================
  1. stats
  2. sort
  3. sort
  4. sort
  5. remove
  6. list
  7. save
  8. remove
  9. load
  10. recent

🚀 TaskTracker> exit

👋 Thank you for using Task Tracker! Have a productive day!
```
---
## 📚 Learning Resources

### C++23 Documentation
- [cppreference.com C++23](https://en.cppreference.com/w/cpp/23)
- [ISO C++23 Standard](https://isocpp.org/std/the-standard)
- [GCC C++23 Support](https://gcc.gnu.org/projects/cxx-status.html)
- [Clang C++23 Support](https://clang.llvm.org/cxx_status.html)
- [C++ Weekly C++23](https://www.youtube.com/playlist?list=PLs3KjaCtOwSYyEh4AoprFYz_3PpNhM2YR)

### Best Practices
- [Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Modern C++ Features](https://github.com/AnthonyCalandra/modern-cpp-features)
- [Awesome Modern C++](https://github.com/rigtorp/awesome-modern-cpp)

> 💡 **Lưu ý**: Ứng dụng này được thiết kế để minh họa **10 kỹ thuật C++23** trong môi trường thực tế. Code được tối cho Clang 17+ và GCC 14+.
