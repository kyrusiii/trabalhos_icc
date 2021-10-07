#include <stdio.h>
#include <matheval.h>
#include <assert.h>

#define STD_BUFFER "x^2 + 4"

int main()
{
  // Cria objeto avaliador da função a partir de f e testa se deu certo.
  void *f;
  f = evaluator_create (STD_BUFFER);
  assert(f);

  // nomes e número de variáveis armazenados e impressos
  char **names;
  int count;
  evaluator_get_variables (f, &names, &count);

  for (int i = 0; i < count; ++i)
    printf("%s\n", names[i]);

  // achar f(1.1) 
  double values[] = {1.1};
  printf("f(1.1) = %g\n", evaluator_evaluate(f, count, names, values));

  // achar f'(x)
  void *f_prim = evaluator_derivative(f, "x");

  // imprimir função e sua derivada
  printf("A função: %s\n", evaluator_get_string(f));
  printf("A derivada: %s\n", evaluator_get_string(f_prim));
  
  evaluator_destroy(f);

  return 0;
}

