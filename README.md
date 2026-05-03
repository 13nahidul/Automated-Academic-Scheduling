# Automated Academic Scheduling

A robust desktop application for automating academic scheduling across educational institutions. This system efficiently handles course scheduling, teacher allocation, and room assignment using greedy/heuristic algorithms to generate conflict-free timetables.

## 🎯 Overview

Automated Academic Scheduling is designed to solve the complex NP-hard problem of academic timetabling. The system automates the time-consuming process of creating schedules for universities, colleges, and high schools while ensuring no conflicts in student enrollments, teacher assignments, or room allocations.

### Key Features

- **Multi-Level Scheduling Support**
  - University/college course scheduling
  - High school class scheduling
  - Teacher and room allocation
  - Multi-section course management

- **Intelligent Conflict Resolution**
  - Automatic detection of scheduling conflicts
  - Student enrollment clash prevention
  - Teacher availability management
  - Room capacity and availability tracking

- **Flexible Constraint Handling**
  - Hard constraints (must be satisfied)
  - Soft constraints (preferential optimization)
  - Customizable scheduling rules
  - Time slot preferences

- **Efficient Algorithm**
  - Greedy/heuristic approach for fast scheduling
  - Optimized for large-scale institutions
  - Handles complex constraint scenarios
  - Quick schedule generation and updates

## 🚀 Getting Started

### Prerequisites

- C/C++ compiler (GCC 7.0+ or MSVC 2017+)
- CMake 3.10 or higher (if using CMake build)
- Operating System: Windows, Linux, or macOS

### Installation

1. Clone the repository:
```bash
git clone https://github.com/13nahidul/Automated-Academic-Scheduling.git
cd Automated-Academic-Scheduling
```

2. Compile the project:

**Using Make:**
```bash
make
```

**Using CMake:**
```bash
mkdir build
cd build
cmake ..
make
```

**Using G++ directly:**
```bash
g++ -o scheduler main.cpp -std=c++11
```

3. Run the application:
```bash
./scheduler
```

## 📋 Usage

### Basic Workflow

1. **Input Data Preparation**
   - Define courses, sections, and credit hours
   - Add teacher information and availability
   - Specify room capacities and availability
   - Set time slot preferences

2. **Configure Constraints**
   - Set hard constraints (required conditions)
   - Define soft constraints (preferences)
   - Specify scheduling rules

3. **Generate Schedule**
   - Run the scheduling algorithm
   - Review generated timetable
   - Make manual adjustments if needed

4. **Export Results**
   - Generate timetables for students
   - Create teacher schedules
   - Export room allocation charts

### Example

```cpp
// Example usage (pseudocode)
Scheduler scheduler;

// Add courses
scheduler.addCourse("CS101", "Introduction to Programming", 3);
scheduler.addCourse("MATH201", "Calculus II", 4);

// Add teachers
scheduler.addTeacher("Dr. Smith", {"CS101", "CS102"});
scheduler.addTeacher("Dr. Johnson", {"MATH201"});

// Add rooms
scheduler.addRoom("Room 101", 50);
scheduler.addRoom("Lab A", 30);

// Set constraints
scheduler.setConstraint(NO_TEACHER_CONFLICT);
scheduler.setConstraint(NO_ROOM_CONFLICT);

// Generate schedule
Schedule result = scheduler.generateSchedule();
result.display();
```

## 🏗️ Architecture

### Core Components

- **Scheduler Engine**: Core algorithm implementation
- **Constraint Manager**: Handles scheduling rules and constraints
- **Data Manager**: Manages courses, teachers, rooms, and students
- **Conflict Detector**: Identifies and resolves scheduling conflicts
- **Output Generator**: Creates formatted timetables and reports

### Algorithm Details

The system uses a **greedy/heuristic approach** that:
1. Prioritizes courses based on constraints and demand
2. Assigns time slots iteratively
3. Backtracks when conflicts are detected
4. Optimizes for both hard and soft constraints
5. Provides near-optimal solutions in reasonable time

## 📊 Features in Detail

### Constraint Types

**Hard Constraints (Must be satisfied):**
- No teacher can teach two classes simultaneously
- No room can host two classes at the same time
- No student can be enrolled in overlapping courses
- Room capacity must not be exceeded
- Teachers only teach assigned courses

**Soft Constraints (Preferential):**
- Minimize gaps in student schedules
- Distribute workload evenly across days
- Respect teacher time preferences
- Optimize room utilization
- Group related courses appropriately

### Supported Scheduling Scenarios

- Fixed credit system scheduling
- Open credit system scheduling
- Multi-campus scheduling
- Lab and lecture coordination
- Theory and practical session pairing
- Elective course management

## 🛠️ Configuration

### Input File Formats

The system accepts input data in structured text or CSV formats:

**courses.txt**
```
CourseCode,CourseName,Credits,Sections
CS101,Intro to Programming,3,2
MATH201,Calculus II,4,1
```

**teachers.txt**
```
TeacherID,Name,Courses,Availability
T001,Dr. Smith,CS101;CS102,MTWThF
T002,Dr. Johnson,MATH201,MWF
```

**rooms.txt**
```
RoomID,Capacity,Type,Availability
R101,50,Classroom,All
LAB-A,30,Laboratory,All
```

## 📈 Performance

- Handles **500+ courses** efficiently
- Supports **100+ teachers** simultaneously
- Manages **1000+ students** without performance degradation
- Generates schedules in **under 1 minute** for medium-sized institutions
- Memory efficient with optimized data structures

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Development Guidelines

- Follow C++ best practices and coding standards
- Add comments for complex algorithms
- Write unit tests for new features
- Update documentation as needed

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👤 Author

**Md Nahidul Islam**
- GitHub: [@13nahidul](https://github.com/13nahidul)

## 🙏 Acknowledgments

- Inspired by classical constraint satisfaction and scheduling problems
- Thanks to the academic scheduling research community
- Built to address real-world timetabling challenges in educational institutions


## 🐛 Known Issues

- Check the [Issues](https://github.com/13nahidul/Automated-Academic-Scheduling/issues) page for known bugs
- Report new issues with detailed reproduction steps

## 🔮 Future Enhancements

- [ ] Web-based interface
- [ ] Database integration for persistent storage
- [ ] Multi-language support
- [ ] Advanced visualization of schedules
- [ ] REST API for integration with other systems
- [ ] Mobile application for schedule viewing
- [ ] Machine learning optimization
- [ ] Cloud deployment support

## 📞 Support

If you encounter any issues or have questions:
- Open an issue on GitHub
- Check existing documentation
- Review closed issues for solutions

## ⭐ Show Your Support

If this project helped you, please consider giving it a ⭐️!

---

**Note:** This is an academic project developed to demonstrate efficient scheduling algorithms and their practical applications in educational institutions.
