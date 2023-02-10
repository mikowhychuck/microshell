#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#define CZERWONY "\e[0;31m"
#define ZIELONY "\e[0;32m"
#define MAGNOLIA "\e[0;35m"
#define NIEBIESKI "\e[0;36m"
#define RESET "\e[0m"

void start();
void help();
void pwd();
int cd(char **argument);
int ls(char **argument);
int cp(char **argument);
int nowydir(char **argument);

void help() {
    printf("╔═══════════════════════════════════════╗\n");
    printf("║Microshell - Adam Mikolajczak (481890) ║\n");
    printf("║Komendy:                               ║\n");
    printf("║1. help                                ║\n");
    printf("║2. pwd  :)                             ║\n");
    printf("║3. cd [sciezka]                        ║\n");
    printf("║4. ls [sciezka]                        ║\n");
    printf("║5. cp [plik zrodlowy] [plik docelowy]  ║\n");
    printf("║6. history                             ║\n");
    printf("║7. mkdir [nazwa folderu]               ║\n");
    printf("║8. exit                                ║\n");
    printf("╚═══════════════════════════════════════╝\n");
}

void start(){
    char sciezka[1024];
    char *login = getenv("USER");
    char prompter[] = "$";
    getcwd(sciezka, 1024);
    printf(ZIELONY"[%s] ", sciezka);
    printf(MAGNOLIA"%s ", login);
    printf(CZERWONY"%s "RESET, prompter);
}

void pwd() {
    char cwd[1024];
    getcwd(cwd, 1024);
    printf("%s\n", cwd);
}

int cd(char **argument){
    if(argument[1]==NULL)
        return 1;
    int a;
    if(strcmp(argument[1], "~")==0)
        a = chdir(getenv("HOME"));
    else
        a = chdir(argument[1]);
    if (a == 0)
        return 0;
    else
        return 1;

}

int ls(char **argument){
    char *sciezka;
    if (argument[1] == NULL) {
        sciezka = ".";
    }else
        sciezka = argument[1];
    DIR *cwd = opendir(sciezka);
    if (cwd){
        struct dirent *rzecz = readdir(cwd);
        while (rzecz != NULL) {
            int typ = rzecz->d_type;
            if (typ == 4)
                printf(NIEBIESKI"%s\n"RESET, rzecz->d_name);
            else
                printf("%s\n", rzecz->d_name);
            rzecz = readdir(cwd);
        }
        return 0;
    }else
        return 1;
}

int nowydir(char **argument){
    if(argument[1]==NULL)
        return 1;
    int a = mkdir(argument[1], 0700);
    if(a == 0)
        return 0;
    else
        return 1;
}

int cp(char **argument){
    if(argument[1]==NULL || argument[2]==NULL)
        return 1;
    FILE *plikZrodlowy = fopen(argument[1], "r");
    FILE *plikDocelowy = fopen(argument[2], "w");
    if(plikZrodlowy == NULL || plikDocelowy ==  NULL)
        return 1;
    char bufor[BUFSIZ];
    size_t wczytaneBajty;
    while ((wczytaneBajty = fread(bufor, 1, BUFSIZ, plikZrodlowy)) > 0) {
        fwrite(bufor, 1, wczytaneBajty, plikDocelowy);
    }
    fclose(plikZrodlowy);
    fclose(plikDocelowy);
    return 0;
}

int main() {
    char history[50][256];
    int j = 0;
    while(1){
        start();
        char tekstWpisany[256];
        fgets(tekstWpisany, 256, stdin);
        unsigned long dlugosc = strlen(tekstWpisany);
        tekstWpisany[dlugosc - 1] = '\0';

        strcpy(history[j], tekstWpisany);
        if(j<=50)
            j++;
        else
            j=0;
        //dzielenie polecenia na osobne slowa
        int i = 0;
        char *ptr = strtok(tekstWpisany, " ");
        char *slowa[10];
        while(ptr != NULL){
            slowa[i++] = ptr;
            ptr = strtok(NULL, " ");
        }
        slowa[i] = NULL;

        if(strcmp(slowa[0],"help") == 0){
            help();
        }
        else if(strcmp(slowa[0],"pwd") == 0){
            pwd();
        }
        else if(strcmp(slowa[0],"cd") == 0){
            if(cd(slowa) == 1 )
                printf("Wprowadzona sciezka jest nieprawidlowa\n");
        }
        else if(strcmp(slowa[0], "exit") == 0){
            printf("Do widzenia.\n");
            exit(0);
        }
        else if(strcmp(slowa[0], "ls") == 0){
            if(ls(slowa) == 1){
                printf("Podany katalog nie istnieje");
            }
        }else if(strcmp(slowa[0], "mkdir") == 0){
            if(nowydir(slowa)!=0)
                printf("Wystapil blad\n");
        }else if(strcmp(slowa[0], "cp") == 0) {
            if(cp(slowa)!=0)
                printf("Wystapil blad\n");
        }else if(strcmp(slowa[0], "history") == 0){
            for(int i=0; i<j; i++){
                printf("%d. %s \n", i+1, history[i]);
            }
        }else{
            if (fork() == 0)
                exit(execvp(slowa[0], slowa));
            else{
                int status;
                wait(&status);
                if (status != 0){
                    printf("Nieprawidlowa komenda\n");
                    printf("Kod bledu: %d\n", status);
                }
            }
        }
    }
}
