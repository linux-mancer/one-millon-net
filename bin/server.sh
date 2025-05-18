cd `dirname $0`

ulimit -n
ulimit -n 200000
ulimit -n

# ip address
cmd='ip=any'
# server port
cmd=$cmd' port=4567'

cmd=$cmd' thread_count=1'
cmd=$cmd' send_buffer_size=102400'
cmd=$cmd' recv_buffer_size=102400'
cmd=$cmd' max_clients=102400'

cmd=$cmd' -p'
cmd=$cmd' -check_msg_id'
cmd=$cmd' -sendfull'
cmd=$cmd' -sendback'

./server $cmd

read -p "press any key to exit.." var
