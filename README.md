# Taskman

Since my last couple of programs, im trying to focus one alinging my programs more with the UNIX philosophy - doing one thing and doing it well!

Taskman is a simple task manager that allows you to add, remove, and view tasks and appointments from the command line. 
It stores the data in a simple SQlight database, making it easy to manage and query your tasks.

## Features
- Add general tasks with a description
- Dated task for specific days
- Appointments with a description, date and time
- Add reccurring tasks (daily, weekly, monthly) with simple syntax
- View and outlook tasks andWith the command '-n "task | date"' you can add a task with a description and a date.
- Postpone tasks and appointments
- Marked tasks and appointments as completed
- Command to allow a check of appiontments comming up in the next 15mins -> perfect for a Crontab job (-check)


### Task Input
Taskman -h or --help will show you the help menu with all commands and options. Here is a quick overview of the basic commands:

task -> "task/appointment | *date | *time"

recuring tasks -> "task/appointment | frequency | start_date | *end_date | *time"
 * [*] denotes optional fields and will alter the task type accordingly


### Bash Example Session

A few input examples:
![input1](images/input5.png)
![input2](images/input2.png)
![input3](images/input3.png)
![input4](images/input4.png)
![input5](images/input6.png)
![input6](images/input7.png)
![input7](images/input8.png)
![input8](images/inputting.png)

then looking at todays tasks:
![today](images/today.png)

now to complete a task:
![complete](images/completinh.png)

and now that task is gone from the list:
![aftercomplete](images/after_complete.png)

some forcase views:
![outlook](images/monthly.png)
![outlook2](images/weekly.png)
![outlook3](images/fort.png)
![outlook4](images/tomorrow.png)

Crontab reminder:
![crontab](images/crontab.png)
sooo beautiful!


## Installation
To install Taskman, simply clone the repository and compile the code using gcc with the Makefile provided.
Best practise is to change the database path from "tasks.db" in the Markfile with your desired database file path. 
Make a Crontab job with the -check command to get notified of upcoming appointments. Then you're ready to go!
