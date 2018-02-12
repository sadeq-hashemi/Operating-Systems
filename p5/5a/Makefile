all: xv6_fsck
xv6_fsck: xv6_fsck.c file.c dir.c
	gcc xv6_fsck.c file.c dir.c -o xv6_fsck -Wall -Werror -g
clean: 
	rm -f xv6_fsck
