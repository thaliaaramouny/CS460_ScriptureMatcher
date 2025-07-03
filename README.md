# CS460_ScriptureMatcher 

## Project Overview
This project analyzes user input text for emotional content using a custom emotion graph and unique intensity scoring algorithm. It identifies top emotions expressed and recommends relevant Bible verses based on the detected emotions, word matching, and tone similarity.

--------------------------------------------------------------------------------
## Repository Contents

- EmotionGraph.cpp, EmotionGraph.h — Custom emotion graph algorithm and traversal
- InputProcessor.cpp, InputProcessor.h — Input text processing, intensity scoring, negation, and repetition handling
- VerseMapper.cpp, VerseMapper.h — Bible verse mapping and ranking based on similarity to input
- main.cpp — Entry point, ties components together and handles user input/output
- test_cases.txt — Test cases used for manual verification of the system
- README.md — This documentation

--------------------------------------------------------------------------------
## Build Instructions (Visual Studio)

1. Open Visual Studio (2019 or later recommended).
2. Create a new Console Application project.
3. Add the provided .cpp and .h files to the project.
4. Build the solution (Build > Build Solution).

--------------------------------------------------------------------------------
## Run Instructions

- After building, run the executable 
- The program will prompt:
**Enter your feelings or thoughts:**
- Type your input and press Enter.
- The program outputs the top detected emotions with corresponding Bible verses.

--------------------------------------------------------------------------------
## Running Tests

- Test cases are listed in test_cases.txt for manual verification.
- To manually test, run the program with inputs from test_cases.txt and compare outputs to expected results listed in the report.

--------------------------------------------------------------------------------
## Notes

- Final commit is tagged as v1.0-final
- Tested on Windows 10 with Visual Studio 2019.

--------------------------------------------------------------------------------
## Author

Thalia Aramouny
San Diego State University - CS460

