# Flight Management System

The Flight Management System is a robust software solution designed to streamline the management of flight schedules for airlines or aviation companies. This system provides a comprehensive set of features to efficiently handle flight operations, including adding new flights, canceling existing flights, and dynamically rearranging schedules based on specified times.

## Overview

Managing flight schedules is a critical aspect of airline operations, requiring precise coordination and optimization of resources. The Flight Management System offers a sophisticated approach to organize and manage flight schedules effectively. By employing a bucket-based architecture, the system optimizes resource utilization and ensures timely execution of flights.

## Key Features

### 1. Adding New Flights
- **Function:** `addNewFP(Bucket **dashboard, Flight_Plan *newFP)`
  - **Description:** This function adds a new flight plan to the flight management system. It dynamically updates the flight schedule by inserting the new flight plan in the appropriate position based on its arrival time. If necessary, it creates new buckets to accommodate the new flight.

### 2. Canceling Flights
- **Function:** `cancelFP(Bucket **dashboard, int FPid)`
  - **Description:** This function cancels a flight with the specified flight ID from the flight management system. It traverses through the flight schedule to find the flight with the given ID and removes it from the system. If the cancellation results in an empty bucket, it cleans up the bucket accordingly.

### 3. Dynamic Schedule Rearrangement
- **Function:** `BucketRearrange(Bucket **dashboard, Time *newT)`
  - **Description:** This function dynamically rearranges the flight schedule based on a specified time. It adjusts the bucket boundaries and redistributes flights to ensure efficient resource utilization. Flights with arrival times earlier than the specified time are moved to preceding buckets if necessary.

### 4. Detailed Dashboard
- **Function:** `displayDashboard(Bucket *dashboard, char *fileName)`
  - **Description:** This function generates a detailed dashboard of the flight schedule and saves it to a text file. It traverses through the flight schedule, including bucket information and associated flight details, and writes them to the output file for easy monitoring and management.

### 5. Input File Support
- **Function:** `input(Bucket *dashboard, char *fileName)`
  - **Description:** This function reads flight details from a CSV file and populates the flight management system. It parses the input file containing flight IDs, departure times, and arrival times and adds the flights to the system using the `addNewFP` function.

## Usage

1. **Input File Format**: Ensure that the flight details are provided in the correct CSV format (`FlightID,DepartureHour,DepartureMinute,ArrivalHour,ArrivalMinute`).
2. **Compilation**: Compile the program using a compatible C compiler (e.g., GCC).
3. **Execution**: Run the compiled executable to initiate the Flight Management System.
4. **Interact**: Use the provided functionalities to add, cancel, or rearrange flights as needed.
5. **Output**: The flight schedule information will be displayed on the console and saved to an output text file (`flights.txt`).

## Example

Suppose we have a CSV file `flights.csv` containing the following flight details:

FlightID,DepartureHour,DepartureMinute,ArrivalHour,ArrivalMinute
100,8,30,10,0
101,9,0,11,0
102,10,0,12,0


After executing the program, the flight schedule will be displayed on the console and saved to `flights.txt`.

## Dependencies

- This project requires a compatible C compiler to build and execute.

## Contributions

Contributions to the Flight Management System project are welcome! Whether it's bug fixes, feature enhancements, or documentation improvements, feel free to contribute to make this project even better.
