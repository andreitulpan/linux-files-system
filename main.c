#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_INPUT_LINE_SIZE 300
#define MAX_DIRECTORY_SIZE 256

// Definire structuri
struct Dir;
struct File;
struct Cmd;

typedef struct Cmd {
	char **args;
	int counter;
} Cmd;

typedef struct Dir {
	char *name;
	struct Dir *parent;
	struct File *head_children_files;
	struct Dir *head_children_dirs;
	struct Dir *next;
} Dir;

typedef struct File {
	char *name;
	struct Dir *parent;
	struct File *next;
} File;

// Pentru a crea un fisier nou, parcurg lista de fisiere
// a directorului si adaug la finalul listei fisierul nou
void touch (Dir *parent, char *name) {
	// Creez fisierul nou
	File *fileToAdd = malloc(sizeof(File));
	assert(fileToAdd);
	fileToAdd->name = strdup(name);
	fileToAdd->next = NULL;
	fileToAdd->parent = parent;

	int error = 0;
	File *temp_file = parent->head_children_files;
	Dir *temp_dir = parent->head_children_dirs;

	// Verific daca exista un director cu numele
	// fisierului creat pentru adaugare
	while (temp_dir != NULL) {
			if (!strcmp(fileToAdd->name, temp_dir->name)) {
				error = 1;
				break;
			}
			temp_dir = temp_dir->next;
	}

	// Verific daca mai exista fisiere in director
	if (parent->head_children_files == NULL) {
		// Pun fisierul primul in director
		// daca numele este disponibil
		if (!error) {
			parent->head_children_files = fileToAdd;
		} else {
			printf("File already exists\n");
			free(fileToAdd->name);
			free(fileToAdd);
		}
	} else {	// Cazul in care exista si alte fisiere

		// Pun fisierul la final in director
		// daca numele este disponibil
		while (temp_file->next != NULL) {	// Parcurg lista de fisiere
			if (!strcmp(fileToAdd->name, temp_file->name)) {
				error = 1;
				break;
			}
			temp_file = temp_file->next;
		}

		// Verific disponibilitatea numelui si pentru ultimul element
		if (!strcmp(fileToAdd->name, temp_file->name)) {
			error = 1;
		}

		// Adaug fisierul daca este posibil sau printez eroarea
		if (!error) {
			temp_file->next = fileToAdd;
		} else {
			printf("File already exists\n");
			free(fileToAdd->name);
			free(fileToAdd);
		}

	}

}

// Pentru a crea un director nou, parcurg lista de directoare
// a directorului curent si adaug noul director la finalul listei
void mkdir (Dir *parent, char *name) {
	// Creez directorul nou
	Dir *dirToAdd = malloc(sizeof(Dir));
	assert(dirToAdd);
	dirToAdd->head_children_dirs = NULL;
	dirToAdd->head_children_files = NULL;
	dirToAdd->name = strdup(name);
	dirToAdd->next = NULL;
	dirToAdd->parent = parent;

	int error = 0;
	File *temp_file = parent->head_children_files;
	Dir *temp_dir = parent->head_children_dirs;

	// Verific daca exista un fisier cu numele
	// directorului creat pentru adaugare
	while (temp_file != NULL) {
			if (!strcmp(dirToAdd->name, temp_file->name)) {
				error = 1;
				break;
			}
			temp_file = temp_file->next;
	}

	// Verific daca mai exista alte directoare
	if (parent->head_children_dirs == NULL) {
		// Pun directorul primul daca
		// numele este disponibil
		if (!error) {
			parent->head_children_dirs = dirToAdd;
		} else {
			printf("Directory already exists\n");
			free(dirToAdd->name);
			free(dirToAdd);
		}
	} else { // Cazul in care exista si alte directoare

		// Pun directorul la final daca
		// numele este disponibil
		while (temp_dir->next != NULL) {
			if (!strcmp(dirToAdd->name, temp_dir->name)) {
				error = 1;
				break;
			}
			temp_dir = temp_dir->next;
		}

		// Verific disponibilitatea numelui si pentru ultimul element
		if (!strcmp(dirToAdd->name, temp_dir->name)) {
			error = 1;
		}

		// Adaug fisierul daca este posibil sau printez eroarea
		if (!error) {
			temp_dir->next = dirToAdd;
		} else {
			printf("Directory already exists\n");
			free(dirToAdd->name);
			free(dirToAdd);
		}

	}

}

