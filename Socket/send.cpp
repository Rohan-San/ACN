FILE *fd = fopen(filename, "rb");
size_t rret, wret;
int bytes_read;
while (!feof(fd)) {
    if ((bytes_read = fread(&buffer, 1, BUFFER_SIZE, fd)) > 0)
        send(sock, buffer, bytes_read, 0);
    else
        break;
}
    fclose(fd);
