//
// server.c
//
// Computer Science 50
// Problem Set 6
//

// feature test macro requirements
#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#define _XOPEN_SOURCE_EXTENDED

// limits on an HTTP request's size, based on Apache's
// http://httpd.apache.org/docs/2.2/mod/core.html
#define LimitRequestFields 50
#define LimitRequestFieldSize 4094
#define LimitRequestLine 8190

// number of bytes for buffers
#define BYTES 512

// header files
#include <arpa/inet.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// types
typedef char BYTE;

// prototypes
bool connected(void);
void error(unsigned short code);
void freedir(struct dirent** namelist, int n);
void handler(int signal);
char* htmlspecialchars(const char* s);
char* indexes(const char* path);
void interpret(const char* path, const char* query);
void list(const char* path);
bool load(FILE* file, BYTE** content, size_t* length);
const char* lookup(const char* path);
bool parse(const char* line, char* path, char* query);
const char* reason(unsigned short code);
void redirect(const char* uri);
bool request(char** message, size_t* length);
void respond(int code, const char* headers, const char* body, size_t length);
void start(short port, const char* path);
void stop(void);
void transfer(const char* path, const char* type);
char* urldecode(const char* s);

// server's root
char* root = NULL; //going to point to the string that repreesnts the root (directory path) of the server

// file descriptor for sockets
int cfd = -1, sfd = -1; //reading from network connections (similar in spirit to file i/o) //client-file descripter to webclient //server-file descripter to server

// flag indicating whether control-c has been heard
bool signaled = false;

int main(int argc, char* argv[])
{
    // a global variable defined in errno.h that's "set by system 
    // calls and some library functions [to a nonzero value]
    // in the event of an error to indicate what went wrong"
    errno = 0;

    // default to port 8080f
    int port = 8080;

    // usage
    const char* usage = "Usage: server [-p port] /path/to/root";

    // parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "hp:")) != -1)
    {
        switch (opt)
        {
            // -h for help
            case 'h':
                printf("%s\n", usage);
                return 0;

            // -p port
            case 'p':
                port = atoi(optarg);
                break;
        }
    }

    // ensure port is a non-negative short and path to server's root is specified
    if (port < 0 || port > SHRT_MAX || argv[optind] == NULL || strlen(argv[optind]) == 0) //error checking of port
    {
        // announce usage
        printf("%s\n", usage);

        // return 2 just like bash's builtins
        return 2;
    }

    // start server
    start(port, argv[optind]);

    // listen for SIGINT (aka control-c) //registering an event listener in this case for control c
    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);

    // a message and its length
    char* message = NULL;
    size_t length = 0;

    // path requested
    char* path = NULL;

    // accept connections one at a time
    while (true)
    {
        // free last path, if any
        if (path != NULL)
        {
            free(path); //free memory that might have been allocated by a previous iteration of the loop
            path = NULL;
        }

        // free last message, if any
        if (message != NULL)
        {
            free(message);
            message = NULL;
        }
        length = 0;

        // close last client's socket, if any
        if (cfd != -1)
        {
            close(cfd);
            cfd = -1;
        }

        // check for control-c
        if (signaled)  //checks if been signalled to stop the server
        {
            stop();
        }

        // check whether client has connected
        if (connected()) //cs50 wrote this function for us (returns truee of browser or curl connected to the server)
        {
            // check for request (reads what's inside the virtual envelope)
            if (request(&message, &length)) //cs50 wrote this function for us (similar to getting address of a pointer aka double pointer)
            {
                // extract message's request-line
                // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
                const char* haystack = message;
                const char* needle = strstr(haystack, "\r\n");
                if (needle == NULL)
                {
                    error(500); //read man page
                    continue;
                }
                char line[needle - haystack + 2 + 1];
                strncpy(line, haystack, needle - haystack + 2);
                line[needle - haystack + 2] = '\0';

                // log request-line
                printf("%s", line);

                // parse request-line
                char abs_path[LimitRequestLine + 1]; //abs path: method SP request-target SP HTTP-version CRLF
                char query[LimitRequestLine + 1];
                if (parse(line, abs_path, query))  //parses http request, storing it's "absolute path'" and "query" iside of two arrays that are passed into it by reference                                              
                {                                    //takes the first line and extract absolute path //implement parse! knowing request-target breaks down into
                    // URL-decode absolute-path         //absolute-path [ "?" query ] like q = path as query // figure out how to iterate over line, char to char, looking for absoluate path and query 
                    char* p = urldecode(abs_path);   //method validation then space should be easy to check for using pset2 char checking, http ver is like http 1.1
                    if (p == NULL)
                    {
                        error(500);
                        continue; //jump back to the start of the list
                    }

                    // resolve absolute-path to local path
                    path = malloc(strlen(root) + strlen(p) + 1); //figure out exactly what file was requested on the server itself
                    if (path == NULL)
                    {
                        error(500);
                        continue;
                    }
                    strcpy(path, root);
                    strcat(path, p);
                    free(p);
                   // printf("This is the path before lookup %s", path);

                    // ensure path exists
                    if (access(path, F_OK) == -1)
                    {
                        error(404);
                        continue;
                    }

                    // if path to directory
                    struct stat sb;
                    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) //if user asekd for file or directory
                    { 
                        // redirect from absolute-path to absolute-path/
                        if (abs_path[strlen(abs_path) - 1] != '/')
                        {
                            char uri[strlen(abs_path) + 1 + 1];
                            strcpy(uri, abs_path);
                            strcat(uri, "/");
                            redirect(uri); //forcing user to redirect to foo slash
                            continue;
                        }

                        // use path/index.php or path/index.html, if present, instead of directory's path
                        char* index = indexes(path); //have to implement this
                        if (index != NULL)
                        {
                            free(path);         //we want to show contents of default file, not contents of directory
                            path = index;
                        }

                        // list contents of directory
                        else
                        {
                            list(path);
                            continue;
                        }
                    }

                    // look up MIME type for file at path
                    const char* type = lookup(path);     //is jpeg or what?
                    if (type == NULL)
                    {
                        error(501);
                        continue;
                    }

                    // interpret PHP script at path
                    if (strcasecmp("text/x-php", type) == 0) //is it a php file? //
                    {
                       // printf("This is the PHP type %s", type); 
                         //strcat(path, ".php"); //prob shouldn't be doing this but
                        interpret(path, query); //why would query repeat 8180 times?
                    }

                    // transfer file at path
                    else
                    {
                        transfer(path, type); //transfer file from server to use if not php
                    }
                }
            }
        }
    }
}

