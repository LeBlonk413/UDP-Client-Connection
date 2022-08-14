This is the code for a client that connects to a server with books. Each user
has their own library where they can add, check or delete books. For each
command the client creates a request that is then sent to the server. The
response from the server is then processed into an output for the client. The
client will remember the cookie and token of the server for ease of use.

--> exit
This is the command that ends the connection between the client and the
server and then closes the program.

--> register
To create a new account, the client sends a request to the server containing
a username and a password and then checks if the username is in use or not.

--> login
The client sends a similar request as the "register" command to the server.
The server then checks if the login credentials are correct, in which case the
response contains a cookie that will be used to confirm the user for the next
commands.

--> enter_library
The client sends a request containing the user's cookie to make sure that
someone is indeed authenticated on the account. The server then sends back the
token, which will be used to make edits in the user's library.

--> get_books
The client sends a request containing the user's cookie and token to the
server. If the client is logged in and has access to the library, the server
then sends back a list will all the books in his library and their IDs.

--> get_book
The client sends a request to the server containing the user's cookie, the
token and the ID of the boo. The server sends back all the book's details. 

--> add_book
After giving the input of the new book's details, the client sends a request
to the server with the cookie, token and the JSON object that contains the
book's details. The server then adds the book to the user's library. 

--> delete_book
After checking the cookie and the token, the server also checks if the ID of
the book the client wants to delete is valid, in which case the user's library
is updated.

--> logout
Before logging out the client makes sure that he is logged in. As he logs out,
the cookie and token become NULL.