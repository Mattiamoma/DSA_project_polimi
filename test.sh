# exec name
app="test.exe"

# test folder
folder="test/"
out_folder="out/"

# test number (max is 11)
num=11

nums=$(seq 1 1 $num)
printf "__________ TESTING $num FILES __________\n\n"
printf "Test 0: "

./${app} < "${folder}/example.txt" > "${out_folder}/out.txt"
DIFF=$(diff -q --strip-trailing-cr "${out_folder}/out.txt" "${folder}/example.output.txt")

if [ "$DIFF" != "" ]; then
    printf "FAILED!\n"
else
    printf "OK\n"
fi

for num in $nums; do
    printf "Test $num: "

    ./${app} < "${folder}/open${num}.txt" > "${out_folder}/out${num}.txt"
    DIFF=$(diff -q --strip-trailing-cr "${out_folder}/out${num}.txt" "${folder}/open${num}.output.txt")

    if [ "$DIFF" != "" ]; then
        printf "FAILED!\n"
    else
        printf "OK\n"
    fi
done


printf "\n               [ DONE ]\n"
