/* Question 3:
 * Complete the functions below according to their comments.
 * You may not change anything outside of the functions
 * A sample input file has been provided, you can run it as
 * ./temps < test_data.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct temperature {
    double high;
    double low;
};

struct record {
    char *period;
    int num_days;
    struct temperature *temps;
};

/* [2 marks]
 * Print data  in the following format:
 * "period : (x1, y1) (x2, y2) ...
 * where xi, yi represent the x and y values of each point in points, to one
 * decimal of precision
 * To print a double to one decimal of precision, use the following formatting
 * string: "%.1lf"
 *
 * Example: If the input "weekend 2 20 5 10 -15" was used to create the data
 * then the print_data function would print
 * weekend : (20.0, 5.0) (10.0, -15.0)
 */
void print_data(struct record *data) {
    // TODO
    printf("%s : ", data -> period);
    for(int i = 0; i < data -> num_days; i++){
        printf("(%.1lf, %.1lf) ", data -> temps[i].high, data -> temps[i].low);
    }
}


/* [2 marks]
 * Return the maximum absolute difference between the high and low in the temps
 * component of data. If the temps data was "(20.0, 7.0) (10.0, -15.0)", 
 * the the maximum difference is 25.
 */
double max_span(struct record *data) {
    // TODO
    double max = 0.0;
    for(int i = 0; i < data -> num_days; i++){
        double diff = data -> temps[i].high - data -> temps[i].low;
        if(diff > max){
            max = diff;
        }
    }
    return max;
}


/* [6 marks]
 * Create and return a new record with a copy of the given parameters.
 */
struct record *create_data(char *period, int days, struct temperature *t) {
    // TODO
    struct record *a = malloc(sizeof(struct record));
    strcpy(a ->period, period);
    a->num_days = days;
    a->temps = t;
}


/* [2 marks]
 * Free all the memory allocated for data
 */
void free_data(struct record *data) {
    // TODO
    free(data->period);
    free(data);
}


// Do not change anything below this line.
// You can run the program as 
// ./temps < test_data.txt
#ifndef TESTING

int main(int argc, char **argv) {
    char line[256];
    int num;
    struct temperature temps[12];

    scanf("%s ", line);
    scanf("%d", &num);
    for (int i = 0; i < num; i++) {
        scanf("%lf %lf", &temps[i].high, &temps[i].low);
    }
    struct record *p1 = create_data(line, num, temps);

    scanf("%s ", line);
    scanf("%d ", &num);
    for (int i = 0; i < num; i++) {
        scanf("%lf %lf", &temps[i].high, &temps[i].low);
    }
    struct record *p2 = create_data(line, num, temps);
    print_data(p1);
    printf("max diff = %lf\n", max_span(p1));
    print_data(p2);
    printf("max diff = %lf\n", max_span(p2));
    free_data(p1);
    free_data(p2);
    return 0;
}
#endif
