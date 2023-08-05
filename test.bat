@echo off
echo Running test...
y.exe test.swift > output.txt

echo Comparing outputs...
fc /b output.txt expected_output.txt > nul

if not errorlevel 1 (
    echo All tests passed. OK.
) else (
    echo "Test(s) failed:"
    fc output.txt expected_output.txt
)

pause