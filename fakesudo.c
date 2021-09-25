#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *file;
    int i;

    // remove argv[0] (aka "sudo")
    --argc;
    memmove(argv, argv + 1, argc * sizeof(char *));

    // let's ignore options
    for (i = 0; i < argc;)
    {
        const char *arg = argv[i];

        if (strcmp(arg, "--") == 0)
            break;

        int shift = 0;

        // sudo 1.8 options with a value
        static const char *longopt_val[] = {"close-from", "preserve-env", "group", "host", "prompt", "role", "type", "other-user", "command-timeout", "user"};
        static const char shortopt_val[] = "CghprtUTu";

        // long options with value
        for (int j = 0; j < sizeof(longopt_val) / sizeof(const char *); ++j)
        {
            size_t len_opt = strlen(longopt_val[j]);
            if (strncmp(arg, longopt_val[j], len_opt) == 0)
            {
                if (arg[len_opt] == '\0') // --user root
                    shift = 2;
                else if (arg[len_opt] == '=') // --user=root
                    shift = 1;

                else
                    continue;
                break;
            }
        }

        // short options with value
        if (shift == 0 && arg[0] == '-')
        {
            const char *o = strchr(shortopt_val, arg[0]);
            if (o != NULL)
            {
                if (o[1] == '\0')
                    shift = 2; // -u root
                else
                    shift = 1; // -uroot
                break;
            }
        }

        // all other options
        if (shift == 0 && arg[0] == '-')
            shift = 1;

        // remove the option
        if (shift != 0)
        {
            argc -= shift;
            if (argc != 0)
                memmove(argv + i, argv + i + shift, (argc - i - shift) * sizeof(char *));
        }

        i += shift + 1;
    }

    if (argc == 0)
    {
        printf("usage: sudo [options] file ...\n");
        exit(2);
    }

    argv[argc] = NULL;
    file = argv[0];

    // search for file in PATH
    if (strchr(file, '/') == NULL)
    {
        char *path = strdup(getenv("PATH"));
        char *tok = strtok(path, ":");
        do
        {
            char *newfile = (char *)malloc(strlen(tok) + 1 + strlen(file) + 1);

            strcpy(newfile, tok);
            strcat(newfile, "/");
            strcat(newfile, file);

            struct stat sb;
            lstat(newfile, &sb);

            if ((sb.st_mode & S_IFMT) == S_IFREG && (sb.st_mode & S_IEXEC) == S_IEXEC)
            {
                file = newfile;
                break;
            }

            free(newfile);

        } while ((tok = strtok(NULL, ":")) != NULL);

        free(path);
    }

    seteuid(0);
    setegid(0);
    setuid(0);
    setgid(0);

    execv(file, argv); // does not return if successful

    perror(file);
    if (file != argv[0])
        free(file);

    return 2;
}
