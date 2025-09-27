# Taskman

Since my last couple of programs, im trying to focus one alinging my programs more with the UNIX philosophy - doing one thing and doing it well!

Taskman is a simple task manager that allows you to add, remove, and view tasks and appointments from the command line. 
It stores the data in a simple SQlight database, making it easy to manage and query your tasks.

## Features
- Add tasks with a description and date
- Appiontments are added with time and date for easy searching
- Postpone tasks and appointments
- Marked tasks and appointments as completed
- Command to allow a check of appiontments comming up in the next 15mins -> perfect for a Crontab job (-check)


### Task Input

With the command '-n "task | date"' you can add a task with a description and a date.
Appointments can be added with the command '-n "appointment | date | time"' time must be formatted HH:MM.
The date Syntax is very flexible, their is only one requriement that is putting either just the date of the day, and then it will defualt to current month and year, or you can put the full date in the format DD.MM.YYYY.
You can also use daynames and today/tomorrow as keywords for the date, for example: "Meeting | thursday | 14:00" or "Task due | 12.5" 
Using dayname will set the date to the next occurance of that day, so if today is wednesday, and you put "Dentist | Monday | 14:00" it will set the date to the following week, Monday.


#### Basic Commands
- -n "task/appointment | date | time" -> Add a new task / appointment
- -s range -> Show tasks / appointments wit the range of days. range can also be keywords "today", "tomorrow", "week"
- -a date -> Showing appointments only on a specific date
- -s! date -> Show tasks / appointments on a specific date 
- -sa -> Show all tasks / appointments
- -check -> Check for appointments in the next 15mins (perfect for a Crontab job)
- -c id -> Mark task / appointment as completed
- -p id date -> Postpone task / appointment by date
- -rm id -> Remove task / appointment

- -'today/tomorrow/tom/week' can be used in place of a date in the -s command to get the tasks accordingly.
- -sql "sql query" -> Execute a custom SQL query on the database (advanced users only)




## Installation
Replace "tasks.db" with your desired database file path.

