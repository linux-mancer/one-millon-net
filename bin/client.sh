cd `dirname $0`

ulimit -n
ulimit -n 200000
ulimit -n

# ip address
cmd='ip=192.168.242.129'
# server port
cmd=$cmd' port=4567'

cmd=$cmd' thread_count=4'
cmd=$cmd' client_count=100000'
cmd=$cmd' msg_count=1'
cmd=$cmd' send_sleep=1000'
cmd=$cmd' work_sleep=1'
cmd=$cmd' -check_send_back=1'
cmd=$cmd' send_buffer_size=102400'
cmd=$cmd' recv_buffer_size=102400'

cmd=$cmd' -check_msg_id'

./client $cmd

read -p "press any key to exit.." var
