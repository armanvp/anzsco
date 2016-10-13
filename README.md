# ANZSCO Status Checker
A simple utility written in C++ and cURL lib to get the latest ANZSCO status for applying state sponsorship visa in Australia. You can schedule this in a cron job and automatically email it to you for convience

# Compile
```
./build.sh
```

# Dependencies
cURL library

# Usage
Run ANZSCO with default code to 261312 (Developer Programmer)
```
./anzsco
```
or with explicit ANZSCO code
```
./anzsco 261313
```


