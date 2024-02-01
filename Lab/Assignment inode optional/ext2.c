#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fs.h>
#include <ext2fs/ext2_fs.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

unsigned int block_size;

char * con_tm(unsigned int t){
	time_t tmp = t;
	struct tm *real_time;
	time(&tmp);
	return asctime(localtime(&tmp));		
}

void show_inode(struct ext2_inode inode) {
	printf("File mode: %u\n", inode.i_mode);
	printf("Owner Uid: %d\n", inode.i_uid);
	printf("Size: %d\n", inode.i_size);
	printf("Access time: %s", con_tm(inode.i_atime));
	printf("Creation time: %s", con_tm(inode.i_ctime));
	printf("Modification time: %s", con_tm(inode.i_mtime));
	printf("Deletion time:  %s", con_tm(inode.i_dtime));
        printf("Group ID: %d\n", inode.i_gid);
        printf("No of Links: %d\n", inode.i_links_count);
        printf("Block Count: %d\n", inode.i_blocks);
        printf("File Flag: %u\n", inode.i_flags);
        return;
}


int ret_inodeno_file(int fd, struct ext2_inode inode, char *str) {
	struct ext2_dir_entry_2 dir;
	int j, size = 0;
		
	lseek(fd, 0, SEEK_SET);
	for(j = 0; j < block_size; j++) {
		lseek(fd, inode.i_block[0], SEEK_CUR);
	}
	read(fd, &dir, sizeof(struct ext2_dir_entry_2));
	while(size < inode.i_size) {
		if(strncmp(dir.name, str, dir.name_len) == 0) {
			break;
		}
		else {
			size = size + dir.rec_len;
			lseek(fd, dir.rec_len - sizeof(struct ext2_dir_entry_2), SEEK_CUR);
			read(fd, &dir, sizeof(struct ext2_dir_entry_2));
		}
			
	}
	
	if(size >= inode.i_size)
		return 0;
	return dir.inode;
}

void show_inode_directory(int fd,  struct ext2_inode inode) {
	
	struct ext2_dir_entry_2 dir;
	int j, length, size = 0;
	char *str;

	lseek(fd, 0, SEEK_SET);
	for(j = 0; j < block_size; j++) {
		lseek(fd, inode.i_block[0], SEEK_CUR);
	}

	read(fd, &dir, sizeof(struct ext2_dir_entry_2));

	while(size < inode.i_size) {
		length = dir.name_len;
		str = (char*)malloc(sizeof(char) * l);
		for(j = 0; j < length; j++) {
			str[j] = dir.name[j];
		}
		
		printf("%s\n", str);
		size = size + dir.rec_len;
		lseek(fd, dir.rec_len - sizeof(struct ext2_dir_entry_2), SEEK_CUR);
		read(fd, &dir, sizeof(struct ext2_dir_entry_2));

		free(str);
	}
	return;
		
}

void show_inode_data(int fd, struct ext2_inode inode) {

	int i, j, k, size = 0, cur_size = 0, count = 0;
	int blocks;
	int block_entry[128];
	char ch;
	
	lseek(fd, 0, SEEK_SET);

	for(i=0; i<EXT2_N_BLOCKS; i++) {
			
		if (i < EXT2_NDIR_BLOCKS) {
			if(inode.i_block[i] != 0) {
				
				lseek(fd, 0, SEEK_SET);
				for(j = 0; j < block_size; j++) {
					lseek(fd, inode.i_block[i], SEEK_CUR);
				}

				while(size < inode.i_size && cur_size < block_size) {
					count = read(fd, &ch, 1);
					printf("%c", ch);
					size += count;
					cur_size += count;
				}

				cur_size = 0;	
			}
		}
		else if (i == EXT2_IND_BLOCK) {
			if(inode.i_block[i] != 0) {
				
				lseek(fd, 0, SEEK_SET);
				for(j = 0; j < block_size; j++) {
					lseek(fd, inode.i_block[i], SEEK_CUR);
				}
		
				blocks = (inode.i_size - size) / block_size;
				if((inode.i_size - size) % block_size != 0) 
					blocks = blocks + 1;


				for(j = 0; j < blocks; j++) {
					read(fd, &block_entry[j], 4);	
				}

				for(j = 0; j < blocks; j++) {
					lseek(fd, 0, SEEK_SET);
					for(k = 0; k < block_size; k++) {
						lseek(fd, block_entry[j], SEEK_CUR);
					}

					while(size < inode.i_size && cur_size < block_size) {
						count = read(fd, &ch, 1);
						printf("%c", ch);
						size += count;
						cur_size += count;
					}

					cur_size = 0;
				}

			}
		}
					
		
			
	}
	return;

}

