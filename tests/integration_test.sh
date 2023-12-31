PATH_TO_BIN="../build/bin/server"
PATH_TO_CONFIG="../src/server_config"
DELAY=5 # How long to wait for each curl request
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

sleep 5

################# Test 1 #################
actual_output=$(curl --header "test:test" $host:$EXPOSED_PORT/echo --max-time $DELAY)
expected_output=$'GET echo HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\ntest: test\r\n\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 1 passed"
else
    echo "Integration Test 1 failed"
    exit_status=1
fi

################# Test 2 #################
actual_output=$(curl --header "Accept: test" $host:$EXPOSED_PORT/echo --max-time $DELAY)
expected_output=$'GET echo HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: test\r\n\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 2 passed"
else
    echo "Integration Test 2 failed"
    exit_status=1
fi

################# Test 3 #################
actual_output=$(curl --header "" $host:$EXPOSED_PORT/echo --max-time $DELAY)
expected_output=$'GET echo HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 3 passed"
else
    echo "Integration Test 3 failed"
    exit_status=1
fi

mkdir -p img
touch img/file.txt
echo "test" > img/file.txt

################# Test 4 #################
actual_output=$(curl --max-time $DELAY --header "" $host:$EXPOSED_PORT/static/all/img/file.txt --output -)
expected_output="test"
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 4 passed"
else
    echo "Integration Test 4 failed"
    exit_status=1
fi

################# Test 5 #################
actual_output=$(curl --max-time $DELAY --header "" $host:$EXPOSED_PORT/static/image/file.txt --output -)
expected_output="test"
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 5 passed"
else
    echo "Integration Test 5 failed"
    exit_status=1
fi

head -c 200000 /dev/urandom > img/200thousandbytes.txt
head -c 100 /dev/urandom > img/100bytes.txt

################# Test 6 #################
actual_output=$(curl --max-time $DELAY --header "" $host:$EXPOSED_PORT/static/image/200thousandbytes.txt --output -)
expected_output="$(cat img/200thousandbytes.txt)"
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 6 passed"
else
    echo "Integration Test 6 failed"
    exit_status=1
fi

################# Test 7 #################
actual_output=$(curl --max-time $DELAY --header "" $host:$EXPOSED_PORT/static/image/100bytes.txt --output -)
expected_output="$(cat img/100bytes.txt)"
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 7 passed"
else
    echo "Integration Test 7 failed"
    exit_status=1
fi

rm img/file.txt img/200thousandbytes.txt img/100bytes.txt

################# Test 8 #################
actual_output=$(curl --max-time $DELAY --header "" $host:$EXPOSED_PORT/does_not_exist --output -)
expected_output=$'<html><head><title>404 Not Found</title></head><body><p>404 Not Found</p></body></html>'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 8 passed"
else
    echo "Integration Test 8 failed"
    exit_status=1
fi

################# Test 9 #################
actual_output=$(curl --max-time $DELAY --header "Content-Type: application/json" \
    --request POST \
    --data "test" \
    $host:$EXPOSED_PORT/api/test --output -)
expected_output=$'{"id": 1}'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 9 passed"
else
    echo "Integration Test 9 failed"
    exit_status=1
fi

################# Test 10 #################
actual_output=$(curl --max-time $DELAY --header "Content-Type: application/json" \
    --request GET \
    $host:$EXPOSED_PORT/api/test/1 --output -)
expected_output=$'test'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 10 passed"
else
    echo "Integration Test 10 failed"
    exit_status=1
fi

################# Test 11 #################
actual_output=$(curl --header "Content-Type: application/json" \
    --request GET \
    $host:$EXPOSED_PORT/api/test --output -)
expected_output=$'[1]'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 11 passed"
else
    echo "Integration Test 11 failed"
    exit_status=1
fi

################# Test 12 #################
curl --header "Content-Type: application/json" \
    --request PUT \
    --data "update_test" \
    $host:$EXPOSED_PORT/api/test/1 --output -

actual_output=$(curl --max-time $DELAY --header "Content-Type: application/json" \
    --request GET \
    $host:$EXPOSED_PORT/api/test/1 --output -)
expected_output=$'update_test'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 12 passed"
else
    echo "Integration Test 12 failed"
    exit_status=1
fi

################# Test 13 #################
curl --header "Content-Type: application/json" \
    --request DELETE \
    $host:$EXPOSED_PORT/api/test/1 --output -

actual_output=$(curl --max-time $DELAY --header "Content-Type: application/json" \
    --request GET \
    $host:$EXPOSED_PORT/api/test --output -)
expected_output=$'[]'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 13 passed"
else
    echo "Integration Test 13 failed"
    exit_status=1
fi

################# Test 14 #################
actual_output=$(curl --header "" $host:$EXPOSED_PORT/sleep $host:$EXPOSED_PORT/echo --max-time $DELAY)
expected_output=$'GET echo HTTP/1.1\r\nHost: '"$host"$'\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 14 passed"
else
    echo "Integration Test 14 failed"
    exit_status=1
fi

################# Test 15 #################
actual_output=$(curl --header "" $host:$EXPOSED_PORT/health --output -)
expected_output=$'OK'
if [ "$actual_output" = "$expected_output" ]; then
    echo "Integration Test 15 passed"
else
    echo "Integration Test 15 failed"
    exit_status=1
fi

################# Test 16 #################
actual_output=$(curl --max-time $DELAY \
    --request POST \
    $host:$EXPOSED_PORT/chess/new --output -)

# Extract the integer value from the actual_output string
game_id=$(echo "$actual_output" | sed -n 's/.*"id": \([0-9]*\).*/\1/p')

# Access the file stored for this file
actual_output=$(curl --max-time $DELAY \
    --request GET \
    $host:$EXPOSED_PORT/chess/games/$game_id --output -)

# Ensure the role and fen fields are correct:
expected_role='"w"'
expected_fen='"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"'

if [[ "$actual_output" == *"\"role\": $expected_role"* ]] \
   && [[ "$actual_output" == *"\"fen\": $expected_fen"* ]]; then
    # Role and fen strings match the expected values.
    echo "Integration Test 16 passed"
else
    echo "Integration Test 16 failed"
    exit_status=1
fi

################# Test 17 #################
# Note: this test depends on test 16

# Update fen string:
expected_fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
curl --max-time $DELAY \
     --request PUT \
     --header "Content-Type: application/json" \
     --data $expected_fen \
     $host:$EXPOSED_PORT/chess/games/$game_id \
     --output -

actual_output=$(curl --max-time $DELAY \
                     --request GET \
                     $host:$EXPOSED_PORT/chess/games/$game_id \
                     --output -)

# Ensure the role and fen fields are correct:
if [[ "$actual_output" == *"\"fen\": \"$expected_fen\""* ]]; then
    # Role and fen strings match the expected values.
    echo "Integration Test 17 passed"
else
    echo "Integration Test 17 failed"
    exit_status=1
fi

# Kill server
kill -9 $server_pid

exit $exit_status
