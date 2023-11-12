#include "collections/passenger.h"

int verify_passenger_tokens(Tokens tokens) {
    char** parameter = tokens->data;

    // Whitespace verifier
    if(have_whitespace(parameter, 2) == 0)  return 0;
    
    return 1;
}

Passenger make_passenger(
    int flight_id,
    UserId(user_id)) {
    
    Passenger passenger = malloc(sizeof(PASSENGER));

    passenger->flight_id = flight_id;
    strncpy(passenger->user_id, user_id, (size_t)MAX_USER_ID_LEN);

    return passenger;
}

Passenger parse_passenger(Tokens tokens) {
    char** parameter = tokens->data;

    int flight_id = atoi(parameter[0]);
    Passenger passenger = make_passenger(flight_id, parameter[1]);
    return passenger;
}

void discard_passenger(void* raw_data, ParserStore store) {
    void** discard_file = &g_array_index(store, void*, 0);
    if (*discard_file == NULL) {
        char* parts[2] = {get_cwd()->str, "Resultados/passengers_errors.csv"};
        char* full_path = join_paths(parts, 2);
        *discard_file = OPEN_FILE(full_path, "w");

        void** file_header = &g_array_index(store, void*, 1);
        rt_assert(
            *file_header != NULL,
            "Could not open discard file: Dataset header missing."
        );

        fputs(*file_header, *discard_file);
        free(full_path);
    }

    Tokens tokens = (Tokens)raw_data;

    discard_to_file(tokens, store);
}

// PASSENGER parsePassengerFromLine(char* line, int len) {
//     IGNORE_ARG(line);
//     IGNORE_ARG(len);
// }