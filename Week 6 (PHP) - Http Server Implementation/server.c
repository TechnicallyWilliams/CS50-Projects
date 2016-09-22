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

// header files (These header files are missing in the repository)
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
            free(path); 
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

        // check for control-c  // (checks if been signaled to stop the server)
        if (signaled) 
        {
            stop();
        }

        // check whether client has connected // cs50 wrote this function for us (returns true if browser or curl connected to the server)
        if (connected()) 
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
                if (parse(line, abs_path, query))  //parses http request, storing it's "absolute path'" and "query" inside of two arrays that are passed into it by memory reference                                              
                {                                   
                    // URL-decode absolute-path         
                    char* p = urldecode(abs_path);   
                    if (p == NULL)
                    {
                        error(500);
                        continue; 
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

                    // ensure path exists
                    if (access(path, F_OK) == -1)
                    {
                        error(404);
                        continue;
                    }

                    // if path to directory
                    struct stat sb;
                    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) //check if user asekd for file or directory
                    { 
                        // redirect from absolute-path to absolute-path
                        if (abs_path[strlen(abs_path) - 1] != '/')
                        {
                            char uri[strlen(abs_path) + 1 + 1];
                            strcpy(uri, abs_path);
                            strcat(uri, "/");
                            redirect(uri);
                            continue;
                        }

                        // use path/index.php or path/index.html, if present, instead of directory's path
                        char* index = indexes(path); //student implementation
                        if (index != NULL)
                        {
                            free(path); //we want to show contents of default file, not contents of directory
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
                    const char* type = lookup(path);  //student implementation
                    if (type == NULL)
                    {
                        error(501);
                        continue;
                    }

                    // interpret PHP script at path
                    if (strcasecmp("text/x-php", type) == 0) 
                    {
                        interpret(path, query);
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
    memset(&cli_addr, 0, sizeof(cli_addr)); 
    socklen_t cli_len = sizeof(cli_addr);
    cfd = accept(sfd, (struct sockaddr*) &cli_addr, &cli_len);
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
char* indexes(const char* path) 
{
    // TODO: Check if index.php exists and return if it does. Do the same with index.html
    char* thePHP = "/index.php"; //this slashed solved redirection problems
    char* theHTML = "/index.html"; //slashes solved redirection problems
    char* tempPHP = malloc(sizeof(char) * (LimitRequestLine + 1));
    char* tempHTML = malloc(sizeof(char) * (LimitRequestLine + 1));
    
    //Because path is a read-only string, here, we make a copy and append index.php to it. We then check if that file exists.
    strcpy(tempPHP, path);
    strcat(tempPHP, thePHP);
    FILE* file = fopen(tempPHP, "r");
    if (file != NULL)
    {
        fclose(file);
        return tempPHP;
    }
    
    //Because path is a read-only string, here, we make a copy and append index.html to it. We then check if that file exists.
    strcpy(tempHTML, path);
    strcat(tempHTML, theHTML);
    FILE* file2 = fopen(tempHTML, "r");
    if (file2 != NULL)
    {
        free(tempPHP);
        fclose(file2);
        return tempHTML;
    }
    
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
        error(500);
        return;
    }
    
    FILE* file = popen(command, "r"); //open's a "pipe" to a process (php-cgi), providing a FILE pointer to read the process's standard output (as though its a file)
    if (file == NULL)
    {
        error(500);
        return;
    }

    // load interpreter's content
    char* content;
    size_t length;

    if (load(file, &content, &length) == false) //student implementation of reading interpreters' output into memory
    {
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
bool load(FILE* file, BYTE** content, size_t* length) 
{

    //TODO: Load the output from php interpreter or load into memory all bytes from a file that a user has requested
    //Note: Did not use fseek within this scope because it damages the ability to read pipes

    *length = 0; //this doesn't guarantee NULL
    
     BYTE* nextBuffer = NULL;
     nextBuffer = malloc(sizeof(BYTE)); //Starting off with one Byte due to the stdin pipe operation which doesn't make it easy to predetermine the amount of bytes needed

    int i = 0;                                       
    int fileChar = 0;

    while ((fileChar = fgetc(file)) != EOF) //Read from text, image, and pipe one BYTE at a time.
    {
            nextBuffer = realloc(nextBuffer, ((i + 1) * sizeof(BYTE)) + sizeof(BYTE) ); //Create space for an extra BYTE
            nextBuffer[i] = (char) fileChar; //
            i++; 
    }
   
    nextBuffer[i] = '\0'; //To make sure the last index in the buffer is null terminated
    *content = nextBuffer;

    *length = i; //use i because you can't do strlen on a binary file //strlen also ignores null terminator
    
    if ( *length == i) 
    {
        return true;
    }
    
    return false;

}

/**
 * Returns MIME type for supported extensions, else NULL.
 */
const char* lookup(const char* path)  // look up MIME aka content type of that file text/html or image/jpeg //find dot and file extension
{
    // TODO: Create a redundant copy of path to work with (unnecessary after realizing path is read-only)
    char* forLookUp = malloc(sizeof(char) * strlen(path));
    strcpy(forLookUp, path);
    
    
    char* MIME = NULL; //Sets MIME to null upon returning to the function
    if (MIME != NULL)
    {
        free(MIME);
        MIME = NULL;
    }
    MIME = malloc(6 * sizeof(char)); 
    char period = 46;
    MIME = strrchr(forLookUp, period); //Search for where the file extension begins
    
    
    char temp;
    int extLength = strlen(MIME);
    for (int i = 1; i < extLength; i++)
    {
        temp = toupper(MIME[i]);
        MIME[i] = temp; //make everything capital for comparison purposes
    }
    
    
    MIME[strlen(MIME) + 1] = '\0'; //inject null terminator just in case there are unecessary values beyond the file extension
    //Search for uppercase cases. NOTE: Did not free MIME on each case because it threw an error
    if (strcmp(MIME, ".CSS") == 0  )
    {
        free(forLookUp);
        return "text/css";
        
    } else if (strcmp(MIME, ".HTML") == 0 )
    {
        free(forLookUp);
        return "text/html";
        
    } else if (strcmp(MIME, ".GIF") == 0 )
    {
        free(forLookUp);
        return "image/gif";
        
    } else if (strcmp(MIME, ".ICO") == 0 )
    {
        free(forLookUp);
        return "image/x-icon";
        
    } else if (strcmp(MIME, ".JPG") == 0 )
    {
        free(forLookUp);
        return "image/jpeg";
        
    } else if (strcmp(MIME, ".JS") == 0  )
    {
        free(forLookUp);
        return "text/javascript";
        
    } else if (strcmp(MIME, ".PHP") == 0  )
    {
        free(forLookUp);
        return "text/x-php";
        
    } else
    {
        if ( strcmp(MIME, ".PNG") == 0 )
        {
            free(forLookUp);
            return "image/png";
        }
    }
    
    
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
    //Create characters in dynamic memory for string comparisons later
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
   
    int oldPathSize = (int) strlen(abs_path);
    int newPathSize = 0;
    
    int oldQuerySize = (int) strlen(query);
    int newQuerySize = 0;
    
    int lineLength = (int) strlen(lineProx);
    
    
    //Checks if the beginning of the request start off alright (Makes sure Get is in the right format )
    if ( strstr(lineProx, "GET ") == NULL || (strstr(lineProx, "abcGET") != NULL) || (strstr(lineProx, " GET ") != NULL) ) 
    {
        error(405);
        return false; 

    } else if ( strncmp(&lineSlash[0], &slash[0], 1) != 0 ) //Does the immiediate middle start-off alright? (Slashes are not allowed)
    {
        error(501); 
        return false; 
        
    } else if (!strstr(lineProx, " HTTP/1.1")) //Does the end start-off alright?
    {
        error(505);
        return false;
        
    } else //examine all the stuff in the middle (exclusively between line 4 and the line space right before the http version, filter out what I dont want)
    {
        for ( int i = 4; i < lineLength - 10; i++ )
        {
            if (strncmp(&lineProx[i], "?", 1) == 0)
            {
                i++; //Don't include "?"
                for (int j = i; j < lineLength - 11; j++) 
                {
                    strncpy(&query[j - i], &lineProx[j], 1); //Even if ? is present, [0] should be " " if nothing comes after the ?
                    newQuerySize++;
                    
                    // Making sure the query doesn't contain quotes or space
                    if ( lineProx[j] == quote[0] || lineProx[i] == space[0] ) 
                    {
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
                //THis is simply copying everything prior to the query
                strncpy(&abs_path[i - 4], &lineProx[i], 1);   
                newPathSize++;
            }
        }
    
        while (oldPathSize > newPathSize) //make sure old memory is not interfering with new memory
        {
            abs_path[oldPathSize - 1] = 0; 
            oldPathSize--;
        }
        
        while (oldQuerySize > newQuerySize) //make sure old memory is not interfering with new memory
        {
            query[oldQuerySize - 1] = 0; 
            oldQuerySize--;
        }
        

        free(lineSlash);
        free(slash); //either this or lineProx broke
        return true;
        
    }
    
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
{                                 
    // ensure socket is open                
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