/**
 * Checks (without blocking) whether a client has connected to server. 
 * Returns true iff so.
 */
bool connected(void)
{
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr)); //use manpage for memset
    socklen_t cli_len = sizeof(cli_addr);
    cfd = accept(sfd, (struct sockaddr*) &cli_addr, &cli_len); //use manpage for accept
    if (cfd == -1)
    {
        return false;
    }
    return true;
}

/**
 * Responds to client with specified status code.
 */
void error(unsigned short code)
{
    // determine code's reason-phrase
    const char* phrase = reason(code);
    if (phrase == NULL)
    {
        return;
    }

    // template for response's content
    char* template = "<html><head><title>%i %s</title></head><body><h1>%i %s</h1></body></html>";

    // render template
    char body[(strlen(template) - 2 - ((int) log10(code) + 1) - 2 + strlen(phrase)) * 2 + 1];
    int length = sprintf(body, template, code, phrase, code, phrase);
    if (length < 0)
    {
        body[0] = '\0';
        length = 0;
    }

    // respond with error
    char* headers = "Content-Type: text/html\r\n";
    respond(code, headers, body, length);
}

/**
 * Frees memory allocated by scandir.
 */
void freedir(struct dirent** namelist, int n)
{
    if (namelist != NULL)
    {
        for (int i = 0; i < n; i++)
        {
            free(namelist[i]);
        }
        free(namelist);
    }
}
 
/**
 * Handles signals.
 */
void handler(int signal)
{
    // control-c
    if (signal == SIGINT)
    {
        signaled = true;
    }
}

/**
 * Escapes string for HTML. Returns dynamically allocated memory for escaped
 * string that must be deallocated by caller.
 */
char* htmlspecialchars(const char* s)
{
    // ensure s is not NULL
    if (s == NULL)
    {
        return NULL;
    }

    // allocate enough space for an unescaped copy of s
    char* t = malloc(strlen(s) + 1);
    if (t == NULL)
    {
        return NULL;
    }
    t[0] = '\0';

    // iterate over characters in s, escaping as needed
    for (int i = 0, old = strlen(s), new = old; i < old; i++)
    {
        // escape &
        if (s[i] == '&')
        {
            const char* entity = "&amp;";
            new += strlen(entity);
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, entity);
        }

        // escape "
        else if (s[i] == '"')
        {
            const char* entity = "&quot;";
            new += strlen(entity);
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, entity);
        }

        // escape '
        else if (s[i] == '\'')
        {
            const char* entity = "&#039;";
            new += strlen(entity);
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, entity);
        }

        // escape <
        else if (s[i] == '<')
        {
            const char* entity = "&lt;";
            new += strlen(entity);
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, entity);
        }

        // escape >
        else if (s[i] == '>')
        {
            const char* entity = "&gt;";
            new += strlen(entity);
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, entity);
        }

        // don't escape
        else
        {
            strncat(t, s + i, 1);
        }
    }

    // escaped string
    return t;
}

/**
 * Checks, in order, whether index.php or index.html exists inside of path.
 * Returns path to first match if so, else NULL.
 */
char* indexes(const char* path) //decide whether or not there exists a file called index.php or index.html inside the directory of the path you are passed //we not showing directory contents but index.html or index.php by default //use hints in the source code how you can actually check if file exists in certain directory.
{
    // TODO append index.html or php?
    // GOING TO DO THE fiLE POINTER THING BECAUSE the path checked happened before this function
    // Path is goign to get cleared out and replaced with what's returned
    char* thePHP = "/index.php"; //this slashed solved redirection problems
    char* theHTML = "/index.html"; //slashes solved redirection problems
    char* tempPHP = malloc(sizeof(char) * (LimitRequestLine + 1));
    char* tempHTML = malloc(sizeof(char) * (LimitRequestLine + 1));
    
    //printf("I made it to indexes");
    
    strcpy(tempPHP, path);
    strcat(tempPHP, thePHP);
    FILE* file = fopen(tempPHP, "r");
    if (file != NULL)
    {
       // printf("I made it index.php\n");
        fclose(file);
        return tempPHP;
    }
    
    strcpy(tempHTML, path);
    strcat(tempHTML, theHTML);
    FILE* file2 = fopen(tempHTML, "r");
    if (file2 != NULL)
    {
       //printf("I made it index.html\n");
       // printf("I should be here %s\n", tempHTML);
       free(tempPHP);
       fclose(file2);
        return tempHTML;
    }
    
    //printf("returning NULL from indexes\n");
    return NULL;
}

/**
 * Interprets PHP file at path using query string.
 */
