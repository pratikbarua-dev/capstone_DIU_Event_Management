# capstone_DIU_Event_Management

An Event Management System developed as a Capstone Project at **Daffodil International University (DIU)**. This application features both a lightweight C Command Line Interface (CLI) and a GTK+ 3 Graphical User Interface (GUI) to manage events, registrations, venue conflict detection, attendance check-ins, and analytical summary reports.

---

## 🌟 Features

- **Role-Based Access Control (RBAC)**:
  - **Organizer**: Add, update, delete events, check venue conflicts, generate reports, and manage backup/restore.
  - **Participant**: View event schedules, search/filter events, register for events, cancel registration, and leave feedback.
  - **Volunteer**: View schedule and mark participant attendance.
- **Venue & Schedule Conflict Detection**: Prevents scheduling two events at the exact same venue, date, and time.
- **Real-Time Capacity Tracking**: Enforces slot availability and tracks attendance statistics.
- **Data Persistence**: CSV file-based storage (`events.csv`, `registrations.csv`).
- **Data Backup & Restore**: Automated backup to `events_backup.csv` and `regs_backup.csv`.
- **Dual Interface**:
  - **CLI (`main.c`)**: Terminal dashboard.
  - **GUI (`gui.c`)**: GTK+ 3 dark mode interface.

---

## 👥 Group Project Contributors

| Contributor | Contribution Area | Contribution Ratio |
|---|---|---|
| **Pratik Barua** | Core Architecture, CLI Engine, Conflict Logic, GTK Dialogs, Build Config | **80%** |
| **Ayshi** | GTK3 Desktop Layout, Custom CSS Styling, Role Stack UI | **15%** |
| **Mosihiur Rahman** | Backup & Recovery Subsystem, File I/O | **5%** |

---

## 🚀 Getting Started

### Prerequisites
- GCC Compiler (`sudo apt install gcc`)
- GTK+ 3.0 Development Libraries (Optional for GUI: `sudo apt install libgtk-3-dev`)

### Building the Application

Using `make`:
```bash
# Build CLI version only
make cli

# Build GUI version (requires libgtk-3-dev)
make gui

# Build both
make
```

Or manually with GCC:
```bash
# CLI Build
gcc -Wall main.c -o main

# GUI Build
gcc -Wall gui.c -o event_gui `pkg-config --cflags --libs gtk+-3.0`
```

---

## 💻 Running the Application

- **Run CLI System**:
  ```bash
  ./main
  ```
- **Run GUI System**:
  ```bash
  ./event_gui
  ```

---

## 📄 License
This project is for academic evaluation at Daffodil International University (DIU).
