DELAY=1 # How long to wait for each curl request
EXPOSED_PORT=80

# Store host IP
host=34.27.90.7

exit_status=0

################# Test 1 #################
actual_output=$(curl --header "test:test" $host:$EXPOSED_PORT --max-time $DELAY)
expected_output=$'GET / HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\ntest:test\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "E2E Test 1 passed"
else
    echo "E2E Test 1 failed"
    exit_status=1
fi

################# Test 2 #################
actual_output=$(curl --header "Accept: test" $host:$EXPOSED_PORT --max-time $DELAY)
expected_output=$'GET / HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: test\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "E2E Test 2 passed"
else
    echo "E2E Test 2 failed"
    exit_status=1
fi

################# Test 3 #################
actual_output=$(curl --header "" $host:$EXPOSED_PORT --max-time $DELAY)
expected_output=$'GET / HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "E2E Test 3 passed"
else
    echo "E2E Test 3 failed"
    exit_status=1
fi

################# Test 4 #################
actual_output=$(curl -I --header "" $host:$EXPOSED_PORT --max-time $DELAY)
expected_output=$'HTTP/1.1 200 0K\r\nContent-Type: text/plain\r\n\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "E2E Test 4 passed"
else
    echo "E2E Test 4 failed"
    exit_status=1
fi

exit $exit_status