void interpret(const char* path, const char* query) //pass the contents to PHP's interpeter (php-cgi)
{
    // ensure path is readable
    if (access(path, R_OK) == -1)
    {
        error(403);
        return;
    }

    // open pipe to PHP interpreter
    char* format = "QUERY_STRING=\"%s\" REDIRECT_STATUS=200 SCRIPT_FILENAME=\"%s\" php-cgi"; //query-string is an environement variable
    char command[strlen(format) + (strlen(path) - 2) + (strlen(query) - 2) + 1];
    if (sprintf(command, format, query, path) < 0)
    {
         //printf("sprintf(command, format, query, path) %s", path);
        error(500);
        return;
    }
    
    FILE* file = popen(command, "r"); //open's a "pipe" to a process (php-cgi), providing a FILE pointer to read the process's standard output (as though its a file)
    if (file == NULL)
    {
         //printf("file == NULL %s", path);
        error(500);
        return;
    }

    // load interpreter's content
    char* content;
    size_t length;
   /* 
    long filelen;
    fseek(file, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(file);             // Get the current byte offset in the file
    rewind(file);    
    
    content = malloc(sizeof(char) * (filelen + 1));
    //fscanf(file, "%s", buffer);
    fread(&content, filelen, 1, file);
    //
    printf("This is &buffer:: %s\n", content );
    */
    
    if (load(file, &content, &length) == false) //read interprets' output into memory
    {
        // printf("load(file, &content, &length) %s", path);
        error(500);
        return;
    }

    // close pipe
    pclose(file);

    // subtract php-cgi's headers from content's length to get body's length
    char* haystack = content;
    char* needle = strstr(haystack, "\r\n\r\n");
        printf("needle == NULL %s \n", haystack);
        printf("needle == NULL %s \n", needle);
    if (needle == NULL)
    {
        free(content);
        error(500);
        return;
    }

    // extract headers
    char headers[needle + 2 - haystack + 1];
    strncpy(headers, content, needle + 2 - haystack);
    headers[needle + 2 - haystack] = '\0';

    // respond with interpreter's content
    respond(200, headers, needle + 4, length - (needle - haystack + 4)); //respond to the brower with (dynamically generated) output

    // free interpreter's content
    free(content);
}

/**
 * Responds to client with directory listing of path.
 */
void list(const char* path) //generate HTML using C
{
    // ensure path is readable and executable
    //printf("This is the path fucking me over: %s", path);
    if (access(path, R_OK | X_OK) == -1)
    {
        error(403);
        return;
    }

    // open directory
    DIR* dir = opendir(path);
    if (dir == NULL)
    {
        return;
    }

    // buffer for list items
    char* list = malloc(1);
    list[0] = '\0';

    // iterate over directory entries
    struct dirent** namelist = NULL;
    int n = scandir(path, &namelist, NULL, alphasort);
    for (int i = 0; i < n; i++)
    {
        // omit . from list
        if (strcmp(namelist[i]->d_name, ".") == 0)
        {
            continue;
        }

        // escape entry's name
        char* name = htmlspecialchars(namelist[i]->d_name);
        if (name == NULL)
        {
            free(list);
            freedir(namelist, n);
            error(500);
            return;
        }

        // append list item to buffer
        char* template = "<li><a href=\"%s\">%s</a></li>";
        list = realloc(list, strlen(list) + strlen(template) - 2 + strlen(name) - 2 + strlen(name) + 1);
        if (list == NULL)
        {
            free(name);
            freedir(namelist, n);
            error(500);
            return;
        }
        if (sprintf(list + strlen(list), template, name, name) < 0)
        {
            free(name);
            freedir(namelist, n);
            free(list);
            error(500);
            return;
        }

        // free escaped name
        free(name);
    }

    // free memory allocated by scandir
    freedir(namelist, n);

    // prepare response
    const char* relative = path + strlen(root);
    char* template = "<html><head><title>%s</title></head><body><h1>%s</h1><ul>%s</ul></body></html>";
    char body[strlen(template) - 2 + strlen(relative) - 2 + strlen(relative) - 2 + strlen(list) + 1];
    int length = sprintf(body, template, relative, relative, list);
    if (length < 0)
    {
        free(list);
        closedir(dir);
        error(500);
        return;
    }

    // free buffer
    free(list);

    // close directory
    closedir(dir);

    // respond with list
    char* headers = "Content-Type: text/html\r\n";
    respond(200, headers, body, length);
}

/**
 * Loads a file into memory dynamically allocated on heap.
 * Stores address thereof in *content and length thereof in *length.
 */
