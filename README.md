# Fleet Management System

![Fleet Management](https://img.icons8.com/color/96/000000/truck.png)

A C application for Machinery Management Ltd. to manage their equipment fleet with secure login and comprehensive reporting features.

## Features

- 🔒 **Password-protected access** with secure input masking
- 🗃️ **Linked list database** for efficient machine storage
- 📊 **Statistics generation** by machine type
- 📝 **Detailed reporting** with export to file
- 🔄 **CRUD operations** (Create, Read, Update, Delete) for machines
- 📂 **File persistence** - data saved between sessions
- 🏷️ **Unique chassis numbers** enforced
- 📱 **Input validation** for all fields

## Data Structure

```plaintext
Machine {
    chassisNumber (Unique ID)
    make
    model
    year
    cost
    currentValuation
    currentMileage
    nextServiceMileage
    ownerName
    ownerEmail (validated format)
    ownerPhone
    machineType (Tractor/Excavator/Roller/Crane/Mixer)
    breakdowns (Never/<3/<5/>5 times)
    *next (Pointer to next machine)
}
user name is "admin"
password is "admin1"
