#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "check ls runs without errors" {
    run ./dsh.out <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Local: Shell starts and exits" {
  run "./dsh.out" <<EOF
exit
EOF
  echo "Captured stdout:"
  echo "$output"
  echo "Exit Status: $status"
  [ "$status" -eq 0 ]
}

@test "Local: rc command returns 0" {
  run "./dsh.out" <<EOF
rc
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout: $output"
  echo "Stripped: $stripped_output"
  echo "Exit Status: $status"
  [ "$status" -eq 0 ]
}

@test "Local: ls command lists files" {
  run "./dsh.out" <<EOF
ls
exit
EOF
  echo "Captured stdout: $output"
  echo "Exit Status: $status"
  [[ "$output" == *"dsh.out"* ]]
  [ "$status" -eq 0 ]
}

@test "Local: ls | grep dragon outputs dragon.c" {
  run "./dsh.out" <<EOF
ls | grep dragon
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout: $output"
  echo "Stripped: $stripped_output"
  echo "Exit Status: $status"
  [[ "$stripped_output" == *"dragon.c"* ]]
  [ "$status" -eq 0 ]
}

@test "Local: cat dragon.c | grep \"dragon()\" outputs expected line" {
  run "./dsh.out" <<EOF
cat dragon.c | grep "dragon()"
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout: $output"
  echo "Stripped: $stripped_output"
  echo "Exit Status: $status"
  expected_line="externvoidprint_dragon(){"
  [[ "$stripped_output" == *"$expected_line"* ]]
  [ "$status" -eq 0 ]
}

@test "Server: Shell runs in server mode" {
  ./dsh.out -s &
  server_pid=$!
  sleep 1

  ps -p $server_pid > /dev/null
  [ "$?" -eq 0 ]

  kill $server_pid
}


@test "Client: Shell runs in client mode and executes rc" {
  ./dsh.out -s &
  server_pid=$!
  sleep 1
  run ./dsh.out -c <<EOF
rc
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout: $output"
  echo "Stripped: $stripped_output"
  echo "Exit Status: $status"
  [ "$status" -eq 0 ]
  kill $server_pid
}

@test "Client: ls command works" {
  ./dsh.out -s &
  sleep 1
  run ./dsh.out -c <<EOF
ls
stop-server
EOF
  echo "Captured stdout: $output"
  echo "Exit Status: $status"
  [[ "$output" == *"dsh.out"* ]]
  [ "$status" -eq 0 ]
}

@test "Client: ls | grep dragon works" {
  ./dsh.out -s &
  server_pid=$!
  sleep 1
  run ./dsh.out -c <<EOF
ls | grep dragon
exit
EOF
  echo "Captured stdout: $output"
  echo "Exit Status: $status"
  [[ "$output" == *"dragon.c"* ]]
  [ "$status" -eq 0 ]
  kill $server_pid
}

@test "Client: cat dragon.c | grep \"dragon()\" works" {
  ./dsh.out -s &
  server_pid=$!
  sleep 1
  run ./dsh.out -c <<EOF
cat dragon.c | grep "dragon()"
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout: $output"
  echo "Stripped: $stripped_output"
  echo "Exit Status: $status"
  expected_line="externvoidprint_dragon(){"
  [[ "$stripped_output" == *"$expected_line"* ]]
  [ "$status" -eq 0 ]
  kill $server_pid
}
