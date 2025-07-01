# 🎯 Task Tracker CLI - C++23 Modern Implementation

Một ứng dụng Task Tracker CLI được xây dựng để showcase 10 kỹ thuật mới của C++23, được tối ưu hóa để biên dịch với Clang.

## 🚀 Các Tính Năng

- ✅ Quản lý task với trạng thái (Pending, In Progress, Completed, Cancelled)
- ✅ Thiết lập priority và category cho task
- ✅ Tìm kiếm và sắp xếp task theo nhiều tiêu chí
- ✅ Thống kê chi tiết về task completion rate
- ✅ Giao diện CLI thân thiện với emoji và formatting

## 🔧 Build Instructions

```bash
# Clone và build project
mkdir build && cd build
cmake .. -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug
make

# Run ứng dụng
./NewTechniquesCpp23
```

## 📋 Các Lệnh CLI

| Lệnh | Mô tả | Ví dụ |
|------|-------|-------|
| `add` | Thêm task mới | `add "Buy groceries" "Get milk and bread"` |
| `list` | Liệt kê tất cả task hoặc theo status | `list`, `list pending` |
| `complete` | Đánh dấu task hoàn thành | `complete 1` |
| `remove` | Xóa task | `remove 2` |
| `status` | Thay đổi trạng thái task | `status 1 progress` |
| `priority` | Thiết lập priority | `priority 1 5` |
| `category` | Thiết lập category | `category 1 Shopping` |
| `find` | Tìm kiếm task theo từ khóa | `find grocery` |
| `sort` | Sắp xếp task | `sort priority`, `sort created`, `sort title` |
| `stats` | Hiển thị thống kê | `stats` |
| `save` | Lưu tasks vào file JSON | `save tasks.json` |
| `load` | Tải tasks từ file JSON | `load tasks.json` |
| `help` | Hiển thị help | `help` |
| `exit` | Thoát ứng dụng | `exit` |

## 🎯 10 Kỹ Thuật C++23 Được Sử Dụng

### 1. **Enhanced Enum Classes với Underlying Type Deduction**
```cpp
enum class TaskStatus : std::uint8_t {
    Pending,
    InProgress, 
    Completed,
    Cancelled
};
```
- Enum classes mạnh mẽ hơn với kiểu dữ liệu underlying rõ ràng
- Memory-efficient và type-safe

### 2. **Concepts for Type Constraints**
```cpp
template<typename T>
concept Stringable = requires(T t) {
    { t.to_string() } -> std::convertible_to<std::string>;
};

template<typename T>
concept TaskLike = requires(T t) {
    { t.getId() } -> std::convertible_to<int>;
    { t.getTitle() } -> std::convertible_to<std::string>;
    { t.getStatus() } -> std::convertible_to<TaskStatus>;
};
```
- Ràng buộc kiểu dữ liệu mạnh mẽ và có thể đọc hiểu
- Better template error messages

### 3. **std::expected for Error Handling**
```cpp
using TaskResult = std::expected<bool, TaskError>;
using TaskOptional = std::expected<Task, TaskError>;

TaskResult addTask(const std::string& title, const std::string& description = "");
```
- Thay thế exceptions bằng functional error handling
- Performance tốt hơn và explicit error handling
- *(Note: Sử dụng polyfill vì clang chưa hỗ trợ hoàn toàn)*

### 4. **Enhanced Structured Bindings với Pack Expansion**
```cpp
struct TaskMetadata {
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
    std::optional<std::chrono::system_clock::time_point> completed_at;
    std::string category;
    int priority = 0;
    
    auto tie() const {
        return std::tie(created_at, updated_at, completed_at, category, priority);
    }
};
```
- Structured bindings mạnh mẽ hơn cho data manipulation
- Easy serialization/deserialization

### 5. **Defaulted Comparison Operators với Auto**
```cpp
bool operator==(const Task& other) const {
    return id_ == other.id_ && title_ == other.title_ && status_ == other.status_;
}

bool operator<(const Task& other) const {
    if (id_ != other.id_) return id_ < other.id_;
    if (title_ != other.title_) return title_ < other.title_;
    return static_cast<int>(status_) < static_cast<int>(other.status_);
}
```
- Auto-generated comparison operators
- Consistent và efficient comparisons

