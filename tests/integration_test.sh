PATH_TO_BIN="../build/bin/server"
PATH_TO_CONFIG="../src/server_config"
DELAY=1 # How long to wait for each curl request
EXPOSED_PORT=80

# Store host IP
host=$(hostname -I)

# Remove trailing spaces
host=`echo $host | sed -e 's/^[[:space:]]*//'`

exit_status=0

# Run server in background
$PATH_TO_BIN $PATH_TO_CONFIG &
server_pid=$!
echo $server_pid

################# Test 1 #################
actual_output=$(curl --header "test:test" $host:$EXPOSED_PORT --max-time $DELAY)
expected_output=$'GET / HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\ntest:test\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 1 passed"
else
    echo "Integration Test 1 failed"
    exit_status=1
fi

################# Test 2 #################
actual_output=$(curl --header "Accept: test" $host:$EXPOSED_PORT --max-time $DELAY)
expected_output=$'GET / HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: test\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 2 passed"
else
    echo "Integration Test 2 failed"
    exit_status=1
fi

################# Test 3 #################
actual_output=$(curl --header "" $host:$EXPOSED_PORT --max-time $DELAY)
expected_output=$'GET / HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 3 passed"
else
    echo "Integration Test 3 failed"
    exit_status=1
fi

################# Test 4 #################
actual_output=$(curl -I --header "" $host:$EXPOSED_PORT --max-time $DELAY)
expected_output=$'HTTP/1.1 200 0K\r\nContent-Type: text/plain\r\n\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 4 passed"
else
    echo "Integration Test 4 failed"
    exit_status=1
fi

# Kill server
kill -9 $server_pid

exit $exit_status
