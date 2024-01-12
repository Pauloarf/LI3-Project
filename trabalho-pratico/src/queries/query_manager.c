/**
 * QUERY MANAGER
 * 
 * This module manages and executes queries according to a source (line or file).
 */

#include "queries/queries.h"
#include "util/string.h"
#include "tests/test.h"

Tokens tokenize_query(char* line, ssize_t len) {
    char* ptr = strdup(line);
    char* ptr_root = ptr;

    if (ptr == NULL) exit(EXIT_FAILURE);

    if (ptr[len - 1] == '\n') {
        ptr[len - 1] = '\0';
    }

    int seps = 1;
    for (int i = 0; line[i]; i++) seps += (line[i] == ' ');

    char** arr = (char**)malloc(seps * sizeof(char*));
    memset(arr, 0, seps * sizeof(char*));

    char* token;
    int i = 0;
    while ((token = strsep(&ptr, " ")) != NULL) {
        char* tokenData = strdup(token);

        arr[i++] = tokenData;
    }

    Tokens ret = (Tokens)malloc(sizeof(TOKENS));
    ret->data = arr;
    ret->len = seps;

    free(ptr_root);
    return ret;
}

// #pragma region Parser helpers
// #pragma GCC push_options
// #pragma GCC optimize ("O0")
void query_preprocessor(FILE* stream, ParserStore store, va_list args) {
    IGNORE_ARG(stream);

    gpointer null_element = NULL;

    // ------- OUTPUT DIRECTORY -------
    g_array_append_vals(store, &null_element, 1); // Directory for outputs

    char* output_dir = va_arg(args, char*);
    
    // Using more verbose code
    char** s_output_dir = (char**)&g_array_index(store, void*, 0);
    *s_output_dir = resolve_to_cwd(output_dir);
    // Because the alternative is this abomination:
    // *((char**)(&(((void**) (void *) (store)->data) [(0)]))) = resolve_to_cwd(output_dir);
    
    // ------- QUERY NUMBER -------
    g_array_append_vals(store, &null_element, 1); // Current query number

    int* query_num = (int*)malloc(sizeof(int));
    *query_num = 1;

    int** s_query_num = (int**)&g_array_index(store, void*, 1);
    *s_query_num = query_num;

    // ------- CATALOGUES -------
    g_array_append_vals(store, &null_element, 1);
    Catalog** catalogues = va_arg(args, Catalog**);

    Catalog*** s_catalogues = (Catalog***)&g_array_index(store, void*, 2);
    *s_catalogues = catalogues;

    *s_catalogues = catalogues;

    // ------- TIMERS -------
    #ifdef MAKE_TEST
        double* timers = (double*)malloc(sizeof(double) * 10);  //TODO: CONVERTER ESSE 10 PARA UM DEFINE DO GENERO, NUM_QUERIES. esse define no test.h
        memset(timers, 0, sizeof(double) * 10);
        g_array_append_vals(store, &timers, 1);
    #endif
}

int query_verifier(Tokens tokens, ParserStore store) {
    // As per the project document, the queries are guaranteed to be valid.
    // So, only a syntatic sanity check will be performed.

    IGNORE_ARG(store);

    for (int i = 0; i < tokens->len; i++) {
        if (tokens->data[i] == NULL) return 0;
    }

    return 1;
}

void* query_parser(Tokens tokens) {
    Query query = (Query)malloc(sizeof(QUERY));
    memset(query, 0, sizeof(QUERY));

    char id[QUERIES_CHAR_LEN] = { 0 };
    char flag = 0;

    int j = 0;
    for (int i = 0; tokens->data[0][i] != '\0'; i++) {
        if (tokens->data[0][i] >= '0' && tokens->data[0][i] <= '9') {
            id[j++] = tokens->data[0][i];
        } else {
            flag = tokens->data[0][i];
        }
    }

    id[j] = '\0';

    char* args[QUERIES_MAX_ARGS] = { 0 };

    j = 0;
    for (int i = 1; i < tokens->len; i++, j++) {
        if (tokens->data[i][0] == '\"') {
            int totalLen = 1;
            char* temp = tokens->data[i];
            int tempsize = strlen(temp);
            int start = i, end = i;

            while(temp[tempsize - 1] != '\"') {
                totalLen += tempsize + 1;
                end++;

                temp = tokens->data[i + (end - start)];
                tempsize = strlen(temp);
            }
            totalLen += tempsize;

            args[j] = (char*)malloc(totalLen * sizeof(char*));

            strcpy(args[j], tokens->data[i] + 1);
            for (int k = start + 1; k <= end; k++) {
                strcat(args[j], " ");
                strcat(args[j], tokens->data[k]);
            }
            args[j][totalLen - 3] = '\0';

            query->argc++;

            i += end - start;
        } else {
            args[j] = strdup(tokens->data[i]);
            if (tokens->data[i][0] != '\0') query->argc++;
        }
    }
    
    strcpy(query->id, id);
    query->flag = flag;
    memcpy(query->argv, args, QUERIES_MAX_ARGS * sizeof(char*));

    return query;
}