### 6. **Modern Getter Methods (Explicit Object Parameter Simulation)**
```cpp
int getId() const { return id_; }
const std::string& getTitle() const { return title_; }
// ...with both const and non-const versions
```
- Clean interface design
- Perfect forwarding simulation for better performance

### 7. **std::format Integration**
```cpp
std::string Task::to_string() const {
    std::ostringstream oss;
    oss << std::format("Task [ID: {}]\n", id_);
    oss << std::format("  Title: {}\n", title_);
    oss << std::format("  Status: {}\n", taskStatusToString(status_));
    // ...
    return oss.str();
}
```
- Modern string formatting thay thế printf/iostreams
- Type-safe và readable
- *(Note: Sử dụng iostreams + format cho compatibility)*

### 8. **Spaceship Operator (Three-way Comparison)**
```cpp
// Simulated spaceship operator behavior
bool operator<(const Task& other) const {
    if (id_ != other.id_) return id_ < other.id_;
    if (title_ != other.title_) return title_ < other.title_;
    return static_cast<int>(status_) < static_cast<int>(other.status_);
}
```
- Một operator cho tất cả comparisons
- Consistent và efficient sorting

### 9. **Ranges và Views Integration**
```cpp
auto getTasksByStatus(TaskStatus status) const {
    return filterTasks([status](const Task& task) {
        return task.getStatus() == status;
    });
}

template<std::predicate<const Task&> Predicate>
auto filterTasks(Predicate&& pred) const {
    return tasks_ | std::views::filter(std::forward<Predicate>(pred));
}
```
- Functional programming style với ranges
- Lazy evaluation và composable operations
- Memory efficient data processing

### 10. **Enhanced Lambda Expressions với Template Parameters**
```cpp
commands_["add"] = Command{
    .name = "add",
    .description = "Add a new task (add \"title\" [description])",
    .handler = [this](const auto& args) { handleAdd(args); },
    .min_args = 1,
    .max_args = 2
};

template<typename Compare>
requires std::strict_weak_order<Compare, Task, Task>
std::vector<Task> getSortedTasks(Compare&& comp) const {
    auto sorted_tasks = tasks_;
    std::ranges::sort(sorted_tasks, std::forward<Compare>(comp));
    return sorted_tasks;
}
```
- Generic lambdas với template constraints
- Designated initializers cho clean syntax
- Perfect forwarding trong lambdas

## 🔍 Các Tính Năng Bổ Sung

### Designated Initializers
```cpp
Command{
    .name = "add",
    .description = "Add a new task",
    .handler = [this](const auto& args) { handleAdd(args); },
    .min_args = 1,
    .max_args = 2
};
```

### Advanced Template Metaprogramming
```cpp
template<typename Compare>
requires std::strict_weak_order<Compare, Task, Task>
std::vector<Task> getSortedTasks(Compare&& comp) const;
```

### Modern Error Handling Patterns
```cpp
if (result) {
    std::cout << "✅ Success!\n";
} else {
    handleError(result.error());
}
```

## 🎨 Code Style và Conventions

### Naming Conventions
- **Private member variables**: Sử dụng tiền tố `_` (underscore prefix)
  ```cpp
  class Task {
  private:
      int _id;                    // ✅ Correct
      std::string _title;         // ✅ Correct  
      TaskStatus _status;         // ✅ Correct
      TaskMetadata _metadata;     // ✅ Correct
  };
  ```

- **Public methods**: CamelCase
  ```cpp
  int getId() const;              // ✅ Correct
  void setTitle(const std::string& title);  // ✅ Correct
  ```

- **Constants và Enums**: PascalCase
  ```cpp
  enum class TaskStatus {
      Pending,                    // ✅ Correct
      InProgress,                 // ✅ Correct
      Completed                   // ✅ Correct
  };
  ```

## 🛠️ Kiến Trúc Technical

- **Separation of Concerns**: Task logic tách biệt với CLI logic
- **RAII**: Automatic resource management
- **Modern C++ Idioms**: Smart pointers, move semantics, perfect forwarding
- **Template Metaprogramming**: Concepts và constraints
- **Functional Programming**: Ranges, algorithms, lambdas

## 📊 Performance Features

- **Memory Efficient**: Enum classes với fixed underlying types
- **Cache Friendly**: Contiguous memory layout với std::vector
- **Lazy Evaluation**: Ranges views không copy data
- **Move Semantics**: Efficient object transfers
- **Template Specialization**: Zero-cost abstractions

