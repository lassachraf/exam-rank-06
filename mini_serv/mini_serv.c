# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <strings.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <sys/socket.h>

typedef struct s_client {
	int		id;
	char	msg[110000];
}	t_client;

t_client	clients[1024];

fd_set		fds, read_fds, write_fds;

int			max_fd = 0;
int			next_id = 0;

char		read_buffer[120000];
char		write_buffer[120000];

void		fatal() {
	write(2, "Fatal error\n", 13);
	exit(1);
}

void		broadcaster(int sender) {
	for (int fd = 0; fd <= max_fd; fd++)
		if (FD_ISSET(fd, &write_fds) && fd != sender)
			send(fd, write_buffer, strlen(write_buffer), 0);
}

int        init_server(char* port) {
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		fatal();

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(2130706433);
	addr.sin_port = htons(atoi(port));

	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		fatal();
	if (listen(sockfd, 128) < 0)
		fatal();

	return sockfd;
}

void		handle_new_connection(int sockfd) {
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	int client_fd = accept(sockfd, (struct sockaddr *)&addr, &len);
	if (client_fd < 0)
		fatal();
	if (client_fd > max_fd)
		max_fd = client_fd;

	clients[client_fd].id = next_id++;
	FD_SET(client_fd, &fds);

	sprintf(write_buffer, "server: client %d just arrived\n", clients[client_fd].id);
	broadcaster(client_fd);
}

void		handle_disconnect(int client_fd) {
	sprintf(write_buffer, "server: client %d just left\n", clients[client_fd].id);
	broadcaster(client_fd);

	FD_CLR(client_fd, &fds);
	close(client_fd);
	bzero(clients[client_fd].msg, sizeof(clients[client_fd].msg));
}

void		handle_message(int fd) {
	int bytes = recv(fd, read_buffer, sizeof(read_buffer), 0);
	if (bytes <= 0) {
		handle_disconnect(fd);
		return ;
	}

	for (int i = 0, j = strlen(clients[fd].msg); i < bytes; i++, j++) {
		clients[fd].msg[j] = read_buffer[i];
		if (clients[fd].msg[j] == '\n') {
			clients[fd].msg[j] = '\0';

			sprintf(write_buffer, "client %d: %s\n", clients[fd].id, clients[fd].msg);
			broadcaster(fd);
			bzero(clients[fd].msg, sizeof(clients[fd].msg));
			j = -1;
		}
	}
}

int		main(int ac, char** av) {
	if (ac != 2) {
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}

	FD_ZERO(&fds);
	bzero(clients, sizeof(clients));

	int sockfd = init_server(av[1]);

	max_fd = sockfd;
	FD_SET(sockfd, &fds);

	while (1) {
		read_fds = write_fds = fds;
		if (select(max_fd + 1, &read_fds, &write_fds, NULL, NULL) < 0)
			continue;

		for (int fd = 0; fd <= max_fd; fd++) {
			if (!FD_ISSET(fd, &read_fds))
				continue;
			if (sockfd == fd)
				handle_new_connection(fd);
			else
				handle_message(fd);
		}
	}

	return 0;
}
