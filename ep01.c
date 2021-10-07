#include <stdio.h>
#include <stdlib.h>
#include <matheval.h>
#include <assert.h>

#define ITER_LIMIT 100
#define MAX_BUFFER_SIZE 256
#define DERIV_VARIABLE "x"

#define NEWTON_NEXTVAL "(fx + fx1)/fx1"

/***********************
 ESTRUTURA DE ITERAÇÕES
************************/
typedef struct {
    int size;
    int max_iter;
    int iteration[ITER_LIMIT][3];
} iterations_t;

iterations_t *new_iterations_t(int max_iter)
{
    iterations_t *new_it = malloc(sizeof(iterations_t));
    new_it->size = 0;
    new_it->max_iter = max_iter;

    return new_it;
}

void *destroy_iterations_t(iterations_t *max_iter)
{
    if (max_iter)
        free(max_iter);
}

void push_iteration(iterations_t *it, double aprox, double crit)
{
    int last = it->size;
    if (it->size < it->max_iter){
        it->iteration[last][0] = last;
        it->iteration[last][1] = aprox;
        it->iteration[last][2] = crit;
        (it->size)++;
    }
}

/**************************
 FUNÇÕES DE ZEROS
**************************/

double calculate_function(void *f, double x)
{
    char **names;
    double values[] = { x };
    int count;
    evaluator_get_variables(f, &names, &count);
    double f_x = evaluator_evaluate(f, count, names, values);
}

double next_value(double x, void *f, void *f_prim)
{
    
}

double calculate_error(double cur, double next)
{   
    // abs((next -cur)/next) * 100 
    return abs(1 - cur/next);
}

iterations_t *calculate_zero(void *f, double x0, double epsilon, int max_iter)
{
    iterations_t *iterations = new_iterations_t(max_iter);

    // pega derivada
    void *f_prim = evaluator_derivative(f, DERIV_VARIABLE);
    assert(f_prim);

    // Calcular iterações
    double x1 = next_value(x0, f, f_prim);
    double error = calculate_error(x0, x1);

    push_iteration(iterations, x0, error);

    evaluator_destroy(f_prim);

    return iterations;
}

int main()
{
    char buffer[MAX_BUFFER_SIZE];
    void *f;
    double x0, epsilon;
    int max_iter;

    // ler parâmetros e conferir equação
    scanf("%s %lf %lf %d", buffer, &x0, &epsilon, &max_iter);
    f = evaluator_create(buffer);
    assert(f);

    iterations_t *newton_it = calculate_zero(f, x0, epsilon, max_iter);
    // iterations_t *sec_it = calculate_zero(f, x0, epsilon, max_iter);

    evaluator_destroy(f);
    return 0;
}