## 🎨 User Experience

- **Rich CLI Interface**: Emoji và colors
- **Intuitive Commands**: Natural language-like syntax
- **Error Handling**: Clear error messages
- **Help System**: Comprehensive documentation
- **Flexible Input**: Quoted strings support

## 🧪 Testing

```bash
# Build và run
cd build && make && ./NewTechniquesCpp23

# Test các commands
add "Test Task" "This is a test"
list
priority 1 5
complete 1
stats
exit
```

## 📝 Notes về Compatibility

- Code được tối ưu hóa cho **Clang 14+** và **GCC 11+**
- Một số tính năng C++23 được simulate để đảm bảo compatibility
- Build system sử dụng CMake với C++23 standard
- Cross-platform support (macOS, Linux, Windows)

---

**Tác giả**: Ứng dụng showcase các kỹ thuật C++23 tiên tiến  
**Phiên bản**: 1.0.0  
**License**: MIT

### 2. **std::print** - In ấn được cải thiện
```cpp
std::print("✅ Task added successfully with ID: {}\n", task_id);
```
Cải thiện hiệu suất và ergonomics so với std::cout.

### 3. **Multidimensional subscript operator** - Toán tử chỉ mục đa chiều
```cpp
auto operator[](std::size_t id, std::string_view field) const 
    -> std::expected<std::string, TaskError>;
```
Cho phép truy cập dữ liệu theo nhiều chiều: `task_manager[1, "description"]`

### 4. **if consteval** - Kiểm tra thời gian biên dịch
```cpp
template<typename T>
auto set_priority(T priority) -> std::expected<void, TaskError> {
    if consteval {
        static_assert(std::is_same_v<T, Priority> || std::is_integral_v<T>);
    }
    // ...
}
```
Phân biệt code chạy compile-time vs runtime.

### 5. **auto(x)** - Decay copy
```cpp
auto clone() const -> Task {
    return Task{auto(id_), auto(description_), auto(priority_)};
}
```
Tạo copy rõ ràng với decay semantics.

### 6. **Explicit object parameter (deducing this)** - Tham số đối tượng tường minh
```cpp
template<typename Self>
auto get_id(this Self&& self) -> std::size_t {
    return self.id_;
}
```
Loại bỏ code duplication cho const/non-const overloads.

### 7. **std::mdspan** - Multi-dimensional array view
```cpp
std::mdspan<int, std::dextents<std::size_t, 2>> matrix_view{
    task_data.data(), tasks_.size(), 2uz
};
```
View đa chiều hiệu quả cho dữ liệu.

### 8. **Literal suffix for size_t** - Hậu tố literal cho size_t
```cpp
constexpr std::size_t DEFAULT_CAPACITY = 100uz;
for (std::size_t i = 0uz; i < matrix_view.extent(0); ++i)
```
Literal rõ ràng cho size_t để tránh warnings.

### 9. **Contains function for associative containers** - Hàm contains
```cpp
auto task_exists(std::size_t id) const -> bool {
    return id_to_index_.contains(id);
}
```
Kiểm tra sự tồn tại đơn giản hơn so với find().

### 10. **Improved constexpr support** - Hỗ trợ constexpr được cải thiện
```cpp
consteval auto get_max_priority() -> int {
    return 10;
}
```
Nhiều thuật toán và containers hỗ trợ constexpr.

## 🛠️ Build Instructions

### Prerequisites
- C++23 compatible compiler:
  - GCC 13+ 
  - Clang 16+
  - MSVC 2022 (17.4+)
- CMake 3.20+

### Build Commands
```bash
mkdir build && cd build
cmake ..
make
```

### Run
```bash
./NewTechniquesCpp23
```

## 📋 Usage Examples

