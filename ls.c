#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

#define CTIME_MONTH_POSITION 12
#define CTIME_MINUTE_POSITION 4

int main(int argc, char const *argv[]) {
	//Open the same dir twice, once for calculate total blocks and the second to print everything else
	DIR *d;
	DIR *d2;
	struct dirent *dir;
	struct stat st;
	struct passwd *pd;
	struct group *gr;
	int status;
	int totalBlocks = 0;
	//If there is no argument, print all files in the directory
	if (argc==1){
		d = opendir(".");
		int i = 0;
		if (d) {
			while ((dir = readdir(d)) != NULL) {
				//Skip "." and ".."
				if (i<2){
					i++;
					continue;
				}
				printf("%s\n", dir->d_name);
			}
		closedir(d);
		}
	}

	//If there is argument
	if (argc>=2){

		//Check if the argument is -l, if not or more than 1 argument, return 1
		if ((strcmp(argv[1],"-l") != 0) || argc>2){
			printf("Program only accept -l as an argument right now\n");
			return 1;
		}

		//If argument = '-l'
		if (strcmp(argv[1],"-l") == 0){
		d = opendir(".");
		d2 = opendir(".");

		//Set to skip . and ..
		int i = 0, j = 0;

		//Directory can be open?
		if (d) {
			//Loop through the dir once to calculate block sizes of every thing
			while ((dir = readdir(d2)) != NULL) {
				if (j<2){
					j++;
					continue;
				}
				status = lstat(dir->d_name, &st);
				totalBlocks += st.st_blocks;
			}

			//Close after read
			closedir(d2);

			//Print totalBlocks
			printf("total %d\n",totalBlocks);

			//Loop through the dir twice to print other data
			while ((dir = readdir(d)) != NULL) {
				if (i<2){
					i++;
					continue;
				}
				//Get stat from each file
				status = lstat(dir->d_name, &st);
				//Get user and group name by getpwuid and getgrgid from stat
				if (NULL == (pd = getpwuid((int) st.st_uid)))
					perror("getpwuid() error.");
				if (NULL == (gr = getgrgid((int) st.st_gid)))
					perror("getpwuid() error.");
				//Check permission for each mode, if tree return the mode if not return "-"
				printf((S_ISDIR(st.st_mode)) ? "d" : "-");
				printf((st.st_mode & S_IRUSR) ? "r" : "-");
				printf((st.st_mode & S_IWUSR) ? "w" : "-");
				printf((st.st_mode & S_IXUSR) ? "x" : "-");
				printf((st.st_mode & S_IRGRP) ? "r" : "-");
				printf((st.st_mode & S_IWGRP) ? "w" : "-");
				printf((st.st_mode & S_IXGRP) ? "x" : "-");
				printf((st.st_mode & S_IROTH) ? "r" : "-");
				printf((st.st_mode & S_IWOTH) ? "w" : "-");
				printf((st.st_mode & S_IXOTH) ? "x" : "-");
				//Print information as requested
				printf(" %d %s %s %lld ", st.st_nlink, pd->pw_name, gr->gr_name,st.st_size);
				//Print time by indexing ctime
				printf("%.*s ", CTIME_MONTH_POSITION,ctime(&st.st_mtime)+CTIME_MINUTE_POSITION);
				printf("%s\n", dir->d_name);
			}
			//Close directory
			closedir(d);
		}
	}
	}
	return 0;

}
