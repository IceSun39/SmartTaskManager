# 📝 Smart Task Manager

A lightweight, full-stack Task Management application built with a modern **C++ REST API** and a vanilla **HTML/JS/CSS** frontend.

This project demonstrates the implementation of a robust backend architecture using the **Crow** microframework, secure user authentication with Bearer tokens, and seamless database integration using **SQLite**.

---

## ✨ Features

- **🔐 Secure User Authentication:**
  - Registration with password hashing.
  - Login/Logout functionality using custom-generated Bearer tokens.
  - Session management persisted in the database.
- **✅ Task Management (CRUD):**
  - Create new tasks.
  - Retrieve a list of active tasks for the authenticated user.
  - Update task status (Pending / Completed).
  - Delete tasks safely via unique Database IDs.
- **🎨 Dark-Themed Dashboard:** A clean, responsive, and minimalist frontend interface that communicates with the C++ API via asynchronous `fetch` requests.
- **🏗️ MVC-like Architecture:** Clean separation of concerns with dedicated Controllers (`AuthController`, `TaskController`) and Database Managers.

---

## 🛠️ Tech Stack

**Backend:**
- **[C++](https://isocpp.org/)** (Modern C++ standards)
- **[Crow](https://crowcpp.org/)** - Fast and lightweight C++ microframework for web APIs.
- **[SQLite3 & SQLiteCpp](https://srombauts.github.io/SQLiteCpp/)** - Embedded relational database and C++ wrapper.
- **[nlohmann/json](https://github.com/nlohmann/json)** - JSON for Modern C++.

**Frontend:**
- Vanilla **HTML5**, **CSS3**, and **JavaScript** (ES6+).

**Build System:**
- **[CMake](https://cmake.org/)**

---

## 🚀 Getting Started

### Prerequisites (Linux)

Ensure you have the required dependencies installed on your system. For Arch Linux, you can install them using `pacman` and `yay`:

```bash
# Install core build tools and SQLite
sudo pacman -S base-devel cmake sqlite asio libxcrypt

# Install SQLiteCpp wrapper from AUR
yay -S sqlitecpp
```

### Building the Project

1. Clone the repository:

```bash
git clone https://github.com/your-username/SmartTaskManager.git
cd SmartTaskManager
```

2. Build with CMake:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Running the Server

> **Important:** The server must be executed from the `server` directory so it can correctly locate the `templates` (HTML) and `static` (CSS) folders.

```bash
# Navigate to the server directory
cd ../server

# Run the compiled executable
../build/server/my_server
```

The application will now be running at `http://localhost:8080`.

---

## 📂 Project Structure

```
SmartTaskManager/
├── core/
│   ├── include/          # Backend logic & Database headers
│   └── src/              # Core implementation (DatabaseManager, PasswordHasher)
├── server/
│   ├── AuthController.h  # API routes for Registration, Login, Logout
│   ├── TaskController.h  # API routes for CRUD operations
│   ├── Utils.h           # Token generation and JSON helpers
│   ├── main.cpp          # Server initialization and entry point
│   ├── static/           # CSS stylesheets
│   └── templates/        # Frontend HTML views
├── CMakeLists.txt        # Root build configuration
└── json.hpp              # Third-party JSON library
```

---

## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to check the [issues page](https://github.com/your-username/SmartTaskManager/issues).

## 📜 License

This project is open-source and available under the [MIT License](LICENSE).
