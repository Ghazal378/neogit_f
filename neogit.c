#include <dirent.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#ifndef DT_DIR
#define DT_DIR 4
#endif
#ifndef DT_REG
#define DT_REG 8
#endif
int goto_neogit() {
  char cwd[PATH_MAX];
  if (getcwd(cwd, PATH_MAX) == NULL) {
    perror("Error getting current working directory");
    return 1;
  }
  do {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(".");

    if (dir == NULL) {
      perror("Error opening current directory");
      return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0) {
        return 0;
      }
    }
    // Change directory to the parent directory
    if (strcmp(cwd, "/") != 0) {
      if (chdir("..")) {
        perror("Error changing directory to parent");
        return 1;
      }
    }

    if (getcwd(cwd, PATH_MAX) == NULL) {
      perror("Error getting current working directory");
      return 1;
    }
  } while (strcmp(cwd, "/") != 0);

  return 0;
}
int is_staged(const char *filename) {
  if (goto_neogit() == 1) {
    perror("You have to initialize a repository first.\n");
    exit(1);
  }

  DIR *dir = opendir(".neogit/stagingArea");
  if (dir == NULL) {
    perror("Error opening stagingArea.\n");
    exit(1);
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(filename, entry->d_name) == 0) {
      closedir(dir);
      return 0; // File is staged
    }
  }

  closedir(dir);
  return 1; // File is not staged
}
void generateHash(char *hash, size_t hashSize) {
  // Use timestamp as the base
  time_t currentTime = time(NULL);
  snprintf(hash, hashSize, "%ld", currentTime);

  // Append some random data for uniqueness
  size_t timestampLength = strlen(hash);
  for (size_t i = 0; i < hashSize - timestampLength; ++i) {
    hash[timestampLength + i] =
        'a' + rand() % 26; // Use random letters for simplicity
  }

  hash[hashSize - 1] = '\0'; // Null-terminate the hash
}
time_t getFileModificationTime(const char *filename) {
  struct stat fileStat;
  if (stat(filename, &fileStat) == -1) {
    perror("Error in stat");
    return 0;
  }

  return fileStat.st_mtime;
}
void get_current_time(char *time_str) {
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
}
int configuration() {
  if (chdir(".neogit"))
    return 1;
  if (mkdir("stagingArea", 0755))
    return 1;
  if (mkdir("commits", 0755))
    return 1;
  if (mkdir("files", 0755))
    return 1;
  if (mkdir("refs", 0755))
    return 1;
  if (mkdir("refs/heads", 0755))
    return 1;
  FILE *file1, *file2, *file3, *file4, *file5, *file6, *file7, *file8, *file9,
      *file10;
  file1 = fopen("refs/heads/master", "w");
  file2 = fopen("config", "w");
  file3 = fopen("ALIAS_FILE", "w");
  file4 = fopen("valid_commands", "w");
  file5 = fopen("messages", "w");
  file6 = fopen("tracking", "w");
  file7 = fopen("staging", "w");
  file8 = fopen("Head", "w");
  file9 = fopen("commit_hash", "w");
  file10 = fopen("index", "w");
  if (file1 == NULL || file2 == NULL || file3 == NULL || file4 == NULL ||
      file5 == NULL || file6 == NULL || file7 == NULL || file8 == NULL ||
      file9 == NULL) {
    perror("Error opening files");
    return 1;
  }
  fprintf(file4, "add\ncommit\nset\nreplace\nremove\nstatus\nconfig\ninit\nrese"
                 "t\nstatus\nlog\nbranch\ncheckout\n");
  fprintf(file8, ".neogit/refs/heads/master");
  fclose(file2);
  fclose(file3);
  fclose(file1);
  fclose(file4);
  fclose(file5);
  fclose(file6);
  fclose(file7);
  fclose(file8);
  fclose(file9);
  fclose(file10);
  chdir("..");
  return 0;
}
int initialize_repo(char *argv[], int argc) {
  if (argc > 2) {
    perror("Too many arguments!");
    return 1;
  }
  // Does .neogit exist
  char cwd[1024];
  if (getcwd(cwd, 1024) == NULL)
    return 1;
  int exists = 0;
  char temp_cwd[1024];

  do {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(".");
    if (dir == NULL) {
      perror("Error opening current directory");
      return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0) {
        exists = 1;
        break;
      }
    }

    closedir(dir);

    if (getcwd(temp_cwd, 1024) == NULL)
      return 1;

    // Change dir to parent directory
    if (strcmp(temp_cwd, "/") && chdir(".."))
      return 1;
  } while (strcmp(temp_cwd, "/"));

  // Return to the initial cwd
  if (chdir(cwd))
    return 1;

  if (!exists) {
    // Make .neogit
    if (mkdir(".neogit", 0755))
      return 1;
    printf("repository initilized seccessfully.\n");
    return configuration();
  } else {
    perror("neogit repository has already initialized");
    return 1;
  }
}
int copy_file(const char *sourceFile, const char *destinationDir) {
  char command[PATH_MAX * 2 + 5];
  sprintf(command, "cp -r %s %s", sourceFile, destinationDir);

  // Execute the command using the system function
  int status = system(command);

  // Check if the command execution was successful
  if (status != 0) {
    printf("Error copying the file.\n");
  }
}
int compare(char *filename, char *source_path) {
  goto_neogit();
  char stage_path[PATH_MAX];
  sprintf(stage_path, ".neogit/stagingArea/%s", filename);
  time_t time1 = getFileModificationTime(source_path);
  time_t time2 = getFileModificationTime(stage_path);
  if (time1 != time2) {
    return 1;
  }
  return 0;
}
int add_to_stage(char *path_file) {
  char *lastSlash = strrchr(path_file, '/');
  char filename[PATH_MAX];
  if (lastSlash == NULL) {
    strcpy(filename, path_file);
  } else {
    strcpy(filename, lastSlash + 1);
  }
  char source[PATH_MAX];
  strcpy(source, path_file);
  char *destination = ".neogit/stagingArea";
  char cwd[PATH_MAX];
  if (getcwd(cwd, PATH_MAX) == NULL)
    return 1;
  if (goto_neogit() == 1) {
    perror("There is no repository initilized!");
    return 1;
    // }if(is_staged(filename)){
    //   //compare if modified
    //   if(compare(filename,source) == 0){
    //     return 1;
    //   }
    // }
  }
  FILE *tracking_file = fopen(".neogit/tracking", "a");
  char absolute_destination[PATH_MAX];
  if (realpath(destination, absolute_destination) == NULL)
    return 1;
  if (chdir(cwd) == -1)
    return 1;
  strcat(cwd, "/");
  strcat(cwd, source);
  struct stat file_info;
  if (stat(cwd, &file_info)) {
    perror("Error getting file/directory information");
    return 1;
  }

  if (S_ISREG(file_info.st_mode)) {
    fprintf(tracking_file, "%s\n", cwd);
    fclose(tracking_file);
    copy_file(cwd, absolute_destination);
  } else if (S_ISDIR(file_info.st_mode)) {
    DIR *dir = opendir(cwd);
    if (dir == NULL)
      return 1;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_REG) {
        char sub_path[PATH_MAX * 2];
        snprintf(sub_path, sizeof(sub_path), "%s/%s", source, entry->d_name);
        if (add_to_stage(sub_path) == 1) {
          closedir(dir);
          return 1;
        }
      }
    }
  }
}
int run_add(char *argv[], int argc) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, PATH_MAX) == NULL) {
    perror("Error getting the path of cwd.\n");
    return 1;
  }
  if (goto_neogit() == 1) {
    perror("You have to initilize a repository first!\n");
    return 1;
  }
  chdir(cwd);
  if (argc < 3) {
    perror("Too few arguments, please specify a file or directory.\n");
    return 1;
  }
  if (!strcmp(argv[2], "-f")) {
    for (int i = 3; i < argc; i++) {
      add_to_stage(argv[i]);
    }
  } else if (!strcmp(argv[2], "-n")) {
    DIR *dir = opendir(cwd);
    if (dir == NULL) {
      perror("Error opening cwd.\n");
      return 1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_REG) {
        if (is_staged(entry->d_name) == 0) {
          printf("%s is staged.\n", entry->d_name);
        } else {
          printf("%s is not added to staging area.\n", entry->d_name);
        }
      }
    }
  } else {
    if (argc == 3) {
      return add_to_stage(argv[2]);
    } // wildcard
    else {
      for (int i = 2; i < argc; i++) {
        add_to_stage(argv[i]);
      }
    }
  }
  return 0;
}
int run_config(char *argv[], int argc) {
  if (argc < 3) {
    perror("Too few arguments for config command!\n");
    return 1;
  }
  if (strcmp(argv[2], "--global") == 0 && strstr(argv[3], "alias") == NULL) {
    FILE *file, *tmp;
    char home_path[PATH_MAX];
    sprintf(home_path, "%s/.neogitconfig", getenv("HOME"));
    file = fopen(home_path, "r");
    char temp_path[PATH_MAX];
    sprintf(temp_path, "%s/.tmpconfig", getenv("HOME"));
    tmp = fopen(temp_path, "a");
    if (file == NULL || tmp == NULL) {
      system("touch ~/.neogitconfig");
    }
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
      if (strstr(line, argv[3])) {
        found = 1;
        fprintf(tmp, "%s: %s\n", argv[3], argv[4]);
      } else {
        fprintf(tmp, "%s", line);
      }
    }

    if (!found) {
      fprintf(tmp, "%s: %s\n", argv[3], argv[4]);
    }

    fclose(file);

    fclose(tmp);
    system("rm ~/.neogitconfig");
    system("mv ~/.tmpconfig ~/.neogitconfig");
  } else if (strcmp(argv[2], "--global") == 0 &&
             strstr(argv[3], "alias") != NULL) {
    char path_e[PATH_MAX];
    strcat(path_e, getenv("HOME"));
    strcat(path_e, "/.ALIAS_GLOBAL");
    FILE *file = fopen(path_e, "a+");
    if (file == NULL) {
      perror("Error opening or creating ~/.ALIAS_GLOBAL");
      return 1;
    }

    char alias[1024];
    strcpy(alias, argv[3]);
    memmove(alias, alias + 6, strlen(alias) - 5);

    fprintf(file, "%s : %s\n", alias, argv[4]);
    fclose(file);

  } else if (strstr(argv[2], "alias") != NULL) {
    FILE *file;
    if (goto_neogit() == 1) {
      perror("you have to first initilize a repo\n");
      return 1;
    }
    file = fopen(".neogit/ALIAS_FILE", "a");
    if (file == NULL) {
      perror("Error opening the file.\n");
      return 1;
    }
    char alias[1024];
    strcpy(alias, argv[2]);
    memmove(alias, alias + 6, strlen(alias) - 5);

    fprintf(file, "%s : %s\n", alias, argv[3]);
    fclose(file);

  } else {
    FILE *file, *tmp;
    if (goto_neogit() == 1) {
      perror("you have to first initilize a repo\n");
      return 1;
    }
    file = fopen(".neogit/config", "r");
    tmp = fopen(".neogit/tmpconfig", "a");
    if (file == NULL) {
      perror("Error opening the file.\n");
      return 1;
    }
    char line[1024]; // Adjust the size as needed
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
      if (strstr(line, argv[2])) {
        found = 1;
        fprintf(tmp, "%s: %s\n", argv[2], argv[3]);
      } else {
        fprintf(tmp, "%s", line);
      }
    }

    if (!found) {
      fprintf(tmp, "%s: %s\n", argv[2], argv[3]);
    }

    fclose(file);
    if (remove(".neogit/config") == 1) {
      perror("Error removing the file.\n");
      return 1;
    }
    if (rename(".neogit/tmpconfig", ".neogit/config"))
      return 1;
    fclose(tmp);
  }
}
int run_alias(char *argv[], int argc) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, PATH_MAX) == NULL)
    return 1;
  if (goto_neogit() == 1) {
    perror("you have to initilize a repository first.\n");
    return 1;
  }
  FILE *file = fopen(".neogit/ALIAS_FILE", "r");
  if (file == NULL) {
    perror("Error opening the alias file.\n");
    return 1;
  }
  char line[1024];
  int found = 0;
  while (fgets(line, sizeof(line), file) != NULL) {
    char alias[1024], value[1024], command[20];
    if (sscanf(line, "%s : %[^\n]", alias, value) != 2)
      return 1;
    if (sscanf(value, "%[^ \n]", command) != 1)
      return 1;
    strcat(command, "\n");
    int valid = 0;
    FILE *commands = fopen(".neogit/valid_commands", "r");
    if (commands == NULL)
      return 1;
    char buffer[45];
    while (fgets(buffer, 45, commands)) {
      if (strcmp(buffer, command) == 0) {
        valid = 1;
        break;
      }
    }
    fclose(commands);
    if (!valid) {
      printf("Invalid command!\n");
      return 1;
    }
    if (strcmp(alias, argv[1]) == 0) {
      found = 1;
      char syscommand[1000];
      strcpy(syscommand, "neogit ");
      strcat(syscommand, value);
      chdir(cwd);
      system(syscommand);
    }
  }
  fclose(file);
  if (found == 0) {
    char path_u[PATH_MAX];
    sprintf(path_u, "%s/.ALIAS_GLOBAL", getenv("HOME"));
    FILE *global = fopen(path_u, "a+");
    if (global == NULL) {
      perror("Error opening file.\n");
      return 1;
    }
    char input[1024];
    while (fgets(input, sizeof(input), global) != NULL) {
      char alias_g[1024], value_g[1024], command_g[20];
      if (sscanf(input, "%s : %[^\n]", alias_g, value_g) != 2)
        return 1;
      if (sscanf(value_g, "%[^ \n]", command_g) != 1)
        return 1;
      strcat(command_g, "\n");
      int valid = 0;
      if (goto_neogit() == 1) {
        perror("you have to initilize a repository first.\n");
        return 1;
      }
      FILE *commands = fopen(".neogit/valid_commands", "r");
      if (commands == NULL)
        return 1;
      char buffer[45];
      while (fgets(buffer, 45, commands)) {
        if (strcmp(buffer, command_g) == 0) {
          valid = 1;
          break;
        }
      }
      if (!valid) {
        printf("Invalid command!\n");
        return 1;
      }
      if (strcmp(alias_g, argv[1]) == 0) {
        found = 1;
        char syscommand[1000];
        strcpy(syscommand, "neogit ");
        strcat(syscommand, value_g);
        chdir(cwd);
        system(syscommand);
      }
    }
  }
  if (!found) {
    perror("Invalid command!\n");
    return 1;
  }
  return 0;
}
void get_user_info(char *user, char *email, size_t size) {
  char neogitconfig_path[PATH_MAX];
  strcat(neogitconfig_path, getenv("HOME"));
  strcat(neogitconfig_path, "/.neogitconfig");
  const char *neogit_config_path = ".neogit/config";
  FILE *glo = fopen(neogitconfig_path, "r");
  FILE *file = fopen(neogit_config_path, "r");
  fseek(file, 0, SEEK_END);
  long size_f = ftell(file);
  fclose(file);
  FILE *user_file;
  if (glo == NULL && size_f == 0) {
    perror("Tell me who you are.\n");
    exit(1);
  }
  if (glo == NULL & size_f != 0) {
    char user_file_path[PATH_MAX];
    snprintf(user_file_path, sizeof(user_file_path), ".neogit/config");
    user_file = fopen(user_file_path, "r");
    if (user_file == NULL) {
      perror("Error loading the config file");
      return;
    }

    char line_u[1024];

    while (fgets(line_u, 1024, user_file) != NULL) {
      char key[1024], value_u[1024];

      if (sscanf(line_u, "%[^:]: %[^\n]", key, value_u) != 2)
        continue;

      if (strcmp(key, "user.name") == 0) {
        snprintf(user, size, "%s", value_u);
      } else {
        sprintf(email, "%s", value_u);
      }
    }

    fclose(user_file);
    return;
  }
  time_t neogitconfig_mod_time = getFileModificationTime(neogitconfig_path);
  time_t neogit_config_mod_time = getFileModificationTime(neogit_config_path);

  if (neogit_config_mod_time < neogitconfig_mod_time) {
    char user_file_path[PATH_MAX];
    snprintf(user_file_path, sizeof(user_file_path), "%s/.neogitconfig",
             getenv("HOME"));
    user_file = fopen(user_file_path, "r");
  } else {
    char user_file_path[PATH_MAX];
    snprintf(user_file_path, sizeof(user_file_path), ".neogit/config");
    user_file = fopen(user_file_path, "r");
  }

  if (user_file == NULL) {
    perror("Error loading the config file");
    return;
  }

  char line_u[1024];

  while (fgets(line_u, 1024, user_file) != NULL) {
    char key[1024], value_u[1024];

    if (sscanf(line_u, "%[^:]: %[^\n]", key, value_u) != 2)
      continue;

    if (strcmp(key, "user.name") == 0) {
      snprintf(user, size, "%s", value_u);
      break;
    } else {
      sprintf(email, "%s", value_u);
      break;
    }
  }

  fclose(user_file);
}
char *get_path(char *filename) {
  FILE *file = fopen(".neogit/tracking", "r");
  if (file == NULL) {
    perror("Error opening tracking file.\n");
    exit(1);
  }
  char line[1024];
  while (fgets(line, 1024, file) != NULL) {
    const char *lastSlash = strrchr(line, '/');
    char *tracking_name = malloc(PATH_MAX);
    if (lastSlash != NULL) {
      strcpy(tracking_name, lastSlash + 1);
      tracking_name[strcspn(tracking_name, "\n")] = '\0';
    }
    if (strcmp(tracking_name, filename) == 0) {
      fclose(file);
      return strdup(line);
    }
    free(tracking_name);
  }
  fclose(file);
  return NULL;
}
void get_current_branch(char *curr) {
  goto_neogit();
  DIR *dir = opendir(".neogit/Head");
  int count = 0;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      count++;
    }
  }
  if (count == 1) {
    strcpy(curr, "master");
  } else {
  }
}
int get_last_commit(const char *filename, char *result) {
  goto_neogit();
  char path[PATH_MAX] = ".neogit/files/";
  strcat(path, filename);
  DIR *dir;
  dir = opendir(path);
  if (dir == NULL) {
    return 0;
  }

  int has_commits = 0; // Flag to check if the file has commits
  time_t temp = 0;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      has_commits = 1; // The file has at least one commit
      char file_path[2 * PATH_MAX];
      sprintf(file_path, "%s/%s", path, entry->d_name);
      time_t time = getFileModificationTime(file_path);
      if (time > temp) {
        temp = time;
        strcpy(result, file_path);
      }
    }
  }

  closedir(dir);

  if (!has_commits) {
    strcpy(result, ""); // The file has never been committed
  }

  return has_commits;
}
int compare_last(char *filename) {
  goto_neogit();
  char hash[50];
  get_last_commit(filename, hash);
  char source_path[PATH_MAX];
  sprintf(source_path, ".neogit/files/%s/%s", filename, hash);
  char stage_path[PATH_MAX];
  FILE *source, *stage;
  source = fopen(source_path, "r");
  stage = fopen(stage_path, "r");
  if (source == NULL || stage == NULL) {
    perror("Error opening files or directories.\n");
    exit(1);
  }
  char line1[1024], line2[1024];
  while (fgets(line1, 1024, source) != NULL &&
         fgets(line2, 1024, stage) != NULL) {
    if (strcmp(line1, line2))
      return 0;
  }
  if (fgets(line1, 1024, source) != NULL || fgets(line2, 1024, stage) != NULL) {
    return 0;
  }
  return 1;
}
int check_with_last_commit() {
  goto_neogit();
  DIR *dir = opendir(".neogit/stagingArea");
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      if (compare_last(entry->d_name) == 1) {
        return 1;
      }
    }
  }
  return 0;
}
void make_file_versions(const char *filename, char *file_path,char *commithash) {
  char path[PATH_MAX];
  snprintf(path, sizeof(path), ".neogit/files/%s", filename);
  mkdir(path, 0755);
  char absolute_destination[PATH_MAX];
  if (realpath(path, absolute_destination) == NULL) {
    perror("Error getting the absolute destination.\n");
    exit(1);
  }
  file_path[strcspn(file_path, "\n")] = '\0';
  copy_file(file_path, absolute_destination);
  strcat(absolute_destination, "/");
  strcat(absolute_destination, filename);
  const char *file_extension = strrchr(filename, '.');
  char new_filename[PATH_MAX * 2];
  snprintf(new_filename, sizeof(new_filename), "%s/%s%s", path, commithash,
           file_extension);
  if (rename(absolute_destination, new_filename) != 0) {
    perror("Error renaming file.\n");
    exit(1);
  }
}
int is_empty_stagingArea() {
  goto_neogit();
  DIR *dir = opendir(".neogit/stagingArea");
  struct dirent *entry;
  int count;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      count++;
    }
  }
  return count;
}
int run_commit(char *argv[], int argc) {
  if (argc < 4) {
    perror("too few arguments\n");
    return 1;
  }
  char cwd[PATH_MAX];

  if (goto_neogit() != 0) {
    perror("You have to initialize a repo first.\n");
    return 1;
  }
  char message[1024];
  if (!strcmp(argv[2], "-s")) {
    FILE *messages = fopen(".neogit/messages", "r");
    if (messages == NULL) {
      perror("Error opening messages files.\n");
      return 1;
    }
    char line_s[1024];
    int found = 0;
    while (fgets(line_s, 1024, messages)) {
      char shortcut[1024], value[1024];
      if (sscanf(line_s, "%s = %[^\n]", shortcut, value) != 2)
        return 1;
      if (strcmp(shortcut, argv[3]) == 0) {
        found = 1;
        strcpy(message, value);
      }
    }
    if (!found) {
    perror("There is no shourcut name for given info.\n");
    return 1;
  }
  } else {
    strcpy(message, argv[3]);
  }
  if (strlen(message) > 72) {
    perror("message too long\n");
    return 1;
  }
  char branch[1024];
  FILE *head_file=fopen(".neogit/Head","r");
  char line[PATH_MAX];
  fgets(line,1024,head_file);
  sscanf(line,".neogit/refs/heads/%s",branch);
  char user[1024];
  char email[1024];
  get_user_info(user,email,1024);
  char commit_hash[41];
  generateHash(commit_hash,sizeof(commit_hash));
  FILE *file_hash=fopen(".neogit/commit_hash","w");
  if(file_hash==NULL){
    perror("Error opening commit file.\n");
    return 1;
  }
  fprintf(file_hash,"%s",commit_hash);
  fclose(file_hash);
  FILE *HEAD_f=fopen(line,"w");
  fprintf(HEAD_f,"%s",commit_hash);
  char commit_file[256];
  snprintf(commit_file, sizeof(commit_file), ".neogit/commits/%s",commit_hash);
  FILE *commit_files = fopen(commit_file, "w");
  if (commit_files == NULL) {
    perror("Error making the commit file.\n");
  }
  char time_str[24];
  get_current_time(time_str);
  fprintf(commit_files, "commit_hash = %s on branch %s\n", commit_hash,branch);
  fprintf(commit_files, "commit_time = %s\n", time_str);
  fprintf(commit_files, "commit_message = %s\n", message);
  fprintf(commit_files,"author= %s <%s>\n",user,email);
  fprintf(commit_files,"file's paths:\n");
  DIR *stagingarea;
  stagingarea = opendir(".neogit/stagingArea");
  struct dirent *entry;
  while ((entry = readdir(stagingarea)) != NULL) {
    if (entry->d_type == DT_REG) {
        char path[PATH_MAX];
        char *tracking_path = get_path(entry->d_name);
        make_file_versions(entry->d_name,tracking_path,commit_hash);
        if (tracking_path != NULL) {
            fprintf(commit_files, "%s \n", tracking_path);
            free(tracking_path);
            // Delete the path from the tracking area
            char path_to_delete[PATH_MAX] = "sed -i '/";
            strcat(path_to_delete, entry->d_name);
            strcat(path_to_delete, "/d' .neogit/tracking");
            system(path_to_delete);
        }
        char path_to_delete[PATH_MAX] = "rm .neogit/stagingArea/";
        strcat(path_to_delete, entry->d_name);
        system(path_to_delete);

    }
  }
  printf("change commited with %s commit hash\n%s\n**%s**\n",commit_hash,time_str,message);
}
int shortcut_messages(char *argv[], int argc) {
  if (goto_neogit() == 1) {
    perror("You have to initilize a repo first.\n");
    return 1;
  }
  FILE *file = fopen(".neogit/messages", "a");
  if (file == NULL) {
    perror("Error opening file.\n");
    return 1;
  }
  fprintf(file, "%s = %s\n", argv[5], argv[3]);
  fclose(file);
  return 0;
}
int replace_shourtcut(char *argv[], int argc) {
  if (goto_neogit() == 1) {
    perror("You have to initialize a repo first.\n");
    return 1;
  }

  FILE *file, *tmp;
  file = fopen(".neogit/messages", "r");
  tmp = fopen(".neogit/tmp", "w");

  if (file == NULL || tmp == NULL) {
    perror("Error opening file\n");
    return 1;
  }

  char line[1024];

  while (fgets(line, 1024, file)) {
    char shortcut[1024], message[1024];

    if (sscanf(line, "%s = %[^\n]", shortcut, message) == 2) {
      if (strcmp(shortcut, argv[5]) == 0) {
        fprintf(tmp, "%s = %s\n", shortcut, argv[3]);
      } else {
        fprintf(tmp, "%s", line);
      }
    } else {
      fprintf(stderr, "Invalid line format: %s", line);
    }
  }

  fclose(file);
  fclose(tmp);

  if (remove(".neogit/messages") == 1)
    return 1;
  if (rename(".neogit/tmp", ".neogit/messages") == 1)
    return 1;

  return 0;
}
int remove_shortcut(char *argv[], int argc) {
  if (goto_neogit() == 1) {
    perror("You have to initialize a repo first.\n");
    return 1;
  }
  FILE *file = fopen(".neogit/messages", "r");
  FILE *tmp = fopen(".neogit/tmp", "a");
  if (tmp == NULL || file == NULL) {
    perror("Error opening the file.\n");
    return 1;
  }
  char line[1024];
  int found = 0;
  while (fgets(line, 1024, file)) {
    char shortcut[1024], message[1024];
    if (sscanf(line, "%s = %[^\n]", shortcut, message) != 2)
      return 1;
    if (strcmp(argv[3], shortcut) == 0) {
      found = 1;
    } else {
      fprintf(tmp, "%s = %s\n", shortcut, message);
    }
  }
  fclose(file);
  fclose(tmp);
  if (found == 0) {
    perror("no shortcut with given name.\n");
    return 1;
  }

  if (remove(".neogit/messages") == 1)
    return 1;
  if (rename(".neogit/tmp", ".neogit/messages") == 1)
    return 1;

  return 0;
}
void reset_file(const char *file_path) {
  char dest_path[PATH_MAX];
  snprintf(dest_path, sizeof(dest_path), ".neogit/stagingArea/%s", file_path);
  if (remove(dest_path) == 0) {
    printf("File %s has been unstaged.\n", file_path);
  } else {
    perror("Error unstaging file.\n");
  }
}
void reset_directory(const char *dir_path) {
  DIR *dir = opendir(dir_path);
  if (dir == NULL) {
    perror("Error opening directory.\n");
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      reset_file(entry->d_name);
    }
  }

  closedir(dir);
}
int reset(char *argv[], int argc) {
  if (argc < 3) {
    perror("Too few arguments.\n");
    return 1;
  }
  if (!strcmp(argv[2], "-undo")) {
  }
  const char *path = argv[2];
  DIR *dir = opendir(path);

  if (dir != NULL) {
    // Input is a directory
    reset_directory(path);
    closedir(dir);
  } else {
    // Input is a file
    reset_file(path);
  }
  return 0;
}
void compare_files(const char *source_file, const char *commit_file) {
  goto_neogit();
  FILE *file1, *file2;
  char line1[1024], line2[1024];
  if ((file1 = fopen(source_file, "r")) == NULL) {
    perror("Error opening source file.\n");
    exit(1);
  }

  if ((file2 = fopen(commit_file, "r")) == NULL) {
    perror("Error opening commit file.\n");
    fclose(file1);
    exit(1);
  }

  while (fgets(line1, sizeof(line1), file1) != NULL &&
         fgets(line2, sizeof(line2), file2) != NULL) {
    if (strcmp(line1, line2) != 0) {
      printf("M\n");
      fclose(file1);
      fclose(file2);
      return;
    }
  }

  if (fgets(line1, sizeof(line1), file1) != NULL ||
      fgets(line2, sizeof(line2), file2) != NULL) {
    printf("M\n");
    fclose(file1);
    fclose(file2);
    return;
  }

  printf("A\n");

  fclose(file1);
  fclose(file2);
  // Check for permission changes
  struct stat stat_source, stat_commit;
  if (stat(source_file, &stat_source) == -1 ||
      stat(commit_file, &stat_commit) == -1) {
    perror("Error getting file information.\n");
    exit(1);
  }

  if (stat_source.st_mode != stat_commit.st_mode) {
    printf("T\n");
    return;
  }
}
void run_status(char *argv[], int argc) {
  char cwd[PATH_MAX];

  if (getcwd(cwd, PATH_MAX) == NULL) {
    perror("Error getting cwd.\n");
    exit(1);
  }

  if (goto_neogit() == 1) {
    perror("You have to initialize a repository first.\n");
    exit(1);
  }

  DIR *dir;
  if ((dir = opendir(cwd)) == NULL) {
    perror("Error opening cwd.\n");
    exit(1);
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      printf("%s ", entry->d_name);

      char commit_file_path[PATH_MAX];
      sprintf(commit_file_path, ".neogit/files/%s", entry->d_name);
      if (is_staged(entry->d_name) == 1) {
        printf("-");
      } else {
        printf("+");
      }
      char last_path[PATH_MAX];
      if (get_last_commit(entry->d_name, last_path) == 0) {
        printf("A\n"); // File has never been committed
        continue;
      }
      char path[2 * PATH_MAX];
      sprintf(path, "%s/%s", cwd, entry->d_name);
      compare_files(path, last_path);
    }
  }

  closedir(dir);
}
int create_branch(const char *branch_name) {
  if (goto_neogit() == 1) {
    perror("You have to first initilize a repository.\n");
    return 1;
  }
  char branch_path[PATH_MAX];
  snprintf(branch_path, sizeof(branch_path), ".neogit/refs/heads/%s",
           branch_name);
  FILE *file = fopen(".neogit/commit_hash", "r");
  if (file == NULL) {
    perror("Error getting last commit hash.\n");
    return 1;
  }
  char last_commit_hash[50];
  if (fgets(last_commit_hash, 50, file) == NULL) {
    perror("You cant make a new branch without commiting!\nstill on branch "
           "\"master\"");
    return 1;
  }
  // Check if the branch already exists
  FILE *branch_file = fopen(branch_path, "r");
  if (branch_file != NULL) {
    fclose(branch_file);
    printf("Branch '%s' already exists.\n", branch_name);
    return 1;
  }
  // Create the branch file
  branch_file = fopen(branch_path, "w");
  if (branch_file == NULL) {
    perror("Error creating branch file.\n");
    return 1;
  }
  fprintf(branch_file, "%s", last_commit_hash);
  fclose(branch_file);
  printf("Branch '%s' created successfully.\n", branch_name);
  return 0;
}
void list_branches() {
  if (goto_neogit() == 1) {
    perror("You should first initilize a repository first.\n*****use neogit "
           "init\n");
    return;
  }
  DIR *dir;
  dir = opendir(".neogit/refs/heads");
  if (dir == NULL) {
    perror("Error opening refs directory\n");
    return;
  }
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
      printf("%s\n", entry->d_name);
  }
}
int run_branch(char *argv[], int argc) {
  if (argc == 3) {
    create_branch(argv[2]);
  } else {
    list_branches();
  }
}
int is_branch(char *branchnanme) {
  goto_neogit();
  char path[PATH_MAX] = ".neogit/refs/heads/";
  strcat(path, branchnanme);
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    return 0;
  }
  return 1;
}
int copy_file_tofile(const char *source_file, const char *destination_file) {
    char command[PATH_MAX * 2 + 5];
    snprintf(command, sizeof(command), "cp %s %s", source_file, destination_file);

    // Execute the command using the system function
    int status = system(command);

    // Check if the command execution was successful
    if (status != 0) {
        fprintf(stderr, "Error copying the file: %s\n", source_file);
        return 1;
    }

    return 0;
}
void checkout_commithash(char *hash) {
  goto_neogit();
  char path[PATH_MAX];
  sprintf(path,".neogit/commits/%s",hash);
  FILE *file=fopen(path,"r");
  if(file == NULL){
    perror("no commit found with given input!\nPlease enter a valid commit hash.\n");
    return;
  }
  char line[1024];
  while(fgets(line,1024,file)!=NULL){
    if(strstr(line,"file's paths:") == NULL){
      continue;
    }
    else{
      break;
    }
  }
  while(fgets(line,1024,file)!=NULL){
    line[strcspn(line,"\n")]='0';
    const char *lastSlash = strrchr(line, '/');
    char filename[PATH_MAX];
    if(lastSlash == NULL){
      strcpy(filename,lastSlash);
    }else{
      strcpy(filename,lastSlash+1);
    }
    char path_ver[PATH_MAX];
    sprintf(path_ver,".neogit/files/%s/%s",filename,hash);
    copy_file_tofile(line,path_ver);
    char syscom1[PATH_MAX*2+5];
    sprintf(syscom1,"mv .neogit/files/%s/%s .neogit/files/%s/%s",filename,hash,filename,filename);
    system(syscom1);
    char syscom2[PATH_MAX*2+5];
    sprintf(syscom2,"cp .neogit/files/%s/%s %s",filename,filename,line);
    system(syscom2);
  }
}
void checkout_HEAD() {}
void checkout_branch(char *argv) {
  goto_neogit();
  FILE *file = fopen(".neogit/Head", "w");
  if (file == NULL) {
    perror("Error switching branch.\n");
    return;
  }
  fprintf(file, ".neogit/refs/heads/%s", argv);
  fclose(file);
}
int run_checkout(char *argv[], int argc) {
  if (is_branch(argv[2]))
    checkout_branch(argv[2]);
  else if (strcmp(argv[2], "HEAD") == 0)
    checkout_HEAD();
  else
    checkout_commithash(argv[2]);
}
void print_commit_log(const char *commit_file) {
  goto_neogit();
  char path[PATH_MAX];
  sprintf(path, ".neogit/commits/%s", commit_file);
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    perror("Error opening files in commits directory.\n");
    exit(1);
  }
  int count = 0;
  char line[1024];
  while (fgets(line, 1024, file) != NULL) {
    if (strstr(line, "path") == NULL && strstr(line, "/") == NULL) {
      printf("%s", line);
    } else {
      count++;
    }
  }

  printf("number of files commited: %d\n\n", count - 1);
}
int run_log(char *argv[], int argc) {
  goto_neogit();
  DIR *dir = opendir(".neogit/commits");
  struct dirent *entry;
  if (dir == NULL) {
    perror("Error opening commits directory.\n");
    return 1;
  }
  char filename[1000][50];
  int i = 0;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG)
      strcpy(filename[i++], entry->d_name);
  }
  closedir(dir);
  if (argc == 2) {
    for (int j = 0; j < i; j++) {
      print_commit_log(filename[j]);
    }
  }else if (strcmp(argv[2], "-n") == 0) {
    int n = atoi(argv[3]);
    for (int j = i - 1; j >= 0 && n > 0; j--, n--) {
        print_commit_log(filename[j]);
    }
}
 else if (strcmp(argv[2], "-branch") == 0) {
    // Implement logic for -branch option
  } else if (strcmp(argv[2], "-author") == 0) {
    for (int j = 0; j < i; j++) {
      char path_s[PATH_MAX];
      sprintf(path_s, ".neogit/commits/%s", filename[j]);
      FILE *file_s = fopen(path_s, "r");
      char name[50];
      strcpy(name, argv[3]);
      char line[1024];
      while (fgets(line, 1024, file_s) != NULL) {
        char key[1024], value[1024];
        if (sscanf(line, "%[^=]= %s", key, value) != 2)
          continue;
        if (strcmp(key, "author") == 0) {
          if (strcmp(value, name) == 0) {
            print_commit_log(filename[j]);
            break;
          }
        }
      }
    }
  } else if (strcmp(argv[2], "-since") == 0) {
    // Convert the since date argument to a time_t value
    struct tm since_tm;
    strptime(argv[3], "%Y-%m-%d %H:%M:%S", &since_tm);
    time_t since_time = mktime(&since_tm);
    for (int j = 0; j < i; j++) {
      char path_s[PATH_MAX];
      sprintf(path_s, ".neogit/commits/%s", filename[j]);
      FILE *file_s = fopen(path_s, "r");
      char line[1024];
      while (fgets(line, 1024, file_s) != NULL) {
        char key[1024], value[1024];
        if (sscanf(line, "%s = %[^\n]", key, value) != 2)
          continue;

        if (strcmp(key, "commit_time") == 0) {
          // Convert the commit time from string to time_t
          struct tm commit_tm;
          strptime(value, "%Y-%m-%d %H:%M:%S", &commit_tm);

          time_t commit_time = mktime(&commit_tm);

          // Compare with since_time
          if (difftime(commit_time, since_time) > 0) {
            print_commit_log(filename[j]);
            break;
          }
        }
      }

      fclose(file_s);
    }

  } else if (strcmp(argv[2], "-before") == 0) {
    // Convert the before date argument to a time_t value
    struct tm before_tm;
    if (strptime(argv[3], "%Y-%m-%d %H:%M:%S", &before_tm) == NULL) {
      fprintf(stderr, "Invalid date format. Use YYYY-MM-DD HH:MM:SS\n");
      exit(1);
    }
    time_t before_time = mktime(&before_tm);

    for (int j = 0; j < i; j++) {
      char path_s[PATH_MAX];
      sprintf(path_s, ".neogit/commits/%s", filename[j]);
      FILE *file_s = fopen(path_s, "r");
      char line[1024];

      while (fgets(line, 1024, file_s) != NULL) {
        char key[1024], value[1024];
        if (sscanf(line, "%s = %[^\n]", key, value) != 2)
          continue;

        if (strcmp(key, "commit_time") == 0) {
          // Convert the commit time from string to time_t
          struct tm commit_tm;
          if (strptime(value, "%Y-%m-%d %H:%M:%S", &commit_tm) == NULL) {
            fprintf(stderr, "Invalid date format in commit file.\n");
            exit(1);
          }
          time_t commit_time = mktime(&commit_tm);

          // Compare with before_time
          if (difftime(commit_time, before_time) < 0) {
            print_commit_log(filename[j]);
            break;
          }
        }
      }

      fclose(file_s);
    }
  } else if (strcmp(argv[2], "-search") == 0) {
    for (int j = 0; j < i; j++) {
        char path_s[PATH_MAX];
        sprintf(path_s, ".neogit/commits/%s", filename[j]);
        FILE *file_s = fopen(path_s, "r");
        char line[1024];
        char word[58];
        strcpy(word, argv[3]);

        while (fgets(line, 1024, file_s) != NULL) {
            char key[1024], value[1024];
            if (sscanf(line, "%s = %[^\n]", key, value) != 2)
                continue;

            if (strcmp(key, "commit_message") == 0) {
                if (strstr(value, word) != NULL) {
                    print_commit_log(filename[j]);
                    break;
                }
            }
        }

        fclose(file_s);
    }
}
else {
    printf("Invalid option for log command.\n");
  }

  return 0;
}
int main(int argc, char *argv[]) {
  if (argc < 2) {
    perror("Too few arguments!\n");
    return 1;
  }

  else if (!strcmp(argv[1], "init")) {
    return initialize_repo(argv, argc);
  } else if (!strcmp(argv[1], "config")) {
    run_config(argv, argc);
  } else if (!strcmp(argv[1], "add")) {
    return run_add(argv, argc);
  } else if (!strcmp(argv[1], "commit")) {
    run_commit(argv, argc);
  } else if (!strcmp(argv[1], "set")) {
    shortcut_messages(argv, argc);
  } else if (!strcmp(argv[1], "replace")) {
    replace_shourtcut(argv, argc);
  } else if (!strcmp(argv[1], "remove")) {
    remove_shortcut(argv, argc);
  } else if (!strcmp(argv[1], "reset")) {
    reset(argv, argc);
  } else if (!strcmp(argv[1], "status")) {
    run_status(argv, argc);
  } else if (!strcmp(argv[1], "log")) {
    run_log(argv, argc);
  } else if (!strcmp(argv[1], "branch")) {
    run_branch(argv, argc);
  } else if (!strcmp(argv[1], "checkout")) {
    run_checkout(argv, argc);
  } else {
    run_alias(argv, argc);
  }
  return 0;
}