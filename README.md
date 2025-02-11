# Webserv

Webserv is a **42 School** project that involves building a simple **HTTP/1.1 web server** from scratch using **C++**. This project introduces core networking concepts, socket programming, and multi-client handling while following the **HTTP/1.1 specification**.

## Features

- **HTTP/1.1 compliant server**
- **Supports GET, POST, and DELETE methods**
- **Multi-client handling with non-blocking sockets**
- **Custom configuration file support**
- **CGI (Common Gateway Interface) execution**
- **Static and dynamic content serving**
- **Error handling and custom error pages**
- **Chunked transfer encoding support**

## Installation

```sh
git clone https://github.com/DijkmanJ/webserv.git
cd webserv
make
```

## Usage

Run the server by specifying a configuration file:
```sh
./webserv config/config.conf
```

Or run the server using the default configuation:
```sh
./webserv
```

## Example Configuration File

```conf
{
SERVER server1
PORT 8080
HOST 127.0.0.1
CLIENT_MAX_BODY_SIZE 200000

ERROR_PAGE error.html
INDEX_FILE index.html
CGI_PATH /cgi-bin
TAG_AUTOINDEX /list-files
AUTOINDEX_PAGE autoindex.html
ROOT /www

[
	PATH /www
	ALLOWED_METHODS GET
	AUTO_INDEX on
]

[
	PATH /www/uploads
	ALLOWED_METHODS GET, POST, DELETE
	AUTO_INDEX on
	POST_ALLOWED_TYPES jpg, png, json
]

[
	PATH /www/images
	ALLOWED_METHODS GET, DELETE
	AUTO_INDEX on
]

[
	PATH /www/css
	ALLOWED_METHODS GET
]

[
	PATH /www/js
	ALLOWED_METHODS GET
]

[
	PATH /www/cgi-bin
	ALLOWED_METHODS GET, POST
]
}
```

## Requirements

- **GNU Make**
- **G++ Compiler**
- **Linux**
- **Basic knowledge of networking**

## Project Guidelines

This project follows the **42 School** rules:
- No external libraries (except standard C++ STL)
- Proper memory management (no leaks allowed)
- Efficient handling of multiple connections

## Contributing

Feel free to fork and submit pull requests. Contributions are welcome!

## License

This project is licensed under the **MIT License**.