// Pentru comanda ls parcurg lista de directoare si lista
// de fisiere si printez fiecare element al acestor liste
void ls (Dir *parent) {
	// Parcurg toata lista de directoare din director si o printez
	Dir *temp_dir = parent->head_children_dirs;
	while (temp_dir != NULL) {
			printf("%s\n", (char *)(temp_dir->name));
			temp_dir = temp_dir->next;
	}

	// Parcurg toata lista de fisiere din directorsi o printez
	File *temp_file = parent->head_children_files;
	while (temp_file != NULL) {
			printf("%s\n", (char *)(temp_file->name));
			temp_file = temp_file->next;
	}
}

// Pentru a sterge un fisier il caut in lista de fisiere
// a directorului si ii sterg legaturile, urmand sa eliberez memoria
void rm (Dir *parent, char *name) {

	// Verific daca este primul fisier sau nu
	if (parent->head_children_files != NULL) {
		int find = 0;
		File *findFile = parent->head_children_files;

		if (!strcmp(findFile->name, name)) {

			// Fiind primul fisier il sterg si actualizez head-ul
			File *delFile = findFile;
			parent->head_children_files = findFile->next;
			free(delFile->name);
			free(delFile);

		} else {

			// Ajung la elementul precedent celui ce trebuie sters
			while (findFile->next != NULL) {
				if (!strcmp(findFile->next->name, name)) {
					find = 1;
					break;
				}
				findFile = findFile->next;
			}

			// Daca am gasit elementul il sterg si eliberez memoria
			if (find == 1) {
				File *delFile = findFile->next;
				findFile->next = findFile->next->next;
				free(delFile->name);
				free(delFile);
			} else {
				printf("Could not find the file\n");
			}
		}
	} else {
		printf("Could not find the file\n");
	}
}

// Pentru a sterge un director il caut in lista de directoare, ii sterg
// legaturile, apoi sterg toate fisierele si directoarele aferente acestuia
void rmdir (Dir *parent, char *name) {

	// Verific daca este primul director sau nu
	if (parent->head_children_dirs != NULL) {
		int find = 0;
		Dir *findDir = parent->head_children_dirs;
		if (!strcmp(findDir->name, name)) {

			// Fiind primul director il sterg si actualizez head-ul
			Dir *delDir = findDir;
			parent->head_children_dirs = findDir->next;

			// Sterg toate fisierele aferente
			File *findFile = delDir->head_children_files;
			while (findFile != NULL) {
				File *tmp = findFile->next;
				free(findFile->name);
				free(findFile);
				findFile = tmp;
			}

			// Sterg apeland recursiv functia toate directoarele aferente
			Dir *recursiveDir = delDir->head_children_dirs;
			while (recursiveDir != NULL) {
				Dir *tmp = recursiveDir->next;
				rmdir(delDir, recursiveDir->name);
				recursiveDir = tmp;
			}

			free(delDir->name);
			free(delDir);

		} else {

			// Ajung la elementul precedent celui ce trebuie sters
			while (findDir->next != NULL) {
				if (!strcmp(findDir->next->name, name)) {
					find = 1;
					break;
				}
				findDir = findDir->next;
			}

			// Daca am gasit elementul il sterg si eliberez memoria
			if (find == 1) {

				// Sterg legaturile directorului
				Dir *delDir = findDir->next;
				findDir->next = findDir->next->next;

				// Sterg toate fisierele aferente
				File *findFile = delDir->head_children_files;
				while (findFile != NULL) {
					File *tmp = findFile->next;
					free(findFile->name);
					free(findFile);
					findFile = tmp;
				}

				// Sterg apeland recursiv functia toate directoarele aferente
				Dir *recursiveDir = delDir->head_children_dirs;
				while (recursiveDir != NULL) {
					Dir *tmp = recursiveDir->next;
					rmdir(delDir, recursiveDir->name);
					recursiveDir = tmp;
				}

				free(delDir->name);
				free(delDir);
			} else {
				printf("Could not find the dir\n");
			}
		}
	} else {
		printf("Could not find the dir\n");
	}
}

