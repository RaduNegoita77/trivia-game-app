# trivia-game-app

A robust, multi-threaded command-line application built in C that features a dynamic quiz engine, progress persistence, and a competitive leaderboard system.

Key Features
Dynamic Quiz Engine: Loads questions, multiple-choice options, and difficulty levels directly from external data files.

Progress Persistence: Automatically saves game state (current question and score) allowing users to resume sessions after an exit.

Competitive Leaderboard: Tracks and ranks top players based on total points, with automatic file-based synchronization.

Interactive UI: Utilizes specialized terminal coloring and screen management (Windows-optimized) for an enhanced user experience.

Tech Stack
Language: C (C99/C11 standards).

Platform: Windows (utilizes windows.h for console styling).

Data Management: Plain-text parsing for questions and leaderboard persistence.

System Logic
Data Parsing: The engine reads questions.txt, extracting strings for questions and options, while converting indices and difficulty ratings into actionable integers.

State Management: A dedicated Progress struct monitors the current index and score, writing to progress.txt after every interaction to prevent data loss.

Scoring Algorithm: Points are awarded based on the specific difficulty attribute of each question, rewarding players for tackling harder content.

Getting Started
Prerequisites
A C compiler (GCC, MinGW, or Clang).

Windows Environment (for terminal API support).

Execution
Ensure questions.txt is present in the root directory.

Compile the source:

gcc project-final.cpp -o trivia_game.exe
Run the application:

./trivia_game.exe