bool load(FILE* file, BYTE** content, size_t* length) //maits either used to load the output from php interepret; reads bytes of output from php interpet into big array; also used to load into memory all bytes from a file that a user has requested (use tricks for pset4)
{

    // TODO: content is the address of the bytes;
    //printf("I made it to load\n"); //content is a string, it's size has yet to be declared
    //int filelen = 1;
    
    // dataTYpe content;
    //char* buffer = malloc(sizeof(char) * 20);
   // *content = NULL; //this doesn't guarantee NULL
    /*if (*content != NULL)
    {
        free(*content);
        printf("Made it here \n");
        *content = NULL;
    }*/
    //*content = NULL;
    *length = 0; //this doesn't guarantee NULL
    

    /*if (length != 0)
    {
        free(length);
        printf("Made it here \n");
        length = 0;
    } */
    //BYTE* buffer = malloc(sizeof(BYTE) * BYTES);
    
    //fseek(file, 0, SEEK_END);          // Jump to the end of the file
    //filelen = (int) ftell(file);             // Get the current byte offset in the file
    //rewind(file);    
    //char byteArray[filelen];
    //memset(content, 0, strlen(*content));
    
   /* if (isalpha(content[0]))
    {
        printf(" should be alpha %lu \n", strlen(*content));
        return 1;
    } */

   /* if (content[0])
    {
      //strcpy(abs_path, space);
      memset(content, 0, sizeof(*content));
    //printf("This is the abs_path %s \n", abs_path[8]);
    } 

    memset(length, 0, sizeof(*length));*/
    //memset(content, 0, strlen(*content));
    
    //BYTE* countBuffer;//[filelen + 1];//[filelen + 1];
   // while ( fread(&countBuffer, sizeof(BYTE), 1, file) == 1 )
   // {
   //     filelen++;
   // }
   // rewind(file);
    //BYTE buffer[filelen];
    
    //*freecontent* already clears this stuff out from the transfer function so I shouldn't have to do it//
    
     // BYTE* nextBuffer = malloc(sizeof(BYTES) * (filelen + 1));
     BYTE* nextBuffer = NULL;
     nextBuffer = malloc(sizeof(BYTE)); //THis is why people kepts reallocing inside the buffer, the pipe read means you don't know how much you need
     
    // BYTE* jpegBuffer = malloc(sizeof(char) * (filelen + 1));
    
    // char* nextBuffer = NULL; //this doesn't guarantee NULL, it really doesn't. it just creates a new pointer, the old one never went anywhere;
    //nextBuffer = NULL;
         //nextBuffer = realloc(nextBuffer, (filelen + 1));

    // nextBuffer = malloc(sizeof(char) * (filelen + 1)); //didint know i could just realloc but this makes you fel as those the old pointer has fresh memory, it doesn't
    // nextBuffer = memset(nextBuffer, 0, strlen(nextBuffer));
    
    //*content = realloc(*content, sizeof(char) * (filelen + 1));
    
   /* bool PHP = false;
    if (strstr(*file, "text/x-php") != NULL )
    {
        PHP = true;
    }*/
   
    //printf("This is the filelen:: %ld\n", filelen );
    //fscanf(file, "%s", buffer);
     // printf("")
    //fscanf(file, "%s", buffer)
    //fread(&buffer, sizeof(filelen), 1, file) == 1 
    int i = 0;                                       //secondly it seems that fread without a while statement just fails
    int fileChar = 0;
    //bool isJpeg = false;
    //printf("This is string size of nextbuffer:: %ld \n", sizeof(nextBuffer));
     //printf("This is the filelen:: %d \n", filelen);
     
    // while( fread(&phpChar, sizeof(unsigned int), 1, file) == 1 ) //try cfd next

    while ((fileChar = fgetc(file)) != EOF) //my 2 day problem could be stemming from that the fact that buffer doesn't need to be malloced!!!!! Interesting how it can't even handle 2 blocks of memory, have to use 1
    {
        //if (fileChar == EOF)
        //break;
        /*char* quote = strchr(buffer, 39);
        if (*quote)
        quote = NULL;*/
        //printf("This is a comma %c \n", *quote);
        //&content[0] == 255 && &content[1] == 216 && &content[2] == 255 && jpegBuff[3] == 224 
          //      *length += 1;
        /*content = realloc (*content, *length);
        //memcpy(&content, &buffer, 1);*/
      /*  if ( i > 4)
        if (i % 512 == 0 ||i % 512 == 1 ||i % 512 == 2 ||i % 512 == 3)
        {
            if (fileChar = 255) &
        } */
        
       /* if (isJpeg == false) WORKING
        {
            if (strcmp(&fileChar, "\377") == 0 )
            {
                isJpeg = true;
                rewind(file);
            }
            
            //printf("This is the buffer size the first time %d \n", sizeof(buffer));
            nextBuffer[i] = fileChar; //ahh ha! you do need to malloc the buffer pointer but simply use brackets because the other way was confusing
                
                   
        } else
        {
            printf("Im filling up jpeg array");
            jpegBuffer[i] = fileChar;
        }
            */ //WORKING
            
            nextBuffer = realloc(nextBuffer, ((i + 1) * sizeof(BYTE)) + sizeof(BYTE) ); //maybe not be able to create a circular reference like this
            nextBuffer[i] = (char) fileChar; //
            i++; // 0, 1, 2, 3,4, 5 so if i is 6 then  ther are 6 spots but only up to 5 in index
            
            
               // memcpy(&nextBuffer[i], &fileChar, sizeof(BYTE));
                //printf("This is nextBUffer %c \n", nextBuffer[i]);
                     //printf("This is string length of nextbuffer:: %ld \n", strlen(nextBuffer));
         /*else
        {

            //strcpy(&buffer[i], );
            for (int k = 0; k < countAgain; k++ )
            {
                //if end of file is reached then break
                //strcat(&nextBuffer[i], buffer[k]);
                if (*buffer[k] == EOF)
                break;
                
                //strncat(nextBuffer, buffer, strlen(buffer));
                i++;
            } 
        }*/
            // memcpy(&nextBuffer[i], &buffer, 1); //Buffer as an array is a single pointer that pionts to the beg. of a continguous block in meory, then fills out the neighboring slots of memory with other reads
            // if (i == 20)
            // break;
    }
    
    //this biggest lesson leared here is that I may need to different reading strategies due to the multiple diff. file types being read
    
    //nextBuffer[strlen(nextBuffer) - 3] = '\0';
    
    //printf("made it to the loop at least %s\n", nextBuffer);
    //printf("Give it a try %s \n", nextBuffer); // this is the last visible character
    //printf("This is size of nextBuffer in Bytes:: %ld \n", sizeof(nextBuffer));
    //printf("THis is the i: %d \n", i);

    // for (int j = 0; j < strlen(size); j++ )
   
    nextBuffer[i] = '\0';
    *content = nextBuffer;
    //*content = malloc(strlen(nextBuffer) + 1); //somehow, we went from 170 to 169 //freeing content
    //memcpy(*content, nextBuffer, strlen(nextBuffer) + 1);
    //*content[strlen(nextBuffer)];
   // printf("This is content %s \n", *content); //This has extra character like 52F
    //free(nextBuffer);
    
   // *content = nextBuffer;
   // memcpy(*content, nextBuffer, strlen(nextBuffer) + 1);
    //printf("This is string length of content:: %ld \n", strlen(*content));
    // printf("This is the content address:: %s \n", content);
      //    printf("This is the nextBuffer address:: %s \n", &nextBuffer);
    //*content[strlen(nextBuffer)] = '\0';
    //*content = nextBuffer; //it tries to free the memory after displaying the page
    //memcpy(*content, nextBuffer, strlen(nextBuffer));

    //memcpy(&nextBuffer, &buffer, 1); //this couldnt read in more than 2 bytes, idk why
    //printf("This is &buffer:: %s\n", nextBuffer );
    
    //strcpy(*content, nextBuffer);
    //free(buffer);
   // printf("This is content:: %s\n", *content);
    //strcpy(*content, buffer); //segmentation fault happens here
    //strcpy(*content, &buffer[0]);// = buffer[0];
    // printf("This is *content:: %s\n", *content );

    //content = malloc(sizeof(char) * 20);
    //int i = 0;
    //strcpy(content[i], &buffer[i]);
    *length = i;//strlen(*content) + 1; //THIS MIGHT BE A PROBLEM this was a problem, use i because you can't do strlen on a binary file,, being resourceful and looking at common mistakes was helpful as well.
   // printf("This is length of nextBuffer:: %lu \n", *length );
    //printf("This is length of content:: %lu \n", strlen(*content) );
    
    if ( *length == i) 
    {
        return true;
    }
    
    //printf("Something went wrong with the loading function\n");   
    return false;

}

