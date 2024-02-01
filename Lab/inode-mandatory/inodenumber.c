#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fs.h>
#include <sys/types.h>
#include <unistd.h>
#include <ext2fs/ext2_fs.h>
#include <time.h>


char * con_tm(unsigned int t){
	time_t tmp = t;
	struct tm *real_time;
	time(&tmp);
	return asctime(localtime(&tmp));		
}

int main(int argc, char*argv[]){
	if(argc!=3) return 1;
	int fd = open(argv[1], O_RDONLY);
	int count, i;
	struct ext2_super_block sb;
	struct ext2_group_desc bgdesc[1024];
	struct ext2_inode fos;
	int ngroups, inodeno, finode, bgno, block_size;
	if(fd == -1){
		perror("readsuper:");
		exit(errno);
	}
	inodeno = atoi(argv[2]);
//	inodeno = argv[2];
	lseek(fd, 1024, SEEK_SET);
	count = read(fd, &sb, sizeof(struct ext2_super_block));
	block_size = 1024<<sb.s_log_block_size;
	ngroups = sb.s_blocks_count / sb.s_blocks_per_group + 1;
	lseek(fd, 1024<< sb.s_log_block_size, SEEK_SET);
	count = read(fd, bgdesc, sizeof(struct ext2_group_desc) * ngroups);
	
	bgno = (inodeno-1) / sb.s_inodes_per_group;
	finode = (inodeno-1) % sb.s_inodes_per_group;
	//printf("%d %d\n", bgno, finode);
	//printf("%u\n",bgdesc[bgno].bg_inode_table);
	lseek(fd,(long long) bgdesc[bgno].bg_inode_table * block_size + finode * sb.s_inode_size, SEEK_SET);
	count = read(fd, &fos, sizeof(struct ext2_inode));

	printf("Inode Number: %d\n", inodeno);
	printf("File mode: %u\n", fos.i_mode);
	printf("Owner Uid: %d\n", fos.i_uid);
	printf("Size: %d\n", fos.i_size);
	printf("Access time: %s", con_tm(fos.i_atime));
	printf("Creation time: %s", con_tm(fos.i_ctime));
	printf("Modification time: %s", con_tm(fos.i_mtime));
	printf("Deletion time:  %s", con_tm(fos.i_dtime));
        printf("Group ID: %d\n", fos.i_gid);
        printf("No of Links: %d\n", fos.i_links_count);
        printf("Block Count: %d\n", fos.i_blocks);
        printf("File Flag: %u\n", fos.i_flags);

	close(fd);
	return 0;
}