void query_writer(void* raw_data, ParserStore store) {
    Query data = (Query)raw_data;

    // ------- Fetch store arguments -------
    char** s_output_dir = (char**)&g_array_index(store, void*, 0);
    int* s_query_num = g_array_index(store, void*, 1);
    Catalog** catalogues = ((Catalog**)g_array_index(store, void*, 2));
    TEST_EXPR(double* timers = (double*)g_array_index(store, double*, 3);)
    
    // ------- Process required variables -------
    char* output_file = isnprintf("command%d_output.txt", *s_query_num);
    char* path = join_paths(2, *s_output_dir, output_file);

    // ------- Execute query -------
    FILE* retFile = OPEN_FILE(path, "w");

    // void* ret = query_execute(data, catalogues, retFile);
    TEST_EXPR(clock_t start_time = clock();)
    query_execute(data, catalogues, retFile);
    TEST_EXPR(clock_t end_time = clock();)
    TEST_EXPR(double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;)
    TEST_EXPR(timers[atoi(data->id) - 1] += elapsed_time;)

    CLOSE_FILE(retFile);

    // // ------- Write output -------
    // FILE* retFile = OPEN_FILE(path, "w");
    // fputs(ret, retFile);
    // CLOSE_FILE(retFile);

    // ------- Free memory -------
    free(output_file);
    free(path);

    // Destroy query
    for (int i = 0; i < data->argc; i++) free(data->argv[i]);
    free(data);

    // ------- Write flags -------

    *s_query_num = *s_query_num + 1;
    return;
}

void query_discarder(void* raw_data, ParserStore store) {
    // Queries are always correct. No discarding needed.
    IGNORE_ARG(raw_data);
    IGNORE_ARG(store);
}

void query_destructor(FILE* stream, ParserStore store) {
    IGNORE_ARG(stream);

    // ------- Fetch store timers for display -------
    #ifdef MAKE_TEST
        double* timers = g_array_index(store, double*, 3);
        double total_time = 0;
        for(int i = 0; i < 10 ; i++) {
            test_trace(" - Execution time for query %2d: %.4f seconds.\n", i+1, timers[i]);
            total_time += timers[i];
        }
        test_trace("\n----===[  GENERAL PROGRAM METRICS  ]===----\n\n");
        test_trace(" -> Execution time for solving all queries: %.4f seconds.\n", total_time);
    #endif

    // ------- Free Memory -------
    for (guint i = 0; i < store->len; ++i) {
        void* element = g_array_index(store, void*, i);
        free(element);
    }

    g_array_free(store, TRUE);
}
// #pragma GCC pop_options
// #pragma endregion

void query_execute(Query query, Catalog** catalogues, FILE* output_file) {
    switch (atoi(query->id)) {
        case 1:  query1 (query->flag, query->argc, query->argv, catalogues, output_file); break;
        case 2:  query2 (query->flag, query->argc, query->argv, catalogues, output_file); break;
        case 3:  query3 (query->flag, query->argc, query->argv, catalogues, output_file); break;
        case 4:  query4 (query->flag, query->argc, query->argv, catalogues, output_file); break;
        case 5:  query5 (query->flag, query->argc, query->argv, catalogues, output_file); break;
        case 6:  query6 (query->flag, query->argc, query->argv, catalogues, output_file); break;
        case 7:  query7 (query->flag, query->argc, query->argv, catalogues, output_file); break;
        case 8:  query8 (query->flag, query->argc, query->argv, catalogues, output_file); break;
        case 9:  query9 (query->flag, query->argc, query->argv, catalogues, output_file); break;
        case 10: query10(query->flag, query->argc, query->argv, catalogues, output_file); break;
    }
};

void query_run_bulk(char* input_file, char* output_dir, Catalog** catalogues) {
    parse_file(
        input_file, 
        &tokenize_query, 
        &query_preprocessor, 
        &query_verifier, 
        &query_parser, 
        &query_writer, 
        &query_discarder,
        &query_destructor,
        output_dir,
        catalogues
    );
    return;
};

void query_run_single(char* query, ssize_t len) {
    parse(query, len, &tokenize_query, &query_verifier, &query_parser, &query_writer, &query_discarder, makeStore());
    return;
};