/**
 * Returns MIME type for supported extensions, else NULL.
 */
const char* lookup(const char* path)  // look up MIME aka content type of that file text/html or image/jpeg //find dot and file extension
{
    // TODO: 
     
    char* forLookUp = malloc(sizeof(char) * strlen(path));
    
    strcpy(forLookUp, path);
    
    char* MIME = NULL;
    if (MIME != NULL)
    {
        free(MIME);
        MIME = NULL;
    }
    MIME = malloc(6 * sizeof(char)); 
    
    char period = 46;
    MIME = strrchr(forLookUp, period);
    
     //printf("this is MIME length, should be 5 for html: %d\n", extLength);
    //upperMIME[0] = MIME[0];
    //char* upperMIME = malloc(6 * sizeof(char)); 
    //strcpy(&upperMIME[0], &MIME[0]);
    
    char temp;
    int extLength = strlen(MIME);
    for (int i = 1; i < extLength; i++)
    {
        temp = toupper(MIME[i]);
        MIME[i] = temp; //make everything capital
    }
    
    MIME[strlen(MIME) + 1] = '\0';
    //upperMIME[strlen(upperMIME) + 1] = '\0';
    
    //convert to uppercase
    if (strcmp(MIME, ".CSS") == 0 /*|| (strcmp(upperMIME, ".CSS") == 0)*/ )
    {
       //printf("It's a CSS FILE \n"); //not enough memory to print?!!
        //free(upperMIME);
        //memset(MIME, 0, strlen(MIME));
        free(forLookUp);
         //memset(MIME, NULL, extLength + 1);
        return "text/css";
    } else if (strcmp(MIME, ".HTML") == 0 /* || (strcmp(upperMIME, ".HTML") == 0) */)
    {
        //printf("It's a HTML FILE \n");
       // printf("It's lookup in html, but I could of done a data structure");
       
       // memset(upperMIME, 0, strlen(upperMIME));
        //memset(MIME, 0, strlen(MIME));
        //free(upperMIME);
        //free(&MIME[0]);
        //free(MIME);
        //printf("THis is the path %s", forLookUp);
        free(forLookUp);
        //memset(MIME, NULL, extLength + 1);
        return "text/html";
    } else if (strcmp(MIME, ".GIF") == 0 /*|| (strcmp(upperMIME, "GIF") == 0) */)
    {
        //printf("It's a GIF FILE \n");
        
        free(forLookUp);
        //memset(MIME, NULL, extLength + 1);
        return "image/gif";
    } else if (strcmp(MIME, ".ICO") == 0 /* || (strcmp(upperMIME, ".ICO") == 0) */)
    {
        //printf("It's a ICO FILE \n");
        
        free(forLookUp);
        //memset(MIME, NULL, extLength + 1);
        return "image/x-icon";
    } else if (strcmp(MIME, ".JPG") == 0 /* || (strcmp(upperMIME, ".JPG") == 0) */)
    {
        //printf("It's a JPG FILE \n");
        //free(upperMIME);
        //free(&MIME[0]);
        free(forLookUp);
        //memset(MIME, NULL, extLength + 1);
        return "image/jpeg";
    } else if (strcmp(MIME, ".JS") == 0 /* || (strcmp(upperMIME, ".JS") == 0)*/ )
    {
        //printf("It's a JS FILE \n");
        
        free(forLookUp);
        //memset(MIME, NULL, extLength + 1);
        return "text/javascript";
    } else if (strcmp(MIME, ".PHP") == 0 /* || ( strcmp(upperMIME, ".PHP") == 0)*/ )
    {
       // MIME[0] = '\0'; //can't free in another scope?
       // upperMIME[0] = '\0'; 
        printf("It's a PHP FILE \n");
        free(forLookUp);
        //memset(MIME, NULL, extLength + 1);
        return "text/x-php";
    } else
    {
        if ( strcmp(MIME, ".PNG") == 0 )
        {
            free(forLookUp);
            //memset(MIME, NULL, extLength + 1);
            return "image/png";
        }
        
        /* printf("Made it to function without period : ) ");
        //free(forLookUp);
        //const char*  theIndex = malloc(sizeof(char) * 6);
       // theIndex = "blah";
       // path = path + theIndex;
        strcat(forLookUp, "/index.html"); //if this doesn't work we count try load(file, content, length)
        redirect(forLookUp);
        return NULL; */
    }
    
    //  printf("ASCII of period should be 46 %c\n", period);
    // printf("The path inside lookup() %s\n", path);
    // printf("print the MIME type %s\n", MIME);
    //printf("Lookup went horribly wrong (?)\n");
    //free(upperMIME);
    free(forLookUp);
    return NULL;
    
}

