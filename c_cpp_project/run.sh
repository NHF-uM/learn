COLOR_RED="\e[31m"
COLOR_GREEN="\e[32m"
COLOR_YELLOW="\e[33m"
COLOR_BLUE="\e[34m"
COLOR_BOLD_GREEN="\e[1;32m"
COLOR_RESET="\e[0m"

export TMP=/c/Users/28445/AppData/Local/Temp
export TEMP=$TMP

rm -f main.exe
D:/path/mingw64/bin/gcc.exe main.c test.c -o main.exe

echo -e "${COLOR_BOLD_GREEN}.exe file has been builded:${COLOR_RESET}"

./main.exe