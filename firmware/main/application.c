#include "application.h"

esp_err_t APPLICATION_Send_data_to_server(uint8_t *data_ptr, size_t data_size)
{
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in info = ZERO_ARRAY;
    info.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    info.sin_family = AF_INET;
    info.sin_port = htons(SERVER_PORT);

    int one = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one));
    int a = connect(s, (const struct sockaddr *)&info, (socklen_t)sizeof(struct sockaddr));
    int bytes_sent = send(s, (const void *)data_ptr, data_size, 0);
    shutdown(s, SHUT_RDWR);
    close(s);

    if (bytes_sent == data_size)
    {
        return ESP_OK;
    }

    return ESP_FAIL;
}