## Descripción

Esta aplicación permite emular partidas de Monopoly con un alta fidelidad, respetando todas sus reglas importantes. Los jugadores pueden realizar acciones como recorrer el mapa, interactuar con las propiedades (compra de propiedad, compra de casas, hipoteca), e incluso pueden llegar a interactuar con sus rivales en la partida si así lo desean (sistema de negociación entre jugadores).

## Cómo compilar y ejecutar

Dado que el sistema está diseñado para ser accesible y fácil de probar, recomendamos usar [Repl.it](http://repl.it/) para una rápida configuración y ejecución. Sigue estos pasos para comenzar:

1. Visita [Repl.it](https://repl.it/).
2. Crea una nueva cuenta o inicia sesión si ya tienes una.
3. Una vez en tu dashboard, selecciona "New Repl" y elige "Import from GitHub".
4. Pega la URL del repositorio: `https://github.com/username/sistema-gestion-pacientes.git`.
5. [Repl.it](http://repl.it/) clonará el repositorio y preparará un entorno de ejecución.
6. Presiona el botón "Run" para compilar y ejecutar la aplicación.

## Funcionalidades

### Funcionando correctamente:

- Inicio de una nueva partida, acompañado de una asignación inicial de datos del jugador (nombre, dinero inicial, turnos).
- Listado de las reglas más importantes del juego.
- Impresión por pantalla del estado del jugador en cada turno (nombre, dinero actual, posición actual, etc.).
- Recorrido del tablero (funcionando bajo la lógica de lanzamiento de 2 dados al azar), aplicando efectos variados dependiendo en la casilla donde se caiga.
- Compra de propiedades, compra y venta de casas para las mismas, además de la posibilidad de hipotecar propiedades para obtener dinero de regreso.
- Finalización del turno por parte del jugador (indicando que ya realizó todas las acciones deseadas para este turno).
- Declaración de bancarrota por parte del jugador (terminando su participación en la partida de manera prematura).

### Problemas conocidos:

### A mejorar:

- Apartado visual del juego (representación visual del tablero y los datos del jugador de una forma más amigable).
- Implementación de la regla que obliga a repartir las casas al comprarlas (un ejemplo de esta regla sería que al comprar 4 casas para un barrio, no podría colocarlas todas en una sola propiedad, teniendo que repartirlas).
- Implementación de las funciones de guardar partida y cargar partida, las cuales estuvieron consideradas hasta etapas avanzadas del proyecto.

## Ejemplo de acciones a realizar en una partida
Comenzaremos mostrando las reglas del juego
````
╔══════════════════════════════════════════════════╗
║        ＭＯＮＯＰＯＬY    ＶＡＬＰＡＲＡÍＳＯ         ║
╚══════════════════════════════════════════════════╝
=== MENÚ INICIAL ===
1. Nueva partida
2. Cargar partida
3. Reglas
4. Salir del juego
Seleccione una opción: 3
````

````
Mostrando reglas del juego...

Reglas del juego:

=== Reglas del Monopoly ===
1. El objetivo del juego es ser el jugador con más cantidad de dinero acumulado.
2. Los jugadores se mueven por el tablero comprando propiedades y cobrando renta.
3. Se pueden construir casas en propiedades para aumentar la renta que generan.
4. Si un jugador cae en una propiedad sin dueño, puede comprarla. Si ya tiene dueño, paga renta.
5. Hay casillas especiales como fortuna y arca comunal, la cual tienen efectos variados que pueden afectar positiva o negativamente al jugador.
6. Si un jugador acaba en la carcel, deberá esperar 3 turnos o pagar una fianza para salir.
7. El juego termina cuando solo queda un jugador con dinero y propiedades, es decir, cuando todo el resto este en bancarrota!.
===========================
````

Ahora comenzaremos una nueva partida

````
╔══════════════════════════════════════════════════╗
║        ＭＯＮＯＰＯＬY    ＶＡＬＰＡＲＡÍＳＯ         ║
╚══════════════════════════════════════════════════╝
=== MENÚ INICIAL ===
1. Nueva partida
2. Cargar partida
3. Reglas
4. Salir del juego
Seleccione una opción: 1
````
Diremos que queremos una partida de 2 jugadores, sus nombres serán Perro y Sombrero.
````
Iniciando nueva partida...
Ingrese el número de jugadores (2 a 4): 2
Ingrese el nombre del jugador 1: Perro
Ingrese el nombre del jugador 2: Sombrero
````
Veremos que el jugador que inicia la partida es Perro, por lo que es su turno y debe lanzar los dados.
````
Perro ES TU TURNO!

╔══════════════════════════════════════════════════╗
║               ESTADO DEL JUGADOR                 ║
╚══════════════════════════════════════════════════╝
════════════════════════════════════════════════════
Jugador: Perro
Dinero actual: 15000
Turnos de penalización en cárcel: 0
Posición en el tablero: 0
Te encuentras en: SALIDA
Tipo: Salida
Propietario: -
════════════════════════════════════════════════════

Presiona enter para lanzar los dados!
````
Al lanzar los dados, vemos que obtenemos dobles, por lo que tenemos un turno adicional, además caimos en una casilla de carta comunal, por lo que se nos aplica el efecto de una carta de ese tipo.
````
¡Haz lanzado los dados: 1 y 1 (Total: 2)!

Haz obtenido dobles!

Ganas un turno extra!

¡Te moviste a la casilla 2!

¡CARTA DE LA ARCA COMÚNAL!

Carta sacada: HOSPITALIZACIÓN. PAGUE $1000.

Presiona enter para continuar...
````
Hemos caido en una propiedad, por lo que vamos a proceder a comprarla.
````
¡Haz lanzado los dados: 6 y 1 (Total: 7)!

¡Te moviste a la casilla 9!

Dinero Actual: 14000

╔══════════════════════════════════════════════════╗
║            DETALLES DE LA PROPIEDAD              ║
╚══════════════════════════════════════════════════╝
Nombre: AV. PEDRO MONT
Sector: VALPARAÍSO
Precio: $1200
Renta: $160
Precio por casa: $500
Numero de casas: 0
Posición: 9

¿Quieres comprar AV. PEDRO MONT por 1200? (s/n): s

Felicidades Perro, has comprado la propiedad AV. PEDRO MONT por 1200!

Presiona enter para continuar...
````

Ya teniendo una propiedad, vamos a hipotecarla para obtener una fracción del dinero por el que la compramos.
````
╔══════════════════════════════════════════════════╗
║               ESTADO DEL JUGADOR                 ║
╚══════════════════════════════════════════════════╝
════════════════════════════════════════════════════
Jugador: Perro
Dinero actual: 12800
Turnos de penalización en cárcel: 0
Posición en el tablero: 9
Te encuentras en: AV. PEDRO MONT
Tipo: Propiedad
Propietario: Perro
════════════════════════════════════════════════════

¿Qué acción deseas realizar?

1. Modificar campos de tus propiedades
2. Intercambiar con otros jugadores
3. Finalizar turno
4. Declararte en bancarrota
1
````
Hipotecamos AV. PEDRO MONT por $600
````
╔══════════════════════════════════════════════════╗
║            DETALLES DE LA PROPIEDAD              ║
╚══════════════════════════════════════════════════╝
Nombre: AV. PEDRO MONT
Sector: VALPARAÍSO
Precio: $1200
Renta: $160
Precio por casa: $500
Numero de casas: 0
Posición: 9

¿Qué deseas hacer?
1. Comprar casas
2. Vender casas
3. Hipotecar propiedades
4. Deshipotecar propiedades
5. Salir de este menú
3
¿
Quieres hipotecar la propiedad AV. PEDRO MONT por 600? (s/n): s

Felicidades Perro, has hipotecado la propiedad AV. PEDRO MONT y has recibido 600!

Recuerda que no podrás cobrar renta por AV. PEDRO MONT mientras esté hipotecada.

¿Deseas continuar en este menú? (s/n):
````
Ya en el turno de Sombrero, tiraremos los dados.
````
Sombrero ES TU TURNO!

╔══════════════════════════════════════════════════╗
║               ESTADO DEL JUGADOR                 ║
╚══════════════════════════════════════════════════╝
════════════════════════════════════════════════════
Jugador: Sombrero
Dinero actual: 15000
Turnos de penalización en cárcel: 0
Posición en el tablero: 0
Te encuentras en: SALIDA
Tipo: Salida
Propietario: -
════════════════════════════════════════════════════

Presiona enter para lanzar los dados!
````
Habiendonos movido a ESTACIÓN PUERTO, la compraremos.
````
¡Haz lanzado los dados: 1 y 4 (Total: 5)!

¡Te moviste a la casilla 5!

Dinero Actual: 15000
╔══════════════════════════════════════════════════╗
║            DETALLES DE LA ESTACIÓN               ║
╚══════════════════════════════════════════════════╝
Nombre: ESTACIÓN PUERTO
Precio: $2000
Renta base: $250
*El valor de la renta se duplica según la cantidad de metros que controlas*

¿Quieres comprar ESTACIÓN PUERTO por 2000? (s/n): s
Felicidades Sombrero, has comprado ESTACIÓN PUERTO por 2000!
````
Finalmente, el jugador Sombrero se declara en bancarrota, por lo que Perro es el ganador de la partida!.
````
╔══════════════════════════════════════════════════╗
║               ESTADO DEL JUGADOR                 ║
╚══════════════════════════════════════════════════╝
════════════════════════════════════════════════════
Jugador: Sombrero
Dinero actual: 13000
Turnos de penalización en cárcel: 0
Posición en el tablero: 5
Te encuentras en: ESTACIÓN PUERTO
Tipo: Metro
Propietario: Sombrero
════════════════════════════════════════════════════

¿Qué acción deseas realizar?

1. Modificar campos de tus propiedades
2. Intercambiar con otros jugadores
3. Finalizar turno
4. Declararte en bancarrota
4

Sombrero haz sido eliminado, estas en bancarrota.

Presione enter para continuar

¡El jugador Perro es el ganador del juego!

Presiona enter para salir
