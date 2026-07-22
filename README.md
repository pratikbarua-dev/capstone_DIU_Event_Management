<div align="center">

![DIU Event Management System Banner](assets/banner.png)

# 🎓 Daffodil International University (DIU)
### Capstone Project: Event Management System (`main.c`)

![Language](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen?style=for-the-badge)
![Institution](https://img.shields.io/badge/DIU-Capstone--Project-00A859?style=for-the-badge)
![License](https://img.shields.io/badge/Academic-Evaluation-orange?style=for-the-badge)

</div>

---

> [!IMPORTANT]
> **OFFICIAL CAPSTONE SCOPE NOTICE**:
> - **Core Capstone Deliverable**: The Command Line Interface (CLI) system implemented in **[`main.c`](file:///home/pratiik/Documents/capstone/main.c)** is the **official capstone project** for academic submission and evaluation.
> - **Experimental GTK GUI (`gui.c`)**: The GTK+ 3 desktop application (`gui.c`) was built as an **experimental test module** to explore GUI prototyping. It is **NOT part of the official Capstone project evaluation**.

---

## 📌 Executive Summary

The **DIU Event Management System** is a lightweight, high-performance event planning, slot management, and registration engine built natively in C. Designed specifically for university department operations, it enforces role-based security, handles venue schedule conflicts, tracks real-time capacity, records participant check-ins, and generates analytical summaries.

---

## 🏗️ System Architecture

![System Architecture Diagram](assets/architecture.png)

```mermaid
graph TD
    A[User Roles] -->|Organizer| B(CLI Core Engine - main.c)
    A -->|Participant| B
    A -->|Volunteer| B

    subgraph Core Engine Modules
        B --> C[Authentication & Access Control]
        B --> D[Venue & Time Conflict Detector]
        B --> E[Capacity & Slot Tracking Engine]
        B --> F[Report Generator & Analytics]
        B --> G[Backup & Recovery Subsystem]
    end

    subgraph Data Layer
        C <--> H[(events.csv)]
        D <--> H
        E <--> H
        F <--> I[(registrations.csv)]
        G <--> J[(events_backup.csv / regs_backup.csv)]
    end
```

---

## 🌟 Key Features

| Module | Features & Capabilities | Functional Requirements |
|---|---|---|
| 🔐 **Authentication** | Role-based dashboard routing (Organizer, Participant, Volunteer) with pass-check obfuscation | FR1, FR2, NFR6 |
| 📅 **Event Creation** | Add events with title, date, time, venue, category, and total seating capacity | FR3 |
| 🚨 **Conflict Prevention** | Automated verification preventing two events at the same date, time, and venue | FR8 |
| 🎟️ **Registration System** | Real-time seat allocation, capacity validation, and confirmation generation | FR7, FR9, FR12 |
| 🔍 **Search & Filter** | Multi-attribute search (title, date) and categorization filter (category, venue) | FR10, FR14 |
| 📋 **Attendance Check-In** | Volunteer & Organizer check-in module updating real-time attendance statistics | FR11 |
| 💬 **Participant Feedback** | Collect and tie participant review comments to specific event records | FR15 |
| 💾 **Backup & Restore** | 1-click database duplication (`events_backup.csv`) and fail-safe recovery | FR16, FR17 |

---

## 🔄 User Workflow Diagram

```mermaid
sequenceDiagram
    autonumber
    actor Org as Organizer
    actor Part as Participant
    actor Vol as Volunteer
    participant Core as System Core (main.c)
    participant DB as CSV Database

    Org->>Core: Login (Organizer Auth)
    Org->>Core: Add New Event (Title, Venue, Date, Time, Capacity)
    Core->>Core: Run checkConflict()
    alt Conflict Found
        Core-->>Org: ERROR: Venue Conflict Detected!
    else No Conflict
        Core->>DB: Write to events.csv
        Core-->>Org: Event Created Successfully!
    end

    Part->>Core: Register for Event (Event ID, Name)
    Core->>Core: Check Available Slots
    Core->>DB: Write to registrations.csv
    Core-->>Part: Registration Confirmed!

    Vol->>Core: Mark Attendance (Event ID, Name)
    Core->>DB: Update Attendance Status
    Core-->>Vol: Participant Marked Present!
```

---

## 👥 Group Project Contribution Ratio

The development responsibility for this Capstone Project was split among the team members as follows:

```mermaid
pie title Team Contribution Breakdown
    "Pratik Barua (Core Engine & Lead Architect)" : 80
    "Ayshi (GTK GUI Test Prototype)" : 15
    "Mosihiur Rahman (Backup & Recovery Subsystem)" : 5
```

| Member | Contribution Area | Contribution Ratio |
|---|---|---|
| **Pratik Barua** | CLI System Core (`main.c`), Conflict Algorithm, Data Structures, Build Config, README | **80%** |
| **Ayshi** | Experimental GTK GUI Layout (`gui.c`) *(Test Module)* | **15%** |
| **Mosihiur Rahman** | Backup & Recovery Module (`manageBackups`) | **5%** |

---

## 🚀 Getting Started

### Prerequisites
- **GCC Compiler**: `sudo apt install -y gcc`
- **Linux Environment**: (Ubuntu / Zorin / Debian / Fedora)

### Compilation & Execution

#### 1. Official Capstone CLI (`main.c`)
To build and run the official Capstone project:
```bash
# Compile CLI Engine
gcc -Wall main.c -o main

# Execute CLI System
./main
```

#### 2. Experimental GTK GUI Test (`gui.c`) *(Optional / Test Only)*
If GTK3 libraries are installed on your system:
```bash
# Compile GTK GUI Prototype (Test Only)
gcc -Wall gui.c -o event_gui `pkg-config --cflags --libs gtk+-3.0`

# Run GTK GUI Prototype
./event_gui
```

---

## 📁 Repository File Structure

```
capstone_DIU_Event_Management/
├── README.md               # System Documentation & Architecture Overview
├── .gitignore              # Git Ignore Rules for Compiled Executables
├── Makefile                # Build Script for CLI & Experimental GUI
├── main.c                  # OFFICIAL CAPSTONE CORE: C CLI Engine
├── gui.c                   # EXPERIMENTAL / TEST ONLY: GTK+ 3 GUI Prototype
├── events.csv              # Primary Events Storage File
├── registrations.csv       # Primary Registrations Storage File
└── assets/
    ├── banner.png          # System Banner Header Image
    └── architecture.png    # System Architecture Block Diagram
```

---

<div align="center">
  <b>Daffodil International University (DIU)</b> • Department of Computer Science & Engineering
</div>
