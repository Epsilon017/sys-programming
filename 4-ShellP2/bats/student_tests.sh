#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Execute ls command" {
    run "./dsh.out" <<EOF                
ls
EOF
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    
    [ "$status" -eq 0 ]
}

@test "Execute uname command" {
    run "./dsh.out" <<EOF
uname
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Linuxdsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Execute uname command with arg" {
    run "./dsh.out" <<EOF
uname -p
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="x86_64dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Change directory and list contents" {
    current=$(pwd)
    cd .
    mkdir -p dsh-test && cd dsh-test
    touch testfile
    
    run "${current}/dsh.out" <<EOF                
cd /tmp/dsh-test
ls
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="testfiledsh2>dsh2>dsh2>cmdloopreturned0"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Random command handling" {
    run "./dsh.out" <<EOF                
randomcmd
EOF
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    
    [ "$status" -eq 0 ]
}

@test "Execute echo command" {
    run "./dsh.out" <<EOF                
echo Hello, world!
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Hello,world!dsh2>dsh2>cmdloopreturned0"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Dragon command execution" {
    run "./dsh.out" <<EOF                
dragon
EOF
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    
    [ "$status" -eq 0 ]
}

@test "Command not found (ENOENT)" {
    run ./dsh.out <<EOF
nonexistentcommand
EOF

    echo "Captured stdout:"
    echo "$output"

    [[ "$output" == *"Command not found in PATH"* ]]
}

@test "Permission denied (EACCES)" {

    run ./dsh.out <<EOF
./dsh_cli.c
EOF

    echo "Captured stdout:"
    echo "$output"

    [[ "$output" == *"Permission denied"* ]]
}