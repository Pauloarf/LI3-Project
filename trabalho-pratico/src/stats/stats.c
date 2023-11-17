#include "catalog/catalogManager.h"
#include "catalog/reservationsCatalog.h"
#include "catalog/usersCatalog.h"
#include "catalog/passengersCatalog.h"
#include "common.h"

typedef struct users_stats {
    char *user_id;
    int user_age;
    int n_flights;
    int total_spent;
} USERS_STATS, *Users_stats;

typedef struct reservations_stats {
    int reservation_id;
    int n_nights;
    int total_price;
} RESERVATIONS_STATS, *Reservations_stats;

typedef struct flights_stats {
    int flight_id;
    int delay;
    int n_passengers;
} FLIGHTS_STATS, *Flights_stats;

typedef struct hotels_stats {
    int hotel_id;
    double average_rating;
    double revenue;
} HOTELS_STATS, *Hotels_stats;

typedef struct aeroport_stats {
    char *aeroport_name;
    int n_passengers;
    double mediana_delays;
} AEROPORT_STATS, *Aeroport_stats;






gint reservation_hotelID_compare_func(gconstpointer a, gconstpointer b) {
    const Reservation *reservation1 = (const Reservation*)a;
    const short int hotel_id2 = (const short int *)b;

    short int hotel_id1 = get_reservation_hotelID(*reservation1);

    if (hotel_id1 < hotel_id2) return -1;
    if (hotel_id1 > hotel_id2) return 1;
    return 0;
}

gint passenger_flightID_compare_func(gconstpointer a, gconstpointer b){
    const Passenger *passenger1 = (const Passenger*)a;
    const int flightID2 = (const int *)b;

    short int flightID1 = get_passenger_flightID(*passenger1);

    if (flightID1 < flightID2) return -1;
    if (flightID1 > flightID2) return 1;
    return 0;
}




//TODO PRECORRE O ARRAY TODO
int calculate_user_n_flights(Catalog *catalog, char *userID){
    int n_flights = 0;
    char *name_to_compare = NULL;
    for(int i = 0; i < catalog_get_item_count(catalog); i++){
        const Passenger passenger_temp = (const Passenger)(catalog_search_in_array(catalog,i));
        name_to_compare = get_passenger_userdID(passenger_temp);
        
        if(strcmp(userID, name_to_compare) == 0){
            n_flights++;
        }

        free(name_to_compare);
    }
    return n_flights;
}

// BUG problema com datas, provavelmente pelos offsets... quanto corresponde a 1 dia?
double calculate_reservation_total_price(Reservation reservation) {
    double n_nights = (get_reservation_end_date(reservation) - get_reservation_begin_date(reservation)) / 86400.0;

    unsigned int price_per_night = get_reservation_price_per_night(reservation);
    unsigned int tax = get_reservation_city_tax(reservation);

    double res = (double)(price_per_night * n_nights) + ((double)(price_per_night * n_nights) / 100.0) * tax;

    return res;
}


// TODO Podes aproveitar para saber o numero de reservas
//TODO retorna o total_gasto e dentro do n_reservation retorna o numero de reservations
double calculate_user_total_spent(Catalog *catalog, char *userID, int *n_reservations){
    double total_spent = 0;
    char *name_to_compare = NULL;
    *n_reservations = 0;
    for(int i = 0; i < catalog_get_item_count(catalog); i++){
        const Reservation reservation_temp = (const Reservation)(catalog_search_in_array(catalog,i));
        name_to_compare = get_reservation_userID(reservation_temp);

        if(strcmp(userID, name_to_compare) == 0){
            total_spent += calculate_reservation_total_price(reservation_temp);
            *n_reservations = *n_reservations + 1;
        }

        free(name_to_compare);
    }
    return total_spent;
}

int calculate_flight_total_passengers(Catalog *catalog, int flightId){
    guint matched_index = 0;
    gboolean exists = catalog_exists_in_array(catalog, flightId, &passenger_flightID_compare_func, &matched_index);
    void *data1, *data2;
    if (exists) {
        int matched_index_down = matched_index;
        
        void *data1 = catalog_search_in_array(catalog, matched_index_down);
        while (get_passenger_flightID((Passenger*)data1)==flightId && matched_index_down > 0) {
            data1 = catalog_search_in_array(catalog, --matched_index_down);
        };
        if(get_reservation_hotelID((Passenger*)data1)!=flightId) matched_index_down++;

        int matched_index_up = matched_index;
        void *data2 = catalog_search_in_array(catalog, matched_index_up);
        while (get_passenger_flightID((Passenger*)data2)==flightId && matched_index_up<catalog_get_item_count(catalog)) {
            data2 = catalog_search_in_array(catalog, ++matched_index_up);
        };
        if(get_reservation_hotelID((Passenger*)data2)!=flightId) matched_index_up--;
        
        return (matched_index_up - matched_index_down + 1);
    } else {
        printf("Flight with that id not found");
        return -1;
    }
}





//TODO verificar se é possivel fazer melhorias
double calculate_hotel_average_rating(Catalog *catalog, int hotel_id) {
    guint matched_index = 0;
    gboolean exists = catalog_exists_in_array(catalog, hotel_id, &reservation_hotelID_compare_func, &matched_index);
    void *data1, *data2;
    if (exists) {
        int matched_index_down = matched_index;
        
        void *data1 = catalog_search_in_array(catalog, matched_index_down);
        while (get_reservation_hotelID((Reservation*)data1)==hotel_id && matched_index_down > 0) {
            data1 = catalog_search_in_array(catalog, --matched_index_down);
        };
        if(get_reservation_hotelID((Reservation*)data1)!=hotel_id) matched_index_down++;

        int matched_index_up = matched_index;
        void *data2 = catalog_search_in_array(catalog, matched_index_up);
        while (get_reservation_hotelID((Reservation*)data2)==hotel_id && matched_index_up<catalog_get_item_count(catalog)) {
            data2 = catalog_search_in_array(catalog, ++matched_index_up);
        };
        if(get_reservation_hotelID((Reservation*)data2)!=hotel_id) matched_index_up--;
        
        
        int i = matched_index_down;
        double rating = 0;
        int quantidade_a_percorrer = (matched_index_up - matched_index_down + 1);
        while ( 0 < quantidade_a_percorrer) {
           const Reservation reservation_temp = (const Reservation)(catalog_search_in_array(catalog,i));
           rating += get_reservation_rating(reservation_temp);
           i++;
           quantidade_a_percorrer--;
        };
        return (rating/(matched_index_up - matched_index_down + 1));
    } else {
        printf("Reservation with that hotel id not found");
        return -1;
    }
}

