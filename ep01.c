#include <stdio.h>
#include <stdlib.h>
#include <matheval.h>
#include <assert.h>
#include <math.h>
#include <string.h>


#define ITER_LIMIT 100
#define MAX_BUFFER_SIZE 256
#define DERIV_VARIABLE "x"

/*****************************************************************
 ESTRUTURA DE ITERAÇÕES
 Matriz que armazena dados das iterações do cálculo de raízes
 por um método numérico.
 
 size: número de linhas da matriz
 max_iter: Número máximo de iterações permitido para a matriz
 iteration: matriz de iterações.
    - iteration[i][0]: x aproximado na iteração i
    - iteration[i][1]: valor do critério de parada na iteração i
*****************************************************************/
typedef struct {
    int size;
    int max_iter;
    double iteration[ITER_LIMIT][2];
} iterations_t;

/* 
    Cria e inicializa instância da estrutura 
*/
iterations_t *new_iterations_t(int max_iter)
{
    iterations_t *new_it = malloc(sizeof(iterations_t));
    new_it->size = 0;
    new_it->max_iter = max_iter;

    return new_it;
}

/* 
    Desaloca estrutura de iterações 
*/
void *destroy_iterations_t(iterations_t *it)
{
    if (it)
        free(it);
}

/*
    Inclui uma iteração no final da matriz  
*/
void push_iteration(iterations_t *it, double aprox, double crit)
{
    int last = it->size;
    if (it->size < it->max_iter){
        it->iteration[last][0] = aprox;
        it->iteration[last][1] = crit;
        (it->size)++;
    }
}

/*******************************
 FUNÇÕES PARA CÁLCULO DE ZEROS
*******************************/

/* 
    Calcula valor de f(x) usando matheval
*/
double calculate_function(void *f, double x)
{
    char **names;
    double values[] = { x };
    int count;
    evaluator_get_variables(f, &names, &count);
    double f_x = evaluator_evaluate(f, count, names, values);

    return f_x;
}

/* 
    Calcula próximo valor de x pelo método Newton-Raphson 
*/
double next_value_newton(double x, void *f, void *f_prim)
{
    double f_x = calculate_function(f, x);
    double f_prim_x = calculate_function(f_prim, x);
    return x - f_x/f_prim_x;
}

/* 
    Calcula próximo valor de x pelo metodo da secante 
*/
double next_value_secante(double x, void *f, double xl)
{
	double xn, f_x, f_xl;
//	calcular fx e fxl	
	f_x = calculate_function(f,x);
	f_xl = calculate_function(f,xl);
	xn = x + (xl / (1 - f_xl/f_x)) - ((x)/ (1 - f_xl/f_x) ) ;
	return xn;
//    return 1.0;
}

/* 
    Calcula critério de parada para valor atual e próximo 
*/
double calculate_relative_error(double cur, double next)
{   
    // abs((next - cur)/next) 
    return fabs(1 - cur/next);
}

/* 
    Calcula zero da função e retorna matriz com iterações
    . method 1: newton
    . method 2: secante (no final funciona com qualquer outro valor)
*/
iterations_t *calculate_zero(void *f, double x0, double epsilon, int max_iter, int method)
{
    iterations_t *iterations = new_iterations_t(max_iter);

    // pega derivada
    void *f_prim = evaluator_derivative(f, DERIV_VARIABLE);
    assert(f_prim);

    // Calcular iterações
    double x_prev, x_cur = x0, x_next, error;
    
    x_next = next_value_newton(x_cur, f, f_prim);
    error = calculate_relative_error(x_cur, x_next);
    push_iteration(iterations, x_cur, error);

    for (int i = 1; i < max_iter; ++i)
    {
        x_prev = x_cur;
        x_cur = x_next;
        if (method == 1) x_next = next_value_newton(x_cur, f, f_prim);
        else x_next = next_value_secante(x_cur, f, x_prev);

        double error = calculate_relative_error(x_cur, x_next);
        push_iteration(iterations, x_cur, error);
    }

    evaluator_destroy(f_prim);

    return iterations;
}

/*
	Calcula o erro entre os métodos empregados
*/
double calculate_absolute_error(double cur, double next){
	return fabs( cur - next );
//	return 1.0;
}

/*
	Calcula os ULP entre cada aproximação
*/

int ulp(double newt, double sec){
	long ni, si, z;
	double n = newt;
	double s = sec;
	double zero = 0.0;
	memcpy(&ni, &n, sizeof(double));
	memcpy(&si, &s, sizeof(double));	
	if (n*s < 0){
		n = fabs(newt);
		s = fabs(sec);
		memcpy(&ni, &n, sizeof(double));
		memcpy(&si, &s, sizeof(double));
		memcpy(&z, &zero, sizeof(double));		
		return  abs(ni) + abs(si) -2*zero - 1;
	}
	else if (ni-si == 0){
		return 0;
	}
	else{
		return abs(ni - si) - 1;
	}
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

    iterations_t *newton_it = calculate_zero(f, x0, epsilon, max_iter, 1);
    iterations_t *sec_it = calculate_zero(f, x0, epsilon, max_iter, 2);

    for (int i = 0; i < newton_it->size; ++i)
    {
        double newton_x = newton_it->iteration[i][0];
        double sec_x = sec_it->iteration[i][0];
        printf("%d, ", i);
        printf("%1.16e, %1.16e, ", newton_it->iteration[i][0], newton_it->iteration[i][1]);
        printf("%1.16e, %1.16e, ", sec_it->iteration[i][0], sec_it->iteration[i][1]);
        printf("%1.16e, ", calculate_absolute_error(newton_x, sec_x));
        printf("%1.16e, ", calculate_relative_error(newton_x, sec_x));
        printf("%d\n", ulp(newton_x, sec_x));
    }
    evaluator_destroy(f);
    return 0;
}

