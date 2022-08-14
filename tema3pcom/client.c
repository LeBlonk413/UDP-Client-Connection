#include <stdio.h>              /* printf, sprintf */
#include <stdlib.h>             /* exit, atoi, malloc, free */
#include <unistd.h>             /* read, write, close */
#include <string.h>             /* memcpy, memset */
#include <sys/socket.h>         /* socket, connect */
#include <netinet/in.h>         /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>              /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <stdbool.h> 
#include <ctype.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define BUFFERS 100

bool number(char *num) {

    while(*num != '\0')
    {
        if(*num < '0' || *num > '9')
            return false;
        num++;
    }
    return true;
} 

int main(int argc, char *argv[])
{
    char *server_request = NULL;
    char *response = NULL;
    int sockfd;

    bool login_session = 0;

    char *cookie = NULL, *token = NULL;

    char *cookies = NULL, *tokens = NULL;
    char *rest = NULL;

    char *buffer = (char*) malloc (BUFFERS);
    char *buff_user = (char*) malloc (BUFFERS);
    char *buff_pass = (char*) malloc (BUFFERS);

    while (1) {
        scanf("%s", buffer);
        
        if(strcmp(buffer, "exit") == 0) {
            
            free(buffer);
            free(buff_user);
            free(buff_pass);

            exit(0);
            return 0;

        } else if (strcmp(buffer, "register") == 0) {
            
            if (login_session == 1) 
                printf("You are logged in already.\n");
            else {
            
            printf("username=");
            //scanf("%s", buff_user);
            fgets(buff_user, 100, stdin);
            /*while (strcmp(buff_user, "\n") == 0) {
                printf("Please type a valid name.\n");
                printf("username=");
                scanf("%s", buff_user);
            }
            */

            printf("password=");
            scanf("%s", buff_pass);
            while (strcmp(buff_pass, "\n") == 0) {
                printf("Please type a valid password.\n");
                printf("password=");
                scanf("%s", buff_pass);
            }

            // create the JSON object and then adapt it to string type
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);

            char *json_string = NULL;
            json_object_set_string(root_object, "username", buff_user);
	   		json_object_set_string(root_object, "password", buff_pass);

            json_string = json_serialize_to_string_pretty(root_value);

            // create the request before sending it to the server
            server_request = compute_post_request("34.241.4.235", "/api/v1/tema/auth/register",
                                        "application/json", &json_string, 1, NULL, 0, NULL);

            // open connection
            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
            // send the request to the server
            send_to_server(sockfd, server_request);
            // get the response
            response = receive_from_server(sockfd);

            if(strstr(response, "error") != NULL)
                printf("Username is taken. Please try again.\n");
            else
                printf("User successfully registered.\n");

            close_connection(sockfd);
            }

        } else if (strcmp(buffer, "login") == 0) {

            if (login_session == 1) 
                printf("You are already logged in.\n");
            else {

                printf("username=");
                scanf("%s", buff_user);
                while (strcmp(buff_user, "\n") == 0) {
                    printf("Please type a valid name.\n");
                    printf("username=");
                    scanf("%s", buff_user);
                }

                printf("password=");
                scanf("%s", buff_pass);
                while (strcmp(buff_pass, "\n") == 0) {
                    printf("Please type a valid password.\n");
                    printf("password=");
                    scanf("%s", buff_pass);
                }

                JSON_Value *root_value = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root_value);

                char *json_string = NULL;
                json_object_set_string(root_object, "username", buff_user);
	   		    json_object_set_string(root_object, "password", buff_pass);

                json_string = json_serialize_to_string_pretty(root_value);

                server_request = compute_post_request("34.241.4.235", "/api/v1/tema/auth/login",
                                        "application/json", &json_string, 1, NULL, 0, NULL);
            
                sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                // send the request to the server
                send_to_server(sockfd, server_request);
                // get the response
                response = receive_from_server(sockfd);

                rest = response;
        
                // save the cookie to confirm the login session
                if (strstr(rest, "error") == 0) {
                    while ((cookies = strtok_r(rest, "\n", &rest))) {
                        if (strstr(cookies, "Set-Cookie:") != 0) {
                            cookie = strtok(cookies, ";");
                            cookie = strtok(cookie, ":");
                            cookie = strtok(0, " ");
                            break;
                        }
                    }
                    printf("User %s is now logged in.\n", buff_user);
                    login_session = 1;
                }
                else
                    printf("Username and password do not match.\n");

                close_connection(sockfd);
            }

        } else if (strcmp(buffer, "enter_library") == 0) {

            if (cookie == NULL)
                printf("You are not logged in.\n");
            else {
                server_request = compute_get_request("34.241.4.235", "/api/v1/tema/library/access", NULL, &cookie, 1, NULL);
                sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd, server_request);
                response = receive_from_server(sockfd);

                rest = response;

                // save the token to confirm user's access to the library
                while ((tokens = strtok_r(rest, "\n", &rest))) {
                    if (strstr(tokens, "token") != 0) {
                        token = strtok(tokens, ":");
                        token = strtok(0, "\n");
                        break;
                    }
                }
                token++;
                strtok(token, "\"");

                printf("Library entered successfully.\n");

                close_connection(sockfd);
            }

        } else if (strcmp(buffer, "get_books") == 0) {
            
            if (cookie == NULL)
                printf("You are not logged in to be able to access the library.\n");
            else if (token == NULL)
                printf("You do not have access to the library.\n");
            else {
                server_request = compute_get_request("34.241.4.235", "/api/v1/tema/library/books", NULL, &cookie, 1, token);
                sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, server_request);

                response = receive_from_server(sockfd);

                // const char* library = strstr(response, "\r\n\r\n");
                // library = library + 4;

                response = strstr(response, "[");
                JSON_Value *value = json_value_init_string(response);

                printf("%s\n", json_serialize_to_string_pretty(value));

                close_connection(sockfd);
            }
        } else if (strcmp(buffer, "get_book") == 0) {
            
            if (cookie == NULL)
                printf("You are not logged in to be able to access the library.\n");
            else if (token == NULL)
                printf("You do not have access to the library.\n");
            else {
                printf("id=");
                char *buff_id = (char*) malloc (BUFFERS);
                scanf("%s", buff_id);

                while(!number(buff_id)) {
                    printf("Please type a valid id.\n");
                    printf("id=");
                    scanf("%s", buff_id);
                }

                char request[200];
            
                strcpy(request, "/api/v1/tema/library/books/");
                strcat(request, buff_id);

                server_request = compute_get_request("34.241.4.235", request, "application/json", &cookie, 1, token);
            
                sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd, server_request);
                response = receive_from_server(sockfd);

                if(strstr(response, "error") != NULL)
                    printf("The book ID does not exist.\n");
                else {
                    const char* library = strstr(response, "\r\n\r\n");
                    library = library + 4;

                    printf("%s\n", library);
                }

                close_connection(sockfd);
            }

        } else if (strcmp(buffer, "add_book") == 0) {

            if (cookie == NULL)
                printf("You are not logged in to be able to access the library.\n");
            else if (token == NULL)
                printf("You do not have access to the library.\n");
            else {

            // TITLE

            printf("title=");
            char* buff_title = (char*) malloc (BUFFERS);
            scanf("%s", buff_title);

            while (strcmp(buff_title, "\n") == 0) {
                printf("Please type a valid title.\n");
                printf("title=");
                scanf("%s", buff_title);
            }

            // AUTHOR

            printf("author=");
            char* buff_author = (char*) malloc (BUFFERS);
            scanf("%s", buff_author);

            while (strcmp(buff_author, "\n") == 0) {
                printf("Please type a valid author.\n");
                printf("author=");
                scanf("%s", buff_author);
            }

            // GENRE

            printf("genre=");
            char* buff_genre = (char*) malloc (BUFFERS);
            scanf("%s", buff_genre);

            while (strcmp(buff_genre, "\n") == 0) {
                printf("Please type a valid genre.\n");
                printf("genre=");
                scanf("%s", buff_genre);
            }

            // PUBLISHER

            printf("publisher=");
            char* buff_publisher = (char*) malloc (BUFFERS);
            scanf("%s", buff_publisher);

            while (strcmp(buff_publisher, "\n") == 0) {
                printf("Please type a valid publisher.\n");
                printf("publisher=");
                scanf("%s", buff_publisher);
            }

            // PAGE COUNT

            printf("page_count=");
            char* buff_page_count = (char*) malloc (BUFFERS);
            scanf("%s", buff_page_count);

            while (!number(buff_page_count)) {
                printf("Please type a valid page count.\n");
                printf("page_count=");
                scanf("%s", buff_page_count);
            }

            char *json_string = NULL;
            JSON_Value *root_value = json_value_init_object();
	   		JSON_Object *root_object = json_value_get_object(root_value);
	   		
            json_object_set_string(root_object, "title", buff_title);
	   		json_object_set_string(root_object, "author", buff_author);
	   		json_object_set_string(root_object, "genre", buff_genre);
            json_object_set_string(root_object, "publisher", buff_publisher);
	   		json_object_set_number(root_object, "page_count", atoi(buff_page_count));

            json_string = json_serialize_to_string_pretty(root_value);
            server_request = compute_post_request("34.241.4.235", "/api/v1/tema/library/books", "application/json",
                                    &json_string, 1, &cookie, 1, token);

            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
            send_to_server(sockfd, server_request);
	   		response = receive_from_server(sockfd);

            printf("Book added successfully.\n");

	   		close_connection(sockfd);
            }

        } else if (strcmp(buffer, "delete_book") == 0) {

            if (cookie == NULL)
                printf("You are not logged in to be able to access the library.\n");
            else if (token == NULL)
                printf("You do not have access to the library.\n");
            else {

                printf("id=");
                char *buff_id = (char*) malloc (BUFFERS);
                scanf("%s", buff_id);

                while(!number(buff_id)) {
                    printf("Please type a valid id.\n");
                    printf("id=");
                    scanf("%s", buff_id);
                }

                char request[200];
                strcpy(request, "/api/v1/tema/library/books/");
                strcat(request, buff_id);

                server_request = compute_delete_request("34.241.4.235", request, "application/json", NULL, 0, &cookie, 1, token);
                sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd, server_request);
                response = receive_from_server(sockfd);

                printf("Book deleted successfully.\n");

                close_connection(sockfd);
            }

        } else if (strcmp(buffer, "logout") == 0) {
            
            if (login_session == 0)
                printf("You are not logged in to be able to logout.\n");
            else {
                server_request = compute_get_request("34.241.4.235", "/api/v1/tema/auth/logout", NULL, &cookie, 1, token);
                sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, server_request);

                response = receive_from_server(sockfd);

                cookie = NULL;
                token = NULL;

                printf("Login session ended.\n");

                login_session = 0;
                close_connection(sockfd);
            }
        } else {
            printf("Invalid command. Please type one of the following commands:\n\n");
            printf("> register - Create a new accoount.\n");
            printf("> login - Login to an existing account.\n");
            printf("> enter_library - Get access to your book collection.\n");
            printf("> get_books - View your book collection.\n");
            printf("> get_books - View the details for one of your books.\n");
            printf("> delete_book - Delete a book from your library.\n");
            printf("> logout - Logout of your account.\n");
            printf("> exit - Closes the client connection.\n");
        }
    }
}