// Pentru a schimba directorul curent, voi updata
// valoarea homeDir-ului din main cu directorul ales
void cd(Dir **target, char *name) {

	// Verific daca primeste ca parametru '..' sau un director
	if (strcmp(name, "..")) {

		// Caut daca exista directorul primit ca parametru
		// si updatez valoarea target-ului
		Dir *findDir = (*target)->head_children_dirs;
		if ((*target)->head_children_dirs != NULL) {
			int find = 0;
			// Dir *findDir = (*target)->head_children_dirs;
			if (!strcmp(findDir->name, name)) {
				*target = findDir;	// Schimb directorul
			} else {
				while (findDir->next != NULL) {
					if (!strcmp(findDir->next->name, name)) {
						find = 1;
						break;
					}
					findDir = findDir->next;
				}
				if (find == 1) {
					findDir = findDir->next;
					*target = findDir;	// Schimb directorul
				} else {
					printf("No directories found!\n");
				}
			}
		} else {
			printf("No directories found!\n");
		}
	} else {

		// Daca parametrul primit este '..' ma intorc la directorul parinte
		if ((*target)->parent != NULL) {
			*target = (*target)->parent;
		}
	}
}

// Am implementat strrev-ul pentru comanda pwd
char *strrev (char *str) {

	char tmp, *front, *back;

	if (!str || !*str)
		return str;

	front = str;
	back = str + strlen(str) - 1;
	while (front < back) {
		tmp  = *front;
		*front = *back;
		*back = tmp;
		front++;
		back--;
	}

	return str;
}

// Pentru comanda pwd am creat un string gol la care concatenez
// numele rasturnate ale directoarelor, iar in final rastorn tot string-ul.
char *pwd (Dir *target) {

	char *to_return = malloc(MAX_DIRECTORY_SIZE);
	assert(to_return);
	strcpy(to_return, "");

	Dir *moveDir = target;
	while (moveDir->parent != NULL) {
		strcat(to_return, strrev(moveDir->name));
		strcat(to_return, "/");
		moveDir = moveDir->parent;
	}
	strcat(to_return, "emoh/");

	return strrev(to_return);
}

// La folosirea comenzii stop eliberez memoria
// pentru toate directoarele si toate fisierele acestora
void stop (Dir *target) {
	File *delFile = target->head_children_files;
	while (delFile != NULL) {
		File *tmp = delFile->next;
		free(delFile->name);
		free(delFile);
		delFile = tmp;
	}
	Dir *delDir = target->head_children_dirs;
	while (delDir != NULL) {
		Dir *tmp = delDir->next;
		stop(delDir);
		delDir = tmp;
	}
	free(target->name);
	free(target);
}

// Pentru comanda tree parcurg prima oara lista de directoare
// si o printez recursiv apoi lista de fisiere a directorului curent
void tree (Dir *target, int level) {

	// Parcurg lista de directoare
	Dir *nextDir = target->head_children_dirs;
	while (nextDir != NULL) {
		if (!level) {
			printf("%s\n", nextDir->name);
		} else {
			printf("%*c%s\n", 4 * level, ' ', nextDir->name);
		}
		tree(nextDir, level + 1);	// Apelez functia recursiv
		nextDir = nextDir->next;
	}

	// Parcurg lista de fisiere
	File *nextFile = target->head_children_files;
	while (nextFile != NULL) {
		if (!level)
			printf("%s\n", nextFile->name);
		else
			printf("%*c%s\n", 4 * (level), ' ', nextFile->name);

		nextFile = nextFile->next;
	}
}

