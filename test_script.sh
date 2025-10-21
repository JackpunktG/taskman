#!/bin/bash

# Colors for better output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counter
TESTS_PASSED=0
TESTS_FAILED=0

# Function to run a test
run_test() {
    local test_name="$1"
    local command="$2"
    local expected_exit_code="${3:-0}"
    
    echo -e "${YELLOW}Running test: ${test_name}${NC}"
    echo "Command: $command"
    
    # Run the command and capture output
    output=$(eval "$command" 2>&1)
    exit_code=$?
    
    echo "Output: $output"
    
    if [ $exit_code -eq $expected_exit_code ]; then
        echo -e "${GREEN}✓ Test passed${NC}"
        ((TESTS_PASSED++))
    else
        echo -e "${RED}✗ Test failed (Exit code: $exit_code, Expected: $expected_exit_code)${NC}"
        ((TESTS_FAILED++))
    fi
    
    echo "--------------------------------------"
}

# Setup - create fresh test database
echo "Setting up test environment..."
make clean
rm -f test_DB.db
make OUT=test_taskman DB_PATH=test_DB.db
echo "y" | ./test_taskman -n "Initial task for setup"

echo "Testing Task Inputs"
# Test basic task operations
run_test "Add general task" "./test_taskman -n 'Test general task'"
run_test "Add dated task" "./test_taskman -n 'Test dated task | tomorrow'"
run_test "Add appointment" "./test_taskman -n 'Test appointment | tomorrow | 14:30'"

# Test recurring tasks
run_test "Add recurring task" "./test_taskman -nr 'Recurring meeting | weekly | tomorrow'"
run_test "Weekly meeting with time" "./test_taskman -nr 'Weekly sync | weekly | tomorrow | 10:00'"
run_test "Daily recurring task" "./test_taskman -nr 'Daily standup | daily | - | 09:00'"
run_test "Monthly recurring task" "./test_taskman -nr 'Monthly report | monthly | 1'"
run_test "Beer walk ever thursday" "./test_taskman -nr 'Beer walk | weekly | thu | 18:00'"

# Get the ID of the first task for further testing
TASK_ID=$(./test_taskman -id 1 | grep -o "id:[0-9]*" | cut -d':' -f2)
echo "Task ID for testing: $TASK_ID"

# Test task manipulation
run_test "View task details" "./test_taskman -id $TASK_ID"
run_test "Complete task" "./test_taskman -c $TASK_ID"
run_test "Postpone task" "./test_taskman -p $TASK_ID 'friday | 16:00'"
echo "y" | run_test "Delete task" "./test_taskman -d $TASK_ID"


# Test views
run_test "Show tomorrow tasks" "./test_taskman tomorrow"
run_test "Show today's tasks" "./test_taskman today"
run_test "Show weekly outlook" "./test_taskman week"

# Summary
echo "========================================"
echo -e "${GREEN}Tests passed: $TESTS_PASSED${NC}"
echo -e "${RED}Tests failed: $TESTS_FAILED${NC}"
echo "========================================"

make clean
# rm -f test_taskman.db
