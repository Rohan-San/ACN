size_t datasize;
FILE* fd = fopen(filename, "wb");
while (!received)
{
    datasize = recv(connection, text, sizeof(text), 0);
    fwrite(&text, 1, datasize, fd);
}
fclose(fd);
