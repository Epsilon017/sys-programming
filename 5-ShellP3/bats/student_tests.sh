#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh.out <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Pipes" {
  run "./dsh.out" <<'EOF'
echo "(5+5)" | bc -l
EOF

  echo "Output: $output"
  [[ "$output" == *"10"* ]]
  [ "$status" -eq 0 ]
}

@test "Simple External Command: echo" {
  run "./dsh.out" <<'EOF'
echo hello
EOF

  echo "Output: $output"
  [[ "$output" == *"hello"* ]]
  [ "$status" -eq 0 ]
}

@test "Built-In rc Command" {
  run "./dsh.out" <<'EOF'
rc
EOF

  echo "Output: $output"
  [[ "$output" == *"0"* ]]
  [ "$status" -eq 0 ]
}

@test "Built-In cd and pwd" {
  run "./dsh.out" <<'EOF'
cd /tmp
pwd
EOF

  echo "Output: $output"
  [[ "$output" == *"/tmp"* ]]
  [ "$status" -eq 0 ]
}

@test "Built-In dragon Command" {
  run "./dsh.out" <<'EOF'
dragon
EOF

  echo "Output: $output"
  [ -n "$output" ]
  [ "$status" -eq 0 ]
}

@test "Invalid Command" {
  run "./dsh.out" <<'EOF'
nonexistentcommand
EOF

  echo "Output: $output"
  [[ "$output" == *"Command not found"* ]]
}

@test "Multiple Commands" {
  run "./dsh.out" <<'EOF'
echo first
echo second
EOF

  echo "Output: $output"
  [[ "$output" == *"first"* ]]
  [[ "$output" == *"second"* ]]
  [ "$status" -eq 0 ]
}

@test "Exit Command" {
  run "./dsh.out" <<'EOF'
exit
EOF

  echo "Output: $output"
  [ "$status" -eq 0 ]
}
