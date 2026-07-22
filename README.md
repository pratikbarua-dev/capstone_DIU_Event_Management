<div align="center">

<img src="assets/logo.png" width="280" alt="DIU Event Management System Emblem" />

# Daffodil International University (DIU)
### Capstone Project: Event Management System (`main.c`)

![Language](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen?style=for-the-badge)
![Institution](https://img.shields.io/badge/DIU-Capstone--Project-00A859?style=for-the-badge)
![License](https://img.shields.io/badge/Academic-Evaluation-orange?style=for-the-badge)

</div>

---

## 👥 Group Project Team Members

| Member | Student ID | Contribution Area | Contribution Ratio |
|---|---|---|---|
| **Pratik Barua** | `252-35-226` | CLI System Core (`main.c`), Conflict Algorithm, Data Structures, Build Config, README | **80%** |
| **Umme Habiba Ayshi** | `252-35-298` | System Testing, Functional Validation & Documentation | **15%** |
| **Moshiur Rahman** | `252-35-184` | Backup & Recovery Module (`manageBackups`) | **5%** |

```mermaid
pie title Team Contribution Breakdown
    "Pratik Barua (252-35-226)" : 80
    "Umme Habiba Ayshi (252-35-298)" : 15
    "Moshiur Rahman (252-35-184)" : 5
```

---

## 📌 Executive Summary

The **DIU Event Management System** is a lightweight, high-performance event planning, slot management, and registration engine built natively in C. Designed specifically for university department operations, it enforces role-based security, handles venue schedule conflicts, tracks real-time capacity, records participant check-ins, and generates analytical summaries.

---

## 🏗️ System Architecture

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
| 🔐 **Authentication** | Role-based dashboard routing (Organizer, Participant, Volunteer) with password check obfuscation | FR1, FR2, NFR6 |
| 📅 **Event Creation** | Add events with title, description, date, time, venue, category, and seating capacity | FR3 |
| 🚨 **Conflict Prevention** | Automated verification preventing two events at the same date, time, and venue | FR8 |
| 🎟️ **Registration System** | Real-time seat allocation, capacity validation, and confirmation generation | FR7, FR9, FR12 |
| 🔍 **Search & Filter** | Multi-attribute search (title, date, description) and filter (category, venue) | FR10, FR14 |
| 📋 **Attendance Check-In** | Volunteer & Organizer check-in module updating real-time attendance statistics | FR11 |
| 📋 **Participant List View** | Dedicated participant lookup per event ID displaying presence status and feedback | FR19 |
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
    Org->>Core: Add New Event (Title, Description, Venue, Date, Time, Capacity)
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

    Vol->>Core: Mark Attendance / View Participant List
    Core->>DB: Update Attendance Status
    Core-->>Vol: Participant Marked Present!
```

---

## 🚀 Getting Started

### Prerequisites
- **GCC Compiler**: `sudo apt install -y gcc`
- **Linux Environment**: (Ubuntu / Zorin / Debian / Fedora)

### Compilation & Execution

To build and run the project:
```bash
# Compile with Makefile
make

# Or compile manually with GCC
gcc -Wall main.c -o main

# Execute CLI System
./main
```

---

## 📁 Repository File Structure

```
capstone_DIU_Event_Management/
├── README.md               # System Documentation & Architecture Overview
├── .gitignore              # Git Ignore Rules for Compiled Executables
├── Makefile                # Build Script for C CLI Engine
├── main.c                  # CAPSTONE CORE: C Event Management Engine
├── events.csv              # Primary Events Storage File
├── registrations.csv       # Primary Registrations Storage File
└── assets/
    └── logo.png            # Minimalist Line-Art Emblem Logo
```

---

<div align="center">
  <b>Daffodil International University (DIU)</b> • Department of Computer Science & Engineering
</div>