/**
 * Parses a request-line, storing its absolute-path maat abs_path 
 * and its query string at query, both of which are assumed
 * to be at least of length LimitRequestLine + 1.
 */
bool parse(const char* line, char* abs_path, char* query) //iterate over line char to char, to make sure it matches that formal definition of that first request line as per the spec// you need to load into abs path that substring //optionally after that path is a question mark and a query string
{
           
 
    // abs_path and query
    // TODO: abs_pth and query are just character limits so line is what I need to read, I can print that out at the next printf
    
    //char* abs_pathProxy = [LimitRequestLine + 1]; //abs path: method SP request-target SP HTTP-version CRLF
    //char* queryProxy[LimitRequestLine + 1];
    char* lineProx = malloc(sizeof(char) * strlen(line) );
    strcpy(lineProx, line);
                
    char* slash = malloc(sizeof(char));
    strcpy(&slash[0], "/");
    
    char* lineSlash = malloc(sizeof(char));
    lineSlash[0] = line[4];
    
    char* space = malloc(sizeof(char));
    strcpy(&space[0], " ");
    
    char* quote = malloc(sizeof(char));
    quote[0] = '"';
   
    
    // if (strlen(abs_path) > 1) //might have to do a cmpr to space then free()
    //strcpy(abs_path, space); //clears out the string in the request if called previously

    /*if (abs_path[0])
    {
        printf("This is the abs_path %s \n", abs_path);
      //strcpy(abs_path, space);
        //memset(abs_path, 0, strlen(abs_path)); //This clears out the html that's held before requesting a jpg but it throws an error
        //abs_path = (char *) realloc(abs_path, (LimitRequestLine + 1) * sizeof(char));
        printf("Didin't throw an error!!!");
    //printf("This is the abs_path %s \n", abs_path[8]);
    } */
   
    int oldPathSize = (int) strlen(abs_path);
    int newPathSize = 0;
    
    int oldQuerySize = (int) strlen(query);
    int newQuerySize = 0;
    
        //printf("Didn't think id be here so oon;");
        //abs_path = NULL;// HUGE LESSON IS LEARNING THE COMBO OF NULL THEN REALLOC
        //**abs_path = realloc(abs_path, LimitRequestLine + 1);
     //   free(&abs_path);
        
        //query = NULL;
        //query = realloc(query, LimitRequestLine + 1);
       // free(&query);
    
    //  if (strlen(query) > 1) //clears out the string in the request if called previously
    /*if (query[0]) 
    {
        printf("This is the query %s \n", query);
        memset(query, 0,strlen(query)); //clears out the string in the request if called previously
    } */
   // query = space;
    
    
    int lineLength = (int) strlen(lineProx);
    
    if ( strstr(lineProx, "GET ") == NULL || (strstr(lineProx, "abcGET") != NULL) || (strstr(lineProx, " GET ") != NULL) ) // strcmp(&line[0], "G") != 0  //does the beginning start off alright?
    {
        //printf("The GET didn't work %d\n", strcmp(&line[0], "G"));
        //printf("The GET didnt work\n");
        error(405);
        return false; 

    } else if ( strncmp(&lineSlash[0], &slash[0], 1) != 0 ) //does the middle start-off alright?
    {
        //printf("The slash in the middle of the request line is out of place\n");
        error(501); //slash and quotes and space not allowed
        return false; //? directly after slash is allowed and a query directly after question mark is allowed
        
    } else if (!strstr(lineProx, " HTTP/1.1")) //does the end start-off alright?
    {
       // printf("Line ends badly, The http version is wrong %s\n", lineProx);
        error(505);
        return false;
        
    } else //examine all the stuff in the middle
    {
        for ( int i = 4; i < lineLength - 10; i++ )
        {
            if (strncmp(&lineProx[i], "?", 1) == 0)
            {
                    i++;
                for (int j = i; j < lineLength - 11; j++) //I don't understand why this is 11 and not 8 or 9, well the line is 36 long so thats an index of 35
                {
                    strncpy(&query[j - i], &lineProx[j], 1); //Even if ? is present, [0] should be " " if nothing comes after the ?
                    newQuerySize++;
                    
                    if ( lineProx[j] == quote[0] || lineProx[i] == space[0] ) //line[i] == space[0] || || line[i] == slash[0]
                    {
                       // printf("The request-target had a mistake %c\n", line[j]);//Is the space directly after the slash alright? yes, 
                       //printf("Something went wrong directly after the question mark (?)\n");
                        error(400);
                        return false;
                    }
                } 
                
                break;
                
            } else if ((strncmp(&lineProx[i], &space[0], 1) == 0) & (strncmp(&lineProx[i + 1], "H", 1) == 0)) //this is the space right before HTTP/1.1
            {
                break;
                
            } else 
            {
                //the last line might longer than the new one so the old one remains
                strncpy(&abs_path[i - 4], &lineProx[i], 1);   
                newPathSize++;
            }
        }
    
        while (oldPathSize > newPathSize) //make sure old memory is not interfering with new memory
        {
            abs_path[oldPathSize - 1] = 0; //try 0 next if \0 doesn't work
            oldPathSize--;
        }
        
        while (oldQuerySize > newQuerySize) //make sure old memory is not interfering with new memory
        {
            query[oldQuerySize - 1] = 0; //try 0 next if \0 doesn't work
            oldQuerySize--;
        }
       // int queryLength = strlen(query);
        
        //printf("This is the query Length:%s\n", query);
        //printf("This is a abs_Path:%s\n", abs_path);
        
        //q starts the query? its about the query after the page request though
        //printf("This is a query: %s\n", query);

        //thnking to hardcode public just as its named in apache50 start public
        //You'll hit indexes if you ask for a public directory like test without a file! Yes!!!

        //free(quote); //this one commented out, space still broke
       //free(space); //or this one but commenting out this , still quot broke
        free(lineSlash);
        free(slash); //either this or lineProx broke
        
        //   printf("Inside load: This is directly after freeing space.\n");
       // printf("This is the line %s\n", lineProx);
        //free(lineProx); //This iserroring out?!
        //printf("Request abs_path: %s\nquery: %s \n", abs_path, query);
        return true;
        //slash and quotes and space not allowed in request-target
        //exclusively between line 4 and the line space right before the http version, filter out what I dont want
    }
    
    /*if ( strcmp(&line[0], "G") != 0 || strcmp(&line[1], "E") != 0 || strcmp(&line[2], "T") != 0   )
    {
        error(405);
        return false;
        
    } else if ( strcmp(&line[3], " ") != 0 || strcmp(&line[4], "/") != 0 )
    {
        error(501);
        return false;
    }*/
    
    //strchr, strrchr, strchrnul - locate character in string
    //The strncpy() function is similar, except that at most n bytes of src are copied. *Warning\*: If there is no null byte 
    //   ...among the first n bytes of src, the string placed in dest will not be null-terminated.
    //strstr() function finds the first occurrence of the substring needle in the string haystack. The terminating null bytes (\0) are not compared.
    
    
    //printf("The request line %s\n", line);
    //printf("GET space / optional ? aftewards not allowing "); //make sure line is consisten with these rulees
   // printf("Parsing didn't work at all\n");
    error(501); //throws error but doesn't stop listening
    return false;
}