// Pentru comanda mv
void mv(Dir *parent, char *oldname, char *newname) {
	int type = 0; // 0 - not found, 1 - file, 2 - directory
	int exist = 0;	// Fisierul poate fi redenumit
	File *saveFile;	// Salvez referinta fisierului
	Dir *saveDir;	// Salvez referinta directorului

	// Verific daca numele nou este disponibil pentru fisiere
	File *findFile = parent->head_children_files;
	while (findFile != NULL) {
		if (!strcmp(findFile->name, newname))
			exist = 1;	// nu poate fi redenumit
		findFile = findFile->next;
	}

	// Verific daca numele nou este disponibil pentru directoare
	Dir *findDir = parent->head_children_dirs;
	while (findDir != NULL) {
		if (!strcmp(findDir->name, newname))
			exist = 1;
		findDir = findDir->next;
	}

	findDir = parent->head_children_dirs;
	findFile = parent->head_children_files;

	// Caut daca elementul de mutat este fisier
	if (parent->head_children_files != NULL) {

		// Verific daca este primul fisier din lista
		if (!strcmp(findFile->name, oldname)) {
			type = 1;
			saveFile = findFile;	// Salvez fisierul

			if (exist == 0)		// Daca se poate muta il sterg
				parent->head_children_files = findFile->next;
		} else {

			// Caut fisierul
			while (findFile != NULL) {
				if (findFile->next != NULL && !strcmp(findFile->next->name, oldname)) {
					type = 1; 	// Am gasit fisierul
					if (exist == 0) {
						saveFile = findFile->next;		// Salvez fisierul
						findFile->next = findFile->next->next;		// Sterg fisierul
					}
				}
				findFile = findFile->next;
			}
		}
	}

	// Caut daca elementul de mutat este director
	if (parent->head_children_dirs != NULL) {

		// Verific daca este primul director din lista
		if (!strcmp(findDir->name, oldname)) {
			type = 2;
			saveDir = findDir;		// Salvez directorul

			if (exist == 0)		// Daca se poate muta il sterg
				parent->head_children_dirs = findDir->next;
		} else {

			// Caut directorul
			while (findDir != NULL) {
				if (findDir->next != NULL && !strcmp(findDir->next->name, oldname)) {
					type = 2;		// Am gasit fisierul
					if (exist == 0) {
						saveDir = findDir->next;		// Salvez directorul
						findDir->next = findDir->next->next;	// Sterg directorul
					}
				}
				findDir = findDir->next;
			}
		}
	}

	if (type == 1) {		// Parametrul primit este fisier
		if (exist == 0) {		// Pot redenumi fisierul

			// Redenumesc fisierul si il adaug in lista
			free(saveFile->name);
			saveFile->name = strdup(newname);
			saveFile->next = NULL;
			findFile = parent->head_children_files;
			if (findFile != NULL) {
				while (findFile->next != NULL) {
					findFile = findFile->next;
				}
				findFile->next = saveFile;
			} else {
				parent->head_children_files = saveFile;
			}
		} else {	// Nu pot redenumi fisierul
			printf("File/Director already exists\n");
		}

	} else if (type == 2) {		// Parametrul primit este director
		if (exist == 0) {		// Pot redenumi directorul

			// Redenumesc directorul si il adaug in lista
			free(saveDir->name);
			saveDir->name = strdup(newname);
			saveDir->next = NULL;
			findDir = parent->head_children_dirs;
			if (findDir != NULL) {
				while (findDir->next != NULL) {
					findDir = findDir->next;
				}
				findDir->next = saveDir;
			} else {
				parent->head_children_dirs = saveDir;
			}
		} else {	// Nu pot redenumi directorul
			printf("File/Director already exists\n");
		}

	} else {	// Nu s-a gasit fisierul/directorul
		printf("File/Director not found\n");
	}
}

