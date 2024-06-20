#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

int main() {
    int opcion;

    // Loop del menú inicial
    do {
        mostrarMenuInicial();
        scanf("%d", &opcion);
        switch(opcion) {
            case 1:
                // Lógica para iniciar una nueva partida
                limpiar_pantalla();
                printf("Iniciando nueva partida...\n");
                                

                TipoPropiedad* propiedades = inicializar_propiedades();

                break;
            case 2:
                // Lógica para cargar una partida guardada
                limpiar_pantalla();
                printf("Cargando partida...\n");

                break;
            case 3:
                limpiar_pantalla();
                printf("Mostrando reglas del juego...\n\n");
                // Lógica para mostrar las reglas del juego
                mostrar_reglas();
                limpiar_pantalla();
                break;
            case 4:
                limpiar_pantalla();
                printf("Saliendo del juego...\n");

                // Lógica para salir del juego
                break;

            case 666:
                limpiar_pantalla();
                testear_funciones();
                //OPCION PROGRAMADOR
                
            default:
                limpiar_pantalla();
                printf("Opción no válida. Por favor, seleccione una opción válida.\n");
        }
    } while(opcion != 4); // Salir del loop si se selecciona la opción 4 (Salir del juego)
    return 0;
}
