#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

/* selective options about the myrm */
int	iflag, rflag, vflag;

int	myrm1(char *);
int	myrm_file(char *);
int	myrm_dir(char *);
int	usage(void);
char 	getyn(char *);

int main(int argc, char **argv)
{
	char *path, *opt;	

	/* if input only contains program name, then give some informations */
	argv++;			
	argc--;
	if (!argc) 
		usage();

	/* read options */
	while (argc != 0 && **argv == '-') {
		opt = ++*argv;
		argc--;
		argv++;	

		/* if the option is "--" then end reading options and goto next argument */
		if (opt[0] == '-' && opt[1] == '\0')  
			break;
		
		/* obtain the options */
		for (; *opt != '\0'; opt++) 
			switch (*opt) {
			case 'i':
                		iflag = 1;
				break;
			case 'v':
				vflag = 1;
				break;
			case 'r':
				rflag = 1;
				break;
			default:
				usage();
			}
	} 

	if (!argc)
		usage();

	/* remove files */
	while ((path = *argv++) != NULL) 
		myrm1(path);

	return(0);
}

int myrm1(char *path) {
	struct stat st;

	/* open the inode of file */
	if (lstat(path, &st)) {
		fprintf(stderr, "Error: File %s not exist!\n", path);
		return(-1);
	}
	
	/* judge whether the file is a file or a directory */
	if (S_ISDIR(st.st_mode)) 
		if (rflag)
			myrm_dir(path);
                                                                                                     
		else		
			fprintf(stderr, "Error: %s is a directory, and can't be removed.\n", path);
	else 			/* not a directory, remove this file */
		myrm_file(path);

	return(0);
}

int myrm_dir(char *path)
{
	DIR *dp;
	struct dirent *dirp;
	char *dirname, ch, *msg;

	/* if iflag is 1, ask whether go into this directory */
	msg = (char *)malloc(strlen(path) + 30);
	if (iflag) {			
		msg = strcat(strcpy(msg, path), ": chdir this directory?(Y/N)");
		if (ch = getyn(msg), ch == 'n' || ch == 'N') 
			return(0);
	}

	/* open the directory */
	if ((dp = opendir(path)) == NULL) {
		fprintf(stderr, "Error: can't open directory %s!.\n", path);
		return(-1);
	}
	
	chdir(path);

	/* read each entry in the directory and remove it*/
	dirp = (struct dirent *)malloc(sizeof(struct dirent));
	while ((dirp = readdir(dp)) != NULL) {		
		if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
			continue;
		
		myrm1(dirp->d_name);
	}

	/* close the directory */
	if (closedir(dp)) {
		fprintf(stderr, "Error: can't close the directory %s!\n", path);
		return(-1);
	}
	
	chdir("..");
	if ((dirname = strrchr(path, '/')) != NULL) 
		dirname++;
	else dirname = path;

	/* remove this empty directory */
	if (rmdir(dirname)) 
		fprintf(stderr, "Error: failed to remove the directory %s!\n", path);
	else if (vflag)
		printf("directory %s has been removed successfully.\n", path);
}

int myrm_file(char *path) {
	char ch, *msg;

	if (iflag) {
		msg = (char *)malloc(strlen(path) + 25);
		msg = strcat(strcpy(msg, path), ": delete it?(Y/N)");
		if (ch = getyn(msg), ch == 'n' || ch == 'N') 
			return(0);	
	}

	if (unlink(path)) { 
		fprintf(stderr, "Error: file %s can't be removed!\n", path);
		return(-1);
	}

	if (vflag)
		printf("file %s has been removed successfully.\n", path);
}

/* get yes or no from the keyboard */
char getyn(char *msg)
{
	char first,ch;

	/* give out some tips */
	printf("%s  ",msg);
	while (1) {
		first = ch = getchar();
		while (first != '\n' && (ch = getchar()) != '\n')
			;
		if (first == 'y' || first == 'Y' || first == 'n' || first == 'N')
			return(first);
		printf("Please: make sure what you input is 'Y/y/N/n!'\n%s  ",msg);
	}
}

int usage(void) 
{
	printf("usage: progname [options] filenames ...\n");
	exit(0);
}
