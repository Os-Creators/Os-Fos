# 🖥️ Os-Fos  

A lightweight educational operating system built for learning systems programming concepts from the ground up.  
We started with a template OS and gradually added advanced features such as semaphores, nth chance clock page replacement, dynamic memory allocation, user/kernel heap, system calls, fault handling, and more.  

---

## 📖 About  

**Os-Fos** is designed as a hands-on kernel project where new OS features are implemented iteratively.  
It emphasizes **memory management, scheduling, concurrency primitives, and system calls**, offering a practical way to understand how modern operating systems function under the hood.  

This project was developed as part of coursework and self-learning, where each milestone introduced a new concept (allocators, schedulers, synchronization tools, etc.) and was tested through structured test cases.  

---

## ✨ Features  

### 🔹 Memory Management  
- **Kernel Heap**:  
  - `kmalloc`, `kfree`, `krealloc` (First-Fit strategy)  
  - `sbrk` for dynamic heap expansion  
  - Virtual ↔ Physical address mapping  

- **User Heap**:  
  - `malloc`, `free`, `realloc` (First-Fit)  
  - `sys_sbrk` for user space heap management  

- **Dynamic Allocator**:  
  - Block allocation with First-Fit (FF) and Best-Fit (BF)  
  - Block freeing and reallocation support  

- **Shared Memory**:  
  - `smalloc`, `sget`, `sfree` for inter-process shared objects  
  - Kernel-side shared memory manager for tracking allocations  

---

### 🔹 Process Management & Scheduling  
- **Priority Round-Robin Scheduler**:  
  - Set process priority & thresholds  
  - Timer tick–based scheduling decisions  
- **System Calls**:  
  - User–Kernel communication layer  
  - Parameter validation and trap handling  

---

### 🔹 Synchronization  
- **Semaphores** (user-level):  
  - Create, Get, Wait, Signal  
  - Tested with producer-consumer & barber shop scenarios  
- **Sleep Locks**:  
  - Sleep, Wakeup-One, Wakeup-All mechanisms  

---

### 🔹 Page Replacement & Fault Handling  
- **Nth-Chance Clock Algorithm**:  
  - Normal and Modified versions implemented  
  - Page replacement testing with custom workloads  
- **Fault Handler**:  
  - Page fault detection and recovery  
  - Invalid pointer checks  

---

📂 Project Structure
```bash
Copy code
kern/
 ├── cmd/                # Command prompt & kernel commands
 ├── conc/               # Concurrency: semaphores, locks, channels
 ├── cpu/                # Scheduling helpers
 ├── mem/                # Kernel/User heap, allocators, shared memory
 ├── proc/               # User environment mgmt
 ├── sched/              # Schedulers
 ├── tests/              # Module-specific tests
 └── trap/               # Syscalls, trap handling, fault handler

lib/
 ├── syscall.c           # Syscall wrappers
 ├── dynamic_allocator.c # Block allocators (FF, BF)
 ├── uheap.c             # User heap
 └── semaphore.c         # User-level semaphores

inc/
 ├── syscall.h           # Syscall definitions
 ├── environment_definitions.h
 └── ...
```
---
    
🚀 Getting Started
Clone the repository:

```bash
git clone https://github.com/Os-Creators/Os-Fos.git
cd Os-Fos
```

Build and run inside your environment.

Use `FOS> prompt to run tests and commands`.

---

## 📌 Topics
- Operating Systems
- Kernel Development
- Memory Management (Heap, Allocators, Shared Memory)
- Concurrency & Synchronization (Semaphores, Locks)
- Scheduling (Priority Round Robin)
- Page Replacement Algorithms (Nth Chance Clock)
- System Calls & Traps
