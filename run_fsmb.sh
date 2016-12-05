#/bin/bash

large(){
# 1KB, 2KB, 4KB, 8KB, 16KB, 32KB, 64KB, 128KB, 256KB, 512KB, 1MB
	for size in 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576
	do
		echo "$size byte blocksize largefile benchmark"
		./fsmb large $size large_file$size_ 3
	done
}

small(){
	for size in 1024 2048 4096 8192 16384 32768 65536 131072
	do
		echo "$size byte filesize smallfile benchmark"
		./fsmb small $size small_file$size_ 3
	done
}

large
small
