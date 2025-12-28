# Fréchet Distance Visualizer

This project is an interactive C++ / Qt application for visualizing the **continuous Fréchet distance** between two polygonal curves.  
It demonstrates the construction of the **free space diagram**, reachability propagation, extraction of the **critical path**, and an animated visualization of the optimal matching between the curves.


## Features

- Visualization of two polygonal curves P and Q
- Construction and display of the **free space diagram**
- Interactive ε-slider for testing reachability
- Automatic detection of the **critical ε value**
- Extraction and visualization of a **monotone critical path**
- Smooth animation of a man and a dog walking along the curves
- Dynamic leash visualization with real-time length display
- Loading polylines from a text file


## Usage

1. Launch the application
2. Click **Load polylines** and select an input file
3. Use the ε-slider to explore reachability
4. When the critical ε is reached:
   - the critical path is computed
   - animation starts automatically
5. Use **Restart animation** to replay the motion

## Input Format

Polylines are loaded from a `.txt` file with the specific format. Reference files from `examples` directory.


## Demo

Demo video of the usage of this software:

https://github.com/user-attachments/assets/02f4b0b8-b151-4be9-8f75-a684fe19693b