void show_info(char* input, char* dfile, char* path, char* type) {	
	struct ext2_super_block sb; 
	struct ext2_group_desc bgdesc;
	struct ext2_inode inode;
	long long inode_add;
	long bg_no, inode_no;
	int i, flag = 0, dataFlag = 0;
	
	int fd = open(dfile, O_RDONLY);
	if(fd == -1) {
		perror("Error:");
		exit(errno);
	}

	char *path_cpy, *str;
	int l = strlen(path);
	path_cpy = (char*)malloc(sizeof(char) * l);
	strcpy(path_cpy, path);

	lseek(fd, 1024, SEEK_CUR);
	read(fd, &sb, sizeof(struct ext2_super_block));
	block_size = 1024 << sb.s_log_block_size;
	
	lseek(fd, block_size, SEEK_SET);
	read(fd, &bgdesc, sizeof(struct ext2_group_desc));

	inode_add = bgdesc.bg_inode_table * block_size + 2*sizeof(struct ext2_inode);
	lseek(fd, inode_add, SEEK_SET);
	read(fd, &inode, sizeof(struct ext2_inode));

	str = strtok(path, "/");
	while(str != NULL) {
		flag = 1;
		inode_no = ret_inodeno_file(fd,inode,str);
		str = strtok(NULL, "/");
		if(str == NULL) {
			if(inode_no == 0) {
				printf("%s : No such file\n", path_cpy);
				break;
			}
			else {
				if(strcmp(type, "inode") == 0) {
					printf("Inode : %ld\n", inode_no);
					break;
				}
				else {
					dataFlag = 1;
				}
			}
		}
			
		bg_no = (inode_no + 1) / sb.s_inodes_per_group;
		
		lseek(fd, block_size, SEEK_SET); 
		lseek(fd, sizeof(struct ext2_group_desc) * bg_no, SEEK_CUR);
		read(fd, &bgdesc, sizeof(struct ext2_group_desc));
		lseek(fd, 0, SEEK_SET);
		
		for(i = 0; i < block_size; i++) {
			lseek(fd, bgdesc.bg_inode_table, SEEK_CUR);
		}
		lseek(fd, (inode_no - bg_no * sb.s_inodes_per_group - 1) * sb.s_inode_size, SEEK_CUR);		
		read(fd, &inode, sizeof(struct ext2_inode));
		
		if(dataFlag == 1) {
			if ((inode.i_mode & S_IFMT) == S_IFDIR) {
				show_inode_directory(fd, inode);		
			}
			else {
				show_inode_data(fd, inode);
			}
		}
	}

	// root directory
	if(flag == 0) {
		if(strcmp(type, "inode") == 0) {
			printf("Inode : 2\n");
		}
		else {
			if ((inode.i_mode & S_IFMT) == S_IFDIR) {
				show_inode_directory(fd, inode);		
			}
		}
			
	}
	
	free(path_cpy);
	close(fd);
	return;
}
	

int main(int argc, char *argv[]) {
	
	if(argc != 4) {
		printf("Format: ./a.out /dev/sda3 /test/t.c inode\n");
		return 0;
	}
	
	if((strcmp(argv[3], "inode") != 0) && (strcmp(argv[3], "data") != 0)) {
		printf("Format: ./a.out /dev/sda3 /test/t.c inode\n");
		return 0;
	}
	show_info(input, argv[1], argv[2], argv[3]);
	return 0;
}


