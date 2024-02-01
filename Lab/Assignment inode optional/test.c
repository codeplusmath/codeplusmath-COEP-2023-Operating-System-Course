#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fs.h>
#include <ext2fs/ext2_fs.h>
//#include "ext2_fs.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

unsigned int block_size;

char * con_tm(unsigned int t){
	time_t tmp = t;
	struct tm *real_time;
	time(&tmp);
	return asctime(localtime(&tmp));		
}


void print_inode(struct ext2_inode inode){
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
        return ;
}





void print_inode_dir(int fd, struct ext2_inode inode){
	struct ext2_dir_entry_2 dirent;
	int i,j, size =0;
	char * str;
	
	lseek(fd, 0, SEEK_SET);
	for(i = 0; i< block_size; i++);
		lseek(fd, inode.i_block[0], SEEK_CUR);
	
	read(fd, &dirent, sizeof(struct ext2_dir_entry_2));
	
	while(size < inode.i_size){
		j = dirent.name_len;
		str = (char*)malloc(sizeof(char) * j);
		for(i = 0; i<j; i++){
			str[i] = dirent.name[j];
		}
		
		printf("%s\n", str);
		size = size + dirent.rec_len;
		lseek(fd, dirent.rec_len - sizeof(struct ext2_dir_entry_2), SEEK_CUR);
		read(fd, &dirent, sizeof(struct ext2_dir_entry_2));
		
		free(str);
	}
	return;
}

void print_inode_data(int fd, struct ext2_inode inode){
	int i, j, k, blocks, block_entry[128];
	int size =0, cur_size =0, count=0;
	char ch;
	
	lseek(fd, 0, SEEK_SET);
	for(i=0; i<EXT2_N_BLOCKS; i++){
		if(i<EXT2_NDIR_BLOCKS){
			if(inode.i_block[i] !=0){
				lseek(fd, 0, SEEK_SET);
				for(j=0; j<block_size;j++){
					lseek(fd, inode.i_block[i], SEEK_CUR);
				}
				
				while(size < inode.i_size && cur_size<block_size){
					count=read(fd, &ch, 1);
					printf("%c", ch);
					size += count;
					cur_size +=count;
				}
				cur_size = 0;
			}
		}
		
		else if(i == EXT2_IND_BLOCK){
			if(inode.i_block[i] != 0){
				lseek(fd, 0, SEEK_SET);
				for(j=0; j<block_size; j++){
					lseek(fd, inode.i_block[i], SEEK_CUR);
				}
				blocks = (inode.i_size - size)/block_size;
				
				if((inode.i_size - size) % block_size != 0){
					blocks = blocks +1;
				}
				
				for(j=0; j<blocks; j++){
					read(fd, &block_entry[j], 4);
				}
				
				for(j =0; j<blocks; j++){
					lseek(fd, 0, SEEK_SET);
					for(k=0; k<block_size; k++){
						lseek(fd, block_entry[j], SEEK_CUR);
					}
					
					while(size < inode.i_size && cur_size<block_size){
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

int get_inode_no(int fd, struct ext2_inode inode, char * tmp_str){
	struct ext2_dir_entry_2 dirent;
	int i, size =0;
	lseek(fd, 0, SEEK_SET);
	for(i = 0; i< block_size; i++);
		lseek(fd, inode.i_block[0], SEEK_CUR);
	
	read(fd, &dirent, sizeof(struct ext2_dir_entry_2));
	
	while(size<inode.i_size){
		if(strncmp(dirent.name, tmp_str, dirent.name_len) == 0){
			break;
		}
		else{
			size = size + dirent.rec_len;
			lseek(fd, dirent.rec_len - sizeof(struct ext2_dir_entry_2), SEEK_CUR);
			read(fd, &dirent, sizeof(struct ext2_dir_entry_2));
		}
	}
	if(size>= inode.i_size) return 0;
	return dirent.inode;
}

void info_of_path(char * device_file, char * path, char * mode){
	char * tmp_path = (char*) malloc(sizeof(char) * 1);
	char * tmp_str;
	printf("\n%s\n", path);
	strcpy(tmp_path, path);
	int len = strlen(path);
	int fd = open(path, O_RDONLY);
	struct ext2_super_block sb;
	struct ext2_group_desc bgdesc;
	struct ext2_inode inode;
	long long inoderp;
	int flag=0, dflag=0;
	long inodeno, bgno;

	if(fd == -1){
		perror("invalid device path: \n");
		exit(errno);
	}
	
	// super block
	lseek(fd, 1024, SEEK_CUR);
	read(fd, &sb, sizeof(struct ext2_super_block));
	block_size = 1024<<sb.s_log_block_size;

	// group descripter table
	lseek(fd, block_size, SEEK_SET);
	read(fd, &bgdesc, sizeof(struct ext2_group_desc));
	
	//inode of root at block group 0 & offset 2;
	inoderp = bgdesc.bg_inode_table * block_size + 2*sizeof(struct ext2_inode);
	lseek(fd, inoderp, SEEK_SET);
	read(fd, &inode, sizeof(struct ext2_inode));

	tmp_str =  strtok(path, "/");
	while(tmp_str!=NULL){
		flag = 1;
		inodeno = get_inode_no(fd, inode, tmp_str);
		tmp_str = strtok(NULL, "/");
		if(tmp_str==NULL){
			if(inodeno==0){
				printf("No file found for: %s\n", tmp_path);
				break;
			}

			else{
				if(strcmp(mode, "inode") == 0){
					printf("Inode: %ld\n", inodeno);
					break;
				}
				else{
					dflag = 1;
				}
			}
		}
		bgno = (inodeno+1)/sb.s_inodes_per_group;
		
		lseek(fd, block_size, SEEK_SET);
		lseek(fd, sizeof(struct ext2_group_desc) * bgno, SEEK_CUR);
		read(fd, &bgdesc, sizeof(struct ext2_group_desc));
		lseek(fd, 0, SEEK_SET);
		
		for(int i =0; i<block_size;i++){
			lseek(fd, bgdesc.bg_inode_table, SEEK_CUR);
		}
		
		lseek(fd, (inodeno - bgno * sb.s_inodes_per_group -1) *sb.s_inode_size, SEEK_CUR);
		read(fd, &inode, sizeof(struct ext2_inode));
		
		if(dflag ==1){
			if(inode.i_mode & S_IFMT == S_IFDIR){
				print_inode_dir(fd, inode);
			}
			else{
				print_inode_data(fd, inode);
			}
		}
	}
	
	if(flag == 0){
		if(strcmp(mode, "inode") == 0){
			printf("Inode: 2\n");
		}
		else{
			if((inode.i_mode & S_IFMT) == S_IFDIR) print_inode_dir(fd, inode);
		}
	}
	free(tmp_path);
	close(fd);
	return;
}


// ./a.out /dev/sda3 /test/t.c inode
int main(int argc, char *argv[]) {
	//int fd = open(argv[1], O_RDONLY); // argv[1] = /dev/sdb1 
	int count;
	//if (argc!=3) return 0;
	
	//if((strcmp(argv[3], "inode") != 0) && (strcmp(argv[3], "data") != 0)) return 0;
	//close(fd); 
	printf("success");
	printf("%s of %s", argv[3], argv[2]);
	info_of_path(argv[1], argv[2], argv[3]);

	return 0;
}
