#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char* header; // the column header
    size_t numEntries; // the number of entries in the column
    char** entries; // the entries in the column
} kv;

typedef struct {
    kv* data; // columns of CSV each with the `header` and the `entries`
    size_t numEntries; // the number of entries in each column
} CSV;

/*
  * parses a csv string.
  * @returns a `CSV` struct containing the data
*/
CSV parse_csv(char* data, long size) {
    CSV csv = {.data=malloc(sizeof(kv))};

    size_t header_len = 0;
    size_t entry_i = 0;
    void* EMPTY = malloc(256);

    char* item = malloc(256);
    char item_char_i = 0;
    bool quotes = false, header = true, nextHeader = true;
    for (int i = 0; i < size; i++) {
        if ((data[i] != ',' && data[i] != '\n') || quotes) {
            if (data[i] == '"') quotes = 1 - quotes;
            item[item_char_i] = data[i];
            item_char_i++;
        } else {
            item[item_char_i] = '\0';
            if (!nextHeader) header = false;
            if (data[i] == '\n') nextHeader = false;
            if (header) {
                header_len++;
                
                csv.data = realloc(csv.data, (entry_i+1)*sizeof(kv));

                // the content at the address of `item` will get lost when we free it.
                // to preserve it, we make a copy
                char* itemcpy = malloc(256);
                memcpy(itemcpy, item, 256);

                csv.data[entry_i] = (kv){
                    .header=itemcpy,
                    .entries=malloc(sizeof(char*)),
                    .numEntries=0
                };
            } else {
                char** entries = csv.data[entry_i%header_len].entries;
                size_t* numEntries = &csv.data[entry_i%header_len].numEntries;

                csv.data[entry_i%header_len].numEntries++;
                if (entry_i % header_len == 0) csv.numEntries++;

                csv.data[entry_i%header_len].entries = realloc(entries, *numEntries*sizeof(char*));

                // same here
                char* itemcpy = malloc(256);
                memcpy(itemcpy, item, 256);

                csv.data[entry_i%header_len].entries[*numEntries-1] = itemcpy;
            }
            
            memcpy(item, EMPTY, 256);
            item_char_i = 0;
            entry_i++;
        }
    }

    free(EMPTY);
    free(item);

    return csv;
}

int main(int argc, char** argv) {
    FILE* file = fopen(argv[1], "r");

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buf = malloc(size);

    char ch;
    long index = 0;
    while ((ch = fgetc(file)) != EOF) {
        buf[index] = ch;
        index++;
    }

    CSV csv = parse_csv(buf, size);

    for (int j = 0; j < 12; j++) {
        printf("~~ %s ~~\n", csv.data[j].header);
        for (int i = 0; i < csv.numEntries; i++) {
            printf("%s\n", csv.data[j].entries[i]);
        }
    }

    free(buf);

    return 0;
}