```
📋 Task Manager> add "Complete C++23 demo" high
✅ Task added successfully with ID: 1

📋 Task Manager> add "Review code" medium  
✅ Task added successfully with ID: 2

📋 Task Manager> list
📋 Task List (2 tasks):
==================================================
🎯 #1: Complete C++23 demo [Pending] (High)
🎯 #2: Review code [Pending] (Medium)

📋 Task Manager> status 1 completed
✅ Task 1 status updated successfully.

📋 Task Manager> filter status completed
🔍 Filtered Tasks (status = completed):
==================================================
🎯 #1: Complete C++23 demo [Completed] (High)

📋 Task Manager> matrix
🔢 Task Matrix View:
==================================================
Task Matrix View (2 x 2):
Format: [ID, Priority]
Task 1: [ID: 1, Priority: 8]
Task 2: [ID: 2, Priority: 5]

📋 Task Manager> help
📚 Available Commands:
==================================================
  add          - Add a new task: add <description> [priority]
  list         - List all tasks: list [status|priority]
  remove       - Remove a task: remove <id>
  update       - Update task description: update <id> <new_description>
  status       - Change task status: status <id> <new_status>
  priority     - Change task priority: priority <id> <new_priority>
  filter       - Filter tasks: filter <status|priority> <value>
  matrix       - Show task matrix view: matrix
  help         - Show help: help
  exit         - Exit the application: exit
```

## 🎯 Features

- ✅ Add/Remove/Update tasks
- ✅ Set task priorities (Low, Medium, High, Critical)
- ✅ Change task status (Pending, InProgress, Completed, Cancelled)
- ✅ Filter tasks by status or priority
- ✅ Matrix view of task data using std::mdspan
- ✅ Modern error handling with std::expected
- ✅ Rich CLI interface with emoji and colors
- ✅ All 10 C++23 techniques integrated
- ✅ JSON serialization/deserialization with std::expected
- ✅ Modern error handling for file I/O operations

## 🎯 JSON Implementation C++23 Features

### Enhanced Error Handling với std::expected
```cpp
enum class JsonError {
    FileNotFound,
    InvalidFormat, 
    WriteError,
    ParseError
};

using JsonResult = std::expected<bool, JsonError>;

JsonResult saveToJson(const std::string& filename) const;
JsonResult loadFromJson(const std::string& filename);
```

### Modern String Formatting với std::format
```cpp
json << std::format("  \"id\": {},\n", _id);
json << std::format("  \"title\": \"{}\",\n", escapeJsonString(_title));
json << std::format("  \"priority\": {},\n", _metadata.priority);
```

### Concepts-Based Validation
```cpp
template<typename T>
concept JsonSerializable = requires(T t) {
    { t.toJson() } -> std::convertible_to<std::string>;
};
```

## 🏗️ Architecture

```
App (CLI Interface)
├── TaskManager (Business Logic)
    ├── Task (Data Model)
    └── Error Handling (std::expected)
```

## 📝 Notes

- Một số features của C++23 có thể chưa được hỗ trợ đầy đủ trên tất cả compilers
- std::print và std::expected có thể cần implementations thay thế cho các compiler cũ
- std::mdspan có thể cần include riêng hoặc sử dụng experimental namespace

## 🔮 Future Enhancements

- ✅ Lưu tasks vào file JSON (IMPLEMENTED)
- Task scheduling với std::chrono
- Multi-threading với std::jthread
- Network sync capabilities
- GUI interface

## 💾 JSON Serialization Features

Ứng dụng hiện hỗ trợ lưu và tải tasks từ file JSON với các tính năng:

### Save Tasks
```bash
save [filename]        # Lưu tất cả tasks, mặc định: tasks.json
save my_tasks.json     # Lưu với tên file tùy chỉnh
```

### Load Tasks  
```bash
load [filename]        # Tải tasks, mặc định: tasks.json
load my_tasks.json     # Tải từ file tùy chỉnh
```

### JSON Format
File JSON được structured như sau:
```json
{
  "version": "1.0",
  "next_id": 4,
  "tasks": [
    {
      "id": 1,
      "title": "Task title", 
      "description": "Task description",
      "status": "Pending",
      "category": "General",
      "priority": 0,
      "created_at": "2025-07-01T09:47:10.015Z",
      "updated_at": "2025-07-01T09:47:10.015Z",
      "completed_at": "2025-07-01T09:47:10.015Z"  // optional
    }
  ]
}
```

### Features
- ✅ **Auto-save ID tracking**: Preserves next available ID
- ✅ **ISO 8601 timestamps**: Standard datetime format
- ✅ **Complete task metadata**: All task properties preserved  
- ✅ **Error handling**: Robust file I/O with meaningful error messages
- ✅ **UTF-8 support**: Proper string escaping for international characters
- ✅ **Backup-friendly**: Human-readable JSON format