/**
 * Returns status code's reason phrase.
 *
 * http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6
 * https://tools.ietf.org/html/rfc2324
 */
const char* reason(unsigned short code)
{
    switch (code)
    {
        case 200: return "OK";
        case 301: return "Moved Permanently";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 414: return "Request-URI Too Long";
        case 418: return "I'm a teapot";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 505: return "HTTP Version Not Supported";
        default: return NULL;
    }
}

/**
 * Redirects client to uri.
 */
void redirect(const char* uri) //redirects client to another location (URL) by sending a location header
{
    char* template = "Location: %s\r\n";
    char headers[strlen(template) - 2 + strlen(uri) + 1];
    if (sprintf(headers, template, uri) < 0)
    {
        error(500);
        return;
    }
    respond(301, headers, NULL, 0);
}

/**
 * Reads (without blocking) an HTTP request's headers into memory dynamically allocated on heap.
 * Stores address thereof in *message and length thereof in *length.
 */
bool request(char** message, size_t* length) //iteratively reads bytes from the client, one buffer's worth at a time, calling realloc as needed to store entire message
{                                           //try to understand this function as much as possible if only for practice
    // ensure socket is open                //ultimately keeps reading bytes form client until it encounters \r\n\r\n (aka CRLF CRLF), which, according to HTTP'SS spec, marks the end of a request's headers
    if (cfd == -1)
    {
        return false;
    }

    // initialize message and its length
    *message = NULL;
    *length = 0;

    // read message 
    while (*length < LimitRequestLine + LimitRequestFields * LimitRequestFieldSize + 4)
    {
        // read from socket
        BYTE buffer[BYTES];
        ssize_t bytes = read(cfd, buffer, BYTES); //like fread except it reads from "file descripter" i.e. an int instead of from a FILE pointer
        if (bytes < 0)  
        {
            if (*message != NULL)
            {
                free(*message);
                *message = NULL;
            }
            *length = 0;
            break;
        }

        // append bytes to message 
        *message = realloc(*message, *length + bytes + 1);
        if (*message == NULL)
        {
            *length = 0;
            break;
        }
        memcpy(*message + *length, buffer, bytes);
        *length += bytes;

        // null-terminate message thus far
        *(*message + *length) = '\0';

        // search for CRLF CRLF
        int offset = (*length - bytes < 3) ? *length - bytes : 3;
        char* haystack = *message + *length - bytes - offset;
        char* needle = strstr(haystack, "\r\n\r\n");
        if (needle != NULL)
        {
            // trim to one CRLF and null-terminate
            *length = needle - *message + 2;
            *message = realloc(*message, *length + 1);
            if (*message == NULL)
            {
                break;
            }
            *(*message + *length) = '\0';

            // ensure request-line is no longer than LimitRequestLine
            haystack = *message;
            needle = strstr(haystack, "\r\n");
            if (needle == NULL || (needle - haystack + 2) > LimitRequestLine)
            {
                break;
            }

            // count fields in message
            int fields = 0;
            haystack = needle + 2;
            while (*haystack != '\0')
            {
                // look for CRLF
                needle = strstr(haystack, "\r\n");
                if (needle == NULL)
                {
                    break;
                }

                // ensure field is no longer than LimitRequestFieldSize
                if (needle - haystack + 2 > LimitRequestFieldSize)
                {
                    break;
                }

                // look beyond CRLF
                haystack = needle + 2;
            }

            // if we didn't get to end of message, we must have erred
            if (*haystack != '\0')
            {
                break;
            }

            // ensure message has no more than LimitRequestFields
            if (fields > LimitRequestFields)
            {
                break;
            }

            // valid
            return true;
        }
    }

    // invalid
    if (*message != NULL)
    {
        free(*message);
    }
    *message = NULL;
    *length = 0;
    return false;
}