// Functia elibereaza memoria alocata comenzii anterioare
void free_commands(Cmd *command)
{
	if (command->counter >= 1) { 	// Verific daca a existat o comanda anterioara
		for (int i = 0; i < command->counter; i++)
			free(command->args[i]);
		free(command->args);
	}
}

// Functia primeste input-ul si il imparte intr-un vector de parametrii
void commands(Cmd *command, char *input)
{
	free_commands(command); 	// Eliberez comanda anterioara
	command->args = malloc(sizeof(char *));
	assert(command->args);

	// Impart input-ul in parametri pentru comanda
	char *tmp;
	tmp = strtok(input, "\n ");
	command->args[0] = strdup(tmp);
	command->counter = 1;
	tmp = strtok(NULL, "\n ");
	while (tmp) {
		command->args = realloc(command->args, (1 + command->counter) * sizeof(char *));
		command->args[command->counter] = strdup(tmp);
		command->counter++;
		tmp = strtok(NULL, "\n ");
	}
}

int main () {
	// Aloc memorie pentru string-ul de input
	char *input = malloc(MAX_INPUT_LINE_SIZE);
	assert(input);

	// Aloc memorie pentru structura de
	// de comenzi si o initializez
	Cmd *command = malloc(sizeof(Cmd));
	assert(command);
	command->counter = 0;

	// Aloc memorie pentru home director si o initializez
	Dir *homeDir = malloc(sizeof(Dir));
	assert(homeDir);
	homeDir->head_children_dirs = NULL;
	homeDir->head_children_files = NULL;
	homeDir->name = strdup("home");
	homeDir->next = NULL;
	homeDir->parent = NULL;
	Dir *toFree = homeDir;	// Salvez o referinta pentru home director

	// Citesc de la tastatura in bucla si execut comenzile aferente input-ului
	do {
		fgets(input, MAX_INPUT_LINE_SIZE, stdin);
		commands(command, input);
		if (!strcmp(command->args[0], "touch")) {
			if (command->counter > 1)
				touch(homeDir, command->args[1]);
			else
				printf("Try 'touch [filename]'.\n");
		} else if (!strcmp(command->args[0], "mkdir")) {
			if (command->counter > 1)
				mkdir(homeDir, command->args[1]);
			else
				printf("Try 'mkdir [directory]'.\n");
		} else if (!strcmp(command->args[0], "ls")) {
			ls(homeDir);
		} else if (!strcmp(command->args[0], "rm")) {
			if (command->counter > 1)
				rm(homeDir, command->args[1]);
			else
				printf("Try 'rm [filename]'.\n");
		} else if (!strcmp(command->args[0], "rmdir")) {
			if (command->counter > 1)
				rmdir(homeDir, command->args[1]);
			else
				printf("Try 'rmdir [directory]'.\n");
		} else if (!strcmp(command->args[0], "cd")) {
			if (command->counter > 1)
				cd(&homeDir, command->args[1]);
			else
				printf("Try 'cd [directory]'.\n");
		} else if (!strcmp(command->args[0], "tree")) {
			tree(homeDir, 0);
		} else if (!strcmp(command->args[0], "pwd")) {
			char *name = pwd(homeDir);
			printf("%s\n", name);
			free(name);
		} else if (!strcmp(command->args[0], "mv")) {
			if (command->counter > 2)
				mv(homeDir, command->args[1], command->args[2]);
			else
				printf("Try 'mv [old name] [new name]'.\n");
		} else if (!strcmp(command->args[0], "stop")) {
			stop(toFree);
		}

	} while (strcmp(command->args[0], "stop"));

	// Eliberez memoria pentru structura comenzile si pentru input
	free_commands(command);
	free(command);
	free(input);
	return 0;
}
