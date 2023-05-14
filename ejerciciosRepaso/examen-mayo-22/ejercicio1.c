#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <sys/time.h>

static int breve = 0;

int main(int argc, char **argv)
{
  int opcion;
  int breve = 0;
  struct option opciones_largas[] = {
      {"breve", no_argument, NULL, 'b'},
      {"verbose", no_argument, NULL, 'v'},
      {"entorno", required_argument, NULL, 'e'},
      {"fecha", no_argument, NULL, 'f'},
      {0, 0, 0, 0}};

  while (1)
  {

    opcion = getopt_long(argc, argv, "e:f", opciones_largas, NULL);

    if (opcion == -1)
      break;

    switch (opcion)
    {
    case 'b':
      breve = 1;
      break;

    case 'v':
      breve = -1;
      break;

    case 'e':
      char *valor = getenv(optarg);
      if (breve == -1)
      {
        printf("La variable de entorno %s tiene el valor %s\n", optarg, valor);
      }
      else if (breve == 1)
      {
        printf("%s\n", valor);
      }
      else
      {
        printf("%s=%s\n", optarg, valor);
      }
      break;

    case 'f':
      time_t t = time(NULL);
      struct tm *tm = localtime(&t);
      char *tiempo = malloc(20);

      if (breve == -1)
      {
        strftime(tiempo, 20, "%A %d/%m/%y, %T \n", tm);
        printf("%s\n", tiempo);
      }
      else if (breve == 0)
      {
        strftime(tiempo, 20, "%d/%m/%y \n", tm);
        printf("%s\n", tiempo);
      }
      else
      {
        strftime(tiempo, 20, "%s \n", tm);
        printf("%s\n", tiempo);
      }
      break;

    default:
      fprintf(stderr, "Usage: %s [--breve] [--verbose] [-e (argumento)| --entorno (argumento)] [-f | --fecha] \n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (optind < argc)
  {
    printf("Argumentos: ");
    while (optind < argc)
    {
      printf("%s ", argv[optind++]);
    }
    printf("\n");
  }
}