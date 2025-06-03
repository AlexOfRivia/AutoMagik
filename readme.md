# AutoMagik - native desktop application for a car workshop

## AutoMagik is a fully functional example project written in C++ using Qt, designed to support the management of a local car repair workshop. It provides a desktop client interface for workshop employees (managers and workers) to handle service orders, internal communication, and vehicle data collection. The backend is powered by Firebase for real-time synchronization and cloud storage.

## Table of Contents

- [Overview](#overview)
- [Technologies used](#technologies-used)
- [Key Features](#key-features)  
  - [User Modes](#user-modes)  
  - [Manager Functions](#manager-functions)  
  - [Worker Functions](#worker-functions)  
- [User Interface Walkthrough](#user-interface-walkthrough)  
  1. [User Selection Screen](#1-user-selection-screen)  
  2. [Sign-In Window](#2-sign-in-window)  
  3. [Manager: Cars / Clients Panel](#3-manager-cars--clients-panel)  
  4. [Manager: Tasks / Workers Panel](#4-manager-tasks--workers-panel)  
  5. [Worker: Tasks Panel](#5-worker-tasks-panel)  
- [Installation](#installation)  
  - [Prerequisites](#prerequisites)  
  - [Clone and Configure](#clone-and-configure)  
  - [Building on Windows/macOS/Linux](#building-on-windowsmacoslinux)  
  - [Running the Application](#running-the-application)  
- [Keywords (for GitHub search)](#keywords-for-github-search)  

## Overview

The purpose of AutoMagik is to present the assumptions and specifications for version 1.0 of a desktop application that supports car repair shop management. AutoMagik allows small and medium-sized workshops to:

- Create, assign, and track repair orders (tasks)
- Manage employee accounts and user roles
- Collect and store vehicle data
- Communicate internally via task-specific comments
- Sync all information in real time through a Firebase backend

Only authorized users (workshop employees with active accounts) can access the system. The application requires an active internet connection to synchronize with Firebase.

---

## Technologies used

**Tech Stack:**
- C++ 17  
- Qt 6 (Qt Widgets)  
- Firebase Realtime Database (client-side SDK)  

## Key Features

### User Modes

Upon launching AutoMagik, users select one of two account types:

- **Manager**  
- **Worker**

Each mode unlocks a distinct set of functions, as detailed below.

### Manager Functions

1. **Task Management**  
   - Create, edit, and delete service tasks (repair orders)  
   - Assign tasks to individual workers or groups  
   - Track task status (e.g., Pending -> In Progress -> Completed)  
   - View all tasks across the workshop

2. **User & Account Management**  
   - Create, edit, deactivate, or delete employee (worker/manager) accounts  
   - Reset passwords or change email addresses for any user  
   - Grant/revoke administrative privileges  
   - Monitor login activity and account status

3. **Vehicle & Client Management**  
   - Add, edit, or remove vehicle records (each vehicle represents one client)  
   - View client history and associated tasks  
   - Attach vehicle details (make, model, license plate, VIN, etc.)  

4. **System Administration**  
   - Oversee data integrity and consistency  
   - Manage application configuration (e.g., Firebase endpoints)  
   - Access all comments and log entries in the system  

### Worker Functions

1. **Availability Status**  
   - Toggle between **Available (Clocked In)** and **Unavailable (Clocked Out)**  

2. **Authentication & Profile**  
   - Log in with company-provided email and password  
   - Change password during or after login  
   - View and edit personal profile details (name, contact info)

3. **Task Workflow**  
   - View a personalized list of assigned tasks  
   - Access basic vehicle information for each task  
   - Update task status (e.g., In Progress, On Hold, Completed)  
   - Add comments or progress updates to assigned tasks  
   - Mark tasks as completed once work is done

4. **Communication**  
   - Post and view comments within tasks to communicate with managers or other workers  

---

## User Interface Walkthrough

Below is a brief overview of each main interface screen:

### 1. User Selection Screen

- On launch, users choose between **Manager** or **Worker** mode.  
- The selection determines which features are shown after login.  

![User Selection Screen](/screenshots/automagik-user.jpg)

### 2. Sign-In Window

- Both Manager and Worker can sign in with **Email** and **Password**.  
- First-time login for Managers uses preset credentials (provided by the workshop owner).  
- User can create new Manager accounts. 
- After the Manager is authenticated:
	- Manager can create new Worker accounts.
  - Each new user receives a system-generated activation email (or temporary password).  
- Users can also change their email or password via dedicated “Change Email” and “Change Password” dialogs.

![Workers-Sign-In Window](/screenshots/worker-sing-in.jpg)

![Managers-Sign-In Window](/screenshots/manager-sing-in.jpg)

![Create-Manader Window](/screenshots/manger-create.jpg)

### 3. Manager: Cars / Clients Panel

- **Purpose:** Manage the list of vehicles (clients).  
- Each vehicle record includes:  
  - Owner Name  
  - Vehicle Make & Model  
  - License Plate / VIN  
  - Contact Information  
- Actions:  
  - **Add Vehicle (New Client)**  
  - **Edit Vehicle Details**  
  - **Delete Vehicle**  
  - **Search / Filter** by license plate, owner, or VIN  

> In AutoMagik, “Client” is synonymous with “Vehicle”—each vehicle entry represents one client.

![Cars / Clients Panel](/screenshots/manager-cars.jpg)

### 4. Manager: Tasks / Workers Panel

- **Purpose:** Create, assign, and monitor service tasks.  
- **Task Fields:**  
  - Task ID (auto-generated)  
  - Vehicle (linked to Cars/Clients)  
  - Assigned Worker(s)  
  - Task Description (e.g., “Oil change”, “Brake inspection”)  
  - Priority (Low / Medium / High)  
  - Status (Pending / In Progress / Completed)  
  - Estimated Completion Date  
- Actions:  
  - **Create New Task**  
  - **Edit Existing Task**  
  - **Assign / Reassign Workers**  
  - **Delete Task**  
  - **View Task Details & Comments**  
- **Worker Management:**  
  - View a list of all active Workers and their availability status  
  - Activate / Deactivate accounts  
  - Reset passwords or change user roles  

![Tasks / Workers Panel](/screenshots/manager-tasks.jpg)

### 5. Worker: Tasks Panel

- **Purpose:** Allow a Worker to view and update their assigned tasks.  
- **Task Details Displayed:**  
  - Vehicle Info (Make, Model, License Plate)  
  - Task Description & Priority  
  - Due Date  
  - Comments from Manager or other Workers  
- **Worker Actions:**  
  - **Update Task Status** (e.g., mark as In Progress)  
  - **Add Comment** (progress updates, questions)  
  - **Mark Task as Completed**  
  - **View Historical Comments** for context

![Worker Tasks Panel](/screenshots/worker-tasks.jpg)

---

## Installation
### Prerequisites
Before installing and building AutoMagik, ensure you have the following installed on your system:

1. **Qt 6.x (Qt Widgets / Qt Core / Qt Network / Qt SQL)**  
   - Download from [qt.io](https://www.qt.io/download).  
   - Make sure the Qt development binaries (qmake / cmake support) are in your PATH.

2. **C++17-Compatible Compiler**  
   - Windows: MSVC 2019 or newer (via Visual Studio)  
   - macOS: Xcode (12.0 or newer)  
   - Linux: GCC 9.0 or newer / Clang 10.0 or newer

3. **CMake 3.16+ (optional)**  
   - If you prefer to build via CMake rather than qmake.

4. **Git**  
   - For cloning the repository and managing version control.

5. **Firebase Account & Project**  
   - Create a Firebase project at [console.firebase.google.com](https://console.firebase.google.com).  
   - Enable **Realtime Database** (or Firestore, if you adapt the code).  
   - Generate a Web/App config snippet (Auth Domain, Database URL).  
   - Download the `google-services.json` (Android) or `GoogleService-Info.plist` (iOS) if you plan to test on those platforms—otherwise, we only need the REST credentials for the desktop client.

6. **RapidJSON (or nlohmann/json)**  
   - For JSON parsing (Firebase REST integration).  
   - The project includes a copy of RapidJSON in the `third_party/rapidjson/` folder by default.

---
### Clone and Configure
1. **Clone the Repository**  
   Open Terminal:
   ```bash
   git clone https://github.com/<your-username>/AutoMagik.git
   cd AutoMagik
   ```

2. **Configure Firebase Credentials**					
   Rename config/firebase.example.json to config/firebase.json.
   Open config/firebase.json and paste your Firebase project’s:
       authDomain
       databaseURL
       projectId
       storageBucket
       messagingSenderId
       appId

Example:
```json
{
  "authDomain": "your-project.firebaseapp.com",
  "databaseURL": "https://your-project.firebaseio.com",
  "projectId": "your-project",
  "storageBucket": "your-project.appspot.com",
  "messagingSenderId": "1234567890",
  "appId": "1:1234567890:web:abcdef123456"
}
```

### Building on Windows/macOS/Linux
Using Qt Creator:
    1. Open AutoMagik.pro in Qt Creator.
    2. Configure a build kit for your OS (e.g. MinGW, MSVC, Clang).
    3. Click Build > Run.

### Running the Application

Running the Application
After building, you can run the application directly:

- **From Qt Creator:**
  -  Press Run.
- **From terminal:**
  - On Linux/macOS:
    ```sh./AutoMagik```
  - On Windows:
   ```shAutoMagik.exe```

Make sure your Firebase credentials and configuration are correctly set up. On first launch, log in using the provided manager credentials.

---

## Keywords 
Qt C++ Car Repair Workshop Management Desktop App Firebase Qt6 Automotive Task Tracking Employee Manager
AutoMagik Mechanic Shop Repair Order System Cross-Platform GUI CRM
