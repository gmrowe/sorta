#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define BUFFER_MAX_LEN (1024 * 1000) // 1 mb
char INTERNAL_STR_BUFFER[BUFFER_MAX_LEN] = {0};
size_t INTERNAL_STR_BUFFER_LEN = 0;

typedef struct SV
{
    char * data;
    size_t length;
} SV;

SV sv_from_str(char *str)
{
    return (SV) {
        .data = str,
        .length = strlen(str),
    };
}

bool sv_starts_with(const SV *sv, const SV *prefix)
{
    if (prefix->length > sv->length)
    {
        return false;
    }

    return strncmp(sv->data, prefix->data, prefix->length) == 0;
}

SV sv_substr(const SV *sv, size_t start, size_t end)
{
    return (SV) {
        .data = sv->data + start,
        .length = end - start,
    };
}

SV sv_chop_1_left(const SV *sv)
{
    if (sv->length > 0)
    {
        return sv_substr(sv, 1, sv->length);
    }
    else
    {
        return *sv;
    }
}

SV sv_triml(const SV *sv)
{
    size_t start = 0;
    while ((start < sv->length) && isspace(sv->data[start]))
    {
        start += 1;
    }

    return sv_substr(sv, start,  sv->length);
}

SV sv_trimr(const SV *sv)
{
    size_t length = sv->length;
    while ((length > 0) && isspace(sv->data[length - 1]))
    {
        length -= 1;
    }

    return (SV) {
        .data = sv->data,
        .length = length
    };
}

SV sv_trim(const SV *l_sv_r)
{
    SV sv_r = sv_triml(l_sv_r);
    SV sv = sv_trimr(&sv_r);
    return sv;
}

void sv_print(const SV *sv)
{
    char *buffer = calloc(sv->length + 1, sizeof(char));
    strncpy(buffer, sv->data, sv->length);
    buffer[sv->length] = '\0';
    printf("%s", buffer);
    free(buffer);
}

size_t get_file_length(FILE *handle)
{
    size_t curr_loc = ftell(handle);
    fseek(handle, 0, SEEK_END);
    size_t length = ftell(handle);
    fseek(handle, curr_loc, SEEK_SET);
    return length;
}

size_t read_file_to_buffer(char *file_path,
                           size_t buffer_size,
                           char buffer[buffer_size])
{
    FILE *handle = fopen(file_path, "rb");

    if (!handle)
    {
        fprintf(stderr, "[FATAL ERROR] Could not open file: %s\n", file_path);
        exit(1);
    }

    size_t length = get_file_length(handle);

    if (length >= buffer_size)
    {
        fprintf(stderr, "[FATAL ERROR] File contents can not fit in buffer\n");
        fprintf(stderr, "Buffer size = %zu; File size = %zu\n",
                buffer_size,
                length);
        fclose(handle);
        exit(1);
    }

    size_t bytes_read = fread(buffer, 1, length, handle);
    buffer[length] = '\0';
    fclose(handle);

    return bytes_read;
}

int main(void)
{
    /* char *file_path = "lorem.txt"; */
    /* size_t length = read_file_to_buffer( */
    /*     file_path, BUFFER_MAX_LEN, INTERNAL_STR_BUFFER); */
    /* INTERNAL_STR_BUFFER_LEN = length + 1; */

    SV foo = sv_from_str("abc");
    SV bar = sv_chop_1_left(&foo);
    sv_print(&bar);
    printf("\n");
    return EXIT_SUCCESS;
}