/**
 * Responds to a client with status code, headers, and body of specified length.
 */
void respond(int code, const char* headers, const char* body, size_t length)
{
    // determine Status-Line's phrase
    // http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6.1
    const char* phrase = reason(code);
    if (phrase == NULL)
    {
        return;
    }

    // respond with Status-Line
    if (dprintf(cfd, "HTTP/1.1 %i %s\r\n", code, phrase) < 0) //write to a "file descripter" instead of a FILE*
    {
        return;
    }

    // respond with headers
    if (dprintf(cfd, "%s", headers) < 0)
    {
        return;
    }

    // respond with CRLF
    if (dprintf(cfd, "\r\n") < 0)
    {
        return;
    }

    // respond with body
    if (write(cfd, body, length) == -1)
    {
        return;
    }

    // log response line
    if (code == 200)
    {
        // green
        printf("\033[32m");
    }
    else
    {
        // red
        printf("\033[33m");
    }
    printf("HTTP/1.1 %i %s", code, phrase);
    printf("\033[39m\n");
}

/**
 * Starts server on specified port rooted at path.
 */
void start(short port, const char* path) ///function that configure the server to listen for connections on a particular TCP port
{
    // path to server's root
    root = realpath(path, NULL);
    if (root == NULL)
    {
        stop();
    }

    // ensure root is executable
    if (access(root, X_OK) == -1)
    {
        stop();
    }

    // announce root
    printf("\033[33m");
    printf("Using %s for server's root", root);
    printf("\033[39m\n");

    // create a socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        stop();
    }

    // allow reuse of address (to avoid "Address already in use")
    int optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // assign name to socket
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    {
        printf("\033[33m");
        printf("Port %i already in use", port);
        printf("\033[39m\n");
        stop();
    }

    // listen for connections
    if (listen(sfd, SOMAXCONN) == -1)
    {
        stop();
    }

    // announce port in use
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sfd, (struct sockaddr*) &addr, &addrlen) == -1)
    {
        stop();
    }
    printf("\033[33m");
    printf("Listening on port %i", ntohs(addr.sin_port));
    printf("\033[39m\n");
}

/**
 * Stop server, deallocating any resources.
 */
void stop(void)
{
    // preserve errno across this function's library calls
    int errsv = errno;

    // announce stop
    printf("\033[33m");
    printf("Stopping server\n");
    printf("\033[39m");

    // free root, which was allocated by realpath
    if (root != NULL)
    {
        free(root);
    }

    // close server socket
    if (sfd != -1)
    {
        close(sfd);
    }

    // stop server
    exit(errsv);
}

/**
 * Transfers file at path with specified type to client.
 */
void transfer(const char* path, const char* type) //handles static content (JPEG's) (but interpret handles dynamic content generated by PHP scripts)
{
    // ensure path is readable
    if (access(path, R_OK) == -1)
    {
        error(403);
        return;
    }

    // open file
    FILE* file = fopen(path, "r");
    if (file == NULL)
    {
        error(500);
        return;
    }

    // load file's content
    BYTE* content;
    size_t length;
    if (load(file, &content, &length) == false) //read some frile from disk
    {
        error(500);
        return;
    }

    //printf("Inside Transfer:  is the file pointer fucking me over: %s", *file);
    // close file

    fclose(file);

    // prepare response
    char* template = "Content-Type: %s\r\n";
    char headers[strlen(template) - 2 + strlen(type) + 1];
    if (sprintf(headers, template, type) < 0)
    {
        error(500);
        return;
    }

    // respond with file's content
    respond(200, headers, content, length);

   
    // free file's content
    free(content);
    
}

/**
 * URL-decodes string, returning dynamically allocated memory for decoded string
 * that must be deallocated by caller.
 */
char* urldecode(const char* s) //converting special character like %20 back to their original values
{
    // check whether s is NULL
    if (s == NULL)
    {
        return NULL;
    }

    // allocate enough (zeroed) memory for an undecoded copy of s
    char* t = calloc(strlen(s) + 1, 1);
    if (t == NULL)
    {
        return NULL;
    }
    
    // iterate over characters in s, decoding percent-encoded octets, per
    // https://www.ietf.org/rfc/rfc3986.txt
    for (int i = 0, j = 0, n = strlen(s); i < n; i++, j++)
    {
        if (s[i] == '%' && i < n - 2)
        {
            char octet[3];
            octet[0] = s[i + 1];
            octet[1] = s[i + 2];
            octet[2] = '\0';
            t[j] = (char) strtol(octet, NULL, 16);
            i += 2;
        }
        else if (s[i] == '+')
        {
            t[j] = ' ';
        }
        else
        {
            t[j] = s[i];
        }
    }

    // escaped string
    return t;
}
